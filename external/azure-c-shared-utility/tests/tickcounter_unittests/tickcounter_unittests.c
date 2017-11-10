// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/threadapi.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "azure_c_shared_utility/tickcounter.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/gballoc.h"

#define BUSY_LOOP_TIME      1000000

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(tickcounter_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(TICK_COUNTER_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

TEST_FUNCTION(tickcounter_create_fails)
{
    ///arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .SetReturn((void*)NULL);

    ///act
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(tickHandle);
}

TEST_FUNCTION(tickcounter_create_succeed)
{
    ///arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    ///act
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();

    ///assert
    ASSERT_IS_NOT_NULL(tickHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    tickcounter_destroy(tickHandle);
}

TEST_FUNCTION(tickcounter_destroy_tick_counter_NULL_succeed)
{
    ///arrange

    ///act
    tickcounter_destroy(NULL);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(tickcounter_destroy_succeed)
{
    ///arrange
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    tickcounter_destroy(tickHandle);

    ///assert
    ASSERT_IS_NOT_NULL(tickHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(tickcounter_get_current_ms_tick_counter_NULL_fail)
{
    ///arrange
    uint64_t current_ms = 0;

    ///act
    int result = tickcounter_get_current_ms(NULL, &current_ms);

    ///assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(tickcounter_get_current_ms_current_ms_NULL_fail)
{
    ///arrange
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();
    umock_c_reset_all_calls();

    ///act
    int result = tickcounter_get_current_ms(tickHandle, NULL);

    ///assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    tickcounter_destroy(tickHandle);
}

TEST_FUNCTION(tickcounter_get_current_ms_succeed)
{
    ///arrange
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();
    umock_c_reset_all_calls();

    uint64_t current_ms = 0;

    ///act
    int result = tickcounter_get_current_ms(tickHandle, &current_ms);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// clean
    tickcounter_destroy(tickHandle);
}

//TEST_FUNCTION(tickcounter_get_current_ms_validate_tick_succeed)
//{
//    ///arrange
//    CTickCounterMocks mocks;
//    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();
//    umock_c_reset_all_calls();
//
//    uint64_t first_ms = 0;
//
//    ThreadAPI_Sleep(1250);
//
//    ///act
//    int result = tickcounter_get_current_ms(tickHandle, &first_ms);
//
//    // busy loop here
//    ThreadAPI_Sleep(1250);
//
//    uint64_t next_ms = 0;
//
//    int resultAlso = tickcounter_get_current_ms(tickHandle, &next_ms);
//
//    ///assert
//    ASSERT_ARE_EQUAL(int, 0, result);
//    ASSERT_ARE_EQUAL(int, 0, resultAlso);
//    ASSERT_IS_TRUE(first_ms > 0);
//    ASSERT_IS_TRUE(next_ms > first_ms);
//    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
//
//    /// clean
//    tickcounter_destroy(tickHandle);
//}

END_TEST_SUITE(tickcounter_unittests)
