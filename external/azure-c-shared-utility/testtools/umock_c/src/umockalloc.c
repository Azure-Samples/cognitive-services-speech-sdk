// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>
#include "umockalloc.h"

void* umockalloc_malloc(size_t size)
{
    /* Codes_SRS_UMOCKALLOC_01_001: [ umockalloc_malloc shall call malloc, while passing the size argument to malloc. ] */
    /* Codes_SRS_UMOCKALLOC_01_002: [ umockalloc_malloc shall return the result of malloc. ]*/
    return malloc(size);
}

void* umockalloc_realloc(void* ptr, size_t size)
{
    /* Codes_SRS_UMOCKALLOC_01_003: [ umockalloc_realloc shall call realloc, while passing the ptr and size arguments to realloc. ] */
    /* Codes_SRS_UMOCKALLOC_01_004: [ umockalloc_realloc shall return the result of realloc. ]*/
    return realloc(ptr, size);
}

void umockalloc_free(void* ptr)
{
    /* Codes_SRS_UMOCKALLOC_01_005: [ umockalloc_free shall call free, while passing the ptr argument to free. ]*/
    free(ptr);
}
