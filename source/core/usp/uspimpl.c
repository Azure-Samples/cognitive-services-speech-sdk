//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspimpl.c: implementation of the USP library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <time.h>
#include <inttypes.h>
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
//Todo: Figure out what to do about user agent build hash and version number
const char g_userAgent[] = "CortanaSDK (Windows;Win32;DeviceType=Near;SpeechClient=2.0.4)";

const char g_requestHeaderUserAgent[] = "User-Agent";
const char g_requestHeaderOcpApimSubscriptionKey[] = "Ocp-Apim-Subscription-Key";
const char g_requestHeaderAuthorization[] = "Authorization";
const char g_requestHeaderSearchDelegationRPSToken[] = "X-Search-DelegationRPSToken";

#ifdef WIN32
uint64_t g_perfCounterFrequency;
#endif

uint64_t telemetry_gettime()
{
#if defined(WIN32)
    LARGE_INTEGER tickCount;
    if (QueryPerformanceCounter(&tickCount) == 0)
    {
        LogError("tickcounter failed: QueryPerformanceCounter failed %d.", GetLastError());
        return 0;
    }
    return (uint64_t)tickCount.QuadPart;

#else
    // Todo: a time function with higher resolution
    return (uint64_t)time(NULL);

#endif
}

UspResult MessageWrite(UspHandle uspHandle, const char *path, const uint8_t *data, uint32_t size)
{
    int ret = -1;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(data, "data");

    if (path == NULL || strlen(path) == 0)
    {
        LogError("The path is null or empty.");
        return USP_INVALID_ARGUMENT;
    }

    Lock(uspHandle->uspContextLock);

    USP_RETURN_ERROR_IF_WRONG_STATE(uspHandle, USP_STATE_CONNECTED);
    ret = TransportMessageWrite(uspHandle->transportRequest, path, data, size);

    Unlock(uspHandle->uspContextLock);

    if (!ret)
    {
        return USP_SUCCESS;
    }
    else
    {
        return USP_TRANSPORT_ERROR_GENERIC;
    }
}

// Write audio stream to the service: call transport to write/flush stream
UspResult AudioStreamWrite(UspHandle uspHandle, const void * data, uint32_t size, uint32_t* bytesWritten)
{
    const char* httpArgs = "/audio";
    int ret = -1;

    LogInfo("TS:%" PRIu64 ", Write %zu bytes audio data.", USP_LIFE_TIME(uspHandle), size);
    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(data, "data");

    if (size == 0)
    {
        LogError("Size should not be 0. Use AudioStreamFlush() to flush the buffer.");
        return USP_INVALID_ARGUMENT;
    }

    metrics_audiostream_data(size);

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
        if (ret != 0)
        {
            LogError("TransportStreamPrepare failed. error=%d", ret);
            return ret;
        }
    }

    ret = TransportStreamWrite(uspHandle->transportRequest, (uint8_t*)data, size);
    if (ret != 0)
    {
        LogError("TransportStreamWrite failed. error=%d", ret);
    }

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

    LogInfo("TS:%" PRIu64 ", Flush audio buffer.", USP_LIFE_TIME(uspHandle));
    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);

    if (!uspHandle->audioOffset)
    {
        return USP_SUCCESS;
    }

    ret = TransportStreamFlush(uspHandle->transportRequest);

    uspHandle->audioOffset = 0;
    metrics_audiostream_flush();
    metrics_audio_end();

    if (ret == 0)
    {
        return USP_SUCCESS;
    }
    else
    {
        LogError("Returns failure, reason: TransportStreamFlush returns %d", ret);
        return USP_TRANSPORT_ERROR_GENERIC;
    }
}

