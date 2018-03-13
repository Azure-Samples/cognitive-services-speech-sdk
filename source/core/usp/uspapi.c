//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspapi.c: Wrapper of API functions exposed by the USP library.
//

#include "uspinternal.h"
#include "transport.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/condition.h"

#ifndef _MSC_VER
#define _strdup strdup
#endif

extern int platform_init(void);

// Todo: read from a configuration file.
const char g_bingSpeechHostname[] = "wss://speech.platform.bing.com/speech/recognition/%s/cognitiveservices/v1";
const char g_CRISHostname[] = "wss://%s.api.cris.ai/speech/recognition/%s/cognitiveservices/v1";
// Todo(1126805) url builder + auth interfaces
const char g_CDSDKHostName[] = "wss://speech.platform.bing.com/cortana/api/v1?environment=Home";
const char g_interactiveMode[] = "interactive";
const char g_conversationMode[] = "conversation";
const char g_dictationMode[] = "dictation";
const char g_langQueryStr[] = "language=%s";
const char g_formatQueryStr[] = "format=%s";
const char g_defaultLangValue[] = "en-us";
const char g_outputDetailedStr[] = "detailed";
const char g_outputSimpleStr[] = "simple";

inline static void UspShutdown(UspContext* uspContext)
{
    Lock(uspContext->lock);
    uspContext->callbacks = NULL;
    uspContext->callbackContext = NULL;
    (void)TransportShutdown(uspContext);
    Unlock(uspContext->lock);

    UspContextDestroy(uspContext);
}

static int UspWorker(void* ptr)
{
    UspHandle uspHandle = (UspHandle)ptr;
    assert(NULL != uspHandle);

    Lock(uspHandle->lock);
    while(uspHandle->state == USP_STATE_CONNECTED)
    {
        uspHandle->inCallback = true;
        TransportDoWork(uspHandle->transport);
        uspHandle->inCallback = false;

        COND_RESULT result = Condition_Wait(uspHandle->workEvent, uspHandle->lock, 200);
#ifdef _DEBUG
        assert(result != COND_INVALID_ARG && result != COND_ERROR);
#else
        (void)(result);
#endif
    };
    Unlock(uspHandle->lock);

    // We're done here (somebody called UspClose),
    // since we're currently on the thread that talks to the transport 
    // and invoke callbacks (both of which only happens in the while loop above),
    // it's now safe to close the transport and clean up all resources needed by USP.
    UspShutdown(uspHandle);

    return 0;
}

void SignalWork(UspHandle handle)
{
    Lock(handle->lock);
    Condition_Post(handle->workEvent);
    Unlock(handle->lock);
}

inline static UspResult UspCreateContextAndSetCallbacks(UspCallbacks *callbacks, void* callbackContext, UspContext** uspContextReturned)
{
    UspContext* uspContext;
    UspResult ret;

    USP_RETURN_ERROR_IF_ARGUMENT_NULL(callbacks, "callbacks");

    // Create UspContext
    if ((ret = UspContextCreate(&uspContext)) != USP_SUCCESS)
    {
        LogError("Failed to create UspContext. Error:0x%x.", ret);
        return ret;
    }

    assert(uspContext != NULL);

    // Set callbacks
    if ((ret = UspSetCallbacks(uspContext, callbacks, callbackContext)) != USP_SUCCESS)
    {
        LogError("Failed to set USP callbacks. Error:0x%x.", ret);
        UspContextDestroy(uspContext);
        return ret;
    }

    uspContext->state = USP_STATE_INITIALIZED;
    uspContext->inCallback = false;
    *uspContextReturned = uspContext;
    return USP_SUCCESS;
}

UspResult UspInit(UspEndpointType type, UspRecognitionMode mode, UspCallbacks *callbacks, void* callbackContext, UspHandle* uspHandle)
{
    UspContext* uspContext;
    UspResult ret;

    USP_RETURN_ERROR_IF_ARGUMENT_NULL(uspHandle, "uspHandle");
    // TODO: rewrite this with std::call_once, as soon as we migrate usp to cpp.
    static bool platform_initialized = false;
    if (!platform_initialized) 
    {
        platform_initialized = true;
        if (platform_init() != 0) {
            return USP_INITIALIZATION_FAILURE;
        }
    }

    if ((ret = UspCreateContextAndSetCallbacks(callbacks, callbackContext, &uspContext)) != USP_SUCCESS)
    {
        return ret;
    }

    uspContext->type = type;
    uspContext->mode = mode;

    *uspHandle = uspContext;
    return USP_SUCCESS;
}

