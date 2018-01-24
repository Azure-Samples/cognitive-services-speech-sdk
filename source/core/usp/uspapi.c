//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspapi.c: Wrapper of API functions exposed by the USP library.
//

#include "uspinternal.h"
#include "azure_c_shared_utility/crt_abstractions.h"

// Todo: read from a configuration file.
const char g_bingSpeechHostname[] = "wss://speech.platform.bing.com/speech/recognition/%s/cognitiveservices/v1";
const char g_CRISHostname[] = "wss://%s.api.cris.ai/speech/recognition/%s/cognitiveservices/v1";
const char g_interactiveMode[] = "interactive";
const char g_conversationMode[] = "conversation";
const char g_dictationMode[] = "dictation";
const char g_langQueryStr[] = "language=%s";
const char g_formatQueryStr[] = "format=%s";
const char g_defaultLangValue[] = "en-us";
const char g_outputDetailedStr[] = "detailed";
const char g_outputSimpleStr[] = "simple";

static int UspEventLoop(void* ptr)
{
    UspHandle uspHandle = (UspHandle)ptr;
    while (1)
    {
        // Todo: deal with concurrency? 
        if (uspHandle->flags == USP_FLAG_SHUTDOWN)
        {
            break;
        }

        if (uspHandle->flags == USP_FLAG_CONNECTED)
        {
            UspRun(uspHandle);
        }

        ThreadAPI_Sleep(200);
    }

    return 0;
}

UspResult UspInit(UspEndpointType type, UspRecognitionMode mode, UspCallbacks *callbacks, void* callbackContext, UspHandle* uspHandle)
{
    UspContext* uspContext;
    UspResult ret;

    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_ARGUMENT_NULL(callbacks, "callbacks");

    // Create UspContext
    if ((ret = UspContextCreate(&uspContext)) != USP_SUCCESS)
    {
        LogError("Failed to create UspContext in %s with error code:0x%x.", __FUNCTION__, ret);
        return ret;
    }

    assert(uspContext != NULL);

    uspContext->type = type;
    uspContext->mode = mode;

    // Set callbacks
    if ((ret = UspSetCallbacks(uspContext, callbacks, callbackContext)) != USP_SUCCESS)
    {
        LogError("Failed to set USP callbacks in %s with error code:0x%x.", __FUNCTION__, ret);
        UspContextDestroy(uspContext);
        return ret;
    }

    uspContext->flags = USP_FLAG_INITIALIZED;
    *uspHandle = uspContext;
    return USP_SUCCESS;
}

UspResult UspSetLanguage(UspHandle uspHandle, const char* language)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_ARGUMENT_NULL(language, "language");
    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_INITIALIZED);

    if (uspHandle->type == USP_ENDPOINT_CRIS)
    {
        LogError("Language option for CRIS service is not supported.");
        return USP_NOT_SUPPORTED_OPTION;
    }

    uspHandle->language = STRING_construct(language);
    return USP_SUCCESS;
}

UspResult UspSetOutputFormat(UspHandle uspHandle, UspOutputFormat format)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_INITIALIZED);

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
        return USP_NOT_SUPPORTED_OPTION;
    }

    return USP_SUCCESS;
}

UspResult UspSetModelId(UspHandle uspHandle, const char* modelId)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_ARGUMENT_NULL(modelId, "model ID");
    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_INITIALIZED);

    if (uspHandle->type != USP_ENDPOINT_CRIS)
    {
        LogError("USP_CONFIG_MODEL_ID is only allowed if the endpoint type is USP_ENDPOINT_CRIS");
        return USP_NOT_SUPPORTED_OPTION;
    }
    uspHandle->modelId = STRING_construct(modelId);

    return USP_SUCCESS;
}


UspResult UspSetAuthentication(UspHandle uspHandle, UspAuthenticationType authType, const char* authData)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_ARGUMENT_NULL(authData, "authentication data");

    switch (authType)
    {
    case USP_AUTHENTICATION_SUBSCRIPTION_KEY:
    case USP_AUTHENTICATION_AUTHORIZATION_TOKEN:
        uspHandle->authType = authType;
        uspHandle->authData = STRING_construct(authData);
        break;

    default:
        LogError("Authentication type %d is not supported", authType);
        return USP_NOT_SUPPORTED_OPTION;
    }

    return USP_SUCCESS;
}


// Todo: Currently we assume that UspLib API is singe-threaded. If it needs to be accessed by multiple concurrent threads,
// We need to make it thread-safe.
UspResult UspConnect(UspHandle uspHandle)
{
    size_t urlLength = 0;
    const char* hostnameStr = NULL;
    const char* modeStr = NULL;
    const char *langStr = NULL;
    const char *formatStr = NULL;
    char separator = '?';

    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_INITIALIZED);

    // Callbacks must be set before initializing transport.
    if (uspHandle->callbacks == NULL)
    {
        LogError("%s: The callbacks for uspHandle is null.", __FUNCTION__);
        return USP_CALLBACKS_NOT_SET;
    }

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
        urlLength += 1 /* query separator char. */ +  strlen(g_langQueryStr) + strlen(langStr);
    }

    if ((formatStr = STRING_c_str(uspHandle->outputFormat)) != NULL)
    {
        urlLength += 1 /* query separator char. */ + strlen(g_formatQueryStr) + strlen(formatStr);
    }

    char *endpointUrl;
    char *endpointUrlFormat;

    urlLength += 1; // for null character at the end.
    if ((endpointUrl = malloc(urlLength)) == NULL || (endpointUrlFormat = malloc(urlLength)) == NULL)
    {
        LogError("Memory allocation failed in %s:%s.", __FUNCTION__, __LINE__);
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

    LogInfo("Connect to service endpoint %s.", endpointUrl);
    if (TransportInitialize(uspHandle, endpointUrl) != USP_SUCCESS)
    {
        LogError("Initialize transport failed in %s", __FUNCTION__);
        free(endpointUrl);
        UspContextDestroy(uspHandle);
        return USP_INITIALIZATION_FAILURE;
    }
    free(endpointUrl);

    // Create work thread for processing USP messages.
    if (ThreadAPI_Create(&uspHandle->workThreadHandle, UspEventLoop, uspHandle) != THREADAPI_OK)
    {
        LogError("Create work thread in USP failed.");
        UspContextDestroy(uspHandle);
        return USP_INITIALIZATION_FAILURE;
    }

    uspHandle->flags = USP_FLAG_CONNECTED;
    return USP_SUCCESS;
}

