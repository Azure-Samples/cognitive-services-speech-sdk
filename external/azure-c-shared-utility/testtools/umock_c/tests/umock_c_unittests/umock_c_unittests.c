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
static UMOCKCALLRECORDER_HANDLE test_cloned_call_recorder = (UMOCKCALLRECORDER_HANDLE)0x4245;

static size_t umocktypes_init_call_count;
static int umocktypes_init_call_result;

static size_t umocktypes_c_register_types_call_count;
static int umocktypes_c_register_types_call_result;

static size_t umockcallrecorder_create_call_count;
static UMOCKCALLRECORDER_HANDLE umockcallrecorder_create_call_result;

static size_t umocktypes_deinit_call_count;

typedef struct umockcallrecorder_destroy_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_destroy_CALL;

static umockcallrecorder_destroy_CALL* umockcallrecorder_destroy_calls;
static size_t umockcallrecorder_destroy_call_count;

typedef struct umockcallrecorder_reset_all_calls_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_reset_all_calls_CALL;

static umockcallrecorder_reset_all_calls_CALL* umockcallrecorder_reset_all_calls_calls;
static size_t umockcallrecorder_reset_all_calls_call_count;
static int umockcallrecorder_reset_all_calls_call_result;

typedef struct umockcallrecorder_get_actual_calls_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_get_actual_calls_CALL;

static umockcallrecorder_get_actual_calls_CALL* umockcallrecorder_get_actual_calls_calls;
static size_t umockcallrecorder_get_actual_calls_call_count;
static const char* umockcallrecorder_get_actual_calls_call_result;

typedef struct umockcallrecorder_get_expected_calls_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_get_expected_calls_CALL;

static umockcallrecorder_get_expected_calls_CALL* umockcallrecorder_get_expected_calls_calls;
static size_t umockcallrecorder_get_expected_calls_call_count;
static const char* umockcallrecorder_get_expected_calls_call_result;

typedef struct umockcallrecorder_get_last_expected_call_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_get_last_expected_call_CALL;

static umockcallrecorder_get_last_expected_call_CALL* umockcallrecorder_get_last_expected_call_calls;
static size_t umockcallrecorder_get_last_expected_call_call_count;
static UMOCKCALL_HANDLE umockcallrecorder_get_last_expected_call_call_result;

typedef struct umockcallrecorder_add_expected_call_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_add_expected_call_CALL;

static umockcallrecorder_add_expected_call_CALL* umockcallrecorder_add_expected_call_calls;
static size_t umockcallrecorder_add_expected_call_call_count;
static int umockcallrecorder_add_expected_call_call_result;

typedef struct umockcallrecorder_add_actual_call_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_add_actual_call_CALL;

static umockcallrecorder_add_actual_call_CALL* umockcallrecorder_add_actual_call_calls;
static size_t umockcallrecorder_add_actual_call_call_count;
static int umockcallrecorder_add_actual_call_call_result;

typedef struct umockcallrecorder_clone_CALL_TAG
{
    UMOCKCALLRECORDER_HANDLE umock_call_recorder;
} umockcallrecorder_clone_CALL;

static umockcallrecorder_clone_CALL* umockcallrecorder_clone_calls;
static size_t umockcallrecorder_clone_call_count;
static UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone_call_result;

