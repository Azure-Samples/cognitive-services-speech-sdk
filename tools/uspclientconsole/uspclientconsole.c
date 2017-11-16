//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspclientconsole.c: A console application for testing USP client library.
//

#include "usp.h"

int main(int argc, char* argv[])
{
    UspHandle handle;
    void* context = NULL;
    UspCallbacks* testCallbacks = NULL;
    uint8_t pBuffer[256];

    (void)argc;
    (void)argv;

    UspInitialize(&handle, testCallbacks, context);

    UspWrite(handle, pBuffer, sizeof(pBuffer));
}
