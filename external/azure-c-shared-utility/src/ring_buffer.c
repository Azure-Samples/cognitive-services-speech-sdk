// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "azure_c_shared_utility/ring_buffer.h"

#define MIN(A,B) ((A) < (B)) ? (A) : (B)

int ring_buffer_new(ring_buffer_t** ppRingBuffer, const int capacity)
{
    ring_buffer_t* pRingBuffer = NULL;

    pRingBuffer = (ring_buffer_t*) malloc (sizeof(ring_buffer_t));
    if (pRingBuffer == NULL)
    {
        return -1;
    }

    pRingBuffer->buffer     = (int16_t*) malloc (sizeof(int16_t) * capacity);
    if (pRingBuffer->buffer == NULL)
    {
        free(pRingBuffer);
        return -1;
    }

    pRingBuffer->capacity   = capacity;
    pRingBuffer->front      = 0;
    pRingBuffer->end        = 0;
    pRingBuffer->size       = 0;

    *ppRingBuffer = pRingBuffer;

    return 0;
}

int ring_buffer_reset(ring_buffer_t* pRingBuffer)
{

    memset(pRingBuffer->buffer, 0, pRingBuffer->capacity);
    pRingBuffer->size  = 0;
    pRingBuffer->front = 0;
    pRingBuffer->end   = 0;

    return 0;
}

int ring_buffer_delete(ring_buffer_t* pRingBuffer)
{
    free(pRingBuffer->buffer);
    free(pRingBuffer);

    return 0;
}

int ring_buffer_process(ring_buffer_t* pRingBuffer, const int16_t* pSamples, const size_t cSamples)
{
    if (cSamples > pRingBuffer->capacity)
    {
        return -1;
    }

    // we can't fit all the samples
    // drop oldest samples that don't fit from queue
    if (pRingBuffer->size + cSamples > pRingBuffer->capacity)
    {
        size_t cExtraNeeded = (pRingBuffer->size + cSamples - pRingBuffer->capacity);
        pRingBuffer->front += cExtraNeeded;
        pRingBuffer->front %= pRingBuffer->capacity;
        pRingBuffer->size  -= cExtraNeeded; 
    }

    size_t cElemsLeft = cSamples;
    size_t cElemsDone = 0;
    size_t curSample  = pRingBuffer->end;

    while(cElemsLeft > 0)
    {
        // grab contiguous chunk size and consume
        size_t chunkSize = MIN(pRingBuffer->capacity - curSample, cElemsLeft);
        memcpy(&(pRingBuffer->buffer[curSample]), &pSamples[cElemsDone], chunkSize * sizeof(int16_t));
        cElemsDone += chunkSize;
        cElemsLeft -= chunkSize;

        curSample  += chunkSize;
        if (curSample >= pRingBuffer->capacity)
        {
            curSample = 0;
        }
    }

    pRingBuffer->end   = curSample;
    pRingBuffer->size += cSamples;

    return 0;
}

int ring_buffer_window(const ring_buffer_t* pRingBuffer, const size_t windowSize, int16_t* windowBuffer)
{
    // copy over all samples from start of keyword to end of buffer
    if (windowSize <= 0 || windowSize > pRingBuffer->capacity)
    {
        return -1;
    }

    memset(windowBuffer, 0, windowSize * sizeof(int16_t));

    int32_t startSample = (int32_t)pRingBuffer->end - (int32_t)windowSize;
    if (startSample < 0 )
    {
        startSample += (int32_t)pRingBuffer->capacity;
    }

    size_t cElemsLeft = windowSize;
    size_t cElemsDone = 0;
    size_t curSample  = startSample;

    while (cElemsLeft > 0)
    {
        size_t chunkSize = MIN(pRingBuffer->capacity - curSample, cElemsLeft);
        memcpy(&windowBuffer[cElemsDone], &(pRingBuffer->buffer[curSample]), chunkSize * sizeof(int16_t));
        cElemsDone += chunkSize;
        cElemsLeft -= chunkSize;

        curSample  += chunkSize;
        if (curSample >= pRingBuffer->capacity)
        {
            curSample = 0;
        }
    }

    return 0;
}