UMOCKCALLRECORDER_HANDLE umockcallrecorder_create(void)
{
    umockcallrecorder_create_call_count++;
    return umockcallrecorder_create_call_result;
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

int umockcallrecorder_reset_all_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_reset_all_calls_CALL* new_calls = (umockcallrecorder_reset_all_calls_CALL*)realloc(umockcallrecorder_reset_all_calls_calls, sizeof(umockcallrecorder_reset_all_calls_CALL) * (umockcallrecorder_reset_all_calls_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_reset_all_calls_calls = new_calls;
        umockcallrecorder_reset_all_calls_calls[umockcallrecorder_reset_all_calls_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_reset_all_calls_call_count++;
    }

    return umockcallrecorder_reset_all_calls_call_result;
}

int umockcallrecorder_add_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call)
{
    umockcallrecorder_add_expected_call_CALL* new_calls = (umockcallrecorder_add_expected_call_CALL*)realloc(umockcallrecorder_add_expected_call_calls, sizeof(umockcallrecorder_add_expected_call_CALL) * (umockcallrecorder_add_expected_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_add_expected_call_calls = new_calls;
        umockcallrecorder_add_expected_call_calls[umockcallrecorder_add_expected_call_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_add_expected_call_call_count++;
    }

    return umockcallrecorder_add_expected_call_call_result;
}

int umockcallrecorder_add_actual_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call)
{
    umockcallrecorder_add_actual_call_CALL* new_calls = (umockcallrecorder_add_actual_call_CALL*)realloc(umockcallrecorder_add_actual_call_calls, sizeof(umockcallrecorder_add_actual_call_CALL) * (umockcallrecorder_add_actual_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_add_actual_call_calls = new_calls;
        umockcallrecorder_add_actual_call_calls[umockcallrecorder_add_actual_call_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_add_actual_call_call_count++;
    }

    return umockcallrecorder_add_actual_call_call_result;
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

const char* umockcallrecorder_get_actual_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_get_actual_calls_CALL* new_calls = (umockcallrecorder_get_actual_calls_CALL*)realloc(umockcallrecorder_get_actual_calls_calls, sizeof(umockcallrecorder_get_actual_calls_CALL) * (umockcallrecorder_get_actual_calls_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_get_actual_calls_calls = new_calls;
        umockcallrecorder_get_actual_calls_calls[umockcallrecorder_get_actual_calls_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_get_actual_calls_call_count++;
    }

    return umockcallrecorder_get_actual_calls_call_result;
}

const char* umockcallrecorder_get_expected_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_get_expected_calls_CALL* new_calls = (umockcallrecorder_get_expected_calls_CALL*)realloc(umockcallrecorder_get_expected_calls_calls, sizeof(umockcallrecorder_get_expected_calls_CALL) * (umockcallrecorder_get_expected_calls_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_get_expected_calls_calls = new_calls;
        umockcallrecorder_get_expected_calls_calls[umockcallrecorder_get_expected_calls_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_get_expected_calls_call_count++;
    }

    return umockcallrecorder_get_expected_calls_call_result;
}

UMOCKCALL_HANDLE umockcallrecorder_get_last_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder)
{
    umockcallrecorder_get_last_expected_call_CALL* new_calls = (umockcallrecorder_get_last_expected_call_CALL*)realloc(umockcallrecorder_get_last_expected_call_calls, sizeof(umockcallrecorder_get_last_expected_call_CALL) * (umockcallrecorder_get_last_expected_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcallrecorder_get_last_expected_call_calls = new_calls;
        umockcallrecorder_get_last_expected_call_calls[umockcallrecorder_get_last_expected_call_call_count].umock_call_recorder = umock_call_recorder;
        umockcallrecorder_get_last_expected_call_call_count++;
    }

    return umockcallrecorder_get_last_expected_call_call_result;
}

int umocktypes_init(void)
{
    umocktypes_init_call_count++;
    return umocktypes_init_call_result;
}

void umocktypes_deinit(void)
{
    umocktypes_deinit_call_count++;
}

int umocktypes_c_register_types(void)
{
    umocktypes_c_register_types_call_count++;
    return umocktypes_c_register_types_call_result;
}

typedef struct test_on_umock_c_error_CALL_TAG
{
    UMOCK_C_ERROR_CODE error_code;
} test_on_umock_c_error_CALL;

static test_on_umock_c_error_CALL* test_on_umock_c_error_calls;
static size_t test_on_umock_c_error_call_count;

static void test_on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    test_on_umock_c_error_CALL* new_calls = (test_on_umock_c_error_CALL*)realloc(test_on_umock_c_error_calls, sizeof(test_on_umock_c_error_CALL) * (test_on_umock_c_error_call_count + 1));
    if (new_calls != NULL)
    {
        test_on_umock_c_error_calls = new_calls;
        test_on_umock_c_error_calls[test_on_umock_c_error_call_count].error_code = error_code;
        test_on_umock_c_error_call_count++;
    }
}

void reset_all_calls(void)
{
    umocktypes_init_call_count = 0;
    umocktypes_init_call_result = 0;

    umocktypes_c_register_types_call_count = 0;
    umocktypes_c_register_types_call_result = 0;

    umockcallrecorder_create_call_count = 0;
    umockcallrecorder_create_call_result = test_call_recorder;

    umocktypes_deinit_call_count = 0;

    if (umockcallrecorder_destroy_calls != NULL)
    {
        free(umockcallrecorder_destroy_calls);
    }
    umockcallrecorder_destroy_calls = NULL;
    umockcallrecorder_destroy_call_count = 0;

    if (umockcallrecorder_reset_all_calls_calls != NULL)
    {
        free(umockcallrecorder_reset_all_calls_calls);
    }
    umockcallrecorder_reset_all_calls_calls = NULL;
    umockcallrecorder_reset_all_calls_call_count = 0;
    umockcallrecorder_reset_all_calls_call_result = 0;

    if (umockcallrecorder_get_actual_calls_calls != NULL)
    {
        free(umockcallrecorder_get_actual_calls_calls);
    }
    umockcallrecorder_get_actual_calls_calls = NULL;
    umockcallrecorder_get_actual_calls_call_count = 0;
    umockcallrecorder_get_actual_calls_call_result = NULL;

    if (umockcallrecorder_get_expected_calls_calls != NULL)
    {
        free(umockcallrecorder_get_expected_calls_calls);
    }
    umockcallrecorder_get_expected_calls_calls = NULL;
    umockcallrecorder_get_expected_calls_call_count = 0;
    umockcallrecorder_get_expected_calls_call_result = NULL;

    if (umockcallrecorder_get_last_expected_call_calls != NULL)
    {
        free(umockcallrecorder_get_last_expected_call_calls);
    }
    umockcallrecorder_get_last_expected_call_calls = NULL;
    umockcallrecorder_get_last_expected_call_call_count = 0;
    umockcallrecorder_get_last_expected_call_call_result = NULL;

    if (umockcallrecorder_add_expected_call_calls != NULL)
    {
        free(umockcallrecorder_add_expected_call_calls);
    }
    umockcallrecorder_add_expected_call_calls = NULL;
    umockcallrecorder_add_expected_call_call_count = 0;
    umockcallrecorder_add_expected_call_call_result = 0;

    if (umockcallrecorder_add_actual_call_calls != NULL)
    {
        free(umockcallrecorder_add_actual_call_calls);
    }
    umockcallrecorder_add_actual_call_calls = NULL;
    umockcallrecorder_add_actual_call_call_count = 0;
    umockcallrecorder_add_actual_call_call_result = 0;

    if (umockcallrecorder_clone_calls != NULL)
    {
        free(umockcallrecorder_clone_calls);
    }
    umockcallrecorder_clone_calls = NULL;
    umockcallrecorder_clone_call_count = 0;
    umockcallrecorder_clone_call_result = test_cloned_call_recorder;

    if (test_on_umock_c_error_calls != NULL)
    {
        free(test_on_umock_c_error_calls);
    }
    test_on_umock_c_error_calls = NULL;
    test_on_umock_c_error_call_count = 0;
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

BEGIN_TEST_SUITE(umock_c_unittests)

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

    umock_c_deinit();
    reset_all_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    reset_all_calls();

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umock_deinit */

/* Tests_SRS_UMOCK_C_01_001: [umock_c_init shall initialize the umock library.] */
/* Tests_SRS_UMOCK_C_01_023: [ umock_c_init shall initialize the umock types by calling umocktypes_init. ]*/
/* Tests_SRS_UMOCK_C_01_004: [ On success, umock_c_init shall return 0. ]*/
/* Tests_SRS_UMOCK_C_01_002: [ umock_c_init shall register the C naive types by calling umocktypes_c_register_types. ]*/
/* Tests_SRS_UMOCK_C_01_003: [ umock_c_init shall create a call recorder by calling umockcallrecorder_create. ]*/
/* Tests_SRS_UMOCK_C_01_006: [ The on_umock_c_error callback shall be stored to be used for later error callbacks. ]*/
TEST_FUNCTION(when_all_calls_succeed_umock_c_init_succeeds)
{
    // arrange

    // act
    int result = umock_c_init(test_on_umock_c_error);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_init_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_c_register_types_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_create_call_count);
}

/* Tests_SRS_UMOCK_C_01_005: [ If any of the calls fails, umock_c_init shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_umocktypes_init_fails_then_umock_c_init_fails)
{
    // arrange
    umocktypes_init_call_result = 1;

    // act
    int result = umock_c_init(test_on_umock_c_error);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_init_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umocktypes_c_register_types_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_create_call_count);
}

/* Tests_SRS_UMOCK_C_01_005: [ If any of the calls fails, umock_c_init shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_umocktypes_c_register_types_fails_then_umock_c_init_fails)
{
    // arrange
    umocktypes_c_register_types_call_result = 1;

    // act
    int result = umock_c_init(test_on_umock_c_error);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_init_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_c_register_types_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_create_call_count);
}

/* Tests_SRS_UMOCK_C_01_005: [ If any of the calls fails, umock_c_init shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_creating_the_call_recorder_fails_then_umock_c_init_fails)
{
    // arrange
    umockcallrecorder_create_call_result = NULL;

    // act
    int result = umock_c_init(test_on_umock_c_error);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_init_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_c_register_types_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_create_call_count);
}

/* Tests_SRS_UMOCK_C_01_007: [ umock_c_init when umock is already initialized shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umock_c_init_when_already_initialized_fails)
{
    // arrange
    umock_c_init(test_on_umock_c_error);

    // act
    int result = umock_c_init(test_on_umock_c_error);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCK_C_01_024: [ on_umock_c_error shall be optional. ]*/
TEST_FUNCTION(umock_c_init_with_NULL_callback_succeeds)
{
    // arrange

    // act
    int result = umock_c_init(NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_init_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_c_register_types_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_create_call_count);
}

/* umock_c_deinit */

/* Tests_SRS_UMOCK_C_01_008: [ umock_c_deinit shall deinitialize the umock types by calling umocktypes_deinit. ]*/
/* Tests_SRS_UMOCK_C_01_009: [ umock_c_deinit shall free the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_deinit_deinitializes_types_and_destroys_call_recorder)
{
    // arrange
    (void)umock_c_init(NULL);

    // act
    umock_c_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umocktypes_deinit_call_count);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_destroy_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_008: [ umock_c_deinit shall deinitialize the umock types by calling umocktypes_deinit. ]*/
/* Tests_SRS_UMOCK_C_01_009: [ umock_c_deinit shall free the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_deinit_when_not_initialized_does_nothing)
{
    // arrange
    (void)umock_c_init(NULL);
    umock_c_deinit();
    reset_all_calls();

    // act
    umock_c_deinit();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umocktypes_deinit_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* umock_c_reset_all_calls */

/* Tests_SRS_UMOCK_C_01_011: [ umock_c_reset_all_calls shall reset all calls by calling umockcallrecorder_reset_all_calls on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_reset_all_calls_calls_the_call_recorder_reset_all_calls)
{
    // arrange
    (void)umock_c_init(NULL);

    // act
    umock_c_reset_all_calls();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_reset_all_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_reset_all_calls_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_025: [ If the underlying umockcallrecorder_reset_all_calls fails, the on_umock_c_error callback shall be triggered with UMOCK_C_RESET_CALLS_ERROR. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_reset_all_calls_fails_then_umock_c_reset_all_calls_triggers_the_on_error_callback)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_reset_all_calls_call_result = 1;

    // act
    umock_c_reset_all_calls();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_reset_all_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_reset_all_calls_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 1, test_on_umock_c_error_call_count);
    ASSERT_ARE_EQUAL(int, (int)UMOCK_C_RESET_CALLS_ERROR, test_on_umock_c_error_calls[0].error_code);
}

/* Tests_SRS_UMOCK_C_01_012: [ If the module is not initialized, umock_c_reset_all_calls shall do nothing. ]*/
TEST_FUNCTION(umock_c_reset_all_calls_when_the_module_is_not_initialized_does_nothing)
{
    // arrange

    // act
    umock_c_reset_all_calls();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_reset_all_calls_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, test_on_umock_c_error_call_count);
}

/* umock_c_get_actual_calls */

/* Tests_SRS_UMOCK_C_01_013: [ umock_c_get_actual_calls shall return the string for the recorded actual calls by calling umockcallrecorder_get_actual_calls on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_get_actual_calls_calls_the_underlying_call_recorder_get_actual_calls)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_actual_calls_call_result = "[a()]";

    // act
    const char* result = umock_c_get_actual_calls();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_actual_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_actual_calls_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_013: [ umock_c_get_actual_calls shall return the string for the recorded actual calls by calling umockcallrecorder_get_actual_calls on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_get_actual_calls_fails_then_umock_c_get_actual_calls_returns_NULL)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_actual_calls_call_result = NULL;

    // act
    const char* result = umock_c_get_actual_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_actual_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_actual_calls_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_014: [ If the module is not initialized, umock_c_get_actual_calls shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_actual_calls_when_the_module_is_not_initialized_fails)
{
    // arrange

    // act
    const char* result = umock_c_get_actual_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_actual_calls_call_count);
}

/* umock_c_get_expected_calls */

/* Tests_SRS_UMOCK_C_01_015: [ umock_c_get_expected_calls shall return the string for the recorded expected calls by calling umockcallrecorder_get_expected_calls on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_get_expected_calls_calls_the_underlying_call_recorder_get_expected_calls)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_expected_calls_call_result = "[a()]";

    // act
    const char* result = umock_c_get_expected_calls();

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_expected_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_expected_calls_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_015: [ umock_c_get_expected_calls shall return the string for the recorded expected calls by calling umockcallrecorder_get_expected_calls on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_get_expected_calls_fails_then_umock_c_get_expected_calls_returns_NULL)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_expected_calls_call_result = NULL;

    // act
    const char* result = umock_c_get_expected_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_expected_calls_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_expected_calls_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_016: [ If the module is not initialized, umock_c_get_expected_calls shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_expected_calls_when_the_module_is_not_initialized_fails)
{
    // arrange

    // act
    const char* result = umock_c_get_expected_calls();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_expected_calls_call_count);
}

/* umock_c_get_last_expected_call */

/* Tests_SRS_UMOCK_C_01_017: [ umock_c_get_last_expected_call shall return the last expected call by calling umockcallrecorder_get_last_expected_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_get_last_expected_call_calls_the_underlying_call_recorder_get_last_expected_call)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_last_expected_call_call_result = test_expected_call;

    // act
    UMOCKCALL_HANDLE result = umock_c_get_last_expected_call();

    // assert
    ASSERT_ARE_EQUAL(void_ptr, test_expected_call, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_last_expected_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_last_expected_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_017: [ umock_c_get_last_expected_call shall return the last expected call by calling umockcallrecorder_get_last_expected_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_get_last_expected_call_fails_then_umock_c_get_last_expected_call_returns_NULL)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_get_last_expected_call_call_result = NULL;

    // act
    UMOCKCALL_HANDLE result = umock_c_get_last_expected_call();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_get_last_expected_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_get_last_expected_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_018: [ If the module is not initialized, umock_c_get_last_expected_call shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_last_expected_call_when_the_module_is_not_initialized_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umock_c_get_last_expected_call();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_get_last_expected_call_call_count);
}

/* umock_c_add_expected_call */

/* Tests_SRS_UMOCK_C_01_019: [ umock_c_add_expected_call shall add an expected call by calling umockcallrecorder_add_expected_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_add_expected_call_calls_the_underlying_call_recorder_add_expected_call)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_add_expected_call_call_result = 0;

    // act
    int result = umock_c_add_expected_call(test_expected_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_add_expected_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_add_expected_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_019: [ umock_c_add_expected_call shall add an expected call by calling umockcallrecorder_add_expected_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_add_expected_call_fails_then_umock_c_add_expected_call_returns_NULL)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_add_expected_call_call_result = 1;

    // act
    int result = umock_c_add_expected_call(test_expected_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_add_expected_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_add_expected_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_020: [ If the module is not initialized, umock_c_add_expected_call shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_add_expected_call_when_the_module_is_not_initialized_fails)
{
    // arrange

    // act
    int result = umock_c_add_expected_call(test_expected_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_add_expected_call_call_count);
}

/* umock_c_add_actual_call */

/* Tests_SRS_UMOCK_C_01_021: [ umock_c_add_actual_call shall add an actual call by calling umockcallrecorder_add_actual_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(umock_c_add_actual_call_calls_the_underlying_call_recorder_add_actual_call)
{
    // arrange
    UMOCKCALL_HANDLE matched_call;
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_add_actual_call_call_result = 0;

    // act
    int result = umock_c_add_actual_call(test_actual_call, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_add_actual_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_add_actual_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_021: [ umock_c_add_actual_call shall add an actual call by calling umockcallrecorder_add_actual_call on the call recorder created in umock_c_init. ]*/
TEST_FUNCTION(when_the_underlying_call_recorder_add_actual_call_fails_then_umock_c_add_actual_call_returns_NULL)
{
    // arrange
    UMOCKCALL_HANDLE matched_call;
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_add_actual_call_call_result = 1;

    // act
    int result = umock_c_add_actual_call(test_actual_call, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_add_actual_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_add_actual_call_calls[0].umock_call_recorder);
}

/* Tests_SRS_UMOCK_C_01_022: [** If the module is not initialized, umock_c_add_actual_call shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_add_actual_call_when_the_module_is_not_initialized_fails)
{
    // arrange
    UMOCKCALL_HANDLE matched_call;

    // act
    int result = umock_c_add_actual_call(test_actual_call, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_add_actual_call_call_count);
}

/* umock_c_get_call_recorder */

/* Tests_SRS_UMOCK_C_01_026: [ umock_c_get_call_recorder shall return the handle to the currently used call recorder. ]*/
TEST_FUNCTION(umock_c_get_call_recorder_returns_a_non_NULL_call_recorder)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);

    // act
    UMOCKCALLRECORDER_HANDLE result = umock_c_get_call_recorder();

    // assert
    ASSERT_IS_NOT_NULL(result);
}

/* Tests_SRS_UMOCK_C_01_027: [ If the module is not initialized, umock_c_get_call_recorder shall return NULL. ]*/
TEST_FUNCTION(umock_c_get_call_recorder_when_not_initialized_fails)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umock_c_deinit();

    // act
    UMOCKCALLRECORDER_HANDLE result = umock_c_get_call_recorder();

    // assert
    ASSERT_IS_NULL(result);
}

/* umock_c_set_call_recorder */

/* Tests_SRS_UMOCK_C_01_028: [ umock_c_set_call_recorder shall replace the currently used call recorder with the one identified by the call_recorder argument. ]*/
/* Tests_SRS_UMOCK_C_01_029: [ On success, umock_c_set_call_recorder shall return 0. ]*/
/* Tests_SRS_UMOCK_C_01_031: [ umock_c_set_call_recorder shall make a copy of call_recorder by calling umockcallrecorder_clone and use the copy for future actions. ]*/
/* Tests_SRS_UMOCK_C_01_034: [ The previously used call recorder shall be destroyed by calling umockcallrecorder_destroy. ]*/
TEST_FUNCTION(umock_c_set_call_recorder_replaces_the_recorder)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);

    // act
    int result = umock_c_set_call_recorder(test_call_recorder);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_clone_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_destroy_call_count);
}

