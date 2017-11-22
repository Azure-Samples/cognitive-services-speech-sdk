// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define VECTOR_create real_VECTOR_create
#define VECTOR_destroy real_VECTOR_destroy
#define VECTOR_push_back real_VECTOR_push_back
#define VECTOR_erase real_VECTOR_erase
#define VECTOR_clear real_VECTOR_clear
#define VECTOR_element real_VECTOR_element
#define VECTOR_front real_VECTOR_front
#define VECTOR_back real_VECTOR_back
#define VECTOR_find_if real_VECTOR_find_if
#define VECTOR_size real_VECTOR_size

#define GBALLOC_H

#include "vector.c"
