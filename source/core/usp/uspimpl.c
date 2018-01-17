//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.c: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/base64.h"

#include "uspinternal.h"

#ifdef __linux__
#include <unistd.h>
#endif

typedef void(*ResponsePathHandler)(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context);

const char g_messagePathSpeechHypothesis[] = "speech.hypothesis";
const char g_messagePathSpeechPhrase[] = "speech.phrase";
const char g_messagePathTurnStart[] = "turn.start";
const char g_messagePathTurnEnd[] = "turn.end";
const char g_messagePathSpeechStartDetected[] = "speech.startDetected";
const char g_messagePathSpeechEndDetected[] = "speech.endDetected";
const char g_messagePathResponse[] = "response";
// const char kUserAgent[] = "CortanaSDK (" CORTANASDK_BUILD_HASH "DeviceType=Near;SpeechClient=" CORTANA_SDK_VERSION ")";

// Todo: add get real time
uint64_t telemetry_gettime()
{
    return 0;
}

// Zhou: write audio stream to the service: call transport to write/flush stream; used by usplib
UspResult AudioStreamWrite(UspHandle handle, const void * data, uint32_t size, uint32_t* bytesWritten)
{
    UspContext* uspContext = (UspContext*)handle;
    const char* httpArgs = "/audio";
    int ret = -1;

    if (!handle || !data || !size)
    {
        return USP_INVALID_PARAMETER;
    }

    Lock(uspContext->transportRequestLock);

    metrics_audiostream_data(size);
    if (!size)
    {
        ret = TransportStreamFlush(uspContext->transportRequest);
    }
    else
    {
        if (!uspContext->audioOffset)
        {
            metrics_audiostream_init();
            // user initiated listening and multi-turn require the request id to 
            // be re-created here to ensure barge-in scenerios work.
            // if (!uspContext->KWTriggered)
            // {
            //    TransportCreateRequestId(uspContext->transportRequest);
            //    // cortana_reset_speech_request_id(uspContext);
            // }

            metrics_audio_start();

            ret = TransportStreamPrepare(uspContext->transportRequest, httpArgs);
            if (ret)
            {
                Unlock(uspContext->transportRequestLock);
                return ret;
            }
        }

        ret = TransportStreamWrite(uspContext->transportRequest, (uint8_t*)data, size);
    }

    Unlock(uspContext->transportRequestLock);

    uspContext->audioOffset += size;

    if (!ret && NULL != bytesWritten)
    {
        *bytesWritten = size;
    }

    if (!ret)
    {
        return USP_SUCCESS;
    }
    else
    {
        return USP_TRANSPORT_ERROR_GENERIC;
    }
}

// Zhou: write audio stream to the service: call transport:stream_flush; used by usplib.c
UspResult AudioStreamFlush(UspHandle handle)
{
    UspContext* uspContext = (UspContext*)handle;
    int ret;
    if (!handle)
    {
        return USP_INVALID_PARAMETER;
    }
    else if (!uspContext->audioOffset)
    {
        return USP_SUCCESS;
    }

    ret = TransportStreamFlush(uspContext->transportRequest);
    uspContext->audioOffset = 0;
    metrics_audiostream_flush();
    metrics_audio_end();

    if (!ret)
    {
        return USP_SUCCESS;
    }
    else
    {
        return USP_TRANSPORT_ERROR_GENERIC;
    }
}

// Zhou: callback for transport error
static void TransportErrorHandler(TransportHandle transportHandle, TransportError reason, void* context)
{
    (void)transportHandle;
    UspResult uspError;

    LogInfo("On TranportError: reason=%d", reason);
    switch (reason)
    {
    default:
        uspError = USP_TRANSPORT_ERROR_GENERIC;
        break;

    case TRANSPORT_ERROR_AUTHENTICATION:
        uspError = USP_AUTH_ERROR;
        break;

    case TRANSPORT_ERROR_CONNECTION_FAILURE:
    case TRANSPORT_ERROR_DNS_FAILURE:
        uspError = USP_CONNECTION_FAILURE;
        break;

    case TRANSPORT_ERROR_REMOTECLOSED:
        uspError = USP_CONNECTION_REMOTE_CLOSED;
    }

    UspContext* uspContext = (UspContext *)(context);
    if (uspContext == NULL)
    {
        LogError("No context provided in TransportErrorHandler.");
    }
    else if (uspContext->callbacks)
    {
        uspContext->callbacks->OnError(uspContext, uspContext->callbackContext, uspError);
    }
}

