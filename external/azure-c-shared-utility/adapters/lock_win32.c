// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/lock.h"
#include <windows.h>
#include <stdlib.h>
#include "azure_c_shared_utility/xlogging.h"

#ifdef WINCE
const char* Lock_ResultAsString[] = { "OK", "ERROR" };
#define ENUM_TO_STRING(l, result) Lock_ResultAsString[result]
#else

#include "azure_c_shared_utility/macro_utils.h"
DEFINE_ENUM_STRINGS(LOCK_RESULT, LOCK_RESULT_VALUES);

#endif

/*SRS_LOCK_99_002:[ This API on success will return a valid lock handle which should be a non NULL value]*/
LOCK_HANDLE Lock_Init(void)
{
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION) malloc(sizeof(CRITICAL_SECTION));
    if (!lpCriticalSection)
    {
        LogError("Could not allocate memory for Critical Section");
    }
    else
    {
        //In low-memory situations, InitializeCriticalSection can raise a STATUS_NO_MEMORY exception.
        InitializeCriticalSection( lpCriticalSection );
    }    
    return (LOCK_HANDLE) lpCriticalSection;
}


LOCK_RESULT Lock(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogErrorResult(LOCK_RESULT, result);
    }
    else 
    {
        EnterCriticalSection( (LPCRITICAL_SECTION) handle );
    }    
    return result;
}

LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogErrorResult(LOCK_RESULT, result);
    }
    else
    {
        LeaveCriticalSection( (LPCRITICAL_SECTION) handle );
    }
    
    return result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogErrorResult(LOCK_RESULT, result);
    }
    else
    {
        DeleteCriticalSection( (LPCRITICAL_SECTION) handle );
        free( (LPCRITICAL_SECTION) handle );
    }
    return result;
}