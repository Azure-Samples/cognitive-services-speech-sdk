// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdlib.h>

#include "azure_c_shared_utility/condition.h"
#include "windows.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

DEFINE_ENUM_STRINGS(COND_RESULT, COND_RESULT_VALUES);

typedef struct CONDITION_TAG
{
    volatile LONG waiting_thread_count;
    HANDLE event_handle;
}
CONDITION;

COND_HANDLE Condition_Init(void)
{
    // Codes_SRS_CONDITION_18_002: [ Condition_Init shall create and return a CONDITION_HANDLE ]

    CONDITION* cond = (CONDITION*)malloc(sizeof(CONDITION));

    // Codes_SRS_CONDITION_18_008: [ Condition_Init shall return NULL if it fails to allocate the CONDITION_HANDLE ]
    if (cond != NULL)
    {
        cond->event_handle = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (cond->event_handle == INVALID_HANDLE_VALUE)
        {
            LogError("CreateEvent failed with error %d", GetLastError());
            free(cond);
            cond = NULL;
        }
        else
        {
            /* Needed to emulate pthread_signal as we only signal the event when there are waiting threads */
            cond->waiting_thread_count = 0;
        }
    }
    else
    {
        LogError("Failed to allocate condition handle");
    }

    return (COND_HANDLE)cond;
}

COND_RESULT Condition_Post(COND_HANDLE handle)
{
    COND_RESULT result;
    if (handle == NULL)
    {
        LogError("Null argument handle passed to Condition_Post");

        // Codes_SRS_CONDITION_18_001: [ Condition_Post shall return COND_INVALID_ARG if handle is NULL ]
        result = COND_INVALID_ARG;
    }
    else
    {
        CONDITION* cond = (CONDITION*)handle;

        /* Emulate pthreads signalling, by only unblocking *one* waiting thread if there is one waiting */
        if (cond->waiting_thread_count == 0 || SetEvent(cond->event_handle))
        {
            // Codes_SRS_CONDITION_18_003: [ Condition_Post shall return COND_OK if it succcessfully posts the condition ]
            result = COND_OK;
        }
        else
        {
            LogError("Failed SetEvent call with error %d", GetLastError());

            result = COND_ERROR;
        }
    }
    return result;
}

COND_RESULT Condition_Wait(COND_HANDLE handle, LOCK_HANDLE lock, int timeout_milliseconds)
{
    COND_RESULT result;

    // Codes_SRS_CONDITION_18_004: [ Condition_Wait shall return COND_INVALID_ARG if handle is NULL ]
    // Codes_SRS_CONDITION_18_005: [ Condition_Wait shall return COND_INVALID_ARG if lock is NULL and timeout_milliseconds is 0 ]
    // Codes_SRS_CONDITION_18_006: [ Condition_Wait shall return COND_INVALID_ARG if lock is NULL and timeout_milliseconds is not 0 ]
    if (handle == NULL || lock == NULL)
    {
        result = COND_INVALID_ARG;
    }
    else if (Unlock(lock) == 0)
    {
        CONDITION* cond = (CONDITION*)handle;
        DWORD wait_result;

        /* Increment the waiting thread count, unlock the lock and wait */
        (void)InterlockedIncrement(&cond->waiting_thread_count);
        
        // Codes_SRS_CONDITION_18_013: [ Condition_Wait shall accept relative timeouts ]
        wait_result = WaitForSingleObject(cond->event_handle, timeout_milliseconds == 0 ? INFINITE : timeout_milliseconds);

        /* If we unlocked ok, it means the lock handle is valid, lock must succeed since it wraps EnterCriticalSection */
        (void)Lock(lock);

        if (wait_result != WAIT_OBJECT_0 && wait_result != WAIT_TIMEOUT)
        {
            LogError("Failed wait, wait returned with %x", wait_result);

            /* cond might be freed at this point, just return error and do not touch condition */
            result = COND_ERROR;
        }
        else
        {
            /* To handle the chance of a race condition reset the event again when there are no more waiting threads */
            if (InterlockedDecrement(&cond->waiting_thread_count) == 0)
            {
                (void)ResetEvent(cond->event_handle);
            }

            if (wait_result == WAIT_TIMEOUT)
            {
                // Codes_SRS_CONDITION_18_011: [ Condition_Wait shall return COND_TIMEOUT if the condition is NOT triggered and timeout_milliseconds is not 0 ]
                result = COND_TIMEOUT;
            }
            else
            {
                // Codes_SRS_CONDITION_18_012: [ Condition_Wait shall return COND_OK if the condition is triggered and timeout_milliseconds is not 0 ]
                result = COND_OK;
            }
        }
    }
    else
    {
        LogError("Invalid lock passed which failed to unlock");
        result = COND_ERROR;
    }
    return result;
}

void Condition_Deinit(COND_HANDLE handle)
{
    // Codes_SRS_CONDITION_18_007: [ Condition_Deinit will not fail if handle is NULL ]
    // Codes_SRS_CONDITION_18_009: [ Condition_Deinit will deallocate handle if it is not NULL 
    if (handle != NULL)
    {
        CONDITION* cond = (CONDITION*)handle;

        (void)CloseHandle(cond->event_handle);
        cond->event_handle = INVALID_HANDLE_VALUE;

        free(cond);
    }
}

