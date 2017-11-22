// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/vector.h"
#include <string.h>


typedef struct VECTOR_TAG
{
    void* storage;
    size_t count;
    size_t elementSize;
} VECTOR;

VECTOR_HANDLE VECTOR_create(size_t elementSize)
{
    VECTOR_HANDLE result;

    VECTOR* vec = (VECTOR*)malloc(sizeof(VECTOR));
    if (vec == NULL)
    {
        result = NULL;
    }
    else
    {
        vec->storage = NULL;
        vec->count = 0;
        vec->elementSize = elementSize;
        result = (VECTOR_HANDLE)vec;
    }
    return result;
}

static void internal_VECTOR_clear(VECTOR* vec)
{
    if (vec->storage != NULL)
    {
        free(vec->storage);
        vec->storage = NULL;
    }
    vec->count = 0;
}

void VECTOR_destroy(VECTOR_HANDLE handle)
{
    if (handle != NULL)
    {
        VECTOR* vec = (VECTOR*)handle;
        internal_VECTOR_clear(vec);
        free(vec);
    }
}

/* insertion */
int VECTOR_push_back(VECTOR_HANDLE handle, const void* elements, size_t numElements)
{
    int result;
    if (handle == NULL || elements == NULL || numElements == 0)
    {
        result = __LINE__;
    }
    else
    {
        VECTOR* vec = (VECTOR*)handle;
        const size_t curSize = vec->elementSize * vec->count;
        const size_t appendSize = vec->elementSize * numElements;

        void* temp = realloc(vec->storage, curSize + appendSize);
        if (temp == NULL)
        {
            result = __LINE__;
        }
        else
        {
            memcpy((unsigned char*)temp + curSize, elements, appendSize);
            vec->storage = temp;
            vec->count += numElements;
            result = 0;
        }
    }
    return result;
}

/* removal */
void VECTOR_erase(VECTOR_HANDLE handle, void* elements, size_t numElements)
{
    if (handle != NULL && elements != NULL && numElements > 0)
    {
        VECTOR* vec = (VECTOR*)handle;
        unsigned char* src = (unsigned char*)elements + (vec->elementSize * numElements);
        unsigned char* srcEnd = (unsigned char*)vec->storage + (vec->elementSize * vec->count);
        (void)memmove(elements, src, srcEnd - src);
        vec->count -= numElements;
        if (vec->count == 0)
        {
            free(vec->storage);
            vec->storage = NULL;
        }
        else
        {
            vec->storage = realloc(vec->storage, (vec->elementSize * vec->count));
        }
    }
}

void VECTOR_clear(VECTOR_HANDLE handle)
{
    if (handle != NULL)
    {
        VECTOR* vec = (VECTOR*)handle;
        internal_VECTOR_clear(vec);
    }
}

/* access */

void* VECTOR_element(const VECTOR_HANDLE handle, size_t index)
{
    void* result = NULL;
    if (handle != NULL)
    {
        const VECTOR* vec = (const VECTOR*)handle;
        if (index <= vec->count)
        {
            result = (unsigned char*)vec->storage + (vec->elementSize * index);
        }
    }
    return result;
}

void* VECTOR_front(const VECTOR_HANDLE handle)
{
    void* result = NULL;
    if (handle != NULL)
    {
        const VECTOR* vec = (const VECTOR*)handle;
        result = vec->storage;
    }
    return result;
}

void* VECTOR_back(const VECTOR_HANDLE handle)
{
    void* result = NULL;
    if (handle != NULL)
    {
        const VECTOR* vec = (const VECTOR*)handle;
        result = (unsigned char*)vec->storage + (vec->elementSize * (vec->count - 1));
    }
    return result;
}

void* VECTOR_find_if(const VECTOR_HANDLE handle, PREDICATE_FUNCTION pred, const void* value)
{
    void* result = NULL;
    size_t i;
    VECTOR* handleData = (VECTOR*)handle;
    if (handle != NULL && pred != NULL && value != NULL)
    {
        for (i = 0; i < handleData->count; ++i)
        {
            void* elem = (unsigned char*)handleData->storage + (handleData->elementSize * i);
            if (!!pred(elem, value))
            {
                result = elem;
                break;
            }
        }
    }
    return result;
}

/* capacity */

size_t VECTOR_size(const VECTOR_HANDLE handle)
{
    size_t result = 0;
    if (handle != NULL)
    {
        const VECTOR* vec = (const VECTOR*)handle;
        result = vec->count;
    }
    return result;
}