/* Tests_SRS_UMOCK_C_01_030: [ If call_recorder is NULL, umock_c_set_call_recorder shall return a non-zero value. ]*/
TEST_FUNCTION(umock_c_set_call_recorder_with_NULL_call_recorder_fails)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);

    // act
    int result = umock_c_set_call_recorder(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* Tests_SRS_UMOCK_C_01_032: [ If umockcallrecorder_clone fails, umock_c_set_call_recorder shall return a non-zero value. ]*/
TEST_FUNCTION(when_cloning_the_call_recorder_fails_umock_c_set_call_recorder_fails)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umockcallrecorder_clone_call_result = NULL;

    // act
    int result = umock_c_set_call_recorder(test_call_recorder);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_call_recorder, umockcallrecorder_clone_calls[0].umock_call_recorder);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

/* Tests_SRS_UMOCK_C_01_033: [ If the module is not initialized, umock_c_set_call_recorder shall return a non-zero value. ]*/
TEST_FUNCTION(when_the_module_is_not_initialize_umock_c_set_call_recorder_fails)
{
    // arrange
    (void)umock_c_init(test_on_umock_c_error);
    umock_c_deinit();
    reset_all_calls();

    // act
    int result = umock_c_set_call_recorder(test_call_recorder);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_clone_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcallrecorder_destroy_call_count);
}

END_TEST_SUITE(umock_c_unittests)
