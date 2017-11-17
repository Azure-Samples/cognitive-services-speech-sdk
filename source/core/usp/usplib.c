//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usplib.c: the library provides the client implementation of the speech USP protocol.
//

#include "usp.h"
#include "uspinternal.h"


int UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext)
{
    UspContext* pUspContext = (UspContext *)malloc(sizeof(UspContext));
    SPEECH_HANDLE pSpeechHandle;

    if (speech_open(&pSpeechHandle, 0, NULL))
    {
        LogError("speech_open failed.");
        return USP_INITIALIZATION_FAILURE;
    }

    pUspContext->speechContext = (SPEECH_CONTEXT *)pSpeechHandle;
    if (Speech_Initialize(pUspContext->speechContext))
    {
        LogError("Speech_Initialize failed.");
        return USP_INITIALIZATION_FAILURE;
    }

    (void)pCallbacks;
    (void)pContext;
    
    *ppHandle = pUspContext;
    return USP_SUCCESS;
}

int UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    (void)handle;
    (void)buffer;
    (void)byteToWrite;

    return USP_NOT_IMPLEMENTED;
}

int UspShutdown(UspHandle handle)
{
    UspContext* pContext = (UspContext *)handle;
    if (handle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

    free(pContext);

    return USP_SUCCESS;
}