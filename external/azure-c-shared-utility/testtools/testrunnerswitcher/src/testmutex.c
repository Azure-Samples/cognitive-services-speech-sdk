// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "testmutex.h"

#ifdef WIN32

#include "windows.h"

#define SEMAPHORE_HIGH_WATER 1000000

TEST_MUTEX_HANDLE testmutex_create(void)
{
    return (TEST_MUTEX_HANDLE)CreateMutexW(NULL, FALSE, NULL);
}

int testmutex_acquire(TEST_MUTEX_HANDLE mutex)
{
    return (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
}

void testmutex_destroy(TEST_MUTEX_HANDLE mutex)
{
    (void)CloseHandle(mutex);
}

int testmutex_release(TEST_MUTEX_HANDLE mutex)
{   
    return ReleaseMutex(mutex);
}

TEST_MUTEX_HANDLE testmutex_acquire_global_semaphore(void)
{
    TEST_MUTEX_HANDLE semaphore = CreateSemaphoreW(NULL, SEMAPHORE_HIGH_WATER, SEMAPHORE_HIGH_WATER, L"Global/MICROMOCK_DLL_BY_DLL");
    if (semaphore != NULL)
    {
        if (WaitForSingleObject(semaphore, INFINITE) != WAIT_OBJECT_0)
        {
            CloseHandle(semaphore);
            semaphore = NULL;
        }
    }

    return semaphore;
}

int testmutex_release_global_semaphore(TEST_MUTEX_HANDLE semaphore)
{
    LONG prev;
    (void)ReleaseSemaphore(semaphore, 1, &prev);
    int result;
    
    if (prev == SEMAPHORE_HIGH_WATER - 1)
    {
        (void)CloseHandle(semaphore);
        result = 1;
    }
    else
    {
        result = 0;
    }
    return result;
}
#endif
