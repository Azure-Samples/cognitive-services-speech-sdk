// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#define malloc mock_malloc
#define calloc mock_calloc
#define realloc mock_realloc
#define free mock_free

extern void* mock_malloc(size_t size);
extern void* mock_calloc(size_t nmemb, size_t size);
extern void* mock_realloc(void* ptr, size_t size);
extern void mock_free(void* ptr);

#undef _CRTDBG_MAP_ALLOC
#include "../src/gballoc.c"