// Callback for transport error
static void TransportErrorHandler(TransportHandle transportHandle, TransportError reason, void* context)
{
    UspResult uspError;

    (void)transportHandle;

    UspContext* uspContext = (UspContext*)context;
    LogInfo("TS:%" PRIu64 ", TransportError: uspContext:0x%x, reason=%d.", USP_LIFE_TIME(uspContext), uspContext, reason);

    assert(uspContext != NULL);
    USP_RETURN_VOID_IF_CALLBACKS_NULL(uspContext);

    if (uspContext->callbacks->OnError == NULL)
    {
        LogInfo("No callback is defined for onError.");
        return;
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
}

// Callback for SpeechStartDetected.
static UspResult SpeechStartHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size)
{
    assert(uspContext != NULL);

#ifdef _DEBUG
    LogInfo("TS:%" PRIu64 ", speech.start Message: path: %s, content type: %s, size: %zu.", USP_LIFE_TIME(uspContext), path, mime, size);
#endif

    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);
    if (uspContext->callbacks->onSpeechStartDetected == NULL)
    {
        LogInfo("No user callback is defined for callbacks->onSpeechStartDetected.");
        return USP_SUCCESS;
    }

    UspMsgSpeechStartDetected* msg = malloc(sizeof(UspMsgSpeechStartDetected));
    // Todo: deal with char to wchar
    // Todo: add more field;
    uspContext->callbacks->onSpeechStartDetected(uspContext, uspContext->callbackContext, msg);
    // Todo: better handling of memory management.
    free(msg);

    return USP_SUCCESS;
}


// Callback for speech.end
static UspResult SpeechEndHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size)
{
    assert(uspContext != NULL);

#ifdef _DEBUG
    LogInfo("TS:%" PRIu64 ", speech.end Message: path: %s, content type: %s, size: %zu.", USP_LIFE_TIME(uspContext), path, mime, size);
#endif

    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

     USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);
    if (uspContext->callbacks->onSpeechEndDetected == NULL)
    {
        LogInfo("No user callback is defined for callbacks->onSpeechEndDetected.");
        return USP_SUCCESS;
    }

    UspMsgSpeechEndDetected* msg = malloc(sizeof(UspMsgSpeechEndDetected));
    // Todo: deal with char to wchar
    // Todo: add more field;
    uspContext->callbacks->onSpeechEndDetected(uspContext, uspContext->callbackContext, msg);
    // Todo: better handling of memory management.
    free(msg);

    return USP_SUCCESS;
}

// callback for turn.end
static UspResult TurnEndHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size)
{
    assert(uspContext != NULL);

#ifdef _DEBUG
    LogInfo("TS:%" PRIu64 ", turn.end Message: path: %s, content type: %s, size: %zu.", USP_LIFE_TIME(uspContext), path, mime, size);
#endif

    (void)path;
    (void)mime;
    (void)buffer;
    (void)size;

    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);
    if (uspContext->callbacks->onTurnEnd == NULL)
    {
        LogInfo("No user callback is defined for callbacks->onTurnEnd.");
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

    telemetry_flush();

    // Todo: need to reset request_id?
    // Todo: the caller need to flush audio buffer?
    Lock(uspContext->uspContextLock);
    TransportCreateRequestId(uspContext->transportRequest);
    Unlock(uspContext->uspContextLock);

    return USP_SUCCESS;
}


// Callback handler for turn.start, speech.hypothesis, speech.phrase, and also for response.
static UspResult ContentPathHandler(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size)
{
    UspResult ret;

    assert(uspContext != NULL);

    if (size == 0)
    {
        PROTOCOL_VIOLATION("response contains no body");
        return USP_INVALID_RESPONSE;
    }

    BUFFER_HANDLE responseContentHandle = BUFFER_create(
        (unsigned char*)buffer,
        size + 1);
    if (!responseContentHandle)
    {
        LogError("BUFFER_create failed.");
        return USP_OUT_OF_MEMORY;
    }

    BUFFER_u_char(responseContentHandle)[size] = 0;

#ifdef _DEBUG
    LogInfo("TS:%" PRIu64 ", Content Message: path: %s, content type: %s, size: %zu, buffer: %s", USP_LIFE_TIME(uspContext), path, mime, size, (char *)BUFFER_u_char(responseContentHandle));
#endif
    ret = ContentDispatch((void*)uspContext, path, mime, NULL, responseContentHandle, size);

    BUFFER_delete(responseContentHandle);

    return ret;
}

