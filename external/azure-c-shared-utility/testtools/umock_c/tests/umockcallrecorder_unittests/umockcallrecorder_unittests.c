// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umockcallrecorder.h"
#include "umockcall.h"

static UMOCKCALL_HANDLE test_expected_umockcall_1 = (UMOCKCALL_HANDLE)0x4242;
static UMOCKCALL_HANDLE test_expected_umockcall_2 = (UMOCKCALL_HANDLE)0x4243;
static UMOCKCALL_HANDLE test_actual_umockcall_1 = (UMOCKCALL_HANDLE)0x4244;
static UMOCKCALL_HANDLE test_actual_umockcall_2 = (UMOCKCALL_HANDLE)0x4245;

typedef struct umockcall_are_equal_CALL_TAG
{
    UMOCKCALL_HANDLE left;
    UMOCKCALL_HANDLE right;
} umockcall_are_equal_CALL;

static umockcall_are_equal_CALL* umockcall_are_equal_calls;
static size_t umockcall_are_equal_call_count;
static int umockcall_are_equal_call_result;

typedef struct umockcall_clone_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_clone_CALL;

static umockcall_clone_CALL* umockcall_clone_calls;
static size_t umockcall_clone_call_count;
static size_t when_shall_umockcall_clone_fail;
static UMOCKCALL_HANDLE umockcall_clone_call_result;

typedef struct umockcall_set_fail_call_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_set_fail_call_CALL;

static umockcall_set_fail_call_CALL* umockcall_set_fail_call_calls;
static size_t umockcall_set_fail_call_call_count;
static int umockcall_set_fail_call_call_result;

typedef struct umockcall_get_fail_call_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_get_fail_call_CALL;

static umockcall_get_fail_call_CALL* umockcall_get_fail_call_calls;
static size_t umockcall_get_fail_call_call_count;
static int umockcall_get_fail_call_call_result;

typedef struct umockcall_destroy_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_destroy_CALL;

static umockcall_destroy_CALL* umockcall_destroy_calls;
static size_t umockcall_destroy_call_count;
static int umockcall_destroy_call_result;

typedef struct umockcall_stringify_CALL_TAG
{
    UMOCKCALL_HANDLE umockcall;
} umockcall_stringify_CALL;

static umockcall_stringify_CALL* umockcall_stringify_calls;
static size_t umockcall_stringify_call_count;
static char* umockcall_stringify_call_result;