//zhou: callback for Speech.EndDetected handler
static void SpeechEndHandler(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context)
{
    (void)transportHandle;
    (void)mime;
    (void)buffer;
    (void)size;
    (void)path;

    // USP handling
    UspContext* uspContext = (UspContext *)context;
    if (uspContext == NULL)
    {
        LogError("No context provided in SpeechEndHandler.");
    }
    else if (uspContext->callbacks)
    {
        UspMsgSpeechEndDetected* msg = malloc(sizeof(UspMsgSpeechEndDetected));
        // Todo: deal with char to wchar
        // Todo: add more field;
        uspContext->callbacks->onSpeechEndDetected(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        free(msg);
    }
}

// zhou: callback for Speech.TurnEnd 
static void TurnEndHandler(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context)
{
    (void)transportHandle;
    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    // USP handling
    UspContext* uspContext = (UspContext *)context;
    if (uspContext == NULL)
    {
        LogError("No context provided in TurnEndHandler.");
    }
    else if (uspContext->callbacks)
    {
        UspMsgTurnEnd* msg = NULL;
        // Todo: deal with char to wchar
        // Todo: add more field;
        uspContext->callbacks->onTurnEnd(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        // free(msg);
    }

    telemetry_flush();

    // Todo: need to reset request_id?
    // Todo: the caller need to flush audio buffer?
    Lock(uspContext->transportRequestLock);
    TransportCreateRequestId(uspContext->transportRequest);
    Unlock(uspContext->transportRequestLock);
}


// Zhou: callback for Speech.TurnStart, Speech.Hypothesis, Speech.Phrase, and also for Response.
static void ContentPathHandler(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context)
{
    (void)transportHandle;

    if (size == 0)
    {
        PROTOCOL_VIOLATION("response contains no body");
        return;
    }

    int ret = -1;
    BUFFER_HANDLE responseContentHandle = BUFFER_create(
        (unsigned char*)buffer,
        size + 1);
    if (!responseContentHandle)
    {
        LogError("BUFFER_create failed");
        return;
    }

    BUFFER_u_char(responseContentHandle)[size] = 0;

    if (context == NULL)
    {
        LogError("Context passed to ContentPathHandler is null.");
    }

    ret = ContentDispatch(context, path, mime, 0, responseContentHandle, size);

    BUFFER_delete(responseContentHandle);
    return;
}

// Zhou: callback for SpeechStartDetected.
static void SpeechStartHandler(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context)
{
    (void)transportHandle;
    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    // USP handling
    UspContext* uspContext = (UspContext *)context;
    if (uspContext == NULL)
    {
        LogError("No context provided in SpeechStartHandler.");
    }
    else if (uspContext->callbacks)
    {
        UspMsgSpeechStartDetected* msg = malloc(sizeof(UspMsgSpeechStartDetected));
        // Todo: deal with char to wchar
        // Todo: add more field;
        uspContext->callbacks->onSpeechStartDetected(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        free(msg);
    }
}

// Zhou: Callback for http response
static void ResponseHandler(TransportHandle transportHandle, const char* path, const char* mime, const unsigned char* buffer, size_t size, void* context)
{
    ContentPathHandler(transportHandle, path, mime, buffer, size, context);
}

// Zhou Dispatch table for PATH message
const struct _PathHandler
{
    const char*            path;
    ResponsePathHandler handler;
} g_pathHandlers[] = {
    { g_messagePathTurnStart, ContentPathHandler },
    { g_messagePathSpeechStartDetected, SpeechStartHandler },
    { g_messagePathSpeechEndDetected, SpeechEndHandler },
    { g_messagePathTurnEnd, TurnEndHandler },
    { g_messagePathSpeechHypothesis, ContentPathHandler },
    { g_messagePathSpeechPhrase, ContentPathHandler },
    { g_messagePathResponse, ResponseHandler },
};

// Zhou: callback for data available on tranport
static void TransportRecvResponseHandler(TransportHandle transportHandle, HTTP_HEADERS_HANDLE responseHeader, const unsigned char* buffer, size_t size, unsigned int errorCode, void* context)
{
    const char * path;
    const char * contentType = NULL;

    if (errorCode != 0)
    {
        LogError("Response error %d", errorCode);
        // TODO: Lower layers need appropriate signals
        return;
    }
    else if (!context || NULL == responseHeader)
    {
        return;
    }

    path = HTTPHeaders_FindHeaderValue(responseHeader, KEYWORD_PATH);
    if (!path)
    {
        PROTOCOL_VIOLATION("response missing '" KEYWORD_PATH "' header");
        return;
    }

    if (size != 0)
    {
        contentType = HTTPHeaders_FindHeaderValue(responseHeader, g_keywordContentType);
        if (NULL == contentType)
        {
            PROTOCOL_VIOLATION("response '%s' contains body with no content-type", path);
            return;
        }
    }

    for (int i = 0; i < sizeof(g_pathHandlers) / sizeof(g_pathHandlers[0]); i++)
    {
        if (!strcmp(path, g_pathHandlers[i].path))
        {
            g_pathHandlers[i].handler(transportHandle, path, contentType, buffer, size, context);
            return;
        }
    }

    PROTOCOL_VIOLATION("unhandled response '%s'", path);
    metrics_transport_unhandledresponse();
}


// Todo: check whether this is still used?
// Deprecated V1 handler for partial results.  Remove once the service switched.
UspResult TextResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext)
{
    (void)path;
    (void)ioBuffer;
    (void)callback;
    (void)asyncContext;
    (void)context;
    (void)buffer;
    (void)bufferSize;

    LogError("Not implemented");

    return USP_NOT_IMPLEMENTED;
}

// Device thumbprint doesn't change when user account switch.
// First, try to hit memory cache.
// If fails, try to read from local storage.
// If fails, require it from CDP API.
// If fails, leave it empty.
const char* GetCdpDeviceThumbprint()
{
#if !defined(ThumbprintBufferSize)
#define ThumbprintBufferSize 50
#endif

    static char thumbprint[ThumbprintBufferSize] = { 0 };

    // if memory cache is empty, try to generate one?
    // Todo: how to get device thumbprint
    if (*thumbprint == 0)
    {
        int result = -1;
        // generate one.
        uint8_t bThumbPrint[32];
        for (int i = 0; i < sizeof(bThumbPrint); i++)
        {
            bThumbPrint[i] = (unsigned char)rand();
        }

        STRING_HANDLE b64 = Base64_Encode_Bytes(bThumbPrint, sizeof(bThumbPrint));
        if (b64)
        {
            if (STRING_length(b64) < sizeof(thumbprint))
            {
                strcpy_s(thumbprint, sizeof(thumbprint), STRING_c_str(b64));
                result = 0;
            }

            STRING_delete(b64);
        }

        if (result == 0)
        {
            LogInfo("result from GetDeviceThumbprint: %s", thumbprint);
        }
    }

    return thumbprint;
}

// zhou: called by speech_initialize. Intialize transport and URL
static UspResult TransportInitialize(UspContext* uspContext, const char *endpoint)
{
    uspContext->transportRequest = TransportRequestCreate(endpoint, uspContext);

    if (NULL == uspContext->transportRequest)
    {
        return USP_TRANSPORT_ERROR_GENERIC;
    }

    TransportSetDnsCache(uspContext->transportRequest, uspContext->dnsCache);
    TransportSetCallbacks(uspContext->transportRequest, TransportErrorHandler, TransportRecvResponseHandler);
    // Todo: does USP require User-Agent in CogSvc?
    // TransportRequestAddRequestHeader(uspContext->transportRequest, "User-Agent", kUserAgent);
    TransportSetTokenStore(uspContext->transportRequest, uspContext->token_store);

    uspContext->transportRequestLock = Lock_Init();
    if (!uspContext->transportRequestLock)
    {
        return USP_INITIALIZATION_FAILURE;
    }

    return USP_SUCCESS;
}

static UspResult TransportShutdown(UspContext* uspContext)
{
    if (uspContext->transportRequest != NULL)
    {
        TransportRequestDestroy(uspContext->transportRequest);
    }

    if (uspContext->transportRequestLock != NULL)
    {
        Lock_Deinit(uspContext->transportRequestLock);
    }

    return USP_SUCCESS;
}

UspResult UspContextDestroy(UspContext* uspContext)
{
    if (uspContext == NULL)
    {
        return USP_INVALID_PARAMETER;
    }

    uspContext->callbacks = NULL;
    uspContext->callbackContext = NULL;
    (void)TransportShutdown(uspContext);

    if (uspContext->dnsCache != NULL)
    {
        DnsCacheDestroy(uspContext->dnsCache);
    }

    telemetry_uninitialize();
    PropertybagShutdown();

    free(uspContext);
    return USP_SUCCESS;
}

UspResult UspContextCreate(UspContext** contextCreated, const char* endpoint)
{
    UspContext* uspContext = (UspContext*)malloc(sizeof(UspContext));
    if (uspContext == NULL)
    {
        LogError("UspInitialize failed: unexpected runtime error.");
        return USP_OUT_OF_MEMORY;
    }
    memset(uspContext, 0, sizeof(UspContext));

    PropertybagInitialize();
    telemetry_initialize();

    uspContext->createTime = telemetry_gettime();
    uspContext->dnsCache = DnsCacheCreate();
    if (uspContext->dnsCache == NULL)
    {
        LogError("Create DNSCache failed in UspContextCreate()");
        UspContextDestroy(uspContext);
        return USP_INITIALIZATION_FAILURE;
    }

    if (TransportInitialize(uspContext, endpoint) != USP_SUCCESS)
    {
        LogError("Initialize transport failed in UspContextCreate()");
        UspContextDestroy(uspContext);
        return USP_INITIALIZATION_FAILURE;
    }

    *contextCreated = uspContext;
    return USP_SUCCESS;
}

UspResult UspSetCallbacks(UspContext* uspContext, UspCallbacks *callbacks, void* callbackContext)
{
    if ((uspContext == NULL) || (callbacks == NULL) || (callbacks->version != USP_VERSION) || (callbacks->size != sizeof(UspCallbacks)))
    {
        return USP_INVALID_PARAMETER;
    }

    uspContext->callbacks = callbacks;
    uspContext->callbackContext = callbackContext;
    return USP_SUCCESS;
}
