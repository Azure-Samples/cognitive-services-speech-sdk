#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ring_buffer_t
{
    size_t capacity;
    size_t size;
    int16_t* buffer;
    size_t front;
    size_t end;

} ring_buffer_t;

int ring_buffer_new(ring_buffer_t** pRingBuffer, const int maxSize);
int ring_buffer_process(ring_buffer_t* pRingBuffer, const int16_t* pSamples, const size_t cSamples);
int ring_buffer_window(const ring_buffer_t* pRingBuffer, const size_t windowSize, int16_t* detectionWindow);
int ring_buffer_reset(ring_buffer_t* pRingBuffer);
int ring_buffer_delete(ring_buffer_t* pRingBuffer);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif