// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CONSTBUFFER_H
#define CONSTBUFFER_H

#include "azure_c_shared_utility/buffer_.h"

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

/*this is the handle*/
typedef struct CONSTBUFFER_HANDLE_DATA_TAG* CONSTBUFFER_HANDLE;

/*this is what is returned when the content of the buffer needs access*/
typedef struct CONSTBUFFER_TAG
{
    const unsigned char* buffer;
    size_t size;
} CONSTBUFFER;

/*this creates a new constbuffer from a memory area*/
MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_Create, const unsigned char*, source, size_t, size);

/*this creates a new constbuffer from an existing BUFFER_HANDLE*/
MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_CreateFromBuffer, BUFFER_HANDLE, buffer);

MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, CONSTBUFFER_Clone, CONSTBUFFER_HANDLE, constbufferHandle);

MOCKABLE_FUNCTION(, const CONSTBUFFER*, CONSTBUFFER_GetContent, CONSTBUFFER_HANDLE, constbufferHandle);

MOCKABLE_FUNCTION(, void, CONSTBUFFER_Destroy, CONSTBUFFER_HANDLE, constbufferHandle);

#ifdef __cplusplus
}
#endif

#endif  /* CONSTBUFFER_H */
