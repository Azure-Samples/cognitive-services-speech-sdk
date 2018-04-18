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

#include "exception.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Impl;

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
    const char* path;
    void* context;
} DeserializeContext;


UspResult ToRecognitionStatus(const char* statusPtr, USP::RecognitionStatus& recognitionStatus)
{
    if (statusPtr == nullptr)
    {
        LogError("RecognitionStatus contains a null value.");
        return USP_INVALID_RESPONSE;
    }

    string statusStr(statusPtr);
    if (statusStr == "Success")
    {
       recognitionStatus = USP::RecognitionStatus::Success;
    }
    else if (statusStr == "NoMatch")
    {
        recognitionStatus = USP::RecognitionStatus::NoMatch;
    }
    else if (statusStr == "InitialSilenceTimeout")
    {
        recognitionStatus = USP::RecognitionStatus::InitialSilenceTimeout;
    }
    else if (statusStr == "BabbleTimeout")
    {
        recognitionStatus = USP::RecognitionStatus::BabbleTimeout;
    }
    else if (statusStr == "Error")
    {
        recognitionStatus = USP::RecognitionStatus::Error;
    }
    else if (statusStr == "EndOfDictation")
    {
        recognitionStatus = USP::RecognitionStatus::EndOfDictation;
    }
    else
    {
        LogInfo("Unknown RecognitionStatus: %s", statusPtr);
        recognitionStatus = USP::RecognitionStatus::Unknown;
    }
    return USP_SUCCESS;
}


UspResult ToTranslationStatus(const char* statusPtr, USP::TranslationStatus& translationStatus)
{
    string statusStr(statusPtr);

    if (statusPtr == nullptr)
    {
        LogError("TranslationStatus contains a null value.");
        return USP_INVALID_RESPONSE;
    }

    if (statusStr == "Success")
    {
        translationStatus = USP::TranslationStatus::Success;
    }
    else if (statusStr == "Error")
    {
        translationStatus = USP::TranslationStatus::Error;
    }
    else
    {
        LogInfo("Unknown TranslationStatus: %s", statusPtr);
        translationStatus = USP::TranslationStatus::Unknown;
    }
    return USP_SUCCESS;
}

std::wstring UspSetString(PROPERTYBAG_HANDLE propertyHandle, const char* fieldName)
{
    auto textStr = PropertybagGetStringValue(propertyHandle, fieldName);
    return (textStr != nullptr) ? wstring(textStr, textStr + strlen(textStr)) : wstring();
}

// handles "speech.phrase/hypothesis/fragment" API path
static int HandleJsonSpeechResult(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);
    DeserializeContext* deserializeContext = reinterpret_cast<DeserializeContext*>(context);
    USP::Callbacks* uspContext = reinterpret_cast<USP::Callbacks*>(deserializeContext->context);

    auto offset = static_cast<USP::OffsetType>(PropertybagGetNumberValue(propertyHandle, "Offset"));
    auto duration = static_cast<USP::OffsetType>(PropertybagGetNumberValue(propertyHandle, "Duration"));

    if (!strcmp(deserializeContext->path, g_messagePathSpeechHypothesis))
    {
        // Speech.hypothesis message.
        USP::SpeechHypothesisMsg msg;
        msg.text = UspSetString(propertyHandle, "Text");
        msg.offset = offset;
        msg.duration = duration;
        uspContext->OnSpeechHypothesis(msg);
    }
    else if (!strcmp(deserializeContext->path, g_messagePathSpeechFragment))
    {
        // speech.Fragment message
        USP::SpeechFragmentMsg msg;
        msg.text = UspSetString(propertyHandle, "Text");
        msg.offset = offset;
        msg.duration = duration;
        uspContext->OnSpeechFragment(msg);
    }
    else
    {
        // Speech.Phrase message
        assert(!strcmp(deserializeContext->path, g_messagePathSpeechPhrase));

        USP::SpeechPhraseMsg msg;
        msg.displayText = UspSetString(propertyHandle, "DisplayText");
        const char* statusPtr = PropertybagGetStringValue(propertyHandle, "RecognitionStatus");
        if (ToRecognitionStatus(statusPtr, msg.recognitionStatus) != USP_SUCCESS)
        {
            LogError("Invalid recognition status in speech.phrase message.");
            return USP_INVALID_RESPONSE;
        } 
        msg.offset = offset;
        msg.duration = duration;
        uspContext->OnSpeechPhrase(msg);
    }
    return USP_SUCCESS;
}

static int HandleTurnStartContext(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    const char* serviceTag = PropertybagGetStringValue(propertyHandle, "serviceTag");
    USP::TurnStartMsg *msg = (USP::TurnStartMsg *)context;
    // Todo: add more field;
    msg->contextServiceTag = string(serviceTag);
    return 0;
}

// handles "turn.start" API path
static int HandleJsonTurnStart(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);
    DeserializeContext* deserializeContext = reinterpret_cast<DeserializeContext*>(context);
    USP::Callbacks* uspContext = reinterpret_cast<USP::Callbacks *>(deserializeContext->context);

    USP::TurnStartMsg msg;

    (void)PropertybagGetChildValue(
        propertyHandle,
        "context",
        HandleTurnStartContext,
        &msg);

    uspContext->OnTurnStart(msg);

    return USP_SUCCESS;
}

