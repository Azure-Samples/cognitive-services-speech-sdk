// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdint.h>
#include <string.h> // for memset

//#define TRACK_USAGE

#if defined(TRACK_USAGE)
#include <stdio.h>
#include "azure_c_shared_utility/lock.h"
#endif

typedef struct ALLOCATION_TAG
{
#if defined(TRACK_USAGE)
    struct ALLOCATION_TAG* pPrev;
    struct ALLOCATION_TAG* pNext;
    const char*file;
    int line;
#endif
    size_t size;
} ALLOCATION;

static size_t totalSize = 0;
static size_t maxSize = 0;
static size_t totalallocs = 0;
static size_t totalfrees = 0;
#if defined(TRACK_USAGE)
static ALLOCATION* pHead = NULL;
static LOCK_HANDLE lock = NULL;
#endif

static void Track(ALLOCATION *result)
{
#if defined(TRACK_USAGE)
    if (!lock)
    {
        lock = Lock_Init();
    }

    Lock(lock);
    result->pNext = pHead;
    result->pPrev = NULL;
    if (pHead)
    {
        pHead->pPrev = result;
    }
    pHead = result;
    Unlock(lock);
#endif
    totalSize += result->size;
    if (maxSize < totalSize)
    {
        maxSize = totalSize;
    }
}

static void Untrack(ALLOCATION *result)
{
    totalSize -= result->size;
#if defined(TRACK_USAGE)
    Lock(lock);
    if (result->pPrev)
    {
        result->pPrev->pNext = result->pNext;
    }
    if (result->pNext)
    {
        result->pNext->pPrev = result->pPrev;
    }
    if (pHead == result)
    {
        pHead = result->pNext;
    }
    Unlock(lock);
#endif
}

int gballoc_init(void)
{
    return 0;
}

void gballoc_deinit(void)
{
}

void* gballoc_malloc(
    size_t size
#if defined(TRACK_USAGE)
    , const char*file, 
    int line
#endif
)
{
    ALLOCATION* result;

    totalallocs++;

    result = malloc(sizeof(ALLOCATION) + size);
#if defined(TRACK_USAGE)
    result->file = file;
    result->line = line;
#endif
    result->size = size;
    Track(result);

    return &result[1];
}

void* gballoc_calloc(size_t nmemb, size_t size
#if defined(TRACK_USAGE)
    , const char*file, int line
#endif
)
{
    void* result;

    totalallocs++;
    result = gballoc_malloc(nmemb * size
#if defined(TRACK_USAGE)
        , file, line
#endif
    );
    if (result)
    {
        memset(result, 0, nmemb * size);
    }

    return result;
}

void* gballoc_realloc(void* ptr, size_t size
#if defined(TRACK_USAGE)
    , const char*file, int line
#endif
)
{
    ALLOCATION* allocation = NULL;

    if (!ptr)
    {
        return gballoc_malloc(size
#if defined(TRACK_USAGE)
            , file, line
#endif
        );
    }

    totalallocs++;
    allocation = (ALLOCATION*)ptr;
    allocation--;

    Untrack(allocation);

    allocation = realloc(allocation, sizeof(ALLOCATION) + size);
    allocation->size = size;
#if defined(TRACK_USAGE)
    allocation->file = file;
    allocation->line = line;
#endif
    Track(allocation);

    return &allocation[1];
}

void gballoc_free(void* ptr)
{
    totalfrees++;
    ALLOCATION* curr = (ALLOCATION*)ptr;
    if (curr)
    {
        curr--;
        Untrack(curr);
    }

    free(curr);
}

size_t gballoc_getMaximumMemoryUsed(void)
{
    return maxSize;
}

size_t gballoc_getCurrentMemoryUsed(void)
{
    return totalSize;
}

size_t gballoc_getCurrentAllocations(void)
{
#if defined(TRACK_USAGE)
    Lock(lock);
    for (struct ALLOCATION_TAG *p = pHead; p!=NULL; p = p->pNext)
    {
        printf("%p %ld\t%s (%d)\n", p, p->size, p->file, p->line);
    }
    pHead = NULL;
    Unlock(lock);
#endif
    return totalallocs;
}

size_t gballoc_getCurrentFrees(void)
{
    return totalfrees;
}
