// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "rtos.h"

DEFINE_ENUM_STRINGS(LOCK_RESULT, LOCK_RESULT_VALUES);

/*Tests_SRS_LOCK_99_002:[ This API on success will return a valid lock handle which should be a non NULL value]*/
LOCK_HANDLE Lock_Init(void)
{
    Mutex* lock_mtx = new Mutex();
  
    return (LOCK_HANDLE)lock_mtx;
}


LOCK_RESULT Lock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /*Tests_SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        Mutex* lock_mtx = (Mutex*)handle;
        if (lock_mtx->lock() == osOK)
        {
            /*Tests_SRS_LOCK_99_005:[ This API on success should return LOCK_OK]*/
            result = LOCK_OK;
        }
        else
        {
            /*Tests_SRS_LOCK_99_006:[ This API on error should return LOCK_ERROR]*/
            result = LOCK_ERROR;
            LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
        }
    }
    return result;
}
LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /*Tests_SRS_LOCK_99_011:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        Mutex* lock_mtx = (Mutex*)handle;
        if (lock_mtx->unlock() == osOK)
        {
            /*Tests_SRS_LOCK_99_009:[ This API on success should return LOCK_OK]*/
            result = LOCK_OK;
        }
        else
        {
            /*Tests_SRS_LOCK_99_010:[ This API on error should return LOCK_ERROR]*/
            result = LOCK_ERROR;
            LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
        }
    }
    return result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result=LOCK_OK ;
    if (NULL == handle)
    {
        /*Tests_SRS_LOCK_99_013:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        /*Tests_SRS_LOCK_99_012:[ This API frees the memory pointed by handle]*/
        Mutex* lock_mtx = (Mutex*)handle;
        delete lock_mtx;
    }
    
    return result;
}
