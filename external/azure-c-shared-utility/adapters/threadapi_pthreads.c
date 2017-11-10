// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define _DEFAULT_SOURCE

#include "azure_c_shared_utility/threadapi.h"

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#ifdef TI_RTOS
#include <ti/sysbios/knl/Task.h>
#else
#include <unistd.h>
#endif

#ifdef __linux__
#ifndef __ANDROID__
#include <sys/syscall.h>
#else
#include <unistd.h>
#endif
#endif

#include <pthread.h>
#include <time.h>
#include "azure_c_shared_utility/xlogging.h"

DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

typedef struct THREAD_INSTANCE_TAG
{
    pthread_t Pthread_handle;
    THREAD_START_FUNC ThreadStartFunc;
    void* Arg;
} THREAD_INSTANCE;

static void* ThreadWrapper(void* threadInstanceArg)
{
    THREAD_INSTANCE* threadInstance = (THREAD_INSTANCE*)threadInstanceArg;
    int result = threadInstance->ThreadStartFunc(threadInstance->Arg);
    return (void*)(intptr_t)result;
}

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;

    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
    }
    else
    {
        THREAD_INSTANCE* threadInstance = malloc(sizeof(THREAD_INSTANCE));
        if (threadInstance == NULL)
        {
            result = THREADAPI_NO_MEMORY;
        }
        else
        {
            threadInstance->ThreadStartFunc = func;
            threadInstance->Arg = arg;

            // set the thread handle here so the caller has a reference to it.
            *threadHandle = threadInstance;
            int createResult = pthread_create(&threadInstance->Pthread_handle, NULL, ThreadWrapper, threadInstance);
            if (0 == createResult)
            {
                return THREADAPI_OK;
            }

            // we failed, set the handle back to NULL.
            *threadHandle = NULL;
            free(threadInstance);
            result = (createResult == EAGAIN) ? THREADAPI_NO_MEMORY : THREADAPI_ERROR;
        }
    }

    LogErrorResult(THREADAPI_RESULT, result);

#ifdef THREADAPI_ABORT_ON_CREATE_FAILURE
    if (result == THREADAPI_NO_MEMORY)
    {
        // A memory leak may cause a thread creation failure.  To help diagnose
        // these issues and reset state, crash the process.
        abort();
    }
#endif

    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int* res)
{
    THREADAPI_RESULT result;
    int ret;
    void* threadResult;

    THREAD_INSTANCE* threadInstance = (THREAD_INSTANCE*)threadHandle;
    if (threadInstance == NULL)
    {
        result = THREADAPI_INVALID_ARG;
    }
    else
    {
        if (threadInstance->Pthread_handle == pthread_self())
        {
            ret = pthread_detach(threadInstance->Pthread_handle);
        }
        else
        {
            ret = pthread_join(threadInstance->Pthread_handle, &threadResult);
        }

        free(threadInstance);

        if (ret != 0)
        {
            result = THREADAPI_ERROR;
        }
        else
        {
            if (res != NULL)
            {
                *res = (int)(intptr_t)threadResult;
            }

            return THREADAPI_OK;
        }
    }

    LogErrorResult(THREADAPI_RESULT, result);

    return result;
}

void ThreadAPI_Exit(int res)
{
    pthread_exit((void*)(intptr_t)res);
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
#ifdef TI_RTOS
    Task_sleep(milliseconds);
#else
    time_t seconds = milliseconds / 1000;
    long nsRemainder = (milliseconds % 1000) * 1000000;
    struct timespec timeToSleep = { seconds, nsRemainder };
    (void)nanosleep(&timeToSleep, NULL);
#endif
}

#ifdef __linux__
// gettid() is linux-only, not POSIX.
THREADAPI_ID ThreadAPI_GetCurrentId()
{
#ifndef __ANDROID__
    return syscall(SYS_gettid);
#else
    return gettid();
#endif
}
#endif