bool userPathHandlerCompare(LIST_ITEM_HANDLE item1, const void* path)
{
    UserPathHandler* value1 = (UserPathHandler*)list_item_get_value(item1);
    return (strcmp(value1->path, path) == 0);
}

/**
* The dispatch table for message.
*/
typedef UspResult(*SystemMessageHandler)(UspContext* uspContext, const char* path, const char* mime, const unsigned char* buffer, size_t size);

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
    UspOnUserMessage userMsgHandler = NULL;

    (void)transportHandle;

    USP_RETURN_VOID_IF_CONTEXT_NULL(context);

    if (errorCode != 0)
    {
        LogError("Response error %d.", errorCode);
        // TODO: Lower layers need appropriate signals
        return;
    }
    else if (responseHeader == NULL)
    {
        LogError("ResponseHeader is NULL.");
        return;
    }

    path = HTTPHeaders_FindHeaderValue(responseHeader, KEYWORD_PATH);
    if (path == NULL)
    {
        PROTOCOL_VIOLATION("response missing '" KEYWORD_PATH "' header");
        return;
    }

    if (size != 0)
    {
        contentType = HTTPHeaders_FindHeaderValue(responseHeader, g_keywordContentType);
        if (contentType == NULL)
        {
            PROTOCOL_VIOLATION("response '%s' contains body with no content-type", path);
            return;
        }
    }

    UspContext *uspContext = (UspContext *)context;

    LogInfo("TS:%" PRIu64 " Response Message: path: %s, content type: %s, size: %zu.", USP_LIFE_TIME(uspContext), path, contentType, size);

    for (unsigned int i = 0; i < ARRAYSIZE(g_SystemMessageHandlers); i++)
    {
        if (!strcmp(path, g_SystemMessageHandlers[i].path))
        {
            g_SystemMessageHandlers[i].handler(uspContext, path, contentType, buffer, size);
            return;
        }
    }

    Lock(uspContext->uspContextLock);
    LIST_ITEM_HANDLE foundItem = list_find(uspContext->userPathHandlerList, userPathHandlerCompare, path);
    if (foundItem != NULL)
    {
        UserPathHandler* pathHandler = (UserPathHandler*)list_item_get_value(foundItem);
        assert(pathHandler->handler != NULL);
        assert(strcmp(pathHandler->path, path) == 0);
        LogInfo("User Message: path: %s, content type: %s, size: %zu.", path, contentType, size);
        userMsgHandler = pathHandler->handler;
    }
    Unlock(uspContext->uspContextLock);

    if (userMsgHandler != NULL)
    {
        userMsgHandler(uspContext, path, contentType, buffer, size, uspContext->callbackContext);
    }
    else
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
    if (uspContext->transportRequest != NULL)
    {
        LogError("TransportHandle has been initialized.");
        return USP_ALREADY_INITIALIZED_ERROR;
    }

    TransportHandle transportHandle = TransportRequestCreate(endpoint, uspContext);
    if (transportHandle == NULL)
    {
        LogError("Failed to create transport request.");
        return USP_INITIALIZATION_FAILURE;
    }

    uspContext->transportRequest = transportHandle;

    TransportSetDnsCache(transportHandle, uspContext->dnsCache);
    TransportSetCallbacks(transportHandle, TransportErrorHandler, TransportRecvResponseHandler);

    if (uspContext->type == USP_ENDPOINT_CDSDK)
    {
        TransportRequestAddRequestHeader(uspContext->transportRequest, g_requestHeaderUserAgent, g_userAgent);
    }
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
        if (TransportRequestAddRequestHeader(transportHandle, g_requestHeaderOcpApimSubscriptionKey, STRING_c_str(uspContext->authData)) != 0)
        {
            LogError("Failed to set authentication using subscription key.");
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
                LogError("Failed to allocate memory for token.");
                return USP_OUT_OF_MEMORY;
            }
            snprintf(tokenStr, tokenStrSize, "%s %s", bearerHeader, tokenValue);
            if (TransportRequestAddRequestHeader(transportHandle, g_requestHeaderAuthorization, tokenStr) != 0)
            {
                LogError("Failed to set authentication using authorization token.");
                ret = USP_INITIALIZATION_FAILURE;
            }
            free(tokenStr);
            return ret;
        }

    // TODO(1126805): url builder + auth interfaces
    case USP_AUTHENTICATION_SEARCH_DELEGATION_RPS_TOKEN:
        if (TransportRequestAddRequestHeader(transportHandle, g_requestHeaderSearchDelegationRPSToken, STRING_c_str(uspContext->authData)) != 0)
        {
            LogError("Failed to set authentication using Search-DelegationRPSToken.");
            return USP_INITIALIZATION_FAILURE;
        }

        break;

    default:
        LogError("Unsupported authentication type %d.", uspContext->authType);
        return USP_INITIALIZATION_FAILURE;
    }

    return USP_SUCCESS;
}

