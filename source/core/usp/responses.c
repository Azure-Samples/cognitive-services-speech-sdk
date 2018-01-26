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
#include "uspinternal.h"

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
    CONTENT_HANDLER_CALLBACK   handler;
} g_ContentHandlers[] =
{
    // ordered by frequency of responses.
    { "application/json; charset=utf-8", JsonResponseHandler },
    { "text/plain; charset=utf-8", TextResponseHandler }
};

static void ContentDispatchAsyncComplete(void* pContext)
{
    ContentBuffer* contentBuffer = (ContentBuffer*)pContext;
    if (NULL != contentBuffer->ioBuffer)
    {
        IoBufferDelete(contentBuffer->ioBuffer);
    }

    if (NULL != contentBuffer->bufferHandle)
    {
        BUFFER_delete(contentBuffer->bufferHandle);
    }

    free(pContext);
}

UspResult ContentDispatch(void* context, const char* path, const char* mime, IOBUFFER* ioBuffer, BUFFER_HANDLE responseContentHandle, size_t responseSize)
{
    unsigned i;

    if (!mime || (!responseContentHandle && !ioBuffer)) { return (UspResult)-1; }

    if (responseSize == USE_BUFFER_SIZE)
    {
        responseSize = BUFFER_length(responseContentHandle);
    }
    else if (responseSize > BUFFER_length(responseContentHandle))
    {
        LogError("responseSize is too large");
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

    LogError("Content '%s' not handled.", mime);
    return USP_UNKNOWN_MESSAGE;
}

typedef struct _DeserializeContext
{
    const char* responseJson;
    void*        context;
} DeserializeContext;

// handles "response" API path
static int HandleJsonIntentResponse(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    // Todo: do we need to handle "response" message?
    (void)propertyHandle;
    (void)context;
    LogError("Not implemented"); 
    return USP_NOT_IMPLEMENTED;
}

// handles "speech.phrase" API path
static int HandleJsonSpeechPhrase(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    if (context == NULL)
    {
        return -1;
    }

    DeserializeContext * deserializeContext = (DeserializeContext*)context;
    UspContext* uspContext = (UspContext*)deserializeContext->context;
    if (uspContext == NULL || uspContext->callbacks == NULL)
    {
        LogError("SpeechContext is null or SpeechContext->Callback is null when handling speech phrase messages.");
        return -1;
    }

    // Zhou: why not differentiae by "Path", but just by "DisplayText" or "Text"??
    //if (pSC->mCallbacks  &&  pSC->mCallbacks->OnSpeech)
    //{

    wchar_t *wcText = NULL;
    if (uspContext->callbacks)
    {
        // Todo: better handling of char to wchar
        const char *displayText = PropertybagGetStringValue(propertyHandle, "Text");
        if (displayText != NULL)
        {
            // V2 of the speech protocol for partial results

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

            //pSC->mCallbacks->OnSpeech(pSC, pSC->mContext, displayText, SPEECH_PHRASE_STATE_PARTIAL);

        }
        else
        {
            // Speech.Phrase message
            UspMsgSpeechPhrase* msg = malloc(sizeof(UspMsgSpeechPhrase));

            const char* statusStr = PropertybagGetStringValue(propertyHandle, "RecognitionStatus");
            if (statusStr == NULL)
            {
                LogError("Incorrect RecognitionStatus in Speech.Phrase.");
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
    }

    return 0;
}

// handles "speech.fragment" API path
static int HandleJsonSpeechFragment(PROPERTYBAG_HANDLE  propertyHandle, void* context)
{
    if (context == NULL)
    {
        return -1;
    }

    DeserializeContext* deserializeContext = (DeserializeContext*)context;
    UspContext* uspContext = (UspContext*)deserializeContext->context;
    if (uspContext == NULL || uspContext->callbacks == NULL)
    {
        LogError("%s: SpeechContext is null or SpeechContext->Callback is null.", __FUNCTION__);
        return -1;
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

    return 0;
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
    if (context == NULL)
    {
        return -1;
    }

    DeserializeContext* deserializeContext = (DeserializeContext*)context;

    // USP handling
    UspContext* uspContext = (UspContext *)(deserializeContext->context);
    if (uspContext == NULL || uspContext->callbacks == NULL)
    {
        LogError("SpeechContext is null or SpeechContext->Callback is null when handling turn.start messages.");
        return -1;
    }

    if (uspContext->callbacks)
    {
        UspMsgTurnStart* msg = malloc(sizeof(UspMsgTurnStart));

        (void)PropertybagGetChildValue(
            propertyHandle,
            "context",
            HandleTurnStartContext,
            msg);

        LogInfo("Turn.Start response: serviceTag: %ls.", msg->contextServiceTag);
        uspContext->callbacks->onTurnStart(uspContext, uspContext->callbackContext, msg);

        // Todo: better handling of memory management.
        free(msg->contextServiceTag);
        free(msg);
    }

    return 0;
}

const struct JSON_CONTENT_HANDLER
{
    const char*                     path;
    PPROPERTYBAG_OBJECT_CALLBACK    handler;
} g_jsonContentHandlers[] =
{
    { g_messagePathResponse, HandleJsonIntentResponse },
    { g_messagePathSpeechPhrase, HandleJsonSpeechPhrase },
    { g_messagePathSpeechHypothesis, HandleJsonSpeechPhrase },
    { g_messagePathSpeechFragment, HandleJsonSpeechFragment },
    { g_messagePathTurnStart, HandleJsonTurnStart },
    { NULL, NULL } // terminator
};

UspResult JsonResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK callback, void* asyncContext)
{
    (void)ioBuffer;
    (void)callback;
    (void)asyncContext;

    if (!context || !buffer || !path)
    {
        return USP_INVALID_PARAMETER;
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
        return USP_UNKNOWN_MESSAGE;
    }

    DeserializeContext deserializeContext;
    deserializeContext.context = context;
    deserializeContext.responseJson = (const char*)buffer;

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

