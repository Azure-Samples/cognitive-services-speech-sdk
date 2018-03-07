//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// reponses.c: handle response messages recevied from the service.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "propbag.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "uspinternal.h"
#include "iobuffer.h"
#include "transport.h"

/**
* Defines the callback function of asynchronous complete during content handling.
* @param context The context provided by the application.
*/
typedef void(*CONTENT_ASYNCCOMPLETE_CALLBACK)(void* context);

/**
* Defines the callback function of handling contents.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
typedef UspResult(*CONTENT_HANDLER_CALLBACK)(
    void* context,
    const char* path,
    uint8_t* buffer,
    size_t bufferSize,
    IOBUFFER* ioBuffer,
    CONTENT_ASYNCCOMPLETE_CALLBACK asyncCompleteCallback,
    void* asyncCompleteContext);

UspResult AudioResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext);
UspResult JsonResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext);
UspResult TextResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext);

/**
* ContentBuffer manages the message buffers.
*/
typedef struct _ContentBuffer
{
    BUFFER_HANDLE bufferHandle;
    IOBUFFER* ioBuffer;
} ContentBuffer;

const char* g_keywordContentType = "Content-Type";

struct CONTENT_HANDLER
{
    const char* mime;
    CONTENT_HANDLER_CALLBACK handler;
} g_ContentHandlers[] =
{
    // ordered by frequency of responses
    // to-do: This is not necessarily true anymore now that mutliple endpoints use the same contentHandler
    { "audio/SILK", AudioResponseHandler },
    { "audio/x-wav", AudioResponseHandler},
    { "application/json; charset=utf-8", JsonResponseHandler },
    { "text/plain; charset=utf-8", TextResponseHandler }
};

static void ContentDispatchAsyncComplete(void* pContext)
{
    ContentBuffer* contentBuffer = (ContentBuffer*)pContext;
    if (contentBuffer->ioBuffer != NULL)
    {
        IoBufferDelete(contentBuffer->ioBuffer);
    }

    if (contentBuffer->bufferHandle != NULL)
    {
        BUFFER_delete(contentBuffer->bufferHandle);
    }

    free(pContext);
}

// Handle WS responses based on content type, also handle HTTP responses, and responses for a stream (see transport.c)
UspResult ContentDispatch(void* context, const char* path, const char* mime, IOBUFFER* ioBuffer, BUFFER_HANDLE responseContentHandle, size_t responseSize)
{
    unsigned i;

    if ((mime == NULL) || ((responseContentHandle == NULL) && (ioBuffer == NULL)))
    {
        return USP_INVALID_ARGUMENT;
    }

    if (responseSize == USE_BUFFER_SIZE)
    {
        responseSize = BUFFER_length(responseContentHandle);
    }
    else if (responseSize > BUFFER_length(responseContentHandle))
    {
        LogError("responseSize is too large.");
        return USP_BUFFER_TOO_SMALL;
    }

    ContentBuffer* contentBuffer = (ContentBuffer*)malloc(sizeof(ContentBuffer));
    if (!contentBuffer)
    {
        return USP_OUT_OF_MEMORY;
    }

    contentBuffer->ioBuffer = ioBuffer;
    contentBuffer->bufferHandle = responseContentHandle;
    for (i = 0; i < ARRAYSIZE(g_ContentHandlers); i++)
    {
        if (!strcmp(g_ContentHandlers[i].mime, mime))
        {
            return g_ContentHandlers[i].handler(context, path, BUFFER_u_char(responseContentHandle), responseSize, ioBuffer, ContentDispatchAsyncComplete, contentBuffer);
        }
    }

    LogError("No handler defined for Content '%s'.", mime);
    return USP_UNKNOWN_PATH_IN_RESPONSE;
}

typedef struct _DeserializeContext
{
    const char* responseJson;
    void* context;
} DeserializeContext;

