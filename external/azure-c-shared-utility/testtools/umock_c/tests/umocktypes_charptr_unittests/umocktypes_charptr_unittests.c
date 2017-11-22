// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"
#include "umocktypes_charptr.h"

typedef struct umocktypes_register_type_CALL_TAG
{
    char* type;
    UMOCKTYPE_STRINGIFY_FUNC stringify_func;
    UMOCKTYPE_ARE_EQUAL_FUNC are_equal_func;
    UMOCKTYPE_COPY_FUNC copy_func;
    UMOCKTYPE_FREE_FUNC free_func;
} umocktypes_register_type_CALL;

static umocktypes_register_type_CALL* umocktypes_register_type_calls;
static size_t umocktypes_register_type_call_count;
static size_t when_shall_umocktypes_register_typecall_fail;
static int umocktypes_register_type_call_result;
static int umocktypes_register_type_fail_call_result;

int umocktypes_register_type(const char* type, UMOCKTYPE_STRINGIFY_FUNC stringify_func, UMOCKTYPE_ARE_EQUAL_FUNC are_equal_func, UMOCKTYPE_COPY_FUNC copy_func, UMOCKTYPE_FREE_FUNC free_func)
{
    int result;

    umocktypes_register_type_CALL* new_calls = (umocktypes_register_type_CALL*)realloc(umocktypes_register_type_calls, sizeof(umocktypes_register_type_CALL) * (umocktypes_register_type_call_count + 1));
    if (new_calls != NULL)
    {
        size_t typename_length = strlen(type);
        umocktypes_register_type_calls = new_calls;
        umocktypes_register_type_calls[umocktypes_register_type_call_count].type = (char*)malloc(typename_length + 1);
        (void)memcpy(umocktypes_register_type_calls[umocktypes_register_type_call_count].type, type, typename_length + 1);
        umocktypes_register_type_calls[umocktypes_register_type_call_count].stringify_func = stringify_func;
        umocktypes_register_type_calls[umocktypes_register_type_call_count].are_equal_func = are_equal_func;
        umocktypes_register_type_calls[umocktypes_register_type_call_count].copy_func = copy_func;
        umocktypes_register_type_calls[umocktypes_register_type_call_count].free_func = free_func;
        umocktypes_register_type_call_count++;
    }

    if (when_shall_umocktypes_register_typecall_fail == umocktypes_register_type_call_count)
    {
        result = umocktypes_register_type_fail_call_result;
    }
    else
    {
        result = umocktypes_register_type_call_result;
    }

    return result;
}

void reset_umocktypes_register_type_calls(void)
{
    if (umocktypes_register_type_calls != NULL)
    {
        size_t i;
        for (i = 0; i < umocktypes_register_type_call_count; i++)
        {
            free(umocktypes_register_type_calls[i].type);
        }

        free(umocktypes_register_type_calls);
        umocktypes_register_type_calls = NULL;
    }
    umocktypes_register_type_call_count = 0;
}

static size_t malloc_call_count;
static size_t calloc_call_count;
static size_t realloc_call_count;
static size_t free_call_count;

