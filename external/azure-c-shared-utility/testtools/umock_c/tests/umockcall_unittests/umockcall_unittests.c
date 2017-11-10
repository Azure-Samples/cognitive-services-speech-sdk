// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umockcall.h"

typedef struct test_mock_call_data_copy_CALL_TAG
{
    void* umockcall_data;
} test_mock_call_data_copy_CALL;

static test_mock_call_data_copy_CALL* test_mock_call_data_copy_calls;
static size_t test_mock_call_data_copy_call_count;
static void* test_mock_call_data_copy_expected_result;

typedef struct test_mock_call_data_free_CALL_TAG
{
    void* umockcall_data;
} test_mock_call_data_free_CALL;

static test_mock_call_data_free_CALL* test_mock_call_data_free_calls;
static size_t test_mock_call_data_free_call_count;

typedef struct test_mock_call_data_are_equal_CALL_TAG
{
    void* left;
    void* right;
} test_mock_call_data_are_equal_CALL;

static test_mock_call_data_are_equal_CALL* test_mock_call_data_are_equal_calls;
static size_t test_mock_call_data_are_equal_call_count;
static int test_mock_call_data_are_equal_expected_result;

typedef struct test_mock_call_data_stringify_CALL_TAG
{
    void* umockcall_data;
} test_mock_call_data_stringify_CALL;

static test_mock_call_data_stringify_CALL* test_mock_call_data_stringify_calls;
static size_t test_mock_call_data_stringify_call_count;
static char* test_mock_call_data_stringify_expected_result = NULL;

void* test_mock_call_data_copy(void* umockcall_data)
{
    test_mock_call_data_copy_CALL* new_calls = (test_mock_call_data_copy_CALL*)realloc(test_mock_call_data_copy_calls, sizeof(test_mock_call_data_copy_CALL) * (test_mock_call_data_copy_call_count + 1));
    if (new_calls != NULL)
    {
        test_mock_call_data_copy_calls = new_calls;
        test_mock_call_data_copy_calls[test_mock_call_data_copy_call_count].umockcall_data = umockcall_data;
        test_mock_call_data_copy_call_count++;
    }
    return test_mock_call_data_copy_expected_result;
}

void test_mock_call_data_free(void* umockcall_data)
{
    test_mock_call_data_free_CALL* new_calls = (test_mock_call_data_free_CALL*)realloc(test_mock_call_data_free_calls, sizeof(test_mock_call_data_free_CALL) * (test_mock_call_data_free_call_count + 1));
    if (new_calls != NULL)
    {
        test_mock_call_data_free_calls = new_calls;
        test_mock_call_data_free_calls[test_mock_call_data_free_call_count].umockcall_data = umockcall_data;
        test_mock_call_data_free_call_count ++;
    }
}

char* test_mock_call_data_stringify(void* umockcall_data)
{
    test_mock_call_data_stringify_CALL* new_calls = (test_mock_call_data_stringify_CALL*)realloc(test_mock_call_data_stringify_calls, sizeof(test_mock_call_data_stringify_CALL) * (test_mock_call_data_stringify_call_count + 1));
    if (new_calls != NULL)
    {
        test_mock_call_data_stringify_calls = new_calls;
        test_mock_call_data_stringify_calls[test_mock_call_data_stringify_call_count].umockcall_data = umockcall_data;
        test_mock_call_data_stringify_call_count++;
    }
    return test_mock_call_data_stringify_expected_result;
}

int test_mock_call_data_are_equal(void* left, void* right)
{
    test_mock_call_data_are_equal_CALL* new_calls = (test_mock_call_data_are_equal_CALL*)realloc(test_mock_call_data_are_equal_calls, sizeof(test_mock_call_data_are_equal_CALL) * (test_mock_call_data_are_equal_call_count + 1));
    if (new_calls != NULL)
    {
        test_mock_call_data_are_equal_calls = new_calls;
        test_mock_call_data_are_equal_calls[test_mock_call_data_are_equal_call_count].left = left;
        test_mock_call_data_are_equal_calls[test_mock_call_data_are_equal_call_count].right = right;
        test_mock_call_data_are_equal_call_count++;
    }
    return test_mock_call_data_are_equal_expected_result;
}

int another_test_mock_call_data_are_equal(void* left, void* right)
{
    (void)left, right;
    return 1;
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

BEGIN_TEST_SUITE(umockcall_unittests)

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

    test_mock_call_data_free_calls = NULL;
    test_mock_call_data_free_call_count = 0;

    test_mock_call_data_are_equal_calls = NULL;
    test_mock_call_data_are_equal_call_count = 0;
    test_mock_call_data_are_equal_expected_result = 1;

    test_mock_call_data_copy_calls = NULL;
    test_mock_call_data_copy_call_count = 0;

    test_mock_call_data_stringify_calls = NULL;
    test_mock_call_data_stringify_call_count = 0;
    test_mock_call_data_stringify_expected_result = NULL;

    reset_malloc_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    free(test_mock_call_data_copy_calls);
    test_mock_call_data_copy_calls = NULL;
    test_mock_call_data_copy_call_count = 0;

    free(test_mock_call_data_free_calls);
    test_mock_call_data_free_calls = NULL;
    test_mock_call_data_free_call_count = 0;

    free(test_mock_call_data_are_equal_calls);
    test_mock_call_data_are_equal_calls = NULL;
    test_mock_call_data_are_equal_call_count = 0;

    free(test_mock_call_data_stringify_calls);
    test_mock_call_data_stringify_calls = NULL;
    test_mock_call_data_stringify_call_count = 0;

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umockcall_create */

/* Tests_SRS_UMOCKCALL_01_001: [ umockcall_create shall create a new instance of a umock call and on success it shall return a non-NULL handle to it. ] */
TEST_FUNCTION(umockcall_create_with_proper_args_succeeds)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NOT_NULL(result);

    // cleanup
    umockcall_destroy(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_function_name_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create(NULL, (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_call_data_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", NULL, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_call_data_copy_function_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, NULL, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_call_data_free_function_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, NULL, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_call_data_stringify_function_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, NULL, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
TEST_FUNCTION(umockcall_create_with_NULL_call_data_are_equal_function_fails)
{
    // arrange

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_call_fails_then_umockcall_create_fails)
{
    // arrange
    when_shall_malloc_fail = 1;

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_function_name_fails_then_umockcall_create_fails)
{
    // arrange
    when_shall_malloc_fail = 2;

    // act
    UMOCKCALL_HANDLE result = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // assert
    ASSERT_IS_NULL(result);
}

/* umockcall_destroy */

/* Tests_SRS_UMOCKCALL_01_004: [ umockcall_destroy shall free a previously allocated umock call instance. ] */
TEST_FUNCTION(umockcall_destroy_frees_call_data)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    reset_malloc_calls();

    // act
    umockcall_destroy(call);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, test_mock_call_data_free_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, test_mock_call_data_free_calls[0].umockcall_data);
    ASSERT_ARE_EQUAL(size_t, 2, free_call_count);
}

/* Tests_SRS_UMOCKCALL_01_005: [ If the umockcall argument is NULL then umockcall_destroy shall do nothing. ]*/
TEST_FUNCTION(umockcall_destroy_with_NULL_argument_does_nothing)
{
    // arrange

    // act
    umockcall_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, test_mock_call_data_free_call_count);
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
}

/* umockcall_are_equal */

/* Tests_SRS_UMOCKCALL_01_006: [ umockcall_are_equal shall compare the two mock calls and return whether they are equal or not. ] */
/* Tests_SRS_UMOCKCALL_01_024: [ If both left and right pointers are equal, umockcall_are_equal shall return 1. ] */
TEST_FUNCTION(umockcall_are_equal_with_the_same_call_for_both_args_returns_1)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_are_equal(call, call);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_006: [ umockcall_are_equal shall compare the two mock calls and return whether they are equal or not. ] */
/* Tests_SRS_UMOCKCALL_01_027: [ If the underlying umockcall_data_are_equal returns 1, then umockcall_are_equal shall return 1. ]*/
TEST_FUNCTION(umockcall_are_equal_with_2_equal_calls_returns_1)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function", (void*)0x4243, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, test_mock_call_data_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4243, test_mock_call_data_are_equal_calls[0].right);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* Tests_SRS_UMOCKCALL_01_024: [ If both left and right pointers are equal, umockcall_are_equal shall return 1. ] */
TEST_FUNCTION(when_left_and_right_are_NULL_umockcall_are_equal_returns_1)
{
    // arrange

    // act
    int result = umockcall_are_equal(NULL, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);
}

/* Tests_SRS_UMOCKCALL_01_015: [ If only one of the left or right arguments are NULL, umockcall_are_equal shall return 0. ] */
TEST_FUNCTION(when_only_left_is_NULL_umockcall_are_equal_returns_0)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_are_equal(NULL, call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_015: [ If only one of the left or right arguments are NULL, umockcall_are_equal shall return 0. ] */
TEST_FUNCTION(when_only_right_is_NULL_umockcall_are_equal_returns_0)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_are_equal(call, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_025: [ If the function name does not match for the 2 calls, umockcall_are_equal shall return 0. ]*/
TEST_FUNCTION(when_the_function_name_does_not_match_then_umockcall_are_equal_returns_0)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function_1", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function_2", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_are_equal_expected_result = -1;

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* Tests_SRS_UMOCKCALL_01_026: [ The call data shall be evaluated by calling the umockcall_data_are_equal function passed in umockcall_create while passing as arguments the umockcall_data associated with each call handle. ]*/
/* Tests_SRS_UMOCKCALL_01_028: [ If the underlying umockcall_data_are_equal returns 0, then umockcall_are_equal shall return 0. ]*/
TEST_FUNCTION(when_the_underlying_are_equal_returns_0_umockcall_are_equal_returns_0)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function", (void*)0x4243, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_are_equal_expected_result = 0;

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, test_mock_call_data_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4243, test_mock_call_data_are_equal_calls[0].right);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* Tests_SRS_UMOCKCALL_01_029: [ If the underlying umockcall_data_are_equal fails (returns anything else than 0 or 1), then umockcall_are_equal shall fail and return -1. ] */
TEST_FUNCTION(when_the_underlying_are_equal_returns_minus_1_umockcall_are_equal_returns_minus_1)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function", (void*)0x4243, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_are_equal_expected_result = -1;

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, test_mock_call_data_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4243, test_mock_call_data_are_equal_calls[0].right);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* Tests_SRS_UMOCKCALL_01_029: [ If the underlying umockcall_data_are_equal fails (returns anything else than 0 or 1), then umockcall_are_equal shall fail and return -1. ] */
TEST_FUNCTION(when_the_underlying_are_equal_returns_2_umockcall_are_equal_returns_minus_1)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function", (void*)0x4243, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_are_equal_expected_result = 2;

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_are_equal_call_count);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, test_mock_call_data_are_equal_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4243, test_mock_call_data_are_equal_calls[0].right);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* Tests_SRS_UMOCKCALL_01_014: [ If the two calls have different are_equal functions that have been passed to umockcall_create then the calls shall be considered different and 0 shall be returned. ] */
TEST_FUNCTION(when_the_are_equal_function_pointers_are_different_umockcall_are_equal_returns_0)
{
    // arrange
    UMOCKCALL_HANDLE call1 = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2 = umockcall_create("test_function", (void*)0x4243, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, another_test_mock_call_data_are_equal);

    test_mock_call_data_are_equal_expected_result = 2;

    // act
    int result = umockcall_are_equal(call1, call2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, test_mock_call_data_are_equal_call_count);

    // cleanup
    umockcall_destroy(call1);
    umockcall_destroy(call2);
}

