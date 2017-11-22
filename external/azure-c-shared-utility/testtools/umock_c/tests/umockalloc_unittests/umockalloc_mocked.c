// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>

extern void* mock_malloc(size_t size);
extern void* mock_calloc(size_t nmemb, size_t size);
extern void* mock_realloc(void* ptr, size_t size);
extern void mock_free(void* ptr);

#ifdef _CRTDBG_MAP_ALLOC
#define _malloc_dbg(size, ...) mock_malloc(size)
#define _calloc_dbg(nmemb, size, ...) mock_calloc(nmemb, size)
#define _realloc_dbg(ptr, size, ...) mock_realloc(ptr, size)
#define _free_dbg(ptr, ...) mock_free(ptr)
#else
#define malloc(size) mock_malloc(size)
#define calloc(nmemb, size) mock_calloc(nmemb, size)
#define realloc(ptr, size) mock_realloc(ptr, size)
#define free(ptr) mock_free(ptr)
#endif

/* include code under test */
#include "../../src/umockalloc.c"
