// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKALLOC_H
#define UMOCKALLOC_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif

extern void* umockalloc_malloc(size_t size);
extern void* umockalloc_realloc(void* ptr, size_t size);
extern void umockalloc_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKALLOC_H */
