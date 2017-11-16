//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usplib.c: the library provides the client implementation of the speech USP protocol.
//

#include "usp.h"

int UspInitialize(UspHandle* ppHandle, UspCallbacks *pCallbacks, void* pContext)
{
    (void)ppHandle;
    (void)pCallbacks;
    (void)pContext;

    return USP_NOT_IMPLEMENTED;
}

int UspWrite(UspHandle handle, const uint8_t* pBuffer, size_t byteToWrite)
{
    (void)handle;
    (void)pBuffer;
    (void)byteToWrite;

    return USP_NOT_IMPLEMENTED;
}