UspResult UspClose(UspHandle uspHandle)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);

    uspHandle->flags = USP_FLAG_SHUTDOWN;

    uspHandle->callbacks = NULL;
    uspHandle->callbackContext = NULL;
    (void)TransportShutdown(uspHandle);

    if (uspHandle->workThreadHandle != NULL)
    {
        LogInfo("Wait for work thread to complete");
        ThreadAPI_Join(uspHandle->workThreadHandle, NULL);
    }

    UspContextDestroy(uspHandle);

    return USP_SUCCESS;
}


// Pass another parameter to return the bytes have been written.
UspResult UspWrite(UspHandle uspHandle, const uint8_t* buffer, size_t bytesToWrite, size_t *bytesWritten)
{
    uint32_t count = 0;

    USP_RETURN_IF_HANDLE_NULL(uspHandle);
    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_CONNECTED);

    if (bytesToWrite == 0)
    {
        if (AudioStreamFlush(uspHandle) != 0)
        {
            return USP_WRITE_ERROR;
        }
    }
    else
    {
        // Todo: mismatch between size_t ad byteToWrite...
        if (AudioStreamWrite(uspHandle, buffer, (uint32_t)bytesToWrite, &count))
        {
            return USP_WRITE_ERROR;
        }
    }

    if (bytesWritten != NULL)
    {
        *bytesWritten = count;
    }

    return USP_SUCCESS;
}

// Todo: Hide it into a work thread.
void UspRun(UspHandle uspHandle)
{
    Lock(uspHandle->transportRequestLock);

    TransportDoWork(uspHandle->transportRequest);

    Unlock(uspHandle->transportRequestLock);

}

UspResult UspRegisterUserMessage(UspHandle uspHandle, const char* messagePath, UspOnUserMessage messageHandler)
{
    USP_RETURN_IF_HANDLE_NULL(uspHandle);

    if (messagePath == NULL || strlen(messagePath) == 0)
    {
        LogError("%s: The messagePath is null or empty.", __FUNCTION__);
        return USP_INVALID_ARGUMENT;
    }

    if (messageHandler == NULL)
    {
        LogError("%s: the callback is null.", __FUNCTION__);
        return USP_INVALID_ARGUMENT;
    }

    USP_RETURN_IF_WRONG_STATE(uspHandle, USP_FLAG_INITIALIZED);

    if (uspHandle->userMessageHandlerTable == NULL)
    {
        uspHandle->userMessageHandlerTable = (UserPathHandler *)malloc(sizeof(UserPathHandler) * USER_PATH_HANDLER_ENTRIES);
        if (uspHandle->userMessageHandlerTable == NULL)
        {
            LogError("Failed to allocate userMessageHandlerTable in %s.", __FUNCTION__);
            return USP_OUT_OF_MEMORY;
        }
        uspHandle->userMessageHandlerTableCapacilty = USER_PATH_HANDLER_ENTRIES;
    }

    for (size_t i = 0; i < uspHandle->userMessageHandlerSize; i++)
    {
        if (!strcmp(messagePath, uspHandle->userMessageHandlerTable[i].path))
        {
            assert(uspHandle->userMessageHandlerTable[i].handler != NULL);
            LogInfo("Replace existing user Message: path: %s.", messagePath);
            free(uspHandle->userMessageHandlerTable[i].path);
            uspHandle->userMessageHandlerTable[i].path = _strdup(messagePath);
            uspHandle->userMessageHandlerTable[i].handler = messageHandler;
            return USP_SUCCESS;
        }
    }

    if (uspHandle->userMessageHandlerSize >= uspHandle->userMessageHandlerTableCapacilty)
    {
        uspHandle->userMessageHandlerTable = (UserPathHandler *)realloc(uspHandle->userMessageHandlerTable, sizeof(UserPathHandler) * (uspHandle->userMessageHandlerSize + USER_PATH_HANDLER_ENTRIES));
        if (uspHandle->userMessageHandlerTable == NULL)
        {
            LogError("Failed to increase userMessageHandlerTable in %s.", __FUNCTION__);
            return USP_OUT_OF_MEMORY;
        }
        uspHandle->userMessageHandlerTableCapacilty += USER_PATH_HANDLER_ENTRIES;
    }

    assert(uspHandle->userMessageHandlerSize < uspHandle->userMessageHandlerTableCapacilty);

    uspHandle->userMessageHandlerTable[uspHandle->userMessageHandlerSize].path = _strdup(messagePath);
    uspHandle->userMessageHandlerTable[uspHandle->userMessageHandlerSize].handler = messageHandler;
    uspHandle->userMessageHandlerSize++;

    return USP_SUCCESS;
}

