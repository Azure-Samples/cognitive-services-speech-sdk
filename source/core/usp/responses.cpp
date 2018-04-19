//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// reponses.cpp: handle response messages recevied from the service.
//

#include "uspcommon.h"
#include "uspinternal.h"
#include "propbag.h"
#include "transport.h"
#include "azure_c_shared_utility_buffer_wrapper.h"
#include "azure_c_shared_utility_xlogging_wrapper.h"
#include "string_utils.h"

#include "exception.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
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