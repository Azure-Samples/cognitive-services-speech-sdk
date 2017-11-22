// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umockcallrecorder.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include "umock_c.h"

static UMOCKCALL_HANDLE test_expected_call = (UMOCKCALL_HANDLE)0x4242;
static UMOCKCALL_HANDLE test_actual_call = (UMOCKCALL_HANDLE)0x4243;
static UMOCKCALLRECORDER_HANDLE test_call_recorder = (UMOCKCALLRECORDER_HANDLE)0x4244;

static size_t umocktypes_deinit_call_count;
static size_t umockcallrecorder_destroy_call_count;
static size_t umockcallrecorder_reset_all_calls_call_count;
static size_t umockcallrecorder_get_actual_calls_call_count;
static size_t umockcallrecorder_get_expected_calls_call_count;
static size_t umockcallrecorder_clone_call_count;
static size_t umock_c_get_last_expected_call_call_count;
static size_t umock_c_add_expected_call_call_count;
static size_t umock_c_add_actual_call_call_count;

UMOCKCALLRECORDER_HANDLE umockcallrecorder_create(void)
{
    return NULL;
}

void umockcallrecorder_destroy(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umockcallrecorder_destroy_call_count++;
}

int umockcallrecorder_reset_all_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umockcallrecorder_reset_all_calls_call_count++;
    return 0;
}

int umockcallrecorder_add_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call)
{
    (void)umock_call_recorder, mock_call;
    umock_c_add_expected_call_call_count++;
    return 0;
}

int umockcallrecorder_add_actual_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call)
{
    (void)umock_call_recorder, mock_call;
    umock_c_add_actual_call_call_count++;
    return 0;
}

UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umockcallrecorder_clone_call_count++;
    return NULL;
}

const char* umockcallrecorder_get_actual_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umockcallrecorder_get_actual_calls_call_count++;
    return NULL;
}

const char* umockcallrecorder_get_expected_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umockcallrecorder_get_expected_calls_call_count++;
    return NULL;
}

UMOCKCALL_HANDLE umockcallrecorder_get_last_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    (void)umock_call_recorder;
    umock_c_get_last_expected_call_call_count++;
    return NULL;
}

int umocktypes_init(void)
{
    return 0;
}

void umocktypes_deinit(void)
{
    umocktypes_deinit_call_count++;
}

int umocktypes_c_register_types(void)
{
    return 0;
}

void* umockalloc_malloc(size_t size)
{
    return malloc(size);
}

void umockalloc_free(void* ptr)
{
    free(ptr);
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

BEGIN_TEST_SUITE(umock_c_without_init_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    TEST_INITIALIZE_MEMORY_DEBUG(global_mutex);

    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    TEST_MUTEX_DESTROY(test_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(global_mutex);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    int mutex_acquire_result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, mutex_acquire_result);

    umocktypes_deinit_call_count = 0;
    umockcallrecorder_destroy_call_count = 0;
    umockcallrecorder_reset_all_calls_call_count = 0;
    umockcallrecorder_get_actual_calls_call_count = 0;
    umockcallrecorder_get_expected_calls_call_count = 0;
    umock_c_get_last_expected_call_call_count = 0;
    umock_c_add_expected_call_call_count = 0;
    umock_c_add_actual_call_call_count = 0;
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    TEST_MUTEX_RELEASE(test_mutex);
}

/* umock_c_deinit */

/* Tests_SRS_UMOCK_C_01_010: [ If the module is not initialized, umock_c_deinit shall do nothing. ] */
TEST_FUNCTION(umock_c_deinit_when_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umocktypes_deinit_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* umock_c_reset_all_calls */

/* Tests_SRS_UMOCK_C_01_012: [ If the module is not initialized, umock_c_reset_all_calls shall do nothing. ]*/
TEST_FUNCTION(umock_c_reset_all_calls_when_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_reset_all_calls();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_reset_all_calls_call_count);
}

/* umock_c_get_actual_calls */

/* Tests_SRS_UMOCK_C_01_014: [ If the module is not initialized, umock_c_get_actual_calls shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_actual_calls_when_not_initialized_does_nothing)
{
    // arrange

    // act
    const char* result = umock_c_get_actual_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_actual_calls_call_count);
}

/* umock_c_get_expected_calls */

/* Tests_SRS_UMOCK_C_01_016: [ If the module is not initialized, umock_c_get_expected_calls shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_expected_calls_when_not_initialized_does_nothing)
{
    // arrange

    // act
    const char* result = umock_c_get_expected_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_expected_calls_call_count);
}

/* umock_c_get_last_expected_call */

/* Tests_SRS_UMOCK_C_01_018: [ If the module is not initialized, umock_c_get_last_expected_call shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_last_expected_call_when_not_initialized_does_nothing)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umock_c_get_last_expected_call();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_get_last_expected_call_call_count);
}

/* umock_c_add_expected_call */

/* Tests_SRS_UMOCK_C_01_020: [ If the module is not initialized, umock_c_add_expected_call shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_add_expected_call_when_not_initialized_does_nothing)
{
    // arrange

    // act
    int result = umock_c_add_expected_call(test_expected_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_add_expected_call_call_count);
}

/* umock_c_add_actual_call */

/* Tests_SRS_UMOCK_C_01_022: [ If the module is not initialized, umock_c_add_actual_call shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_add_actual_call_when_not_initialized_does_nothing)
{
    // arrange
    UMOCKCALL_HANDLE matched_call;

    // act
    int result = umock_c_add_actual_call(test_actual_call, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_add_actual_call_call_count);
}

/* umock_c_get_call_recorder */

/* Tests_SRS_UMOCK_C_01_027: [ If the module is not initialized, umock_c_get_call_recorder shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_call_recorder_when_not_initialized_fails)
{
    // arrange

    // act
    UMOCKCALLRECORDER_HANDLE result = umock_c_get_call_recorder();

    // assert
    ASSERT_IS_NULL(result);
}

/* umock_c_set_call_recorder */

/* Tests_SRS_UMOCK_C_01_033: [ If the module is not initialized, umock_c_set_call_recorder shall return a non-zero value. ]*/
TEST_FUNCTION(when_the_module_is_not_initialize_umock_c_set_call_recorder_fails)
{
    // arrange

    // act
    int result = umock_c_set_call_recorder(test_call_recorder);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

END_TEST_SUITE(umock_c_without_init_unittests)