int umockcall_are_equal(UMOCKCALL_HANDLE left, UMOCKCALL_HANDLE right)
{
    umockcall_are_equal_CALL* new_calls = (umockcall_are_equal_CALL*)realloc(umockcall_are_equal_calls, sizeof(umockcall_are_equal_CALL) * (umockcall_are_equal_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_are_equal_calls = new_calls;
        umockcall_are_equal_calls[umockcall_are_equal_call_count].left = left;
        umockcall_are_equal_calls[umockcall_are_equal_call_count].right = right;
        umockcall_are_equal_call_count++;
    }

    return umockcall_are_equal_call_result;
}

void umockcall_destroy(UMOCKCALL_HANDLE umockcall)
{
    umockcall_destroy_CALL* new_calls = (umockcall_destroy_CALL*)realloc(umockcall_destroy_calls, sizeof(umockcall_destroy_CALL) * (umockcall_destroy_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_destroy_calls = new_calls;
        umockcall_destroy_calls[umockcall_destroy_call_count].umockcall = umockcall;
        umockcall_destroy_call_count++;
    }
}

UMOCKCALL_HANDLE umockcall_clone(UMOCKCALL_HANDLE umockcall)
{
    UMOCKCALL_HANDLE result;
    umockcall_clone_CALL* new_calls = (umockcall_clone_CALL*)realloc(umockcall_clone_calls, sizeof(umockcall_clone_CALL) * (umockcall_clone_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_clone_calls = new_calls;
        umockcall_clone_calls[umockcall_clone_call_count].umockcall = umockcall;
        umockcall_clone_call_count++;
    }

    if ((when_shall_umockcall_clone_fail > 0) && (when_shall_umockcall_clone_fail == umockcall_clone_call_count))
    {
        result = NULL;
    }
    else
    {
        result = umockcall_clone_call_result;
    }

    return result;
}

int umockcall_set_fail_call(UMOCKCALL_HANDLE umockcall, int fail_call)
{
    umockcall_set_fail_call_CALL* new_calls = (umockcall_set_fail_call_CALL*)realloc(umockcall_set_fail_call_calls, sizeof(umockcall_set_fail_call_CALL) * (umockcall_set_fail_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_set_fail_call_calls = new_calls;
        umockcall_set_fail_call_calls[umockcall_set_fail_call_call_count].umockcall = umockcall;
        umockcall_set_fail_call_call_count++;
    }

    return umockcall_set_fail_call_call_result;
}

int umockcall_get_fail_call(UMOCKCALL_HANDLE umockcall)
{
    umockcall_get_fail_call_CALL* new_calls = (umockcall_get_fail_call_CALL*)realloc(umockcall_get_fail_call_calls, sizeof(umockcall_get_fail_call_CALL) * (umockcall_get_fail_call_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_get_fail_call_calls = new_calls;
        umockcall_get_fail_call_calls[umockcall_get_fail_call_call_count].umockcall = umockcall;
        umockcall_get_fail_call_call_count++;
    }

    return umockcall_get_fail_call_call_result;
}

char* umockcall_stringify(UMOCKCALL_HANDLE umockcall)
{
    char* result;

    umockcall_stringify_CALL* new_calls = (umockcall_stringify_CALL*)realloc(umockcall_stringify_calls, sizeof(umockcall_stringify_CALL) * (umockcall_stringify_call_count + 1));
    if (new_calls != NULL)
    {
        umockcall_stringify_calls = new_calls;
        umockcall_stringify_calls[umockcall_stringify_call_count].umockcall = umockcall;
        umockcall_stringify_call_count++;
    }

    if (umockcall_stringify_call_result == NULL)
    {
        result = NULL;
    }
    else
    {
        result = (char*)malloc(strlen(umockcall_stringify_call_result) + 1);
        if (result != NULL)
        {
            (void)strcpy(result, umockcall_stringify_call_result);
        }
    }

    return result;
}

void reset_umockcall_are_equal_calls(void)
{
    if (umockcall_are_equal_calls != NULL)
    {
        free(umockcall_are_equal_calls);
        umockcall_are_equal_calls = NULL;
    }

    umockcall_are_equal_call_count = 0;
}

void reset_umockcall_destroy_calls(void)
{
    if (umockcall_destroy_calls != NULL)
    {
        free(umockcall_destroy_calls);
        umockcall_destroy_calls = NULL;
    }

    umockcall_destroy_call_count = 0;
}

void reset_umockcall_stringify_calls(void)
{
    if (umockcall_stringify_calls != NULL)
    {
        free(umockcall_stringify_calls);
        umockcall_stringify_calls = NULL;
    }

    umockcall_stringify_call_count = 0;
}

void reset_umockcall_clone_calls(void)
{
    if (umockcall_clone_calls != NULL)
    {
        free(umockcall_clone_calls);
        umockcall_clone_calls = NULL;
    }

    when_shall_umockcall_clone_fail = 0;
    umockcall_clone_call_count = 0;
}

void reset_umockcall_set_fail_call_calls(void)
{
    if (umockcall_set_fail_call_calls != NULL)
    {
        free(umockcall_set_fail_call_calls);
        umockcall_set_fail_call_calls = NULL;
    }

    umockcall_set_fail_call_call_result = 0;
    umockcall_set_fail_call_call_count = 0;
}

static size_t malloc_call_count;
static size_t realloc_call_count;
static size_t free_call_count;

static size_t when_shall_malloc_fail;
static size_t when_shall_realloc_fail;

#ifdef __cplusplus
extern "C" {
#endif

    void* mock_malloc(size_t size)
    {
        void* result;
        malloc_call_count++;
        if (malloc_call_count == when_shall_malloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = malloc(size);
        }
        return result;
    }

    void* mock_realloc(void* ptr, size_t size)
    {
        void* result;
        realloc_call_count++;
        if (realloc_call_count == when_shall_realloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
        }
        return result;
    }

    void mock_free(void* ptr)
    {
        free_call_count++;
        free(ptr);
    }

#ifdef __cplusplus
}
#endif

void reset_malloc_calls(void)
{
    malloc_call_count = 0;
    when_shall_malloc_fail = 0;
    realloc_call_count = 0;
    when_shall_realloc_fail = 0;
    free_call_count = 0;
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

BEGIN_TEST_SUITE(umockcallrecorder_unittests)

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

    umockcall_are_equal_call_result = 1;
    reset_umockcall_are_equal_calls();
    reset_umockcall_destroy_calls();
    reset_umockcall_stringify_calls();
    reset_umockcall_clone_calls();
    reset_umockcall_set_fail_call_calls();
    reset_malloc_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    TEST_MUTEX_RELEASE(test_mutex);
}

/* umockcallrecorder_create */

/* Tests_SRS_UMOCKCALLRECORDER_01_001: [ umockcallrecorder_create shall create a new instance of a call recorder and return a non-NULL handle to it on success. ]*/
TEST_FUNCTION(umockcallrecorder_create_succeeds)
{
    // arrange

    // act
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_002: [ If allocating memory for the call recorder fails, umockcallrecorder_create shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_umockcallrecorder_create_fails)
{
    // arrange
    when_shall_malloc_fail = 1;

    // act
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // assert
    ASSERT_IS_NULL(call_recorder);
}

/* umockcallrecorder_destroy */

/* Tests_SRS_UMOCKCALLRECORDER_01_003: [ umockcallrecorder_destroy shall free the resources associated with a the call recorder identified by the umock_call_recorder argument. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_frees_the_call_recorder_resources)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    umockcallrecorder_destroy(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);
    ASSERT_ARE_EQUAL(size_t, 3, free_call_count);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_004: [ If umock_call_recorder is NULL, umockcallrecorder_destroy shall do nothing. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_with_NULL_does_nothing)
{
    // arrange

    // act
    umockcallrecorder_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_003: [ umockcallrecorder_destroy shall free the resources associated with a the call recorder identified by the umock_call_recorder argument. ]*/
TEST_FUNCTION(umockcallrecorder_destroy_with_one_expected_call_frees_the_call_recorder_resources)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_are_equal_call_result = 0;

    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    umockcallrecorder_destroy(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(call_recorder);
    /* 5 = 1 for actual calls, 1 for expected calls, 2 for the strings and one for the call recorder structure */
    ASSERT_ARE_EQUAL(size_t, 5, free_call_count);
    ASSERT_ARE_EQUAL(size_t, 3, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_1, umockcall_destroy_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_2, umockcall_destroy_calls[1].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_actual_umockcall_1, umockcall_destroy_calls[2].umockcall);
}

/* umockcallrecorder_reset_all_calls */

/* Tests_SRS_UMOCKCALLRECORDER_01_005: [ umockcallrecorder_reset_all_calls shall free all the expected and actual calls for the call recorder identified by umock_call_recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_006: [ On success umockcallrecorder_reset_all_calls shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_reset_all_calls_frees_all_existing_expected_and_actual_calls)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_are_equal_call_result = 0;

    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    int result = umockcallrecorder_reset_all_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 2, free_call_count);
    ASSERT_ARE_EQUAL(size_t, 3, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_1, umockcall_destroy_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_expected_umockcall_2, umockcall_destroy_calls[1].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, (void*)test_actual_umockcall_1, umockcall_destroy_calls[2].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_007: [ If umock_call_recorder is NULL, umockcallrecorder_reset_all_calls shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_reset_all_calls_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    int result = umockcallrecorder_reset_all_calls(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_destroy_call_count);
}

/* umockcallrecorder_add_expected_call */

/* Tests_SRS_UMOCKCALLRECORDER_01_008: [ umockcallrecorder_add_expected_call shall add the mock_call call to the expected call list maintained by the call recorder identified by umock_call_recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_009: [ On success umockcallrecorder_add_expected_call shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_add_expected_call_adds_an_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    int result = umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_012: [ If any of the arguments is NULL, umockcallrecorder_add_expected_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_add_expected_call_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    int result = umockcallrecorder_add_expected_call(NULL, test_expected_umockcall_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_012: [ If any of the arguments is NULL, umockcallrecorder_add_expected_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_add_expected_call_with_NULL_mock_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    int result = umockcallrecorder_add_expected_call(call_recorder, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_013: [ If allocating memory for the expected calls fails, umockcallrecorder_add_expected_call shall fail and return a non-zero value. ] */
TEST_FUNCTION(when_allocating_memory_fails_umockcallrecorder_add_expected_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    reset_malloc_calls();
    when_shall_realloc_fail = 1;

    // act
    int result = umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_add_actual_call */

/* Tests_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_018: [ When no error is encountered, umockcallrecorder_add_actual_call shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_add_actual_call_without_any_expected_calls_succeeds)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_NULL(matched_call);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_018: [ When no error is encountered, umockcallrecorder_add_actual_call shall return 0. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_017: [ Comparing the calls shall be done by calling umockcall_are_equal. ]*/
TEST_FUNCTION(umockcallrecorder_add_actual_call_with_a_call_that_does_not_match_succeeds)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcall_are_equal_call_result = 0;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_actual_umockcall_1, umockcall_are_equal_calls[0].right);
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
    ASSERT_IS_NULL(matched_call);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_019: [ If any of the arguments is NULL, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_add_actual_call_with_NULL_call_recorder_fails)
{
    // arrange
    UMOCKCALL_HANDLE matched_call;

    // act
    int result = umockcallrecorder_add_actual_call(NULL, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_019: [ If any of the arguments is NULL, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_add_actual_call_with_NULL_actual_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, NULL, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_019: [ If any of the arguments is NULL, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_add_actual_call_with_NULL_matched_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_020: [ If allocating memory for the actual calls fails, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_actual_calls_fails_umockcallrecorder_add_actual_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    reset_malloc_calls();
    when_shall_realloc_fail = 1;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_021: [ If umockcall_are_equal fails, umockcallrecorder_add_actual_call shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_umockcall_are_equal_fails_then_umockcallrecorder_add_actual_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcall_are_equal_call_result = -1;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_actual_umockcall_1, umockcall_are_equal_calls[0].right);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_016: [ If the call matches one of the expected calls, a handle to the matched call shall be filled into the matched_call argument. ]*/
TEST_FUNCTION(when_the_actual_call_matches_the_expected_call_then_the_matched_call_is_returned)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcall_are_equal_call_result = 1;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_actual_umockcall_1, umockcall_are_equal_calls[0].right);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, matched_call);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_destroy_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_016: [ If the call matches one of the expected calls, a handle to the matched call shall be filled into the matched_call argument. ]*/
TEST_FUNCTION(when_the_actual_call_matches_the_1st_of_2_expected_call_then_the_matched_call_is_returned)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    umockcall_are_equal_call_result = 1;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_actual_umockcall_1, umockcall_are_equal_calls[0].right);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, matched_call);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_destroy_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_016: [ If the call matches one of the expected calls, a handle to the matched call shall be filled into the matched_call argument. ]*/
TEST_FUNCTION(when_the_actual_call_matches_the_2nd_of_2_expected_call_then_the_matched_call_is_returned)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    umockcall_are_equal_call_result = 1;
    umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_umockcall_are_equal_calls();
    reset_umockcall_destroy_calls();
    umockcall_are_equal_call_result = 1;

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_2, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL_WITH_MSG(size_t, 1, umockcall_are_equal_call_count, "Incorrect umock_are_equal call count");
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_actual_umockcall_2, umockcall_are_equal_calls[0].right);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, matched_call);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_destroy_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_018: [ When no error is encountered, umockcallrecorder_add_actual_call shall return 0. ]*/
TEST_FUNCTION(two_actual_calls_can_be_added)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_2, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
TEST_FUNCTION(only_the_first_expected_call_is_checked_for_match)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_are_equal_call_result = 0;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);

    // act
    int result = umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_NULL(matched_call);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_are_equal_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_014: [ umockcallrecorder_add_actual_call shall check whether the call mock_call matches any of the expected calls maintained by umock_call_recorder. ]*/
TEST_FUNCTION(if_matching_fails_subsequent_actual_calls_are_not_matched)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    umockcall_are_equal_call_result = 0;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    umockcall_are_equal_call_result = 1;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_2, &matched_call);

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()][a()]", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_get_actual_calls */

/* Tests_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
TEST_FUNCTION(umockcallrecorder_get_actual_calls_with_no_calls_returns_an_empty_string)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_023: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_actual_calls_with_1_call_returns_one_stringified_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_023: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_actual_calls_with_2_calls_returns_the_stringified_calls)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()][a()]", result);
    ASSERT_ARE_EQUAL(size_t, 2, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_025: [ If umockcall_stringify fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_stringifying_one_call_fails_then_umockcallrecorder_get_actual_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = NULL;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_024: [ If the umock_call_recorder is NULL, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_get_actual_calls_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    const char* result = umockcallrecorder_get_actual_calls(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_026: [ If allocating memory for the resulting string fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_resulting_string_for_no_calls_fails_then_umockcallrecorder_get_actual_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    reset_malloc_calls();

    when_shall_realloc_fail = 1;

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_026: [ If allocating memory for the resulting string fails, umockcallrecorder_get_actual_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_resulting_string_fails_then_umockcallrecorder_get_actual_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    when_shall_realloc_fail = 1;

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_022: [ umockcallrecorder_get_actual_calls shall return a pointer to the string representation of all the actual calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_023: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_actual_calls_when_the_actual_call_does_not_match_the_expected_should_return_the_actual_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    umockcall_are_equal_call_result = 0;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_actual_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_get_expected_calls */

/* Tests_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_with_no_calls_returns_an_empty_string)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_028: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_with_1_call_returns_one_stringified_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_028: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_with_2_calls_returns_the_stringified_calls)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()][a()]", result);
    ASSERT_ARE_EQUAL(size_t, 2, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_030: [ If umockcall_stringify fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_stringifying_one_call_fails_then_umockcallrecorder_get_expected_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    umockcall_stringify_call_result = NULL;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_029: [ If the umock_call_recorder is NULL, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    const char* result = umockcallrecorder_get_expected_calls(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_031: [ If allocating memory for the resulting string fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_resulting_string__for_no_calls_fails_then_umockcallrecorder_get_expected_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    reset_malloc_calls();

    when_shall_realloc_fail = 1;

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_031: [ If allocating memory for the resulting string fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_resulting_string_fails_then_umockcallrecorder_get_expected_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    umockcall_stringify_call_result = "[a()]";
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_malloc_calls();

    when_shall_realloc_fail = 1;

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, realloc_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_031: [ If allocating memory for the resulting string fails, umockcallrecorder_get_expected_calls shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_with_a_matched_expected_call_yields_no_calls_in_the_string)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_are_equal_call_result = 1;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_027: [ umockcallrecorder_get_expected_calls shall return a pointer to the string representation of all the expected calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_028: [ The string for each call shall be obtained by calling umockcall_stringify. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_calls_when_the_actual_call_does_not_match_the_expected_should_return_the_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    umockcall_stringify_call_result = "[a()]";
    umockcall_are_equal_call_result = 0;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_actual_umockcall_1, &matched_call);
    reset_malloc_calls();

    // act
    const char* result = umockcallrecorder_get_expected_calls(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[a()]", result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_get_last_expected_call */

/* Tests_SRS_UMOCKCALLRECORDER_01_034: [ If no expected call has been recorded for umock_call_recorder then umockcallrecorder_get_last_expected_call shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_get_last_expected_call_without_any_expected_calls_returns_NULL)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    UMOCKCALL_HANDLE result = umockcallrecorder_get_last_expected_call(call_recorder);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_033: [ If umock_call_recorder is NULL, umockcallrecorder_get_last_expected_call shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_get_last_expected_call_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcallrecorder_get_last_expected_call(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_032: [ umockcallrecorder_get_last_expected_call shall return the last expected call for the umock_call_recorder call recorder. ]*/
TEST_FUNCTION(umockcallrecorder_get_last_expected_call_with_1_expected_call_returns_the_last_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);

    // act
    UMOCKCALL_HANDLE result = umockcallrecorder_get_last_expected_call(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_032: [ umockcallrecorder_get_last_expected_call shall return the last expected call for the umock_call_recorder call recorder. ]*/
TEST_FUNCTION(umockcallrecorder_get_last_expected_call_with_2_expected_calls_returns_the_last_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);

    // act
    UMOCKCALL_HANDLE result = umockcallrecorder_get_last_expected_call(call_recorder);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_clone */

/* Tests_SRS_UMOCKCALLRECORDER_01_035: [ umockcallrecorder_clone shall clone a call recorder and return a handle to the newly cloned call recorder. ]*/
TEST_FUNCTION(umockcallrecorder_clone_on_a_recorder_with_no_calls_succeeds)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
    umockcallrecorder_destroy(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_036: [ If the umock_call_recorder argument is NULL, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(umockcallrecorder_clone_with_NULL_returns_NULL)
{
    // arrange

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_037: [ If allocating memory for the new umock call recorder instance fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_call_recorder_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    reset_malloc_calls();
    when_shall_malloc_fail = 1;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_038: [ umockcallrecorder_clone shall clone all the expected calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_039: [ Each expected call shall be cloned by calling umockcall_clone. ]*/
TEST_FUNCTION(umockcallrecorder_clone_with_one_expected_call_clones_the_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
    umockcallrecorder_destroy(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_038: [ umockcallrecorder_clone shall clone all the expected calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_039: [ Each expected call shall be cloned by calling umockcall_clone. ]*/
TEST_FUNCTION(umockcallrecorder_clone_with_2_expected_calls_clones_the_expected_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 2, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_clone_calls[1].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
    umockcallrecorder_destroy(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_040: [ If cloning an expected call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_the_expected_call_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = NULL;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_040: [ If cloning an expected call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_the_1st_out_of_2_expected_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;
    when_shall_umockcall_clone_fail = 1;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_destroy_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_040: [ If cloning an expected call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_the_2nd_out_of_2_expected_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;
    when_shall_umockcall_clone_fail = 2;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 2, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_clone_calls[1].umockcall);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (UMOCKCALL_HANDLE)0x4245, umockcall_destroy_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_052: [ If allocating memory for the expected calls fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_expected_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    reset_umockcall_clone_calls();
    reset_malloc_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;
    when_shall_malloc_fail = 2;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_clone_call_count);
    
    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_041: [ umockcallrecorder_clone shall clone all the actual calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_042: [ Each actual call shall be cloned by calling umockcall_clone. ]*/
TEST_FUNCTION(umockcallrecorder_clone_with_one_actual_call_succeeds)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
    umockcallrecorder_destroy(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_041: [ umockcallrecorder_clone shall clone all the actual calls. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_042: [ Each actual call shall be cloned by calling umockcall_clone. ]*/
TEST_FUNCTION(umockcallrecorder_clone_with_2_actual_calls_succeeds)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_2, &matched_call);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 2, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_clone_calls[1].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
    umockcallrecorder_destroy(result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_043: [ If cloning an actual call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_actual_call_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = NULL;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_043: [ If cloning an actual call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_the_1st_of_2_actual_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_2, &matched_call);
    reset_umockcall_clone_calls();
    umockcall_clone_call_result = NULL;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_043: [ If cloning an actual call fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_cloning_the_2nd_of_2_actual_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_2, &matched_call);
    reset_umockcall_clone_calls();
    when_shall_umockcall_clone_fail = 2;
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 2, umockcall_clone_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_clone_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_clone_calls[1].umockcall);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_destroy_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (UMOCKCALL_HANDLE)0x4245, umockcall_destroy_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_053: [ If allocating memory for the actual calls fails, umockcallrecorder_clone shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_actual_calls_fails_umockcallrecorder_clone_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    UMOCKCALL_HANDLE matched_call;
    (void)umockcallrecorder_add_actual_call(call_recorder, test_expected_umockcall_1, &matched_call);
    reset_umockcall_clone_calls();
    reset_malloc_calls();
    umockcall_clone_call_result = (UMOCKCALL_HANDLE)0x4245;
    when_shall_malloc_fail = 2;

    // act
    UMOCKCALLRECORDER_HANDLE result = umockcallrecorder_clone(call_recorder);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_clone_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_get_expected_call_count */

/* Tests_SRS_UMOCKCALLRECORDER_01_044: [ umockcallrecorder_get_expected_call_count shall return in the expected_call_count argument the number of expected calls associated with the call recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_045: [ On success umockcallrecorder_get_expected_call_count shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_call_count_when_no_expected_calls_are_there_yields_0)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    size_t expected_call_count;

    // act
    int result = umockcallrecorder_get_expected_call_count(call_recorder, &expected_call_count);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, expected_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_044: [ umockcallrecorder_get_expected_call_count shall return in the expected_call_count argument the number of expected calls associated with the call recorder. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_045: [ On success umockcallrecorder_get_expected_call_count shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_call_count_when_2_expected_calls_are_there_yields_2)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    size_t expected_call_count;
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);

    // act
    int result = umockcallrecorder_get_expected_call_count(call_recorder, &expected_call_count);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 2, expected_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_046: [ If any of the arguments is NULL, umockcallrecorder_get_expected_call_count shall return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_call_with_NULL_call_recorder_fails)
{
    // arrange
    size_t expected_call_count;

    // act
    int result = umockcallrecorder_get_expected_call_count(NULL, &expected_call_count);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_046: [ If any of the arguments is NULL, umockcallrecorder_get_expected_call_count shall return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_get_expected_call_with_NULL_expected_call_count_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    int result = umockcallrecorder_get_expected_call_count(call_recorder, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* umockcallrecorder_fail_call */

/* Tests_SRS_UMOCKCALLRECORDER_01_047: [ umockcallrecorder_fail_call shall mark an expected call as to be failed by calling umockcall_set_fail_call with a 1 value for fail_call. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_048: [ On success, umockcallrecorder_fail_call shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_fail_call_sets_fail_call_to_1_for_the_first_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 0);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_set_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_set_fail_call_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_047: [ umockcallrecorder_fail_call shall mark an expected call as to be failed by calling umockcall_set_fail_call with a 1 value for fail_call. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_048: [ On success, umockcallrecorder_fail_call shall return 0. ]*/
TEST_FUNCTION(umockcallrecorder_fail_call_sets_fail_call_to_1_for_the_second_call)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_set_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_set_fail_call_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_047: [ umockcallrecorder_fail_call shall mark an expected call as to be failed by calling umockcall_set_fail_call with a 1 value for fail_call. ]*/
/* Tests_SRS_UMOCKCALLRECORDER_01_048: [ On success, umockcallrecorder_fail_call shall return 0. ]*/
TEST_FUNCTION(two_umockcallrecorder_fail_call_calls_succeed)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_2);
    umockcallrecorder_fail_call(call_recorder, 0);

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 2, umockcall_set_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_set_fail_call_calls[0].umockcall);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_2, umockcall_set_fail_call_calls[1].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_049: [ If umock_call_recorder is NULL, umockcallrecorder_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_fail_call_with_NULL_call_recorder_fails)
{
    // arrange

    // act
    int result = umockcallrecorder_fail_call(NULL, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_050: [ If index is invalid, umockcallrecorder_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_fail_call_with_index_too_high_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_set_fail_call_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_050: [ If index is invalid, umockcallrecorder_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(umockcallrecorder_fail_call_with_0_index_when_no_calls_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umockcall_set_fail_call_call_count);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

/* Tests_SRS_UMOCKCALLRECORDER_01_051: [ If umockcall_set_fail_call fails, umockcallrecorder_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(when_umockcall_set_fail_call_fails_then_umockcallrecorder_fail_call_fails)
{
    // arrange
    UMOCKCALLRECORDER_HANDLE call_recorder = umockcallrecorder_create();
    (void)umockcallrecorder_add_expected_call(call_recorder, test_expected_umockcall_1);
    umockcall_set_fail_call_call_result = 1;

    // act
    int result = umockcallrecorder_fail_call(call_recorder, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umockcall_set_fail_call_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_expected_umockcall_1, umockcall_set_fail_call_calls[0].umockcall);

    // cleanup
    umockcallrecorder_destroy(call_recorder);
}

END_TEST_SUITE(umockcallrecorder_unittests)