UspResult UspInitByUrl(const char *endpointUrl, UspCallbacks *callbacks, void* callbackContext, UspHandle* uspHandle)
{
    UspContext* uspContext;
    UspResult ret;

    USP_RETURN_ERROR_IF_ARGUMENT_NULL(uspHandle, "uspHandle");

    if ((ret = UspCreateContextAndSetCallbacks(callbacks, callbackContext, &uspContext)) != USP_SUCCESS)
    {
        return ret;
    }

    uspContext->endpointUrl = STRING_construct(endpointUrl);

    *uspHandle = uspContext;
    return USP_SUCCESS;

}

inline static UspResult CheckStateAndUrl(UspHandle uspHandle, UspState expectedState)
{
    if (uspHandle->state != expectedState)
    {
        LogError("Error: This must be called in state %d. The current state (%d) is not allowed.", USP_STATE_INITIALIZED, uspHandle->state);
        return USP_WRONG_STATE;
    }
    else if (uspHandle->endpointUrl != NULL)
    {
        LogError("Error: It is not allowed to change configuration if the handle is initialized by UspInitByUrl().");
        return USP_NOT_SUPPORTED_OPTION;
    }
    return USP_SUCCESS;
}

UspResult UspSetLanguage(UspHandle uspHandle, const char* language)
{
    UspResult ret;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(language, "language");

    Lock(uspHandle->lock);

    if ((ret = CheckStateAndUrl(uspHandle, USP_STATE_INITIALIZED)) == USP_SUCCESS)
    {
        if (uspHandle->type == USP_ENDPOINT_CRIS)
        {
            LogError("Language option for CRIS service is not supported.");
            ret = USP_NOT_SUPPORTED_OPTION;
        }
        else
        {
            uspHandle->language = STRING_construct(language);
            ret = USP_SUCCESS;
        }
    }

    Unlock(uspHandle->lock);

    return ret;
}

UspResult UspSetOutputFormat(UspHandle uspHandle, UspOutputFormat format)
{
    UspResult ret = USP_SUCCESS;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);

    Lock(uspHandle->lock);

    if ((ret = CheckStateAndUrl(uspHandle, USP_STATE_INITIALIZED)) == USP_SUCCESS)
    {
        if (format == USP_OUTPUT_DETAILED)
        {
            uspHandle->outputFormat = STRING_construct(g_outputDetailedStr);
        }
        else if (format == USP_OUTPUT_SIMPLE)
        {
            uspHandle->outputFormat = STRING_construct(g_outputSimpleStr);
        }
        else
        {
            LogError("The output format is invalid: %d.", format);
            ret = USP_NOT_SUPPORTED_OPTION;
        }
    }

    Unlock(uspHandle->lock);

    return ret;
}

UspResult UspSetModelId(UspHandle uspHandle, const char* modelId)
{
    UspResult ret;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(modelId, "model ID");

    Lock(uspHandle->lock);

    if ((ret = CheckStateAndUrl(uspHandle, USP_STATE_INITIALIZED)) == USP_SUCCESS)
    {
        if (uspHandle->type != USP_ENDPOINT_CRIS)
        {
            LogError("USP_CONFIG_MODEL_ID is only allowed if the endpoint type is USP_ENDPOINT_CRIS");
            ret = USP_NOT_SUPPORTED_OPTION;
        }
        else
        {
            uspHandle->modelId = STRING_construct(modelId);
            ret = USP_SUCCESS;
        }
    }

    Unlock(uspHandle->lock);

    return ret;
}


