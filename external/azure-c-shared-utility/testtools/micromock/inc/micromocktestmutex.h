// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCKTESTMUTEX_H
#define MICROMOCKTESTMUTEX_H

#ifdef WIN32
#include "windows.h"

typedef HANDLE MICROMOCK_MUTEX_HANDLE;
typedef HANDLE MICROMOCK_GLOBAL_SEMAPHORE_HANDLE;

extern "C" HANDLE MicroMockCreateMutex(void);
extern "C" int MicroMockAcquireMutex(HANDLE Mutex);
extern "C" int MicroMockReleaseMutex(HANDLE Mutex);
extern "C" void MicroMockDestroyMutex(HANDLE Mutex);

extern "C" HANDLE MicroMockCreateGlobalSemaphore(const char* name, unsigned int highWaterCount);
extern "C" int MicroMockAcquireGlobalSemaphore(HANDLE semaphore);
extern "C" int MicroMockReleaseGlobalSemaphore(HANDLE semaphore);
extern "C" void MicroMockDestroyGlobalSemaphore(HANDLE semaphore);

#else

typedef void* MICROMOCK_MUTEX_HANDLE;
typedef void* MICROMOCK_GLOBAL_SEMAPHORE_HANDLE;

#define MicroMockCreateMutex()          ((MICROMOCK_MUTEX_HANDLE)1)
#define MicroMockAcquireMutex(Mutex)    (1)
#define MicroMockReleaseMutex(Mutex)    (1)
#define MicroMockDestroyMutex(Mutex)

#define MicroMockCreateGlobalSemaphore(a, b) ((MICROMOCK_GLOBAL_SEMAPHORE_HANDLE)(1))
#define MicroMockAcquireGlobalSemaphore(semaphore) (1)
#define MicroMockReleaseGlobalSemaphore(semaphore) (1)
#define MicroMockDestroyGlobalSemaphore(semaphore)
#endif

#endif /*MICROMOCKTESTMUTEX_H*/

