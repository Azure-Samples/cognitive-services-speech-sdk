// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "stdafx.h"
#include "micromocktestmutex.h"

#ifdef WIN32

#include "windows.h"

HANDLE MicroMockCreateMutex(void)
{
    return CreateMutexW(NULL, FALSE, NULL);
}

HANDLE MicroMockCreateGlobalSemaphore(const char* name, unsigned int highWaterCount)
{
    // we need to convert name to unicode to pass it to the OS APIs
    wchar_t namew[_MAX_PATH + 1];
    HANDLE temp;
    if (MultiByteToWideChar(CP_UTF8, 0, name, -1, namew, _MAX_PATH + 1) == 0)
    {
        temp = NULL;
    }
    else
    {
        temp = CreateSemaphoreW(NULL, highWaterCount, highWaterCount, namew);
    }
    return temp;
}

int MicroMockAcquireMutex(HANDLE Mutex)
{
    int temp = (WaitForSingleObject((Mutex), INFINITE) == WAIT_OBJECT_0);
    return temp;
}

int MicroMockAcquireGlobalSemaphore(HANDLE semaphore)
{
    int temp = (WaitForSingleObject(semaphore, INFINITE) == WAIT_OBJECT_0);
    return temp;
}

int MicroMockReleaseGlobalSemaphore(HANDLE semaphore)
{
    LONG prev;
    (void)ReleaseSemaphore(semaphore, 1, &prev);
    return prev;
}

void MicroMockDestroyGlobalSemaphore(HANDLE semaphore)
{
    (void)CloseHandle(semaphore);
}

void MicroMockDestroyMutex(HANDLE Mutex)
{
    (void)CloseHandle(Mutex);
}

int MicroMockReleaseMutex(HANDLE Mutex)
{   
    return ReleaseMutex(Mutex);
}

#else

typedef void* MICROMOCK_MUTEX_HANDLE;

#define MicroMockCreateMutex()              ((MICROMOCK_MUTEX_HANDLE)1)
#define MicroMockCreateGlobalMutex(x)       ((MICROMOCK_GLOBAL_MUTEX_HANDLE)2)
#define MicroMockDestroyMutex(Mutex)
#define MicroMockAcquireMutex(Mutex)    (1)
#define MicroMockReleaseMutex(Mutex)    (1)

#endif
