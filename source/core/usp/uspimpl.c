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
#include "azure_c_shared_utility/crt_abstractions.h"

#include "uspinternal.h"

#ifdef __linux__
#include <unistd.h>
#endif

const char g_messagePathSpeechHypothesis[] = "speech.hypothesis";
const char g_messagePathSpeechPhrase[] = "speech.phrase";
const char g_messagePathSpeechFragment[] = "speech.fragment";
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

// Write audio stream to the service: call transport to write/flush stream
UspResult AudioStreamWrite(UspHandle uspHandle, const void * data, uint32_t size, uint32_t* bytesWritten)
{
    const char* httpArgs = "/audio";
    int ret = -1;

    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_ARGUMENT_NULL(data, "data");

    if (size == 0)
    {
        LogError("%s: size should not be 0. Use AudioStreamFlush() to flush the buffer.");
        return USP_INVALID_ARGUMENT;
    }

    Lock(uspHandle->transportRequestLock);

    metrics_audiostream_data(size);
    if (!size)
    {
        ret = TransportStreamFlush(uspHandle->transportRequest);
    }
    else
    {
        if (!uspHandle->audioOffset)
        {
            metrics_audiostream_init();
            // user initiated listening and multi-turn require the request id to 
            // be re-created here to ensure barge-in scenerios work.
            // if (!uspHandle->KWTriggered)
            // {
            //    TransportCreateRequestId(uspHandle->transportRequest);
            //    // cortana_reset_speech_request_id(uspHandle);
            // }

            metrics_audio_start();

            ret = TransportStreamPrepare(uspHandle->transportRequest, httpArgs);
            if (ret)
            {
                Unlock(uspHandle->transportRequestLock);
                return ret;
            }
        }

        ret = TransportStreamWrite(uspHandle->transportRequest, (uint8_t*)data, size);
    }

    Unlock(uspHandle->transportRequestLock);

    uspHandle->audioOffset += size;

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

// Write audio stream to the service: call transport:stream_flush
UspResult AudioStreamFlush(UspHandle uspHandle)
{
    int ret;

    USP_RETURN_IF_HANDLE_NULL(uspHandle);

    if (!uspHandle->audioOffset)
    {
        return USP_SUCCESS;
    }

    ret = TransportStreamFlush(uspHandle->transportRequest);
    uspHandle->audioOffset = 0;
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

// Callback for transport error
static UspResult TransportErrorHandler(TransportHandle transportHandle, TransportError reason, void* context)
{
    UspResult uspError;

    (void)transportHandle;

    UspContext* uspContext = (UspContext*)context;
    assert(uspContext != NULL);
    USP_RETURN_IF_CALLBACKS_NULL(uspContext);

    LogInfo("%s: uspContext:0x%x, reason=%d.", uspContext, reason);

    if (uspContext->callbacks->OnError == NULL)
    {
        LogInfo("%s: No callback is defined for onError.", __FUNCTION__);
        return USP_SUCCESS;
    }

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

    uspContext->callbacks->OnError(uspContext, uspContext->callbackContext, uspError);

    return USP_SUCCESS;
}

// Callback for speech.end
static UspResult SpeechEndHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size, bool userCallbackInvoked)
{
    (void)mime;
    (void)buffer;
    (void)size;
    (void)path;

    // Call userback only if the message is not defined in the user-defined handlers.
    if (!userCallbackInvoked)
    {
        assert(uspContext != NULL);
        USP_RETURN_IF_CALLBACKS_NULL(uspContext);
        if (uspContext->callbacks->onSpeechEndDetected == NULL)
        {
            LogInfo("%s: No callback is defined for speech.end.", __FUNCTION__);
            return USP_SUCCESS;
        }

        UspMsgSpeechEndDetected* msg = malloc(sizeof(UspMsgSpeechEndDetected));
        // Todo: deal with char to wchar
        // Todo: add more field;
        uspContext->callbacks->onSpeechEndDetected(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        free(msg);
    }

    return USP_SUCCESS;
}

// callback for turn.end
static UspResult TurnEndHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size, bool userCallbackInvoked)
{
    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    // Call userback only if the message is not defined in the user-defined handlers.
    if (!userCallbackInvoked)
    {
        assert(uspContext != NULL);
        USP_RETURN_IF_CALLBACKS_NULL(uspContext);

        if (uspContext->callbacks->onTurnEnd == NULL)
        {
            LogInfo("%s: No callback is defined for turn.end.", __FUNCTION__);
        }
        else
        {
            UspMsgTurnEnd* msg = NULL;
            // Todo: deal with char to wchar
            // Todo: add more field;
            uspContext->callbacks->onTurnEnd(uspContext, uspContext->callbackContext, msg);
            // Todo: better handling of memory management.
            // free(msg);
        }
    }

    telemetry_flush();

    // Todo: need to reset request_id?
    // Todo: the caller need to flush audio buffer?
    Lock(uspContext->transportRequestLock);
    TransportCreateRequestId(uspContext->transportRequest);
    Unlock(uspContext->transportRequestLock);

    return USP_SUCCESS;
}


// Callback handler for turn.start, speech.hypothesis, speech.phrase, and also for response.
static UspResult ContentPathHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size, bool userCallbackInvoked)
{
    UspResult ret;

    assert(uspContext != NULL);

    if (size == 0)
    {
        PROTOCOL_VIOLATION("response contains no body");
        return USP_INVALID_MESSAGE;
    }

    BUFFER_HANDLE responseContentHandle = BUFFER_create(
        (unsigned char*)buffer,
        size + 1);
    if (!responseContentHandle)
    {
        LogError("BUFFER_create failed in %s", __FUNCTION__);
        return USP_OUT_OF_MEMORY;
    }

    BUFFER_u_char(responseContentHandle)[size] = 0;

#ifdef _DEBUG
    LogInfo("Content Message: path: %s, content type: %s, size: %zu, buffer: %s", path, mime, size, (char *)BUFFER_u_char(responseContentHandle));
#endif
    ret = ContentDispatch((void*)uspContext, path, mime, 0, responseContentHandle, size, userCallbackInvoked);

    BUFFER_delete(responseContentHandle);

    return ret;
}

