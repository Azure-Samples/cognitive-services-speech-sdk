// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/threadapi.h"
#include "windows.h"
#include "azure_c_shared_utility/xlogging.h"

DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;
    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
        LogErrorResult(THREADAPI_RESULT, result);
    }
    else
    {
        // Creat thread suspended. We want  to ensure that *threadHandle is assigned before func is run.
		*threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, CREATE_SUSPENDED, NULL);
		
		if(*threadHandle == NULL)
        {
            result = (GetLastError() == ERROR_OUTOFMEMORY) ? THREADAPI_NO_MEMORY : THREADAPI_ERROR;

            LogErrorResult(THREADAPI_RESULT, result);
        }
        else
        {
            result = THREADAPI_OK;
			ResumeThread(*threadHandle);
        }
    }

    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int *res)
{
    THREADAPI_RESULT result = THREADAPI_OK;
	DWORD exitCode;
    if (threadHandle == NULL)
    {
        result = THREADAPI_INVALID_ARG;
        LogErrorResult(THREADAPI_RESULT, result);
    }
    else
    {
        if (GetThreadId(GetCurrentThread()) != GetThreadId(threadHandle))
        {
            DWORD returnCode = WaitForSingleObject(threadHandle, INFINITE);

            if (returnCode != WAIT_OBJECT_0)
            {
                result = THREADAPI_ERROR;
                LogError("Error waiting for Single Object. Return Code: %d. Error Code: %d", returnCode, result);
            }
            else if (!GetExitCodeThread(threadHandle, &exitCode)) //If thread end is signaled we need to get the Thread Exit Code;
            {
                DWORD errorCode = GetLastError();
                result = THREADAPI_ERROR;
                LogError("Error Getting Exit Code. Error Code: %d.", errorCode);
            }
            else if (res != NULL)
            {
                *res = (int)exitCode;
            }
        }

        CloseHandle(threadHandle);
    }

    return result;
}

void ThreadAPI_Exit(int res)
{
    ExitThread(res);
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
}

THREADAPI_ID ThreadAPI_GetCurrentId()
{
    return GetCurrentThreadId();
}