// handles "speech.phrase" API path
static int HandleJsonSpeechPhrase(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);

    DeserializeContext* deserializeContext = (DeserializeContext*)context;

    UspContext* uspContext = (UspContext*)deserializeContext->context;
    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);

    // Zhou: why not differentiae by "Path", but just by "DisplayText" or "Text"??

    wchar_t *wcText = NULL;
    // Todo: better handling of char to wchar
    const char *displayText = PropertybagGetStringValue(propertyHandle, "Text");
    if (displayText != NULL)
    {
        // V2 of the speech protocol for partial results
        if (uspContext->callbacks->onSpeechHypothesis == NULL)
        {
            LogInfo("No user callback defined for callbacks->onSpeechHypothesis.");
            return USP_SUCCESS;
        }

        // Todo: better handling of char to wchar
        size_t textLen = strlen(displayText) + 1;
        wcText = malloc(textLen * sizeof(wchar_t));
        mbstowcs(wcText, displayText, textLen);

        UspMsgSpeechHypothesis* msg = malloc(sizeof(UspMsgSpeechHypothesis));
        // Todo: deal with char to wchar
        // Todo: add more field;
        msg->text = wcText;
        msg->offset = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
        msg->duration = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

        uspContext->callbacks->onSpeechHypothesis(uspContext, uspContext->callbackContext, msg);
        // Todo: better handling of memory management.
        free(msg);
        free(wcText);
    }
    else
    {
        // Speech.Phrase message
        if (uspContext->callbacks->onSpeechPhrase == NULL)
        {
            LogInfo("No user callback defined for callbacks->onSpeechPhrase.");
            return USP_SUCCESS;
        }

        UspMsgSpeechPhrase* msg = malloc(sizeof(UspMsgSpeechPhrase));

        const char* statusStr = PropertybagGetStringValue(propertyHandle, "RecognitionStatus");
        if (statusStr == NULL)
        {
            LogError("Incorrect RecognitionStatus in Speech.Phrase.");
            return USP_INVALID_RESPONSE;
        }

        if (!strcmp(statusStr, "Success"))
        {
            msg->recognitionStatus = USP_RECOGNITON_SUCCESS;
        }
        else if (!strcmp(statusStr, "NoMatch"))
        {
            msg->recognitionStatus = USP_RECOGNITION_NO_MATCH;
        }
        else if (!strcmp(statusStr, "InitialSilenceTimeout"))
        {
            msg->recognitionStatus = USP_RECOGNITION_INITIAL_SILENCE_TIMEOUT;
        }
        else if (!strcmp(statusStr, "BabbleTimeout"))
        {
            msg->recognitionStatus = USP_RECOGNITION_BABBLE_TIMEOUT;
        }
        else if (!strcmp(statusStr, "Error"))
        {
            msg->recognitionStatus = USP_RECOGNITION_ERROR;
        }
        else if (!strcmp(statusStr, "EndOfDictation"))
        {
            msg->recognitionStatus = USP_RECOGNITION_END_OF_DICTATION;
        }
        else
        {
            LogError("Unknown RecognitionStatus: %s", statusStr);
            return USP_INVALID_RESPONSE;
        }

        msg->offset = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
        msg->duration = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

        displayText = PropertybagGetStringValue(propertyHandle, "DisplayText");
        if (displayText != NULL)
        {
            size_t textLen = strlen(displayText) + 1;
            wcText = malloc(textLen * sizeof(wchar_t));
            mbstowcs(wcText, displayText, textLen);

            // Todo: add more field;
            msg->displayText = wcText;
        }
        else
        {
            msg->displayText = NULL;
        }

        uspContext->callbacks->onSpeechPhrase(uspContext, uspContext->callbackContext, msg);

        // Todo: better handling of memory management.
        free(msg);
        if (displayText != NULL)
        {
            free(wcText);
        }
    }


    return USP_SUCCESS;
}

// handles "speech.fragment" API path
static int HandleJsonSpeechFragment(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);

    DeserializeContext* deserializeContext = (DeserializeContext*)context;

    UspContext* uspContext = (UspContext*)deserializeContext->context;
    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);

    if (uspContext->callbacks->onSpeechFragment == NULL)
    {
        LogInfo("No user callback defined for callbacks->onSpeechFragment.");
        return USP_SUCCESS;
    }

    UspMsgSpeechFragment* msg = malloc(sizeof(UspMsgSpeechFragment));

    wchar_t* wcText = NULL;
    const char* text = PropertybagGetStringValue(propertyHandle, "Text");
    if (text != NULL)
    {
        // Todo: better handling of char to wchar
        size_t textLen = strlen(text) + 1;
        wcText = malloc(textLen * sizeof(wchar_t));
        mbstowcs(wcText, text, textLen);
        msg->text = wcText;
    }

    msg->offset = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
    msg->duration = (UspOffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

    uspContext->callbacks->onSpeechFragment(uspContext, uspContext->callbackContext, msg);

    free(msg);
    if (wcText != NULL)
    {
        free(wcText);
    }

    return USP_SUCCESS;
}

