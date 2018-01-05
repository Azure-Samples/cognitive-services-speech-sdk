//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspapi.c: Wrapper of API functions exposed by the USP library.
//

#include "uspinternal.h"

static int UspEventLoop(UspHandle handle)
{
    UspContext* uspContext = (UspContext *)handle;

    while (1)
    {
        // Todo: deal with concurrency? 
        if (uspContext->flags & USP_FLAG_SHUTDOWN)
        {
            break;
        }

        if (uspContext->flags & USP_FLAG_INITIALIZED)
        {
            UspRun(handle);
        }

        ThreadAPI_Sleep(200);
    }

    return 0;
}

UspResult UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext)
{
    // Todo: use configuration data 
    const char endpoint[] = "wss://speech.platform.bing.com/speech/recognition/interactive/cognitiveservices/v1?language=en-us";
    UspContext* uspContext;
    UspResult ret;

    if (handle == NULL)
    {
        return USP_INVALID_PARAMETER;
    }

    // Create UspContext
    if ((ret = UspContextCreate(&uspContext, endpoint)) != USP_SUCCESS)
    {
        return ret;
    }
    if (uspContext == NULL)
    {
        LogError("uspContext should not be null");
        return USP_RUNTIME_ERROR;
    }

    // Set callbacks
    if ((ret = UspSetCallbacks(uspContext, callbacks, callbackContext)) != USP_SUCCESS)
    {
        UspContextDestroy(uspContext);
        return ret;
    }

    // Create work thread for processing USP messages.
    if (ThreadAPI_Create(&uspContext->workThreadHandle, UspEventLoop, uspContext) != THREADAPI_OK)
    {
        LogError("Create work thread in USP failed.");
        UspContextDestroy(uspContext);
        return USP_INITIALIZATION_FAILURE;
    }

    uspContext->flags = USP_FLAG_INITIALIZED;
    *handle = (UspHandle)uspContext;
    return USP_SUCCESS;
}



// Todo: UspClose is better
UspResult UspShutdown(UspHandle handle)
{
    UspContext* uspContext = (UspContext *)handle;
    if (handle == NULL)
    {
        return USP_INVALID_HANDLE;
    }

    uspContext->flags |= USP_FLAG_SHUTDOWN;

    if (uspContext->workThreadHandle != NULL)
    {
        LogInfo("Wait for work thread to complete");
        ThreadAPI_Join(uspContext->workThreadHandle, NULL);
    }

    UspContextDestroy(uspContext);

    return USP_SUCCESS;
}


// Pass another parameter to return the bytes have been written.
UspResult UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    UspContext* context = (UspContext *)handle;

    if (context == NULL)
    {
        return USP_UNINTIALIZED;
    }

    if (byteToWrite == 0)
    {
        if (AudioStreamFlush(context) != 0)
        {
            return USP_WRITE_ERROR;
        }
    }
    else
    {
        // Todo: mismatch between size_t ad byteToWrite...
        if (AudioStreamWrite(context, buffer, (uint32_t)byteToWrite, NULL))
        {
            return USP_WRITE_ERROR;
        }
    }

    return USP_SUCCESS;
}



// Todo: Hide it into a work thread.
void UspRun(UspHandle handle)
{
    UspContext* uspContext = (UspContext *)handle;

    Lock(uspContext->transportRequestLock);

    TransportDoWork(uspContext->transportRequest);

    Unlock(uspContext->transportRequestLock);

}

