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

TEST_DEFINE_ENUM_TYPE(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

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

BEGIN_TEST_SUITE(umock_c_negative_tests_unittests)

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
    umock_c_negative_tests_deinit();
    reset_all_calls();

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umock_c_negative_tests_init */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_001: [ umock_c_negative_tests_init shall initialize the umock_c negative tests library.] */
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_004: [ On success, umock_c_negative_tests_init shall return 0. ]*/
TEST_FUNCTION(umock_c_negative_tests_init_succeeds)
{
    // arrange

    // act
    int result = umock_c_negative_tests_init();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_005: [ If the module has already been initialized, umock_c_negative_tests_init shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_negative_tests_init_after_init_fails)
{
    // arrange
    (void)umock_c_negative_tests_init();

    // act
    int result = umock_c_negative_tests_init();

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umock_c_negative_tests_deinit */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_002: [ umock_c_negative_tests_deinit shall free all resources associated with the negative tests module. ]*/
TEST_FUNCTION(umock_c_negative_tests_deinit_with_no_snapshot_does_not_free_anything)
{
    // arrange
    (void)umock_c_negative_tests_init();

    // act
    umock_c_negative_tests_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_002: [ umock_c_negative_tests_deinit shall free all resources associated with the negative tests module. ]*/
TEST_FUNCTION(umock_c_negative_tests_deinit_with_a_snapshot_frees_the_snapshot)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    // act
    umock_c_negative_tests_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_destroy_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_003: [ If the module was not previously initialized, umock_c_negative_tests_deinit shall do nothing. ]*/
TEST_FUNCTION(umock_c_negative_tests_deinit_when_not_initialized_does_not_free_anything)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umock_c_negative_tests_snapshot();
    umock_c_negative_tests_deinit();
    reset_all_calls();

    // act
    umock_c_negative_tests_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* umock_c_negative_tests_snapshot */

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_006: [ umock_c_negative_tests_snapshot shall make a copy of the current call recorder for umock_c with all its recorded calls. ]*/
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_007: [ The current call recorder shall be obtained by calling umock_c_get_call_recorder. ]*/
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_008: [ The copy of the recorder shall be made by calling umockcallrecorder_clone. ]*/
TEST_FUNCTION(umock_c_negative_tests_snapshot_makes_a_copy_of_the_recorder)
{
    // arrange
    (void)umock_c_negative_tests_init();
    reset_all_calls();

    // act
    umock_c_negative_tests_snapshot();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_clone_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_009: [ If getting the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_getting_the_recorder_fails_umock_c_negative_tests_snapshot_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    reset_all_calls();
    umock_c_get_call_recorder_call_result = NULL;

    // act
    umock_c_negative_tests_snapshot();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_010: [ If copying the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_cloning_the_recorder_fails_umock_c_negative_tests_snapshot_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    reset_all_calls();
    umockcallrecorder_clone_call_result = NULL;

    // act
    umock_c_negative_tests_snapshot();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

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

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_011: [ umock_c_negative_tests_reset shall reset the call recorder used by umock_c to the call recorder stored in umock_c_negative_tests_reset. ]*/
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_013: [ The reset shall be done by calling umock_c_set_call_recorder and passing the call recorder stored in umock_c_negative_tests_reset as argument. ]*/
TEST_FUNCTION(umock_c_negative_tests_reset_sets_the_call_recorder_to_be_the_stored_one)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    // act
    umock_c_negative_tests_reset();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_set_call_recorder_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_cloned_call_recorder, umock_c_set_call_recorder_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_012: [ If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_no_snapshot_was_done_umock_c_negative_tests_reset_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();

    // act
    umock_c_negative_tests_reset();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_set_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_014: [ If umock_c_set_call_recorder fails, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_setting_the_call_recorder_fails_umock_c_negative_tests_reset_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    umock_c_set_call_recorder_call_result = 1;

    // act
    umock_c_negative_tests_reset();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_set_call_recorder_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_cloned_call_recorder, umock_c_set_call_recorder_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

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

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_017: [ umock_c_negative_tests_fail_call shall call umockcallrecorder_fail_call on the currently used call recorder used by umock_c. ]*/
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_018: [ The currently used recorder shall be obtained by calling umock_c_get_call_recorder. ]*/
TEST_FUNCTION(umock_c_negative_tests_fail_call_calls_the_call_recorder_fail_call)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    // act
    umock_c_negative_tests_fail_call(0);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_fail_call_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_fail_call_calls[0].index);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_017: [ umock_c_negative_tests_fail_call shall call umockcallrecorder_fail_call on the currently used call recorder used by umock_c. ]*/
/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_018: [ The currently used recorder shall be obtained by calling umock_c_get_call_recorder. ]*/
TEST_FUNCTION(umock_c_negative_tests_fail_call_calls_the_call_recorder_fail_call_2)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    // act
    umock_c_negative_tests_fail_call(42);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_fail_call_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 42, umockcallrecorder_fail_call_calls[0].index);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_019: [ If umock_c_get_call_recorder fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_getting_the_call_recorder_fails_umock_c_negative_tests_fail_call_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    umock_c_get_call_recorder_call_result = NULL;

    // act
    umock_c_negative_tests_fail_call(0);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_fail_call_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_025: [ If failing the call by calling umockcallrecorder_fail_call fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_failing_the_call_fails_umock_c_negative_tests_fail_call_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    umockcallrecorder_fail_call_call_result = __LINE__;

    // act
    umock_c_negative_tests_fail_call(0);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_get_call_recorder_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_fail_call_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_fail_call_calls[0].index);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

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

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_021: [ umock_c_negative_tests_call_count shall return the count of expected calls for the current snapshot call recorder by calling umockcallrecorder_get_expected_call_count. ]*/
TEST_FUNCTION(umock_c_negative_tests_call_count_gets_the_expected_call_count_from_the_current_recorder)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    // act
    size_t result = umock_c_negative_tests_call_count();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_expected_call_count_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_expected_call_count_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 0, umock_c_indicate_error_call_count);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_022: [ If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_fail_call shall return 0 and indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(umock_c_negative_tests_call_count_when_no_snapshot_was_done_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    reset_all_calls();

    // act
    size_t result = umock_c_negative_tests_call_count();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_expected_call_count_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_NEGATIVE_TESTS_01_023: [ If umockcallrecorder_get_expected_call_count fails, umock_c_negative_tests_fail_call shall return 0 and indicate the error via the umock error callback with error code UMOCK_C_ERROR. ]*/
TEST_FUNCTION(when_umockcallrecorder_get_expected_call_count_fails_umock_c_negative_tests_call_count_indicates_an_error)
{
    // arrange
    (void)umock_c_negative_tests_init();
    umock_c_negative_tests_snapshot();
    reset_all_calls();

    umockcallrecorder_get_expected_call_count_call_result = __LINE__;

    // act
    size_t result = umock_c_negative_tests_call_count();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_expected_call_count_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_expected_call_count_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 1, umock_c_indicate_error_call_count);
    ASSERT_ARE_EQUAL(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR, umock_c_indicate_error_calls[0].error_code);
}

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

END_TEST_SUITE(umock_c_negative_tests_unittests)