static int HandleTurnStartContext(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    const char* serviceTag = PropertybagGetStringValue(propertyHandle, "serviceTag");
    UspMsgTurnStart *msg = (UspMsgTurnStart *)context;

    // Todo: better handling of char to wchar
    size_t textLen = strlen(serviceTag) + 1;
    wchar_t *wcText = malloc(textLen * sizeof(wchar_t));
    mbstowcs(wcText, serviceTag, textLen);

    // Todo: add more field;
    msg->contextServiceTag = wcText;
    return 0;
}

// handles "turn.start" API path
static int HandleJsonTurnStart(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);

    DeserializeContext* deserializeContext = (DeserializeContext*)context;

    // USP handling
    UspContext* uspContext = (UspContext *)(deserializeContext->context);
    USP_RETURN_ERROR_IF_CALLBACKS_NULL(uspContext);

    if (uspContext->callbacks->onTurnStart == NULL)
    {
        LogInfo("No user callback defined for callbacks->onTurnStart.");
        return USP_SUCCESS;
    }

    UspMsgTurnStart* msg = malloc(sizeof(UspMsgTurnStart));

    (void)PropertybagGetChildValue(
        propertyHandle,
        "context",
        HandleTurnStartContext,
        msg);

    uspContext->callbacks->onTurnStart(uspContext, uspContext->callbackContext, msg);

    // Todo: better handling of memory management.
    free(msg->contextServiceTag);
    free(msg);

    return USP_SUCCESS;
}

const struct JSON_CONTENT_HANDLER
{
    const char*                     path;
    PPROPERTYBAG_OBJECT_CALLBACK    handler;
} g_jsonContentHandlers[] =
{
    { g_messagePathSpeechPhrase, HandleJsonSpeechPhrase },
    { g_messagePathSpeechHypothesis, HandleJsonSpeechPhrase },
    { g_messagePathSpeechFragment, HandleJsonSpeechFragment },
    { g_messagePathTurnStart, HandleJsonTurnStart },
    { NULL, NULL } // terminator
};

/**
* Handles JSON responses.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
UspResult JsonResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext)
{
    (void)ioBuffer;
    (void)callback;
    (void)asyncContext;

    if (context == NULL || buffer == NULL || path == NULL)
    {
        LogError("One of the following parameters is null: uspContext:0x%x, buffer:0x%x, path:0x%x.", context, buffer, path);
        return USP_INVALID_ARGUMENT;
    }

    const struct JSON_CONTENT_HANDLER *ch;
    for (ch = g_jsonContentHandlers; ch->handler != NULL; ++ch)
    {
        if (strcmp(ch->path, path) == 0)
        {
            break;
        }
    }

    if (ch->handler == NULL)
    {
        LogError("The path \"%s\" has no registered JSON handler.  The request was ignored.", path);
        return USP_UNKNOWN_PATH_IN_RESPONSE;
    }

    DeserializeContext deserializeContext;
    deserializeContext.context = (void *)context;
    deserializeContext.responseJson = (const char*)buffer;

    int ret = PropertybagDeserializeJson((const char*)buffer, bufferSize, ch->handler, &deserializeContext);

    //TODO: MSFT 1099211: Memory caused because callback not invoked
    
    if (ret)
    {
        LogError("Propertybagdeserialize_json failed, path=%s, ret=%d", path, ret);
        return USP_RUNTIME_ERROR;
    }
    else
    {
        return USP_SUCCESS;
    }
}

/**
* Handles text responses.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
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

    //TODO: MSFT 1099211: Memory caused because callback not invoked
    USP_RETURN_NOT_IMPLEMENTED();
}

/**
* Handles audio responses (e.g., TTS scenarios).
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
UspResult AudioResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext)
{
    (void)path;
    (void)buffer;
    (void)bufferSize;
    (void)callback;
    (void)asyncContext;

    UspMsgAudioStreamStart *msg;
    UspContext* uspContext = (UspContext*)context;

    if (uspContext->callbacks->onAudioStreamStart == NULL)
    {
        LogInfo("No user callback defined for callbacks->onAudioStreamStart.");
        return USP_SUCCESS;
    }

    msg = (UspMsgAudioStreamStart*)malloc(sizeof(UspMsgAudioStreamStart));
    if (msg == NULL)
    {
        return USP_OUT_OF_MEMORY;
    }

    msg->ioBuffer = ioBuffer;

    uspContext->callbacks->onAudioStreamStart(uspContext, uspContext->callbackContext, msg);

    free(msg);

    //TODO: MSFT 1099211: Memory caused because callback not invoked
    return USP_SUCCESS;
}