/* umockcall_stringify */

/* Tests_SRS_UMOCKCALL_01_016: [ umockcall_stringify shall return a string representation of the mock call in the form \[function_name(arguments)\]. ] */
/* Tests_SRS_UMOCKCALL_01_018: [ The returned string shall be a newly allocated string and it is to be freed by the caller. ]*/
/* Tests_SRS_UMOCKCALL_01_019: [ To obtain the arguments string, umockcall_stringify shall call the umockcall_data_stringify function passed to umockcall_create and pass to it the umockcall_data pointer (also given in umockcall_create). ]*/
TEST_FUNCTION(umockcall_stringify_calls_the_underlying_stringify_function_and_returns_the_strinfigied_call)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_stringify_expected_result = (char*)malloc(1);
    test_mock_call_data_stringify_expected_result[0] = '\0';

    // act
    char* result = umockcall_stringify(call);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_function()]", result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_stringify_call_count);

    // cleanup
    free(result);
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_016: [ umockcall_stringify shall return a string representation of the mock call in the form \[function_name(arguments)\]. ] */
/* Tests_SRS_UMOCKCALL_01_018: [ The returned string shall be a newly allocated string and it is to be freed by the caller. ]*/
/* Tests_SRS_UMOCKCALL_01_019: [ To obtain the arguments string, umockcall_stringify shall call the umockcall_data_stringify function passed to umockcall_create and pass to it the umockcall_data pointer (also given in umockcall_create). ]*/
TEST_FUNCTION(umockcall_stringify_uses_the_stringified_args_as_obtained_from_the_underlying_stringify_function)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_stringify_expected_result = (char*)malloc(strlen("45") + 1);
    (void)strcpy(test_mock_call_data_stringify_expected_result, "45");

    // act
    char* result = umockcall_stringify(call);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "[test_function(45)]", result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_stringify_call_count);

    // cleanup
    free(result);
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_017: [ If the umockcall argument is NULL, umockcall_stringify shall return NULL. ] */
TEST_FUNCTION(umockcall_stringify_with_NULL_fails_and_returns_NULL)
{
    // arrange

    // act
    char* result = umockcall_stringify(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_020: [ If the underlying umockcall_data_stringify call fails, umockcall_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(when_the_underlying_stringify_fails_then_umockcall_stringify_calls_fails)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    test_mock_call_data_stringify_expected_result = NULL;

    // act
    char* result = umockcall_stringify(call);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_stringify_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_021: [ If not enough memory can be allocated for the string to be returned, umockcall_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_umockcall_stringify_fails)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    test_mock_call_data_stringify_expected_result = (char*)malloc(strlen("45") + 1);
    reset_malloc_calls();
    when_shall_malloc_fail = 1;

    // act
    char* result = umockcall_stringify(call);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(int, 1, test_mock_call_data_stringify_call_count);
    ASSERT_ARE_EQUAL(int, 1, free_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* umockcall_get_call_data */

/* Tests_SRS_UMOCKCALL_01_022: [ umockcall_get_call_data shall return the associated umock call data that was passed to umockcall_create. ]*/
TEST_FUNCTION(umockcall_get_call_data_returns_the_call_data_pointer)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    void* result = umockcall_get_call_data(call);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_022: [ umockcall_get_call_data shall return the associated umock call data that was passed to umockcall_create. ]*/
TEST_FUNCTION(umockcall_get_call_data_with_NULL_returns_NULL)
{
    // arrange

    // act
    void* result = umockcall_get_call_data(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* umockcall_clone */

/* Tests_SRS_UMOCKCALL_01_031: [ umockcall_clone shall clone a umock call and on success it shall return a handle to the newly cloned call. ] */
/* Tests_SRS_UMOCKCALL_01_033: [ The call data shall be cloned by calling the umockcall_data_copy function passed in umockcall_create and passing as argument the umockcall_data value passed in umockcall_create. ]*/
/* Tests_SRS_UMOCKCALL_01_035: [ umockcall_clone shall copy also the function name. ]*/
/* Tests_SRS_UMOCKCALL_01_037: [ umockcall_clone shall also copy all the functions passed to umockcall_create (umockcall_data_copy, umockcall_data_free, umockcall_data_are_equal, umockcall_data_stringify). ]*/
TEST_FUNCTION(umockcall_clone_clones_the_call)
{
    // arrange
    char* stringified;
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    test_mock_call_data_copy_expected_result = (void*)0x4243;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call);

    // assert
    ASSERT_IS_NOT_NULL(result);
    test_mock_call_data_stringify_expected_result = (char*)calloc(1, 1);
    stringified = umockcall_stringify(result);
    ASSERT_ARE_EQUAL(char_ptr, "[test_function()]", stringified);

    // cleanup
    free(stringified);
    umockcall_destroy(call);
    umockcall_destroy(result);
}

#if 0
/* Tests_SRS_UMOCKCALL_01_032: [ If umockcall is NULL, umockcall_clone shall return NULL. ]*/
TEST_FUNCTION(umockcall_clone_with_NULL_handle_returns_NULL)
{
    // arrange
    test_mock_call_data_copy_expected_result = (void*)0x4243;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKCALL_01_043: [ If allocating memory for the new umock call fails, umockcall_clone shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umockcall_clone_fails)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    reset_malloc_calls();
    when_shall_malloc_fail = 1;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_034: [ If umockcall_data_copy fails then umockcall_clone shall return NULL. ]*/
TEST_FUNCTION(when_umockcall_data_copy_fails_then_umockcall_clone_clones_the_call)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    test_mock_call_data_copy_expected_result = NULL;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_036: [ If allocating memory for the function name fails, umockcall_clone shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_function_name_fails_umockcall_clone_fails)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    reset_malloc_calls();
    when_shall_malloc_fail = 2;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call);

    // assert
    ASSERT_IS_NULL(result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_037: [ umockcall_clone shall also copy all the functions passed to umockcall_create (umockcall_data_copy, umockcall_data_free, umockcall_data_are_equal, umockcall_data_stringify). ]*/
TEST_FUNCTION(umockcall_are_equal_on_2_clones_calls_succeeds)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    test_mock_call_data_copy_expected_result = (void*)0x4243;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call);

    // assert
    ASSERT_IS_NOT_NULL(result);
    test_mock_call_data_are_equal_expected_result = 1;
    ASSERT_ARE_EQUAL(int, 1, umockcall_are_equal(result, call));

    // cleanup
    umockcall_destroy(call);
    umockcall_destroy(result);
}

/* Tests_SRS_UMOCKCALL_01_037: [ umockcall_clone shall also copy all the functions passed to umockcall_create (umockcall_data_copy, umockcall_data_free, umockcall_data_are_equal, umockcall_data_stringify). ]*/
TEST_FUNCTION(umockcall_clone_on_a_cloned_call_succeeds)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    UMOCKCALL_HANDLE call2;
    test_mock_call_data_copy_expected_result = (void*)0x4243;
    call2 = umockcall_clone(call);
    test_mock_call_data_copy_expected_result = (void*)0x4244;

    // act
    UMOCKCALL_HANDLE result = umockcall_clone(call2);

    // assert
    ASSERT_IS_NOT_NULL(result);

    // cleanup
    umockcall_destroy(call);
    umockcall_destroy(call2);
    umockcall_destroy(result);
}

/* Tests_SRS_UMOCKCALL_01_037: [ umockcall_clone shall also copy all the functions passed to umockcall_create (umockcall_data_copy, umockcall_data_free, umockcall_data_are_equal, umockcall_data_stringify). ]*/
TEST_FUNCTION(freeing_a_cloned_call_succeeds)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    test_mock_call_data_copy_expected_result = (void*)0x4243;
    UMOCKCALL_HANDLE cloned_call = umockcall_clone(call);

    // act
    umockcall_destroy(cloned_call);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, test_mock_call_data_free_call_count);

    // cleanup
    umockcall_destroy(call);
}

/* umockcall_set_fail_call */

/* Tests_SRS_UMOCKCALL_01_038: [ umockcall_set_fail_call shall store the fail_call value, associating it with the umockcall call instance. ]*/
/* Tests_SRS_UMOCKCALL_01_044: [ On success umockcall_set_fail_call shall return 0. ]*/
TEST_FUNCTION(umockcall_set_fail_call_sets_the_fail_call_property)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_set_fail_call(call, 1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umockcall_get_fail_call(call));

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_039: [ If umockcall is NULL, umockcall_set_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(umockcall_set_fail_call_with_NULL_fails)
{
    // arrange

    // act
    int result = umockcall_set_fail_call(NULL, 1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKCALL_01_040: [ If a value different than 0 and 1 is passed as fail_call, umockcall_set_fail_call shall return a non-zero value. ]*/
TEST_FUNCTION(umockcall_set_fail_call_with_an_invalid_fail_Call_value_fails)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);

    // act
    int result = umockcall_set_fail_call(call, 2);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    umockcall_destroy(call);
}

/* umockcall_get_fail_call */

/* Tests_SRS_UMOCKCALL_01_041: [ umockcall_get_fail_call shall retrieve the fail_call value, associated with the umockcall call instance. ]*/
TEST_FUNCTION(umockcall_get_fail_call_retrieves_0)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    (void)umockcall_set_fail_call(call, 0);

    // act
    int result = umockcall_get_fail_call(call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_041: [ umockcall_get_fail_call shall retrieve the fail_call value, associated with the umockcall call instance. ]*/
TEST_FUNCTION(umockcall_get_fail_call_retrieves_1)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    (void)umockcall_set_fail_call(call, 1);

    // act
    int result = umockcall_get_fail_call(call);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);

    // cleanup
    umockcall_destroy(call);
}

/* Tests_SRS_UMOCKCALL_01_042: [ If umockcall is NULL, umockcall_get_fail_call shall return -1. ]*/
TEST_FUNCTION(umockcall_get_fail_call_with_NULL_call_fails)
{
    // arrange

    // act
    int result = umockcall_get_fail_call(NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKCALL_01_041: [ umockcall_get_fail_call shall retrieve the fail_call value, associated with the umockcall call instance. ]*/
TEST_FUNCTION(umockcall_get_fail_call_on_a_cloned_call_retrieves_1)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    (void)umockcall_set_fail_call(call, 1);
    test_mock_call_data_copy_expected_result = (void*)0x4243;
    UMOCKCALL_HANDLE cloned_call = umockcall_clone(call);

    // act
    int result = umockcall_get_fail_call(cloned_call);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);

    // cleanup
    umockcall_destroy(call);
    umockcall_destroy(cloned_call);
}

/* Tests_SRS_UMOCKCALL_01_041: [ umockcall_get_fail_call shall retrieve the fail_call value, associated with the umockcall call instance. ]*/
TEST_FUNCTION(umockcall_get_fail_call_on_a_cloned_call_retrieves_0)
{
    // arrange
    UMOCKCALL_HANDLE call = umockcall_create("test_function", (void*)0x4242, test_mock_call_data_copy, test_mock_call_data_free, test_mock_call_data_stringify, test_mock_call_data_are_equal);
    (void)umockcall_set_fail_call(call, 0);
    test_mock_call_data_copy_expected_result = (void*)0x4243;
    UMOCKCALL_HANDLE cloned_call = umockcall_clone(call);

    // act
    int result = umockcall_get_fail_call(cloned_call);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    umockcall_destroy(call);
    umockcall_destroy(cloned_call);
}
#endif

END_TEST_SUITE(umockcall_unittests)
