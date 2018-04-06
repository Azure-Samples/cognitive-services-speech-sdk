//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// reponses.cpp: handle response messages recevied from the service.
//

#include "uspcommon.h"
#include "uspinternal.h"
#include "propbag.h"
#include "uspinternal.h"
#include "transport.h"
#include "azure_c_shared_utility_buffer_wrapper.h"
#include "azure_c_shared_utility_xlogging_wrapper.h"

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
typedef UspResult (*CONTENT_HANDLER_CALLBACK)(
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
extern "C" UspResult ContentDispatch(void* context, const char* path, const char* mime, IOBUFFER* ioBuffer, BUFFER_HANDLE responseContentHandle, size_t responseSize)
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
    USP::Callbacks* uspContext = (USP::Callbacks*)deserializeContext->context;

    // Zhou: why not differentiae by "Path", but just by "DisplayText" or "Text"??
    // Todo: better handling of char to wchar
    auto textPtr = PropertybagGetStringValue(propertyHandle, "Text");
    if (textPtr != NULL)
    {
        // V2 of the speech protocol for partial results
        USP::SpeechHypothesisMsg msg;
        // Todo: add more field;
        msg.text = std::wstring(textPtr, textPtr + strlen(textPtr));
        msg.offset = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
        msg.duration = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

        uspContext->OnSpeechHypothesis(msg);
    }
    else
    {
        // Speech.Phrase message
        const char* statusPtr = PropertybagGetStringValue(propertyHandle, "RecognitionStatus");
        if (statusPtr == NULL)
        {
            LogError("Incorrect RecognitionStatus in Speech.Phrase.");
            return USP_INVALID_RESPONSE;
        }

        std::string statusStr(statusPtr);

        USP::SpeechPhraseMsg msg;
        if (statusStr == "Success")
        {
            msg.recognitionStatus = USP::RecognitionStatus::Success;
        }
        else if (statusStr == "NoMatch")
        {
            msg.recognitionStatus = USP::RecognitionStatus::NoMatch;
        }
        else if (statusStr == "InitialSilenceTimeout")
        {
            msg.recognitionStatus = USP::RecognitionStatus::InitialSilenceTimeout;
        }
        else if (statusStr == "BabbleTimeout")
        {
            msg.recognitionStatus = USP::RecognitionStatus::BabbleTimeout;
        }
        else if (statusStr == "Error")
        {
            msg.recognitionStatus = USP::RecognitionStatus::Error;
        }
        else if (statusStr == "EndOfDictation")
        {
            msg.recognitionStatus = USP::RecognitionStatus::EndOfDictation;
        }
        else
        {
            LogError("Unknown RecognitionStatus: %s", statusPtr);
            return USP_INVALID_RESPONSE;
        }

        msg.offset = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
        msg.duration = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

        textPtr = PropertybagGetStringValue(propertyHandle, "DisplayText");
        if (textPtr != NULL)
        {
            // Todo: add more field;
            msg.displayText = std::wstring(textPtr, textPtr + strlen(textPtr));
        }

        uspContext->OnSpeechPhrase(msg);
    }


    return USP_SUCCESS;
}

// handles "speech.fragment" API path
static int HandleJsonSpeechFragment(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);
    DeserializeContext* deserializeContext = (DeserializeContext*)context;
    USP::Callbacks* uspContext = (USP::Callbacks*)deserializeContext->context;

    USP::SpeechFragmentMsg msg;
    auto textPtr = PropertybagGetStringValue(propertyHandle, "Text");
    if (textPtr != NULL)
    {
        msg.text = std::wstring(textPtr, textPtr + strlen(textPtr));
    }

    msg.offset = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Offset");
    msg.duration = (USP::OffsetType)PropertybagGetNumberValue(propertyHandle, "Duration");

    uspContext->OnSpeechFragment(msg);

    return USP_SUCCESS;
}

static int HandleTurnStartContext(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    const char* serviceTag = PropertybagGetStringValue(propertyHandle, "serviceTag");
    USP::TurnStartMsg *msg = (USP::TurnStartMsg *)context;
    // Todo: add more field;
    msg->contextServiceTag = std::string(serviceTag);
    return 0;
}

// handles "turn.start" API path
static int HandleJsonTurnStart(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);
    DeserializeContext* deserializeContext = (DeserializeContext*)context;
    USP::Callbacks* uspContext = (USP::Callbacks *)(deserializeContext->context);

    USP::TurnStartMsg msg;

    (void)PropertybagGetChildValue(
        propertyHandle,
        "context",
        HandleTurnStartContext,
        &msg);

    uspContext->OnTurnStart(msg);

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

    USP::AudioStreamStartMsg msg;
    msg.ioBuffer = ioBuffer;
    USP::Callbacks* uspContext = (USP::Callbacks*)context;
    uspContext->OnAudioStreamStart(msg);

    //TODO: MSFT 1099211: Memory caused because callback not invoked
    return USP_SUCCESS;
}