// Callback for SpeechStartDetected.
static UspResult SpeechStartHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size, bool userCallbackInvoked)
{
    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    // Call userback only if the message is not defined in the user-defined handlers.
    if (!userCallbackInvoked)
    {
        assert(uspContext != NULL);
        USP_RETURN_IF_CALLBACKS_NULL(uspContext);
        if (uspContext->callbacks->onSpeechStartDetected == NULL)
        {
            LogInfo("%s: No callback is defined for speech.start.", __FUNCTION__);
            return USP_SUCCESS;
        }

        UspMsgSpeechStartDetected* msg = malloc(sizeof(UspMsgSpeechStartDetected));
        // Todo: deal with char to wchar
        // Todo: add more field;
        uspContext->callbacks->onSpeechStartDetected(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        free(msg);
    }

    return USP_SUCCESS;
}

/**
* The dispatch table for message.
*/
typedef UspResult(*SystemMessageHandler)(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size, bool userCallbackInvoked);

const struct _PathHandler
{
    const char* path;
    SystemMessageHandler handler;
} g_SystemMessageHandlers[] =
{
    { g_messagePathTurnStart, ContentPathHandler },
    { g_messagePathSpeechStartDetected, SpeechStartHandler },
    { g_messagePathSpeechEndDetected, SpeechEndHandler },
    { g_messagePathTurnEnd, TurnEndHandler },
    { g_messagePathSpeechHypothesis, ContentPathHandler },
    { g_messagePathSpeechPhrase, ContentPathHandler },
    { g_messagePathSpeechFragment, ContentPathHandler },
    { g_messagePathResponse, ContentPathHandler },
};

// Callback for data available on tranport
static void TransportRecvResponseHandler(TransportHandle transportHandle, HTTP_HEADERS_HANDLE responseHeader, const unsigned char* buffer, size_t size, unsigned int errorCode, void* context)
{
    const char* path;
    const char* contentType = NULL;
    bool userCallbackInvoked = false;

    (void)transportHandle;

    if (errorCode != 0)
    {
        LogError("Response error %d in %s", errorCode, __FUNCTION__);
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

    // Check user-defined message handler first.
    if (context == NULL)
    {
        LogError("No context provided in %s.", __FUNCTION__);
        return;
    }

    UspContext *uspContext = (UspContext *)context;

    for (size_t i = 0; i < uspContext->userMessageHandlerSize; i++)
    {
        if (!strcmp(path, uspContext->userMessageHandlerTable[i].path))
        {
            assert(uspContext->userMessageHandlerTable[i].handler != NULL);
            LogInfo("User Message: path: %s, content type: %s, size: %zu.", path, contentType, size);
            uspContext->userMessageHandlerTable[i].handler(uspContext, path, contentType, buffer, size, uspContext->callbackContext);
            userCallbackInvoked = true;
            break;
        }
    }

    for (int i = 0; i < ARRAYSIZE(g_SystemMessageHandlers); i++)
    {
        if (!strcmp(path, g_SystemMessageHandlers[i].path))
        {
            g_SystemMessageHandlers[i].handler(uspContext, path, contentType, buffer, size, userCallbackInvoked);
            return;
        }
    }

    if (!userCallbackInvoked)
    {
        PROTOCOL_VIOLATION("unhandled response '%s'", path);
        metrics_transport_unhandledresponse();
    }
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
        for (unsigned int i = 0; i < sizeof(bThumbPrint); i++)
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

UspResult TransportInitialize(UspContext* uspContext, const char* endpoint)
{
    TransportHandle transportHandle = TransportRequestCreate(endpoint, uspContext);
    if (transportHandle == NULL)
    {
        LogError("Failed to create transport request in %s", __FUNCTION__);
        return USP_INITIALIZATION_FAILURE;
    }

    uspContext->transportRequest = transportHandle;

    TransportSetDnsCache(transportHandle, uspContext->dnsCache);
    TransportSetCallbacks(transportHandle, TransportErrorHandler, TransportRecvResponseHandler);
    // Todo: does USP require User-Agent in CogSvc?
    // TransportRequestAddRequestHeader(uspContext->transportRequest, "User-Agent", kUserAgent);

    // Hackhack: Because Carbon API does not support authentication yet, use a default subscription key if no authentication is set.
    // TODO: This should be removed after Carbon API is ready for authentication, and before public release!!!
    if (uspContext->authData == NULL)
    {
        uspContext->authType = USP_AUTHENTICATION_SUBSCRIPTION_KEY;
        uspContext->authData = STRING_construct("92069ee289b84e5594a9564ab77ed2ba");
    }

    switch (uspContext->authType)
    {
    case USP_AUTHENTICATION_SUBSCRIPTION_KEY:
        if (TransportRequestAddRequestHeader(transportHandle, "Ocp-Apim-Subscription-Key", STRING_c_str(uspContext->authData)) != 0)
        {
            LogError("%s: Set authentication using subscription key failed.", __FUNCTION__);
            return USP_INITIALIZATION_FAILURE;
        }
        break;

    case USP_AUTHENTICATION_AUTHORIZATION_TOKEN:
        {
            const char* bearerHeader = "Bearer";
            const char* tokenValue = STRING_c_str(uspContext->authData);
            UspResult ret = USP_SUCCESS;
            size_t tokenStrSize = strlen(bearerHeader) + strlen(tokenValue) + 1 /*space separator*/ + 1 /* the ending \0 */;
            char *tokenStr = malloc(tokenStrSize);
            if (tokenStr == NULL)
            {
                LogError("%s: failed to allocate memory for token.", __FUNCTION__);
                return USP_OUT_OF_MEMORY;
            }
            snprintf(tokenStr, tokenStrSize, "%s %s", bearerHeader, tokenValue);
            if (TransportRequestAddRequestHeader(transportHandle, "Authorization", tokenStr) != 0)
            {
                LogError("%s: Set authentication using authorization token failed.", __FUNCTION__);
                ret = USP_INITIALIZATION_FAILURE;
            }
            free(tokenStr);
            return ret;
        }

        break;

    default:
        LogError("%s: Unsupported authentication type %d.", __FUNCTION__, uspContext->authType);
        return USP_INITIALIZATION_FAILURE;
    }

    uspContext->transportRequestLock = Lock_Init();
    if (uspContext->transportRequestLock == NULL)
    {
        LogError("Failed to create a lock for transport request in %s", __FUNCTION__);
        return USP_INITIALIZATION_FAILURE;
    }

    return USP_SUCCESS;
}

UspResult TransportShutdown(UspContext* uspContext)
{
    USP_RETURN_IF_HANDLE_NULL(uspContext);

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
    USP_RETURN_IF_HANDLE_NULL(uspContext);

    if (uspContext->dnsCache != NULL)
    {
        DnsCacheDestroy(uspContext->dnsCache);
    }

    telemetry_uninitialize();
    PropertybagShutdown();

    STRING_delete(uspContext->language);
    STRING_delete(uspContext->outputFormat);
    STRING_delete(uspContext->modelId);
    STRING_delete(uspContext->authData);

    for (size_t i = 0; i < uspContext->userMessageHandlerSize; i++)
    {
        free(uspContext->userMessageHandlerTable[i].path);
    }
    uspContext->userMessageHandlerSize = 0;
    free(uspContext->userMessageHandlerTable);

    free(uspContext);
    return USP_SUCCESS;
}

UspResult UspContextCreate(UspContext** contextCreated)
{
    UspContext* uspContext = (UspContext*)malloc(sizeof(UspContext));
    if (uspContext == NULL)
    {
        LogError("UspInitialize failed: allocate memory for UspContext failed.");
        return USP_OUT_OF_MEMORY;
    }
    memset(uspContext, 0, sizeof(UspContext));

    PropertybagInitialize();
    telemetry_initialize();

    uspContext->creationTime = telemetry_gettime();
    uspContext->dnsCache = DnsCacheCreate();
    if (uspContext->dnsCache == NULL)
    {
        LogError("Create DNSCache failed in %s", __FUNCTION__);
        UspContextDestroy(uspContext);
        return USP_INITIALIZATION_FAILURE;
    }

    *contextCreated = uspContext;
    return USP_SUCCESS;
}

UspResult UspSetCallbacks(UspContext* uspContext, UspCallbacks *callbacks, void* callbackContext)
{
    USP_RETURN_IF_HANDLE_NULL(uspContext);
    USP_RETURN_IF_ARGUMENT_NULL(callbacks, "callbacks");

    if ((callbacks->version != USP_CALLBACK_VERSION) || (callbacks->size != sizeof(UspCallbacks)))
    {
        LogError("The callbacks passed to %s is invalid. version:%u (expected: %u), size:%u (expected: %zu).", __FUNCTION__, callbacks->version, USP_CALLBACK_VERSION, callbacks->size, sizeof(UspCallbacks));
        return USP_INVALID_ARGUMENT;
    }

    uspContext->callbacks = callbacks;
    uspContext->callbackContext = callbackContext;
    return USP_SUCCESS;
}
