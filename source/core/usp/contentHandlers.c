// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "private-iot-cortana-sdk.h"

const char* kContent_Type   = "Content-Type";
const char  kContent_Json[] = "application/json; charset=utf-8";

struct CONTENT_HANDLER
{
    const char*                 pszMime;
    PCONTENT_HANDLER_CALLBACK   Handler;
} gContentHandlers[] =
{
	// ordered by frequency of responses.
    { "audio/SILK",                 AsyncAudioDecode },
    { "text/plain; charset=utf-8",  Text_ResponseHandler },
	{ kContent_Json,                Json_ResponseHandler },
};

static void Content_Dispatch_AsyncComplete(void* pContext)
{
    ContentBuffers* pContentBuffers = (ContentBuffers*)pContext;
    if (NULL != pContentBuffers->pIoBuffer)
    {
        IOBUFFER_DELETE(pContentBuffers->pIoBuffer);
    }

    if (NULL != pContentBuffers->hBufferHandler)
    {
        BUFFER_delete(pContentBuffers->hBufferHandler);
    }

    free(pContext);
}

SPEECH_RESULT
Content_Dispatch(
    void*           pContext,
    const char*     pszPath,
    const char*     pszMime,
    IOBUFFER*       pIoBuffer,
    BUFFER_HANDLE   hResponseContent,
    size_t          responseSize)
{
    unsigned i;

    if (!pszMime || (!hResponseContent && !pIoBuffer)) { return -1; }

    if (responseSize == USE_BUFFER_SIZE)
    {
        responseSize = BUFFER_length(hResponseContent);
    }
    else if (responseSize > BUFFER_length(hResponseContent))
    {
        LogError("responseSize is too large");
        return -1;
    }

    ContentBuffers* pContentBuffers = (ContentBuffers*)malloc(sizeof(ContentBuffers));
    if (!pContentBuffers)
    {
        return -1;
    }

    pContentBuffers->pIoBuffer = pIoBuffer;
    pContentBuffers->hBufferHandler = hResponseContent;
    for (i = 0; i < sizeof(gContentHandlers) / sizeof(gContentHandlers[0]); i++)
    {
        if (!strcmp(gContentHandlers[i].pszMime, pszMime))
        {
            return gContentHandlers[i].Handler(pContext, pszPath, BUFFER_u_char(hResponseContent), responseSize, pIoBuffer, Content_Dispatch_AsyncComplete, pContentBuffers);
        }
    }

    LogError("Content '%s' not handled.", pszMime);
    return -1;
}

