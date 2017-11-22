// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umockcallrecorder.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include "umock_c.h"
#include "umock_c_negative_tests.h"

TEST_DEFINE_ENUM_TYPE(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VA)

static UMOCKCALLRECORDER_HANDLE test_call_recorder = (UMOCKCALLRECORDER_HANDLE)0x4244;
static UMOCKCALLRECORDER_HANDLE test_cloned_call_recorder = (UMOCKCALLRECORDER_HANDLE)0x4245;

static size_t umock_c_get_call_recorder_call_count;
static UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder_call_result;

typedef struct umockcallrecorder_clone_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_clone_CALL;

static umockcallrecorder_clone_CALL* umockcallrecorder_clone_calls;
static size_t umockcallrecorder_clone_call_count;
static UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone_call_result;

typedef struct umockcallrecorder_get_expected_call_count_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
    size_t* expected_call_count;
} umockcallrecorder_get_expected_call_count_CALL;

static umockcallrecorder_get_expected_call_count_CALL* umockcallrecorder_get_expected_call_count_calls;
static size_t umockcallrecorder_get_expected_call_count_call_count;
static int umockcallrecorder_get_expected_call_count_call_result;

typedef struct umockcallrecorder_fail_call_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
    size_t index;
} umockcallrecorder_fail_call_CALL;

static umockcallrecorder_fail_call_CALL* umockcallrecorder_fail_call_calls;
static size_t umockcallrecorder_fail_call_call_count;
static int umockcallrecorder_fail_call_call_result;

typedef struct umock_c_set_call_recorder_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umock_c_set_call_recorder_CALL;

static umock_c_set_call_recorder_CALL* umock_c_set_call_recorder_calls;
static size_t umock_c_set_call_recorder_call_count;
static int umock_c_set_call_recorder_call_result;

typedef struct umockcallrecorder_destroy_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_destroy_CALL;

static umockcallrecorder_destroy_CALL* umockcallrecorder_destroy_calls;
static size_t umockcallrecorder_destroy_call_count;

typedef struct umock_c_indicate_error_CALL_TAG
{
    UMOCK_C_ERROR_CODE error_code;
} umock_c_indicate_error_CALL;

static umock_c_indicate_error_CALL* umock_c_indicate_error_calls;
static size_t umock_c_indicate_error_call_count;

UMOCKCALLRECORDER_HANDLE umock_c_get_call_recorder(void)
{
    umock_c_get_call_recorder_call_count++;
    return umock_c_get_call_recorder_call_result;
}

UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_clone_CALL* new_calls = (umockcallrecorder_clone_CALL*)realloc(umockcallrecorder_clone_calls, sizeof(umockcallrecorder_clone_CALL) * (umockcallrecorder_clone_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_clone_calls = new_calls;
        umockcallrecorder_clone_calls[umockcallrecorder_clone_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_clone_call_count++;
    }

    return umockcallrecorder_clone_call_result;
}

int umockcallrecorder_get_expected_call_count(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t* expected_call_count)
{
    umockcallrecorder_get_expected_call_count_CALL* new_calls = (umockcallrecorder_get_expected_call_count_CALL*)realloc(umockcallrecorder_get_expected_call_count_calls, sizeof(umockcallrecorder_get_expected_call_count_CALL) * (umockcallrecorder_get_expected_call_count_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_get_expected_call_count_calls = new_calls;
        umockcallrecorder_get_expected_call_count_calls[umockcallrecorder_get_expected_call_count_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_get_expected_call_count_calls[umockcallrecorder_get_expected_call_count_call_count].expected_call_count = expected_call_count;
        umockcallrecorder_get_expected_call_count_call_count++;
    }

    return umockcallrecorder_get_expected_call_count_call_result;
}

int umockcallrecorder_fail_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t index)
{
    umockcallrecorder_fail_call_CALL* new_calls = (umockcallrecorder_fail_call_CALL*)realloc(umockcallrecorder_fail_call_calls, sizeof(umockcallrecorder_fail_call_CALL) * (umockcallrecorder_fail_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_fail_call_calls = new_calls;
        umockcallrecorder_fail_call_calls[umockcallrecorder_fail_call_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_fail_call_calls[umockcallrecorder_fail_call_call_count].index = index;
        umockcallrecorder_fail_call_call_count++;
    }

    return umockcallrecorder_fail_call_call_result;
}

int umock_c_set_call_recorder(UMOCKCALLRECORDER_HANDLE call_recorder)
{
    umock_c_set_call_recorder_CALL* new_calls = (umock_c_set_call_recorder_CALL*)realloc(umock_c_set_call_recorder_calls, sizeof(umock_c_set_call_recorder_CALL) * (umock_c_set_call_recorder_call_count + 1));
    if (new_calls != NULL)
    {
        umock_c_set_call_recorder_calls = new_calls;
        umock_c_set_call_recorder_calls[umock_c_set_call_recorder_call_count].umock_call_recorder = call_recorder;
        umock_c_set_call_recorder_call_count++;
    }

    return umock_c_set_call_recorder_call_result;
}

void umockcallrecorder_destroy(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_destroy_CALL* new_calls = (umockcallrecorder_destroy_CALL*)realloc(umockcallrecorder_destroy_calls, sizeof(umockcallrecorder_destroy_CALL) * (umockcallrecorder_destroy_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_destroy_calls = new_calls;
        umockcallrecorder_destroy_calls[umockcallrecorder_destroy_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_destroy_call_count++;
    }
}

void umock_c_indicate_error(UMOCK_C_ERROR_CODE error_code)
{
    umock_c_indicate_error_CALL* new_calls = (umock_c_indicate_error_CALL*)realloc(umock_c_indicate_error_calls, sizeof(umock_c_indicate_error_CALL) * (umock_c_indicate_error_call_count + 1));
    if (new_calls != NULL)
    {
        umock_c_indicate_error_calls = new_calls;
        umock_c_indicate_error_calls[umock_c_indicate_error_call_count].error_code = error_code;
        umock_c_indicate_error_call_count++;
    }
}

void reset_all_calls(void)
{
    umock_c_get_call_recorder_call_count = 0;
    umock_c_get_call_recorder_call_result = test_call_recorder;

    if (umockcallrecorder_clone_calls != NULL)
    {
        free(umockcallrecorder_clone_calls);
    }
    umockcallrecorder_clone_calls = NULL;
    umockcallrecorder_clone_call_count = 0;
    umockcallrecorder_clone_call_result = test_call_recorder;

    if (umockcallrecorder_get_expected_call_count_calls != NULL)
    {
        free(umockcallrecorder_get_expected_call_count_calls);
    }
    umockcallrecorder_get_expected_call_count_calls = NULL;
    umockcallrecorder_get_expected_call_count_call_count = 0;
    umockcallrecorder_get_expected_call_count_call_result = 0;

    if (umockcallrecorder_fail_call_calls != NULL)
    {
        free(umockcallrecorder_fail_call_calls);
    }
    umockcallrecorder_fail_call_calls = NULL;
    umockcallrecorder_fail_call_call_count = 0;
    umockcallrecorder_fail_call_call_result = 0;

    if (umock_c_set_call_recorder_calls != NULL)
    {
        free(umock_c_set_call_recorder_calls);
    }
    umock_c_set_call_recorder_calls = NULL;
    umock_c_set_call_recorder_call_count = 0;
    umock_c_set_call_recorder_call_result = 0;

    if (umockcallrecorder_destroy_calls != NULL)
    {
        free(umockcallrecorder_destroy_calls);
    }
    umockcallrecorder_destroy_calls = NULL;
    umockcallrecorder_destroy_call_count = 0;

    if (umock_c_indicate_error_calls != NULL)
    {
        free(umock_c_indicate_error_calls);
    }
    umock_c_indicate_error_calls = NULL;
    umock_c_indicate_error_call_count = 0;
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
static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(umock_c_negative_tests_no_init_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    TEST_MUTEX_DESTROY(test_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    int mutex_acquire_result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, mutex_acquire_result);

    reset_all_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    reset_all_calls();

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umock_c_negative_tests_deinit */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_003: [ If the module was not previously initialized, umock_c_negative_tests_deinit shall do nothing. ]*/
TEST_FUNCTION(umock_c_negative_tests_deinit_when_not_initialized_does_not_free_anything)
{
    // arrange

    // act
    umock_c_negative_tests_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* umock_c_negative_tests_snapshot */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_015: [ If the module was not previously initialized, umock_c_negative_tests_snapshot shall do nothing. ]*/
TEST_FUNCTION(umock_c_negative_tests_snapshot_when_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_negative_tests_snapshot();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* umock_c_negative_tests_reset */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_016: [ If the module was not previously initialized, umock_c_negative_tests_reset shall do nothing. ]*/
TEST_FUNCTION(umock_c_negative_tests_reset_when_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_negative_tests_reset();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_set_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* umock_c_negative_tests_fail_call */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_020: [ If the module was not previously initialized, umock_c_negative_tests_fail_call shall do nothing. ]*/
TEST_FUNCTION(umock_c_negative_tests_fail_call_when_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_negative_tests_fail_call(0);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_fail_call_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* umock_c_negative_tests_call_count */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_024: [ If the module was not previously initialized, umock_c_negative_tests_call_count shall return 0. ]*/
TEST_FUNCTION(umock_c_negative_tests_call_count_when_the_module_is_not_initialized_returns_0)
{
    // arrange

    // act
    size_t result = umock_c_negative_tests_call_count();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_expected_call_count_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

END_TEST_SUITE(umock_c_negative_tests_no_init_unittests)
