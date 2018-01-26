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
const char g_CRISHostname[] = "wss://%s.api.cris.ai";
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

UspResult UspOpen(UspEndpointType type, UspRecognitionMode mode, UspCallbacks *callbacks, void* callbackContext, UspHandle* uspHandle)
{
    UspContext* uspContext;
    UspResult ret;

    if (uspHandle == NULL || callbacks == NULL)
    {
        LogError("One of the following parameters is null. uspHandle:0x%x, callbacks:0x%x.", uspHandle, callbacks);
        return USP_INVALID_PARAMETER;
    }

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

    uspContext->flags = USP_FLAG_OPENED;
    *uspHandle = uspContext;
    return USP_SUCCESS;
}

UspResult UspSetOption(UspHandle uspHandle, UspOption optionKey, const char* optionValue)
{
    if (uspHandle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

    if (optionValue == NULL)
    {
        LogError("The option value for key %d must be not null in %s.", optionKey, __FUNCTION__);
        return USP_INVALID_PARAMETER;
    }

    if (uspHandle->flags != USP_FLAG_OPENED)
    {
        LogError("Set option after connecting to service is not supported. Current state: %d", uspHandle->flags);
        return USP_OPERATION_IN_WRONG_STATE;
    }

    switch (optionKey)
    {
    case USP_OPTION_LANGUAGE:
        if (uspHandle->type == USP_ENDPOINT_CRIS)
        {
            LogError("Language option for CRIS service is not supported.");
            return USP_NOT_SUPPORTED_OPTION;
        }
        uspHandle->language = STRING_construct(optionValue);
        break;

    case USP_OPTION_OUTPUT_FORMAT:
        if (_strnicmp(g_outputDetailedStr, optionValue, strlen(g_outputDetailedStr) + 1) == 0)
        {
            uspHandle->outputFormat = STRING_construct(g_outputDetailedStr);
        }
        else if (_strnicmp(g_outputSimpleStr, optionValue, strlen(g_outputSimpleStr) + 1) == 0)
        {
            uspHandle->outputFormat = STRING_construct(g_outputSimpleStr);
        }
        else
        {
            LogError("The output format is invalid: %s.", optionValue);
            return USP_NOT_SUPPORTED_OPTION;
        }
        break;

    default:
        LogError("Option %d is not supported", optionKey);
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

    if (uspHandle == NULL)
    {
        LogError("The uspHandle is null in %s.", __FUNCTION__);
        return USP_INVALID_HANDLE;
    }

    if (uspHandle->flags != USP_FLAG_OPENED)
    {
        LogError("UspConnect() is only allowed when the uspHandle in opened state. Current state: %d", uspHandle->flags);
        return USP_NOT_OPENED;
    }

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
        break;
    case USP_ENDPOINT_CRIS:
        hostnameStr = g_CRISHostname;
        break;
    default:
        return USP_UNKNOWN_SERVICE_ENDPOINT;
    }
    urlLength = strlen(hostnameStr);

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

    snprintf(endpointUrl, urlLength, hostnameStr, modeStr);

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
        return USP_CONNECT_FAILURE;
    }
    free(endpointUrl);

    // Create work thread for processing USP messages.
    if (ThreadAPI_Create(&uspHandle->workThreadHandle, UspEventLoop, uspHandle) != THREADAPI_OK)
    {
        LogError("Create work thread in USP failed.");
        UspContextDestroy(uspHandle);
        return USP_CONNECT_FAILURE;
    }

    uspHandle->flags = USP_FLAG_CONNECTED;
    return USP_SUCCESS;
}

UspResult UspClose(UspHandle uspHandle)
{
    if (uspHandle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

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

    if (uspHandle == NULL)
    {
        return USP_INVALID_PARAMETER;
    }

    if (uspHandle->flags != USP_FLAG_CONNECTED)
    {
        LogError("UspWrite only allowed when the UspHandle is connected. Current state: %d", uspHandle->flags);
        return USP_OPERATION_IN_WRONG_STATE;
    }

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