UspResult UspSetAuthentication(UspHandle uspHandle, UspAuthenticationType authType, const char* authData)
{
    UspResult ret = USP_SUCCESS;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(authData, "authentication data");

    Lock(uspHandle->lock);

    if (uspHandle->state != USP_STATE_INITIALIZED)
    {
        LogError("Error: This must be called in state %d. The current state (%d) is not allowed.", USP_STATE_INITIALIZED, uspHandle->state);
        ret = USP_WRONG_STATE;
    }
    else
    {
        switch (authType)
        {
        case USP_AUTHENTICATION_SUBSCRIPTION_KEY:
        case USP_AUTHENTICATION_AUTHORIZATION_TOKEN:
        case USP_AUTHENTICATION_SEARCH_DELEGATION_RPS_TOKEN:
            uspHandle->authType = authType;
            uspHandle->authData = STRING_construct(authData);
            break;

        default:
            uspHandle->authType = USP_AUTHENTICATION_UNKNOWN;
            uspHandle->authData = NULL;
            LogError("Authentication type %d is not supported", authType);
            ret = USP_NOT_SUPPORTED_OPTION;
        }
    }

    Unlock(uspHandle->lock);

    return ret;
}


static UspResult UspBuildUrl(UspHandle uspHandle, const char** urlResult)
{
    size_t urlLength = 0;
    const char* hostnameStr = NULL;
    const char* modeStr = NULL;
    const char *langStr = NULL;
    const char *formatStr = NULL;
    char separator = '?';
    char *endpointUrl;
    char *endpointUrlFormat;

    // Get the URL length
    switch (uspHandle->type)
    {
    case USP_ENDPOINT_BING_SPEECH:
        hostnameStr = g_bingSpeechHostname;
        langStr = STRING_c_str(uspHandle->language);
        if (langStr == NULL)
        {
            langStr = g_defaultLangValue;
        }
        urlLength = strlen(hostnameStr);
        break;

    case USP_ENDPOINT_CRIS:
        if (uspHandle->modelId == NULL)
        {
            LogError("ModelId must be set before connecting to CRIS service.");
            return USP_INITIALIZATION_FAILURE;
        }
        hostnameStr = g_CRISHostname;
        urlLength = strlen(hostnameStr) + STRING_length(uspHandle->modelId);
        break;

    // TODO(1126805): url builder + auth interfaces
    case USP_ENDPOINT_CDSDK:
        hostnameStr = g_CDSDKHostName;
        langStr = STRING_c_str(uspHandle->language);
        separator = '&';
        if (langStr == NULL)
        {
            langStr = g_defaultLangValue;
        }
        urlLength = strlen(hostnameStr);
        break;

    default:
        return USP_UNKNOWN_SERVICE_ENDPOINT;
    }

    switch (uspHandle->mode)
    {
    case USP_RECO_MODE_INTERACTIVE:
        modeStr = g_interactiveMode;
        break;
    case USP_RECO_MODE_CONVERSATION:
        modeStr = g_conversationMode;
        break;
    case USP_RECO_MODE_DICTATION:
        modeStr = g_dictationMode;
        break;
    default:
        LogError("Invalid recognition mode: %d", uspHandle->mode);
        return USP_UNKNOWN_RECOGNITION_MODE;
    }
    urlLength += strlen(modeStr);

    if (langStr != NULL)
    {
        urlLength += 1 /* query separator char. */ + strlen(g_langQueryStr) + strlen(langStr);
    }

    if ((formatStr = STRING_c_str(uspHandle->outputFormat)) != NULL)
    {
        urlLength += 1 /* query separator char. */ + strlen(g_formatQueryStr) + strlen(formatStr);
    }

    urlLength += 1; // for null character at the end.

    // Allocate memory for building URL string.
    if ((endpointUrl = malloc(urlLength)) == NULL || (endpointUrlFormat = malloc(urlLength)) == NULL)
    {
        LogError("Memory allocation failed.");
        if (endpointUrl != NULL)
        {
            free(endpointUrl);
        }
        return USP_OUT_OF_MEMORY;
    }

    switch (uspHandle->type)
    {
    case USP_ENDPOINT_BING_SPEECH:
        snprintf(endpointUrl, urlLength, hostnameStr, modeStr);
        break;

    case USP_ENDPOINT_CRIS:
        snprintf(endpointUrl, urlLength, hostnameStr, STRING_c_str(uspHandle->modelId), modeStr);
        break;

    case USP_ENDPOINT_CDSDK:
        strcpy_s(endpointUrl, urlLength, hostnameStr);
        break;

    default:
        return USP_UNKNOWN_SERVICE_ENDPOINT;
    }

    if (langStr != NULL)
    {
        assert(strlen(endpointUrl) + strlen(g_langQueryStr) + strlen(langStr) + 1 < urlLength);
        snprintf(endpointUrlFormat, urlLength, "%s%c%s", endpointUrl, separator, g_langQueryStr);
        snprintf(endpointUrl, urlLength, endpointUrlFormat, langStr);
        separator = '&';
    }

    if (formatStr != NULL)
    {
        assert(strlen(endpointUrl) + strlen(g_formatQueryStr) + strlen(formatStr) + 1 < urlLength);
        snprintf(endpointUrlFormat, urlLength, "%s%c%s", endpointUrl, separator, g_formatQueryStr);
        snprintf(endpointUrl, urlLength, endpointUrlFormat, formatStr);
        separator = '&';  // this is needed if there is no langStr and we have other query parameter to append.
    }

    free(endpointUrlFormat);

    if (urlResult != NULL)
    {
        *urlResult = endpointUrl;
    }
    return USP_SUCCESS;
}

