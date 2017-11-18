//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usplib.c: the library provides the client implementation of the speech USP protocol.
//

#include "usp.h"
#include "uspinternal.h"


UspResult UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext)
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
        speech_close(speechHandle);
        return USP_INITIALIZATION_FAILURE;
    }

    (void)callbacks;
    (void)callbackContext;

    *handle = uspContext;
    return USP_SUCCESS;
}

UspResult UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    UspContext* context = (UspContext *)handle;

    if (context->speechContext == NULL)
    {
        return USP_UNINTIALIZED;
    }

    if (byteToWrite == 0)
    {
        if (audiostream_flush(context->speechContext) != 0)
        {
            return USP_WRITE_ERROR;
        }
    }
    else
    {
        // Todo: mismatch between size_t ad byteToWrite...
        if (audiostream_write(context->speechContext, buffer, (uint32_t)byteToWrite, NULL))
        {
            return USP_WRITE_ERROR;
        }
    }

    return USP_SUCCESS;
}

UspResult UspShutdown(UspHandle handle)
{
    UspContext* context = (UspContext *)handle;
    if (handle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

    if (context->speechContext)
    {
        speech_close((SPEECH_HANDLE)context->speechContext);
    }

    free(context);

    return USP_SUCCESS;
}