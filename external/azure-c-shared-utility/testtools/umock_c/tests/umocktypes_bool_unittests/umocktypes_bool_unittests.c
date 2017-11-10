// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"
#include "umocktypes_bool.h"

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

BEGIN_TEST_SUITE(umocktypes_bool_unittests)

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

/* umocktypes_stringify_bool */

/* Tests_SRS_UMOCKTYPES_BOOL_01_002: [ umocktypes_stringify_bool shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_bool_with_false)
{
    // arrange
    bool input = false;

    // act
    char* result = umocktypes_stringify_bool(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "false", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_002: [ umocktypes_stringify_bool shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_bool_with_true)
{
    // arrange
    bool input = true;

    // act
    char* result = umocktypes_stringify_bool(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "true", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_003: [ If value is NULL, umocktypes_stringify_bool shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_bool_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_bool(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_004: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_bool shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_bool_fails)
{
    // arrange
    bool input = false;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_bool(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_bool */

/* Tests_SRS_UMOCKTYPES_BOOL_01_006: [ umocktypes_are_equal_bool shall compare the 2 chars pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_BOOL_01_008: [ If the values pointed to by left and right are equal, umocktypes_are_equal_bool shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_bool_with_2_equal_values_returns_1)
{
    // arrange
    bool left = false;
    bool right = false;

    // act
    int result = umocktypes_are_equal_bool(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_009: [ If the values pointed to by left and right are different, umocktypes_are_equal_bool shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_bool_with_2_different_values_returns_0)
{
    // arrange
    bool left = false;
    bool right = true;

    // act
    int result = umocktypes_are_equal_bool(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_bool shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_bool_with_NULL_left_fails)
{
    // arrange
    bool right = true;

    // act
    int result = umocktypes_are_equal_bool(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_bool shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_bool_with_NULL_right_fails)
{
    // arrange
    bool left = false;

    // act
    int result = umocktypes_are_equal_bool(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_bool */

/* Tests_SRS_UMOCKTYPES_BOOL_01_010: [ umocktypes_copy_bool shall copy the bool value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_BOOL_01_011: [ On success umocktypes_copy_bool shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_bool_succeeds)
{
    // arrange
    bool destination = false;
    bool source = true;

    // act
    int result = umocktypes_copy_bool(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_TRUE(destination);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_012: [ If source or destination are NULL, umocktypes_copy_bool shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_bool_with_NULL_destination_fails)
{
    // arrange
    bool source = false;

    // act
    int result = umocktypes_copy_bool(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_012: [ If source or destination are NULL, umocktypes_copy_bool shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_bool_with_NULL_source_fails)
{
    // arrange
    bool destination = false;

    // act
    int result = umocktypes_copy_bool(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_bool */

/* Tests_SRS_UMOCKTYPES_BOOL_01_013: [ umocktypes_free_bool shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_bool_does_nothing)
{
    // arrange
    bool value = false;

    // act
    umocktypes_free_bool(&value);

    // assert
    // no explicit assert
}

/* umocktypes_bool_register_types */

/* Tests_SRS_UMOCKTYPES_BOOL_01_001: [ umocktypes_bool_register_types shall register support for all the types in the module. ]*/
/* Tests_SRS_UMOCKTYPES_BOOL_01_014: [ On success, umocktypes_bool_register_types shall return 0. ]*/
TEST_FUNCTION(umocktypes_bool_register_types_registers_all_types)
{
    // arrange
    size_t i;

    umocktypes_register_type_fail_call_result = 0;

    // act
    int result = umocktypes_bool_register_types();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 2, umocktypes_register_type_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "bool", umocktypes_register_type_calls[0].type);
    ASSERT_ARE_EQUAL(char_ptr, "_Bool", umocktypes_register_type_calls[1].type);
    
    for (i = 0; i < 2; i++)
    {
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].stringify_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].are_equal_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].copy_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].free_func);
    }
}

/* Tests_SRS_UMOCKTYPES_BOOL_01_015: [ If registering any of the types fails, umocktypes_bool_register_types shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_the_underlying_register_fails_umocktypes_bool_register_types_fails)
{
    size_t i;

    for (i = 0; i < 2; i++)
    {
        // arrange
        reset_umocktypes_register_type_calls();
        umocktypes_register_type_fail_call_result = 1;
        when_shall_umocktypes_register_typecall_fail = i + 1;

        // act
        int result = umocktypes_bool_register_types();

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(int, i + 1, umocktypes_register_type_call_count);
    }
}

END_TEST_SUITE(umocktypes_bool_unittests)
