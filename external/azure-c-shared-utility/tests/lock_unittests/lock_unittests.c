// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/lock.h"

TEST_DEFINE_ENUM_TYPE(LOCK_RESULT, LOCK_RESULT_VALUES);

LOCK_RESULT Lock_Handle_ToString(LOCK_HANDLE handle)
{
    LOCK_RESULT result = LOCK_OK;

    if (handle == NULL)
    {
        result = LOCK_ERROR;
    }

    return result;
}

static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(Lock_UnitTests)

TEST_SUITE_INITIALIZE(a)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
}
TEST_SUITE_CLEANUP(b)
{
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION(Test_Lock_Lock_Unlock)
{
    //arrange
    LOCK_HANDLE handle=NULL;
    LOCK_RESULT result;
    //act
    handle =Lock_Init();
    LOCK_RESULT res = Lock_Handle_ToString(handle);
    //assert
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, res);

    result=Lock(handle);
    /*Tests_SRS_LOCK_99_005:[ This API on success should return LOCK_OK]*/
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, result);
    /*Tests_SRS_LOCK_99_009:[ This API on success should return LOCK_OK]*/
    result = Unlock(handle);
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, result);
    //free
    result = Lock_Deinit(handle);
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, result);
}

TEST_FUNCTION(Test_Lock_Init_DeInit)
{
    //arrange
    LOCK_HANDLE handle = NULL;
    //act
    handle = Lock_Init();
    //assert
    /*Tests_SRS_LOCK_99_002:[ This API on success will return a valid lock handle which should be a non NULL value]*/
    LOCK_RESULT res = Lock_Handle_ToString(handle);
    //assert
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, res);
    //free
    LOCK_RESULT result = Lock_Deinit(handle);
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_OK, result);
}

/*Tests_SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
TEST_FUNCTION(Test_Lock_Lock_NULL)
{
    //arrange
    //act
    LOCK_RESULT result =Lock(NULL);
    /*Tests_SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_ERROR, result);
}

/*Tests_SRS_LOCK_99_011:[ This API on NULL handle passed returns LOCK_ERROR]*/
TEST_FUNCTION(Test_Lock_Unlock_NULL)
{
    //arrange
    //act
    LOCK_RESULT result = Unlock(NULL);
    /*Tests_SRS_LOCK_99_011:[ This API on NULL handle passed returns LOCK_ERROR]*/
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_ERROR, result);
}

TEST_FUNCTION(Test_Lock_Init_DeInit_NULL)
{
    //arrange
    //act
    LOCK_RESULT result = Lock_Deinit(NULL);
    //assert
    /*Tests_SRS_LOCK_99_013:[ This API on NULL handle passed returns LOCK_ERROR]*/
    ASSERT_ARE_EQUAL(LOCK_RESULT, LOCK_ERROR, result);
}


END_TEST_SUITE(Lock_UnitTests);