UspResult UspConnect(UspHandle uspHandle)
{
    UspResult ret = USP_RUNTIME_ERROR;
    const char* endpointUrl;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    // Callbacks must be set before initializing transport.
    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspHandle);

    Lock(uspHandle->lock);

    if (uspHandle->state != USP_STATE_INITIALIZED)
    {
        LogError("Error: UspConnect() must be called in %d state. The current state (%d) is not allowed.", USP_STATE_INITIALIZED, uspHandle->state);
        ret = USP_WRONG_STATE;
    }
    else
    {
        endpointUrl = STRING_c_str(uspHandle->endpointUrl);

        // Either use the specified URL or build URL based on service type and configurations
        if ((endpointUrl != NULL) || ((ret = UspBuildUrl(uspHandle, &endpointUrl)) == USP_SUCCESS))
        {
            LogInfo("Connect to service %s", endpointUrl);
            ret = TransportInitialize(uspHandle, endpointUrl);
        }

        if ((uspHandle->endpointUrl == NULL) && (ret == USP_SUCCESS))
        {
            free((void *)endpointUrl);  // free the string returned by BuildUrl
        }

        if (ret != USP_SUCCESS)
        {
            LogError("Initialize transport failed");
        }
        else if (ThreadAPI_Create(&uspHandle->workThreadHandle, UspWorker, uspHandle) != THREADAPI_OK)
        {
            LogError("Create work thread in USP failed.");
            TransportShutdown(uspHandle);
            ret = USP_RUNTIME_ERROR;
        }
        else
        {
            uspHandle->state = USP_STATE_CONNECTED;
        }
    }

    Unlock(uspHandle->lock);

    return ret;
}

UspResult UspClose(UspHandle uspHandle)
{
    FUNC_ENTER("");

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);

    if (uspHandle->inCallback) 
    {
        // Locks in azure-c-shared are no longer reentrant, acquiring a lock held by the 
        // current thread leads to a deadlock. What's more, joining current thread also 
        // deadlocks. Checking a flag when this is invoked in a callback on the worker thread,
        // helps to get around those both issues.
        uspHandle->state = USP_STATE_SHUTDOWN;
        return USP_SUCCESS;
    }

    Lock(uspHandle->lock);
    if (uspHandle->state == USP_STATE_SHUTDOWN) {
        Unlock(uspHandle->lock);
        return USP_SUCCESS;
    }
    uspHandle->state = USP_STATE_SHUTDOWN;
    Unlock(uspHandle->lock);

    if (uspHandle->workThreadHandle == NULL) {
        // work thread was never created, clean up the handle
        UspContextDestroy(uspHandle);
    }
    else
    {
        // If shutdown was invoked on the worker thread (in a callback), 
        // setting the state is enough to eventually terminate the 'event loop' 
        // (whenever the callback completes), so just return from here (join is a no-op).
        // Otherwise, wait until the worker thread terminates.
        LogInfo("Wait for work thread to complete");
        SignalWork(uspHandle);
        ThreadAPI_Join(uspHandle->workThreadHandle, NULL);
    }

    FUNC_RETURN("");
    return USP_SUCCESS;
}