static int HandleTranslationsText(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    USP::TranslationResult* result = reinterpret_cast<USP::TranslationResult *>(context);

    auto lang = UspSetString(propertyHandle, "Language");
    auto text = UspSetString(propertyHandle, "Text");

    if (lang.empty() && text.empty())
    {
        LogError("emtpy language and text field in translations text.");
        return USP_INVALID_RESPONSE;
    }
    else
    {
        result->translations[lang] = text;
    }
    return USP_SUCCESS;
}

static int HandleTranslationBlock(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    UspResult ret = USP_SUCCESS;

    USP::TranslationResult* result = reinterpret_cast<USP::TranslationResult *>(context);
    if (propertyHandle == nullptr)
    {
        LogInfo("No Translation block in the message.");
        result->translationStatus = USP::TranslationStatus::Unknown;
        result->translations.clear();
    }
    else
    {
        const char* statusPtr = PropertybagGetStringValue(propertyHandle, "TranslationStatus");
        if (ToTranslationStatus(statusPtr, result->translationStatus) != USP_SUCCESS)
        {
            LogError("Invalid translation status in translation response message.");
            result->translationStatus = USP::TranslationStatus::Unknown;
        }

        PropertybagEnumarray(propertyHandle, "Translations", HandleTranslationsText, context);

        if ((result->translationStatus == USP::TranslationStatus::Success) && (result->translations.size() == 0))
        {
            LogError("No Translations text block in the message, but TranslationStatus is succcess.");
            ret = USP_INVALID_RESPONSE;
        }
    }

    return ret;
}

// handles "Translation.Hypothesis/Phrase" API path
static int HandleJsonTranslationResponse(PROPERTYBAG_HANDLE propertyHandle, void* context)
{
    USP_RETURN_ERROR_IF_CONTEXT_NULL(context);
    DeserializeContext* deserializeContext = reinterpret_cast<DeserializeContext*>(context);
    USP::Callbacks* uspContext = reinterpret_cast<USP::Callbacks*>(deserializeContext->context);

    // Todo: refactor to share message structure.
    // handle common fields for hypothesis and phrase
    auto offset = static_cast<USP::OffsetType>(PropertybagGetNumberValue(propertyHandle, "Offset"));
    auto duration = static_cast<USP::OffsetType>(PropertybagGetNumberValue(propertyHandle, "Duration"));

    if (!strcmp(deserializeContext->path, g_messagePathTranslationHypothesis))
    {
        // Translation.hypothesis message.
        USP::TranslationHypothesisMsg msg;
        msg.text = UspSetString(propertyHandle, "Text");

        (void)PropertybagGetChildValue(
            propertyHandle,
            "Translation",
            HandleTranslationBlock,
            &msg.translation);

        msg.offset = offset;
        msg.duration = duration;
        uspContext->OnTranslationHypothesis(msg);
    }
    else
    {
        // Translation.Phrase message
        if (strcmp(deserializeContext->path, g_messagePathTranslationPhrase))
        {
            LogError("Unexpected message path: %s", deserializeContext->path);
            throw ExceptionWithCallStack("Unexpected message path.");
        }

        USP::TranslationPhraseMsg msg;
        msg.text = UspSetString(propertyHandle, "Text");
        (void)PropertybagGetChildValue(
            propertyHandle,
            "Translation",
            HandleTranslationBlock,
            &msg.translation);
        msg.offset = offset;
        msg.duration = duration;

        auto recoStatusPtr = PropertybagGetStringValue(propertyHandle, "RecognitionStatus");
        if (ToRecognitionStatus(recoStatusPtr, msg.recognitionStatus) != USP_SUCCESS)
        {
            LogError("Invalid recognition status in translation.phrase message.");
            return USP_INVALID_RESPONSE;
        }

        uspContext->OnTranslationPhrase(msg);
    }

    return USP_SUCCESS;
}

const struct JSON_CONTENT_HANDLER
{
    const char*                     path;
    PPROPERTYBAG_OBJECT_CALLBACK    handler;
} g_jsonContentHandlers[] =
{
    { g_messagePathSpeechPhrase, HandleJsonSpeechResult },
    { g_messagePathSpeechHypothesis, HandleJsonSpeechResult },
    // For testing with CRIS Dev.
    // { g_messagePathSpeechPhrase, HandleJsonTranslationResponse },
    // { g_messagePathSpeechHypothesis, HandleJsonTranslationResponse },
    { g_messagePathSpeechFragment, HandleJsonSpeechResult },
    { g_messagePathTurnStart, HandleJsonTurnStart },
    { g_messagePathTranslationHypothesis, HandleJsonTranslationResponse },
    { g_messagePathTranslationPhrase, HandleJsonTranslationResponse },
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
    deserializeContext.path = path;

    int ret = PropertybagDeserializeJson((const char*)buffer, bufferSize, ch->handler, &deserializeContext);

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