UspResult TransportShutdown(UspContext* uspContext)
{
    USP_RETURN_ERROR_IF_HANDLE_NULL(uspContext);

    if (uspContext->transportRequest != NULL)
    {
        TransportRequestDestroy(uspContext->transportRequest);
    }

    uspContext->transportRequest = NULL;

    return USP_SUCCESS;
}

UspResult UspContextDestroy(UspContext* uspContext)
{
    LIST_ITEM_HANDLE userPathHandlerItem;
    UserPathHandler* pathHandler;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspContext);

    Lock(uspContext->uspContextLock);
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
    STRING_delete(uspContext->endpointUrl);

    while ((userPathHandlerItem = list_get_head_item(uspContext->userPathHandlerList)) != NULL)
    {
        pathHandler = (UserPathHandler*)list_item_get_value(userPathHandlerItem);
        list_remove(uspContext->userPathHandlerList, userPathHandlerItem);
        free(pathHandler->path);
        free(pathHandler);
    }
    list_destroy(uspContext->userPathHandlerList);

    Unlock(uspContext->uspContextLock);
    assert(uspContext->uspContextLock != NULL);
    Lock_Deinit(uspContext->uspContextLock);

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

#ifdef WIN32
    LARGE_INTEGER perfCounterFrequency;
    if (QueryPerformanceFrequency(&perfCounterFrequency) == 0)
    {
        LogError("Get performance counter frequency failed %d.", GetLastError());
    }
    else
    {
        // Todo: There is a potential concurrency risk. 
        if (g_perfCounterFrequency == 0)
        {
            g_perfCounterFrequency = (uint64_t)perfCounterFrequency.QuadPart;
        }
        LogInfo("The performance counter frequency is %" PRIu64 ".", (uint64_t)perfCounterFrequency.QuadPart);
    }
#endif

    uspContext->uspContextLock = Lock_Init();
    if (uspContext->uspContextLock == NULL)
    {
        LogError("Failed to initialize uspContextLock.");
    }

    uspContext->userPathHandlerList = list_create();
    uspContext->creationTime = telemetry_gettime();
    uspContext->dnsCache = DnsCacheCreate();
    if (uspContext->dnsCache == NULL)
    {
        LogError("Create DNSCache failed.");
        UspContextDestroy(uspContext);
        return USP_INITIALIZATION_FAILURE;
    }

    *contextCreated = uspContext;
    return USP_SUCCESS;
}

UspResult UspSetCallbacks(UspContext* uspContext, UspCallbacks *callbacks, void* callbackContext)
{
    USP_RETURN_ERROR_IF_HANDLE_NULL(uspContext);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(callbacks, "callbacks");

    if ((callbacks->version != USP_CALLBACK_VERSION) || (callbacks->size != sizeof(UspCallbacks)))
    {
        LogError("The version or size of callbacks is invalid. version:%u (expected: %u), size:%u (expected: %zu).", callbacks->version, USP_CALLBACK_VERSION, callbacks->size, sizeof(UspCallbacks));
        return USP_INVALID_ARGUMENT;
    }

    uspContext->callbacks = callbacks;
    uspContext->callbackContext = callbackContext;
    return USP_SUCCESS;
}