// Pass another parameter to return the bytes have been written.
UspResult UspWriteAudio(UspHandle uspHandle, const uint8_t* buffer, size_t bytesToWrite, size_t *bytesWritten)
{
    uint32_t count = 0;
    UspResult ret = USP_WRITE_AUDIO_ERROR;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);

    Lock(uspHandle->lock);

    if (uspHandle->state != USP_STATE_CONNECTED)
    {
        LogError("Error: Sending audio data must be called in state %d. The current state (%d) is not allowed.", USP_STATE_CONNECTED, uspHandle->state);
        ret = USP_WRONG_STATE;
    }
    else
    {
        if (bytesToWrite == 0)
        {
            ret = (AudioStreamFlush(uspHandle) == 0) ? USP_SUCCESS : USP_WRITE_AUDIO_ERROR;
        }
        else
        {
            // Todo: mismatch between size_t ad byteToWrite...
            ret = (AudioStreamWrite(uspHandle, buffer, (uint32_t)bytesToWrite, &count) == 0) ? USP_SUCCESS : USP_WRITE_AUDIO_ERROR;
        }
    }

    Unlock(uspHandle->lock);

    if ((ret == USP_SUCCESS) && (bytesWritten != NULL))
    {
        *bytesWritten = count;
        SignalWork(uspHandle);
    }
    return ret;
}

UspResult UspFlushAudio(UspHandle uspHandle)
{
    return UspWriteAudio(uspHandle, NULL /* buffer */, 0 /* size */, NULL);
}

bool userPathHandlerCompare(LIST_ITEM_HANDLE item1, const void* path);

UspResult UspRegisterUserMessage(UspHandle uspHandle, const char* messagePath, UspOnUserMessage messageHandler)
{
    UspResult ret = USP_SUCCESS;

    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);

    if (messagePath == NULL || strlen(messagePath) == 0)
    {
        LogError("The messagePath is null or empty.");
        return USP_INVALID_ARGUMENT;
    }

    if (messageHandler == NULL)
    {
        LogError("The messageHandler is null.");
        return USP_INVALID_ARGUMENT;
    }

    Lock(uspHandle->lock);

    if (uspHandle->state != USP_STATE_INITIALIZED)
    {
        LogError("Error: UspRegisterUserMessage must be called in state %d. The current state (%d) is not allowed.", USP_STATE_INITIALIZED, uspHandle->state);
        ret = USP_WRONG_STATE;
    }
    else
    {
        assert(uspHandle->userPathHandlerList != NULL);
        LIST_ITEM_HANDLE foundItem = singlylinkedlist_find(uspHandle->userPathHandlerList, userPathHandlerCompare, messagePath);
        if (foundItem != NULL)
        {
            UserPathHandler* existingHandler = (UserPathHandler*)singlylinkedlist_item_get_value(foundItem);
            existingHandler->handler = messageHandler;
        }
        else
        {
            UserPathHandler* newHandler = malloc(sizeof(UserPathHandler));
            if (newHandler == NULL)
            {
                LogError("Failed to allocate userPathHandler entry.");
                ret = USP_OUT_OF_MEMORY;
            }
            else
            {
                newHandler->handler = messageHandler;
                newHandler->path = _strdup(messagePath);
                singlylinkedlist_add(uspHandle->userPathHandlerList, newHandler);
            }
        }
    }

    Unlock(uspHandle->lock);
    return ret;
}

UspResult UspSendMessage(UspHandle uspHandle, const char* messagePath, const uint8_t* buffer, size_t bytesToWrite)
{
    USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_ERROR_IF_ARGUMENT_NULL(buffer, "buffer");

    if (messagePath == NULL || strlen(messagePath) == 0)
    {
        LogError("The messagePath is null or empty.");
        return USP_INVALID_ARGUMENT;
    }

    if (MessageWrite(uspHandle, messagePath, buffer, (uint32_t)bytesToWrite))
    {
        return USP_SEND_USER_MESSAGE_ERROR;
    }

    SignalWork(uspHandle);
    return USP_SUCCESS;
}