static size_t when_shall_malloc_fail;
static size_t when_shall_calloc_fail;
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

    void* mock_calloc(size_t nmemb, size_t size)
    {
        void* result;
        calloc_call_count++;
        if (calloc_call_count == when_shall_calloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = calloc(nmemb, size);
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
    calloc_call_count = 0;
    when_shall_calloc_fail = 0;
    realloc_call_count = 0;
    when_shall_realloc_fail = 0;
    free_call_count = 0;
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

BEGIN_TEST_SUITE(umocktypes_charptr_unittests)

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

    reset_malloc_calls();
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    reset_umocktypes_register_type_calls();

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umocktypes_stringify_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_002: [ umocktypes_stringify_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
TEST_FUNCTION(umocktypes_stringify_charptr_with_an_empty_string_returns_2_quotes)
{
    // arrange
    char* input = "";

    // act
    char* result = umocktypes_stringify_charptr((const char**)&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "\"\"", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_002: [ umocktypes_stringify_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
TEST_FUNCTION(umocktypes_stringify_charptr_with_a_non_empty_string_returns_the_string_surrounded_by_quotes)
{
    // arrange
    const char* input = "test42";

    // act
    char* result = umocktypes_stringify_charptr(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "\"test42\"", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_004: [ If value is NULL, umocktypes_stringify_charptr shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_charptr_with_NULL_argument_returns_NULL)
{
    // arrange

    // act
    char* result = umocktypes_stringify_charptr(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_003: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_charptr shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_umocktypes_stringify_charptr_fails)
{
    // arrange
    char* input = "test";
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_charptr((const char**)&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_005: [ umocktypes_are_equal_charptr shall compare the 2 strings pointed to by left and right. ] */
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_007: [ If left and right are equal, umocktypes_are_equal_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_charptr_with_same_pointer_returns_1)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_charptr(&input, &input);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_005: [ umocktypes_are_equal_charptr shall compare the 2 strings pointed to by left and right. ] */
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_007: [ If left and right are equal, umocktypes_are_equal_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_charptr_with_same_NULL_pointer_returns_1)
{
    // arrange

    // act
    int result = umocktypes_are_equal_charptr(NULL, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_008: [ If only one of the left and right argument is NULL, umocktypes_are_equal_charptr shall return 0. ] */
TEST_FUNCTION(umocktypes_are_equal_charptr_with_left_NULL_returns_0)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_charptr(NULL, &input);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_008: [ If only one of the left and right argument is NULL, umocktypes_are_equal_charptr shall return 0. ] */
TEST_FUNCTION(umocktypes_are_equal_charptr_with_right_NULL_returns_0)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_charptr(&input, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_009: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_charptr_with_string_being_the_same_returns_1)
{
    // arrange
    char* input1 = (char*)malloc(7);
    char* input2 = (char*)malloc(7);
    (void)strcpy(input1, "test42");
    (void)strcpy(input2, "test42");

    // act
    int result = umocktypes_are_equal_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);

    // cleanup
    free(input1);
    free(input2);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_010: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_charptr shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_charptr_with_string_being_different_returns_0)
{
    // arrange
    char* input1 = (char*)malloc(7);
    char* input2 = (char*)malloc(7);
    (void)strcpy(input1, "test42");
    (void)strcpy(input2, "test43");

    // act
    int result = umocktypes_are_equal_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    free(input1);
    free(input2);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_006: [ The comparison shall be case sensitive. ]*/
TEST_FUNCTION(umocktypes_are_equal_charptr_with_string_being_different_in_case_returns_0)
{
    // arrange
    char* input1 = (char*)malloc(5);
    char* input2 = (char*)malloc(5);
    (void)strcpy(input1, "Test");
    (void)strcpy(input2, "test");

    // act
    int result = umocktypes_are_equal_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    free(input1);
    free(input2);
}

/* umocktypes_copy_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_011: [ umocktypes_copy_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_012: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_016: [ On success umocktypes_copy_charptr shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_014: [ umocktypes_copy_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_015: [ The newly allocated string shall be returned in the destination argument. ]*/
TEST_FUNCTION(umocktypes_copy_charptr_copies_an_empty_string)
{
    // arrange
    const char* source = "";
    char* destination;

    // act
    int result = umocktypes_copy_charptr(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, "", destination);

    // cleanup
    free(destination);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_011: [ umocktypes_copy_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_012: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_016: [ On success umocktypes_copy_charptr shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_014: [ umocktypes_copy_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_015: [ The newly allocated string shall be returned in the destination argument. ]*/
TEST_FUNCTION(umocktypes_copy_charptr_copies_a_string)
{
    // arrange
    const char* source = "test42";
    char* destination;

    // act
    int result = umocktypes_copy_charptr(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, "test42", destination);

    // cleanup
    free(destination);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_013: [ If source or destination are NULL, umocktypes_copy_charptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_charptr_with_NULL_destination_fails)
{
    // arrange
    const char* source = "42";

    // act
    int result = umocktypes_copy_charptr(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_013: [ If source or destination are NULL, umocktypes_copy_charptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_charptr_with_NULL_source_fails)
{
    // arrange
    char* destination;

    // act
    int result = umocktypes_copy_charptr(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_036: [ If allocating the memory for the new string fails, umocktypes_copy_charptr shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_copy_charptr_fails)
{
    // arrange
    char* destination;
    const char* source = "b";

    when_shall_malloc_fail = 1;

    // act
    int result = umocktypes_copy_charptr(&destination, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_017: [ umocktypes_free_charptr shall free the string pointed to by value. ]*/
TEST_FUNCTION(umocktypes_free_charptr_frees_the_string)
{
    // arrange
    const char* source = "test42";
    char* destination;

    (void)umocktypes_copy_charptr(&destination, &source);

    // act
    umocktypes_free_charptr(&destination);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, free_call_count);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_018: [ If value is NULL, umocktypes_free_charptr shall do nothing. ] */
TEST_FUNCTION(umocktypes_free_charptr_with_NULL_does_nothing)
{
    // arrange

    // act
    umocktypes_free_charptr(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
}

/* umocktypes_stringify_const_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_019: [ umocktypes_stringify_const_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
TEST_FUNCTION(umocktypes_stringify_const_charptr_with_an_empty_string_returns_2_quotes)
{
    // arrange
    char* input = "";

    // act
    char* result = umocktypes_stringify_const_charptr((const char**)&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "\"\"", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_019: [ umocktypes_stringify_const_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). ] */
TEST_FUNCTION(umocktypes_stringify_const_charptr_with_a_non_empty_string_returns_the_string_surrounded_by_quotes)
{
    // arrange
    const char* input = "test42";

    // act
    char* result = umocktypes_stringify_const_charptr(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "\"test42\"", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_020: [ If value is NULL, umocktypes_stringify_const_charptr shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_const_charptr_with_NULL_argument_returns_NULL)
{
    // arrange

    // act
    char* result = umocktypes_stringify_const_charptr(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_021: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_const_charptr shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_umocktypes_stringify_const_charptr_fails)
{
    // arrange
    const char* input = "test";
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_const_charptr(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_const_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_022: [ umocktypes_are_equal_const_charptr shall compare the 2 strings pointed to by left and right. ] */
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_024: [ If left and right are equal, umocktypes_are_equal_const_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_same_pointer_returns_1)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_const_charptr(&input, &input);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_022: [ umocktypes_are_equal_const_charptr shall compare the 2 strings pointed to by left and right. ] */
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_024: [ If left and right are equal, umocktypes_are_equal_const_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_same_NULL_pointer_returns_1)
{
    // arrange

    // act
    int result = umocktypes_are_equal_const_charptr(NULL, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_025: [ If only one of the left and right argument is NULL, umocktypes_are_equal_const_charptr shall return 0. ] */
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_left_NULL_returns_0)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_const_charptr(NULL, &input);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_025: [ If only one of the left and right argument is NULL, umocktypes_are_equal_const_charptr shall return 0. ] */
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_right_NULL_returns_0)
{
    // arrange
    const char* input = "test42";

    // act
    int result = umocktypes_are_equal_const_charptr(&input, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_026: [ If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_const_charptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_string_being_the_same_returns_1)
{
    // arrange
    char* input1 = (char*)malloc(7);
    char* input2 = (char*)malloc(7);
    (void)strcpy(input1, "test42");
    (void)strcpy(input2, "test42");

    // act
    int result = umocktypes_are_equal_const_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);

    // cleanup
    free(input1);
    free(input2);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_027: [ If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_const_charptr shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_string_being_different_returns_0)
{
    // arrange
    char* input1 = (char*)malloc(7);
    char* input2 = (char*)malloc(7);
    (void)strcpy(input1, "test42");
    (void)strcpy(input2, "test43");

    // act
    int result = umocktypes_are_equal_const_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    free(input1);
    free(input2);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_023: [ The comparison shall be case sensitive. ]*/
TEST_FUNCTION(umocktypes_are_equal_const_charptr_with_string_being_different_in_case_returns_0)
{
    // arrange
    char* input1 = (char*)malloc(5);
    char* input2 = (char*)malloc(5);
    (void)strcpy(input1, "Test");
    (void)strcpy(input2, "test");

    // act
    int result = umocktypes_are_equal_const_charptr((const char**)&input1, (const char**)&input2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    free(input1);
    free(input2);
}

/* umocktypes_copy_const_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_028: [ umocktypes_copy_const_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_029: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_032: [ On success umocktypes_copy_const_charptr shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_030: [ umocktypes_copy_const_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_031: [ The newly allocated string shall be returned in the destination argument. ]*/
TEST_FUNCTION(umocktypes_copy_const_charptr_copies_an_empty_string)
{
    // arrange
    const char* source = "";
    const char* destination;

    // act
    int result = umocktypes_copy_const_charptr(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, "", destination);

    // cleanup
    free((void*)destination);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_028: [ umocktypes_copy_const_charptr shall allocate a new sequence of chars by using umockalloc_malloc. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_029: [ The number of bytes allocated shall accomodate the string pointed to by source. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_032: [ On success umocktypes_copy_const_charptr shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_030: [ umocktypes_copy_const_charptr shall copy the string pointed to by source to the newly allocated memory. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_031: [ The newly allocated string shall be returned in the destination argument. ]*/
TEST_FUNCTION(umocktypes_copy_const_charptr_copies_a_string)
{
    // arrange
    const char* source = "test42";
    const char* destination;

    // act
    int result = umocktypes_copy_const_charptr(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, "test42", destination);

    // cleanup
    free((void*)destination);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_033: [ If source or destination are NULL, umocktypes_copy_const_charptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_const_charptr_with_NULL_destination_fails)
{
    // arrange
    const char* source = "42";

    // act
    int result = umocktypes_copy_const_charptr(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_033: [ If source or destination are NULL, umocktypes_copy_const_charptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_const_charptr_with_NULL_source_fails)
{
    // arrange
    const char* destination;

    // act
    int result = umocktypes_copy_const_charptr(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_037: [ If allocating the memory for the new string fails, umocktypes_copy_const_charptr shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_copy_const_charptr_fails)
{
    // arrange
    const char* destination;
    const char* source = "b";

    when_shall_malloc_fail = 1;

    // act
    int result = umocktypes_copy_const_charptr(&destination, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_const_charptr */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_034: [ umocktypes_free_const_charptr shall free the string pointed to by value. ]*/
TEST_FUNCTION(umocktypes_free_const_charptr_frees_the_string)
{
    // arrange
    const char* source = "test42";
    const char* destination;

    (void)umocktypes_copy_const_charptr(&destination, &source);

    // act
    umocktypes_free_const_charptr(&destination);

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, free_call_count);
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_035: [ If value is NULL, umocktypes_free_const_charptr shall do nothing. ] */
TEST_FUNCTION(umocktypes_free_const_charptr_with_NULL_does_nothing)
{
    // arrange

    // act
    umocktypes_free_const_charptr(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, free_call_count);
}

/* umocktypes_charptr_register_types */

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_001: [ umocktypes_charptr_register_types shall register support for the types char\* and const char\* by using the REGISTER_UMOCK_VALUE_TYPE macro provided by umockc. ]*/
/* Tests_SRS_UMOCKTYPES_CHARPTR_01_038: [ On success, umocktypes_charptr_register_types shall return 0. ]*/
TEST_FUNCTION(umocktypes_charptr_register_types_registers_all_types)
{
    // arrange
    size_t i;

    when_shall_umocktypes_register_typecall_fail = 0;
    umocktypes_register_type_call_result = 0;

    // act
    int result = umocktypes_charptr_register_types();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 2, umocktypes_register_type_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char*", umocktypes_register_type_calls[0].type);
    ASSERT_ARE_EQUAL(char_ptr, "const char*", umocktypes_register_type_calls[1].type);

    for (i = 0; i < 2; i++)
    {
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].stringify_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].are_equal_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].copy_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].free_func);
    }
}

/* Tests_SRS_UMOCKTYPES_CHARPTR_01_039: [ If registering any of the types fails, umocktypes_charptr_register_types shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_the_underlying_register_fails_umocktypes_charptr_register_types_fails)
{
    size_t i;

    for (i = 0; i < 2; i++)
    {
        // arrange
        reset_umocktypes_register_type_calls();
        umocktypes_register_type_fail_call_result = 1;
        when_shall_umocktypes_register_typecall_fail = i + 1;

        // act
        int result = umocktypes_charptr_register_types();

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(int, i + 1, umocktypes_register_type_call_count);
    }
}

END_TEST_SUITE(umocktypes_charptr_unittests)
