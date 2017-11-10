// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include "azure_c_shared_utility/gballoc.h"
#include <stdint.h>
#include <time.h>
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/xlogging.h"

#define INVALID_TIME_VALUE      (time_t)(-1)
typedef struct TICK_COUNTER_INSTANCE_TAG
{
    LARGE_INTEGER perf_freqency;
    LARGE_INTEGER last_perf_counter;
    time_t backup_time_value;
    uint64_t current_ms;
} TICK_COUNTER_INSTANCE;

TICK_COUNTER_HANDLE tickcounter_create(void)
{
    TICK_COUNTER_INSTANCE* result = (TICK_COUNTER_INSTANCE*)malloc(sizeof(TICK_COUNTER_INSTANCE));
    if (result != NULL)
    {
        if (!QueryPerformanceFrequency(&result->perf_freqency))
        {
            result->backup_time_value = time(NULL);
            if (result->backup_time_value == INVALID_TIME_VALUE)
            {
                LogError("tickcounter failed: time return INVALID_TIME.");
                free(result);
                result = NULL;
            }
            else
            {
                result->current_ms = 0;
            }
        }
        else
        {
            if (!QueryPerformanceCounter(&result->last_perf_counter))
            {
                LogError("tickcounter failed: QueryPerformanceCounter failed %d.", GetLastError());
                free(result);
                result = NULL;
            }
            else
            {
                result->backup_time_value = INVALID_TIME_VALUE;
                result->current_ms = 0;
            }
        }
    }
    return result;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    if (tick_counter != NULL)
    {
        free(tick_counter);
    }
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, uint64_t* current_ms)
{
    int result;
    if (tick_counter == NULL || current_ms == NULL)
    {
        LogError("tickcounter failed: Invalid Arguments.");
        result = __LINE__;
    }
    else
    {
        TICK_COUNTER_INSTANCE* tick_counter_instance = (TICK_COUNTER_INSTANCE*)tick_counter;
        if (tick_counter_instance->backup_time_value == INVALID_TIME_VALUE)
        {
            // If the QueryPerformanceCounter is available use this
            LARGE_INTEGER curr_perf_item;
            if (!QueryPerformanceCounter(&curr_perf_item))
            {
                LogError("tickcounter failed: QueryPerformanceCounter failed %d.", GetLastError() );
                result = __LINE__;
            }
            else
            {
                LARGE_INTEGER perf_in_ms;
                LONGLONG remainder;

                perf_in_ms.QuadPart = (curr_perf_item.QuadPart - tick_counter_instance->last_perf_counter.QuadPart) * 1000000;
                remainder = (perf_in_ms.QuadPart % tick_counter_instance->perf_freqency.QuadPart) / 1000000;
                perf_in_ms.QuadPart /= tick_counter_instance->perf_freqency.QuadPart;
                tick_counter_instance->current_ms += perf_in_ms.QuadPart;
                tick_counter_instance->last_perf_counter = curr_perf_item;
                tick_counter_instance->last_perf_counter.QuadPart -= remainder;

                *current_ms = tick_counter_instance->current_ms / 1000;
                result = 0;
            }
        }
        else
        {
            time_t time_value = time(NULL);
            if (time_value == INVALID_TIME_VALUE)
            {
                result = __LINE__;
            }
            else
            {
                tick_counter_instance->current_ms = (uint64_t)(difftime(time_value, tick_counter_instance->backup_time_value) * 1000);
                *current_ms = tick_counter_instance->current_ms;
                result = 0;
            }
        }
    }
    return result;
}
