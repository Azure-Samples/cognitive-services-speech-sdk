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
    UspContext* uspContext = (UspContext *)malloc(sizeof(UspContext));
    SPEECH_HANDLE speechHandle;

    if (speech_open(&speechHandle, 0, NULL))
    {
        LogError("speech_open failed.");
        return USP_INITIALIZATION_FAILURE;
    }

    uspContext->speechContext = (SPEECH_CONTEXT *)speechHandle;
    if (Speech_Initialize(uspContext->speechContext))
    {
        LogError("Speech_Initialize failed.");
        return USP_INITIALIZATION_FAILURE;
    }

    (void)callbacks;
    (void)callbackContext;
    
    *handle = uspContext;
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
    UspContext* context = (UspContext *)handle;
    if (handle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

    free(context);

    return USP_SUCCESS;
}