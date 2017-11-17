//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usplib.c: the library provides the client implementation of the speech USP protocol.
//

#include "usp.h"

int UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext)
{
    (void)handle;
    (void)callbacks;
    (void)callbackContext;

    return USP_NOT_IMPLEMENTED;
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
    (void)handle;

    return USP_NOT_IMPLEMENTED;
}