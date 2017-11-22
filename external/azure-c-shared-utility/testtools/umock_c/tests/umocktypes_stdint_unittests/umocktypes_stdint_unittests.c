// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"
#include "umocktypes_stdint.h"

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

BEGIN_TEST_SUITE(umocktypes_stdint_unittests)

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

/* umocktypes_stdint_register_types */

/* Tests_SRS_UMOCKTYPES_STDINT_01_001: [ umocktypes_stdint_register_types shall register support for all the types in the module. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_002: [ On success, umocktypes_stdint_register_types shall return 0. ]*/
TEST_FUNCTION(umocktypes_stdint_register_types_registers_all_types)
{
    // arrange
    size_t i;

    umocktypes_register_type_fail_call_result = 0;

    // act
    int result = umocktypes_stdint_register_types();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 8, umocktypes_register_type_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "uint8_t", umocktypes_register_type_calls[0].type);
    ASSERT_ARE_EQUAL(char_ptr, "int8_t", umocktypes_register_type_calls[1].type);
    ASSERT_ARE_EQUAL(char_ptr, "uint16_t", umocktypes_register_type_calls[2].type);
    ASSERT_ARE_EQUAL(char_ptr, "int16_t", umocktypes_register_type_calls[3].type);
    ASSERT_ARE_EQUAL(char_ptr, "uint32_t", umocktypes_register_type_calls[4].type);
    ASSERT_ARE_EQUAL(char_ptr, "int32_t", umocktypes_register_type_calls[5].type);
    ASSERT_ARE_EQUAL(char_ptr, "uint64_t", umocktypes_register_type_calls[6].type);
    ASSERT_ARE_EQUAL(char_ptr, "int64_t", umocktypes_register_type_calls[7].type);

    for (i = 0; i < 8; i++)
    {
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].stringify_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].are_equal_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].copy_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].free_func);
    }
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_003: [ If registering any of the types fails, umocktypes_stdint_register_types shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_the_underlying_register_fails_umocktypes_stdint_register_types_fails)
{
    size_t i;

    for (i = 0; i < 1; i++)
    {
        // arrange
        reset_umocktypes_register_type_calls();
        umocktypes_register_type_fail_call_result = 1;
        when_shall_umocktypes_register_typecall_fail = i + 1;

        // act
        int result = umocktypes_stdint_register_types();

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(int, i + 1, umocktypes_register_type_call_count);
    }
}

/* umocktypes_stringify_uint8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_004: [ umocktypes_stringify_uint8_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint8_t_with_0_value)
{
    // arrange
    uint8_t input = 0;

    // act
    char* result = umocktypes_stringify_uint8_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_004: [ umocktypes_stringify_uint8_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint8_t_with_positive_value)
{
    // arrange
    uint8_t input = 255;

    // act
    char* result = umocktypes_stringify_uint8_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "255", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_005: [ If value is NULL, umocktypes_stringify_uint8_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_uint8_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_uint8_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_006: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_uint8_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_uint8_t_fails)
{
    // arrange
    uint8_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_uint8_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_uint8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_008: [ umocktypes_are_equal_uint8_t shall compare the 2 uint8_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_010: [ If the values pointed to by left and right are equal, umocktypes_are_equal_uint8_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint8_t_with_2_equal_values_returns_1)
{
    // arrange
    uint8_t left = 0x42;
    uint8_t right = 0x42;

    // act
    int result = umocktypes_are_equal_uint8_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_011: [ If the values pointed to by left and right are different, umocktypes_are_equal_uint8_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint8_t_with_2_different_values_returns_0)
{
    // arrange
    uint8_t left = 0x42;
    uint8_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint8_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_009: [ If any of the arguments is NULL, umocktypes_are_equal_uint8_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint8_t_with_NULL_left_fails)
{
    // arrange
    uint8_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint8_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_009: [ If any of the arguments is NULL, umocktypes_are_equal_uint8_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint8_t_with_NULL_right_fails)
{
    // arrange
    uint8_t left = 0x42;

    // act
    int result = umocktypes_are_equal_uint8_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_uint8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_012: [ umocktypes_copy_uint8_t shall copy the uint8_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_013: [ On success umocktypes_copy_uint8_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_uint8_t_succeeds)
{
    // arrange
    uint8_t destination = 0;
    uint8_t source = 0x42;

    // act
    int result = umocktypes_copy_uint8_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint8_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_014: [ If source or destination are NULL, umocktypes_copy_uint8_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint8_t_with_NULL_destination_fails)
{
    // arrange
    uint8_t source = 0x42;

    // act
    int result = umocktypes_copy_uint8_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_014: [ If source or destination are NULL, umocktypes_copy_uint8_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint8_t_with_NULL_source_fails)
{
    // arrange
    uint8_t destination = 0;

    // act
    int result = umocktypes_copy_uint8_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_uint8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_015: [ umocktypes_free_uint8_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_uint8_t_does_nothing)
{
    // arrange
    uint8_t value = 0;

    // act
    umocktypes_free_uint8_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_int8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_016: [ umocktypes_stringify_int8_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int8_t_with_minus_128_value)
{
    // arrange
    int8_t input = -128;

    // act
    char* result = umocktypes_stringify_int8_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-128", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_016: [ umocktypes_stringify_int8_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int8_t_with_127_value)
{
    // arrange
    int8_t input = 127;

    // act
    char* result = umocktypes_stringify_int8_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_017: [ If value is NULL, umocktypes_stringify_int8_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_int8_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_int8_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_018: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int8_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_int8_t_fails)
{
    // arrange
    int8_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_int8_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_int8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_020: [ umocktypes_are_equal_int8_t shall compare the 2 int8_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_022: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int8_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int8_t_with_2_equal_values_returns_1)
{
    // arrange
    int8_t left = 0x42;
    int8_t right = 0x42;

    // act
    int result = umocktypes_are_equal_int8_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_023: [ If the values pointed to by left and right are different, umocktypes_are_equal_int8_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_int8_t_with_2_different_values_returns_0)
{
    // arrange
    int8_t left = 0x42;
    int8_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int8_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_021: [ If any of the arguments is NULL, umocktypes_are_equal_int8_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int8_t_with_NULL_left_fails)
{
    // arrange
    int8_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int8_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_021: [ If any of the arguments is NULL, umocktypes_are_equal_int8_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int8_t_with_NULL_right_fails)
{
    // arrange
    int8_t left = 0x42;

    // act
    int result = umocktypes_are_equal_int8_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_int8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_024: [ umocktypes_copy_int8_t shall copy the int8_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_025: [ On success umocktypes_copy_int8_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_int8_t_succeeds)
{
    // arrange
    int8_t destination = 0;
    int8_t source = 0x42;

    // act
    int result = umocktypes_copy_int8_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int8_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_026: [ If source or destination are NULL, umocktypes_copy_int8_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int8_t_with_NULL_destination_fails)
{
    // arrange
    int8_t source = 0x42;

    // act
    int result = umocktypes_copy_int8_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_026: [ If source or destination are NULL, umocktypes_copy_int8_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int8_t_with_NULL_source_fails)
{
    // arrange
    int8_t destination = 0;

    // act
    int result = umocktypes_copy_int8_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_int8_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_027: [ umocktypes_free_int8_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_int8_t_does_nothing)
{
    // arrange
    int8_t value = 0;

    // act
    umocktypes_free_int8_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_uint16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_028: [ umocktypes_stringify_uint16_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint16_t_with_0_value)
{
    // arrange
    uint16_t input = 0;

    // act
    char* result = umocktypes_stringify_uint16_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_028: [ umocktypes_stringify_uint16_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint16_t_with_65535_value)
{
    // arrange
    uint16_t input = 65535;

    // act
    char* result = umocktypes_stringify_uint16_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "65535", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_029: [ If value is NULL, umocktypes_stringify_uint16_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_uint16_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_uint16_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_030: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_uint16_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_uint16_t_fails)
{
    // arrange
    uint16_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_uint16_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_uint16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_032: [ umocktypes_are_equal_uint16_t shall compare the 2 uint16_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_034: [ If the values pointed to by left and right are equal, umocktypes_are_equal_uint16_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint16_t_with_2_equal_values_returns_1)
{
    // arrange
    uint16_t left = 0x42;
    uint16_t right = 0x42;

    // act
    int result = umocktypes_are_equal_uint16_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_035: [ If the values pointed to by left and right are different, umocktypes_are_equal_uint16_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint16_t_with_2_different_values_returns_0)
{
    // arrange
    uint16_t left = 0x42;
    uint16_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint16_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_033: [ If any of the arguments is NULL, umocktypes_are_equal_uint16_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint16_t_with_NULL_left_fails)
{
    // arrange
    uint16_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint16_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_033: [ If any of the arguments is NULL, umocktypes_are_equal_uint16_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint16_t_with_NULL_right_fails)
{
    // arrange
    uint16_t left = 0x42;

    // act
    int result = umocktypes_are_equal_uint16_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_uint16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_036: [ umocktypes_copy_uint16_t shall copy the uint16_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_037: [ On success umocktypes_copy_uint16_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_uint16_t_succeeds)
{
    // arrange
    uint16_t destination = 0;
    uint16_t source = 0x42;

    // act
    int result = umocktypes_copy_uint16_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint32_t, (uint32_t)source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_038: [ If source or destination are NULL, umocktypes_copy_uint16_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint16_t_with_NULL_destination_fails)
{
    // arrange
    uint16_t source = 0x42;

    // act
    int result = umocktypes_copy_uint16_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_038: [ If source or destination are NULL, umocktypes_copy_uint16_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint16_t_with_NULL_source_fails)
{
    // arrange
    uint16_t destination = 0;

    // act
    int result = umocktypes_copy_uint16_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_uint16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_039: [ umocktypes_free_uint16_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_uint16_t_does_nothing)
{
    // arrange
    uint16_t value = 0;

    // act
    umocktypes_free_uint16_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_int16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_040: [ umocktypes_stringify_int16_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int16_t_with_minus_32768_value)
{
    // arrange
    int16_t input = -32768;

    // act
    char* result = umocktypes_stringify_int16_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-32768", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_040: [ umocktypes_stringify_int16_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int16_t_with_32767_value)
{
    // arrange
    int16_t input = 32767;

    // act
    char* result = umocktypes_stringify_int16_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "32767", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_041: [ If value is NULL, umocktypes_stringify_int16_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_int16_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_int16_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_042: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int16_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_int16_t_fails)
{
    // arrange
    int16_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_int16_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_int16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_044: [ umocktypes_are_equal_int16_t shall compare the 2 int16_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_046: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int16_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int16_t_with_2_equal_values_returns_1)
{
    // arrange
    int16_t left = 0x42;
    int16_t right = 0x42;

    // act
    int result = umocktypes_are_equal_int16_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_047: [ If the values pointed to by left and right are different, umocktypes_are_equal_int16_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_int16_t_with_2_different_values_returns_0)
{
    // arrange
    int16_t left = 0x42;
    int16_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int16_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_045: [ If any of the arguments is NULL, umocktypes_are_equal_int16_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int16_t_with_NULL_left_fails)
{
    // arrange
    int16_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int16_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_045: [ If any of the arguments is NULL, umocktypes_are_equal_int16_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int16_t_with_NULL_right_fails)
{
    // arrange
    int16_t left = 0x42;

    // act
    int result = umocktypes_are_equal_int16_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_int16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_048: [ umocktypes_copy_int16_t shall copy the int16_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_049: [ On success umocktypes_copy_int16_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_int16_t_succeeds)
{
    // arrange
    int16_t destination = 0;
    int16_t source = 0x42;

    // act
    int result = umocktypes_copy_int16_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int16_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_050: [ If source or destination are NULL, umocktypes_copy_int16_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int16_t_with_NULL_destination_fails)
{
    // arrange
    int16_t source = 0x42;

    // act
    int result = umocktypes_copy_int16_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_050: [ If source or destination are NULL, umocktypes_copy_int16_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int16_t_with_NULL_source_fails)
{
    // arrange
    int16_t destination = 0;

    // act
    int result = umocktypes_copy_int16_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_int16_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_051: [ umocktypes_free_int16_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_int16_t_does_nothing)
{
    // arrange
    int16_t value = 0;

    // act
    umocktypes_free_int16_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_uint32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_052: [ umocktypes_stringify_uint32_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint32_t_with_0_value)
{
    // arrange
    uint32_t input = 0;

    // act
    char* result = umocktypes_stringify_uint32_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_052: [ umocktypes_stringify_uint32_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint32_t_with_32_bit_max_value)
{
    // arrange
    uint32_t input = UINT32_MAX;

    // act
    char* result = umocktypes_stringify_uint32_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "4294967295", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_053: [ If value is NULL, umocktypes_stringify_uint32_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_uint32_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_uint32_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_054: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_uint32_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_uint32_t_fails)
{
    // arrange
    uint32_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_uint32_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_uint32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_056: [ umocktypes_are_equal_uint32_t shall compare the 2 uint32_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_058: [ If the values pointed to by left and right are equal, umocktypes_are_equal_uint32_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint32_t_with_2_equal_values_returns_1)
{
    // arrange
    uint32_t left = 0x42;
    uint32_t right = 0x42;

    // act
    int result = umocktypes_are_equal_uint32_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_059: [ If the values pointed to by left and right are different, umocktypes_are_equal_uint32_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint32_t_with_2_different_values_returns_0)
{
    // arrange
    uint32_t left = 0x42;
    uint32_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint32_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_057: [ If any of the arguments is NULL, umocktypes_are_equal_uint32_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint32_t_with_NULL_left_fails)
{
    // arrange
    uint32_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint32_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_057: [ If any of the arguments is NULL, umocktypes_are_equal_uint32_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint32_t_with_NULL_right_fails)
{
    // arrange
    uint32_t left = 0x42;

    // act
    int result = umocktypes_are_equal_uint32_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_uint32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_060: [ umocktypes_copy_uint32_t shall copy the uint32_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_061: [ On success umocktypes_copy_uint32_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_uint32_t_succeeds)
{
    // arrange
    uint32_t destination = 0;
    uint32_t source = 0x42;

    // act
    int result = umocktypes_copy_uint32_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint32_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_062: [ If source or destination are NULL, umocktypes_copy_uint32_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint32_t_with_NULL_destination_fails)
{
    // arrange
    uint32_t source = 0x42;

    // act
    int result = umocktypes_copy_uint32_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_062: [ If source or destination are NULL, umocktypes_copy_uint32_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint32_t_with_NULL_source_fails)
{
    // arrange
    uint32_t destination = 0;

    // act
    int result = umocktypes_copy_uint32_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_uint32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_063: [ umocktypes_free_uint32_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_uint32_t_does_nothing)
{
    // arrange
    uint32_t value = 0;

    // act
    umocktypes_free_uint32_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_int32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_064: [ umocktypes_stringify_int32_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int32_t_with_min_value)
{
    // arrange
    int32_t input = INT32_MIN;

    // act
    char* result = umocktypes_stringify_int32_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-2147483648", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_064: [ umocktypes_stringify_int32_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int32_t_with_max_value)
{
    // arrange
    int32_t input = INT32_MAX;

    // act
    char* result = umocktypes_stringify_int32_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "2147483647", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_065: [ If value is NULL, umocktypes_stringify_int32_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_int32_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_int32_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_066: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int32_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_int32_t_fails)
{
    // arrange
    int32_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_int32_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_int32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_068: [ umocktypes_are_equal_int32_t shall compare the 2 int32_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_070: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int32_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int32_t_with_2_equal_values_returns_1)
{
    // arrange
    int32_t left = 0x42;
    int32_t right = 0x42;

    // act
    int result = umocktypes_are_equal_int32_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_071: [ If the values pointed to by left and right are different, umocktypes_are_equal_int32_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_int32_t_with_2_different_values_returns_0)
{
    // arrange
    int32_t left = 0x42;
    int32_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int32_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_069: [ If any of the arguments is NULL, umocktypes_are_equal_int32_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int32_t_with_NULL_left_fails)
{
    // arrange
    int32_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int32_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_069: [ If any of the arguments is NULL, umocktypes_are_equal_int32_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int32_t_with_NULL_right_fails)
{
    // arrange
    int32_t left = 0x42;

    // act
    int result = umocktypes_are_equal_int32_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_int32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_072: [ umocktypes_copy_int32_t shall copy the int32_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_073: [ On success umocktypes_copy_int32_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_int32_t_succeeds)
{
    // arrange
    int32_t destination = 0;
    int32_t source = 0x42;

    // act
    int result = umocktypes_copy_int32_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int32_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_074: [ If source or destination are NULL, umocktypes_copy_int32_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int32_t_with_NULL_destination_fails)
{
    // arrange
    int32_t source = 0x42;

    // act
    int result = umocktypes_copy_int32_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_074: [ If source or destination are NULL, umocktypes_copy_int32_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int32_t_with_NULL_source_fails)
{
    // arrange
    int32_t destination = 0;

    // act
    int result = umocktypes_copy_int32_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_int32_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_075: [ umocktypes_free_int32_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_int32_t_does_nothing)
{
    // arrange
    int32_t value = 0;

    // act
    umocktypes_free_int32_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_uint64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_076: [ umocktypes_stringify_uint64_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint64_t_with_min_value)
{
    // arrange
    uint64_t input = 0;

    // act
    char* result = umocktypes_stringify_uint64_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_076: [ umocktypes_stringify_uint64_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_uint64_t_with_max_value)
{
    // arrange
    uint64_t input = UINT64_MAX;

    // act
    char* result = umocktypes_stringify_uint64_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "18446744073709551615", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_077: [ If value is NULL, umocktypes_stringify_uint64_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_uint64_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_uint64_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_078: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_uint64_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_uint64_t_fails)
{
    // arrange
    uint64_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_uint64_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_uint64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_080: [ umocktypes_are_equal_uint64_t shall compare the 2 uint64_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_082: [ If the values pointed to by left and right are equal, umocktypes_are_equal_uint64_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint64_t_with_2_equal_values_returns_1)
{
    // arrange
    uint64_t left = 0x42;
    uint64_t right = 0x42;

    // act
    int result = umocktypes_are_equal_uint64_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_083: [ If the values pointed to by left and right are different, umocktypes_are_equal_uint64_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint64_t_with_2_different_values_returns_0)
{
    // arrange
    uint64_t left = 0x42;
    uint64_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint64_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_081: [ If any of the arguments is NULL, umocktypes_are_equal_uint64_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint64_t_with_NULL_left_fails)
{
    // arrange
    uint64_t right = 0x43;

    // act
    int result = umocktypes_are_equal_uint64_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_081: [ If any of the arguments is NULL, umocktypes_are_equal_uint64_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_uint64_t_with_NULL_right_fails)
{
    // arrange
    uint64_t left = 0x42;

    // act
    int result = umocktypes_are_equal_uint64_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_uint64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_084: [ umocktypes_copy_uint64_t shall copy the uint64_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_085: [ On success umocktypes_copy_uint64_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_uint64_t_succeeds)
{
    // arrange
    uint64_t destination = 0;
    uint64_t source = 0x42;

    // act
    int result = umocktypes_copy_uint64_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint64_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_086: [ If source or destination are NULL, umocktypes_copy_uint64_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint64_t_with_NULL_destination_fails)
{
    // arrange
    uint64_t source = 0x42;

    // act
    int result = umocktypes_copy_uint64_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_086: [ If source or destination are NULL, umocktypes_copy_uint64_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_uint64_t_with_NULL_source_fails)
{
    // arrange
    uint64_t destination = 0;

    // act
    int result = umocktypes_copy_uint64_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_uint64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_087: [ umocktypes_free_uint64_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_uint64_t_does_nothing)
{
    // arrange
    uint64_t value = 0;

    // act
    umocktypes_free_uint64_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_int64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_088: [ umocktypes_stringify_int64_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int64_t_with_min_value)
{
    // arrange
    int64_t input = INT64_MIN;

    // act
    char* result = umocktypes_stringify_int64_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-9223372036854775808", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_088: [ umocktypes_stringify_int64_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int64_t_with_max_value)
{
    // arrange
    int64_t input = INT64_MAX;

    // act
    char* result = umocktypes_stringify_int64_t(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "9223372036854775807", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_089: [ If value is NULL, umocktypes_stringify_int64_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_int64_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_int64_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_090: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int64_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_int64_t_fails)
{
    // arrange
    int64_t input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_int64_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_int64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_092: [ umocktypes_are_equal_int64_t shall compare the 2 int64_t values pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_094: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int64_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int64_t_with_2_equal_values_returns_1)
{
    // arrange
    int64_t left = 0x42;
    int64_t right = 0x42;

    // act
    int result = umocktypes_are_equal_int64_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_095: [ If the values pointed to by left and right are different, umocktypes_are_equal_int64_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_int64_t_with_2_different_values_returns_0)
{
    // arrange
    int64_t left = 0x42;
    int64_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int64_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_093: [ If any of the arguments is NULL, umocktypes_are_equal_int64_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int64_t_with_NULL_left_fails)
{
    // arrange
    int64_t right = 0x43;

    // act
    int result = umocktypes_are_equal_int64_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_093: [ If any of the arguments is NULL, umocktypes_are_equal_int64_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int64_t_with_NULL_right_fails)
{
    // arrange
    int64_t left = 0x42;

    // act
    int result = umocktypes_are_equal_int64_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_int64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_096: [ umocktypes_copy_int64_t shall copy the int64_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_STDINT_01_097: [ On success umocktypes_copy_int64_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_int64_t_succeeds)
{
    // arrange
    int64_t destination = 0;
    int64_t source = 0x42;

    // act
    int result = umocktypes_copy_int64_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint64_t, source, destination);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_098: [ If source or destination are NULL, umocktypes_copy_int64_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int64_t_with_NULL_destination_fails)
{
    // arrange
    int64_t source = 0x42;

    // act
    int result = umocktypes_copy_int64_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_STDINT_01_098: [ If source or destination are NULL, umocktypes_copy_int64_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int64_t_with_NULL_source_fails)
{
    // arrange
    int64_t destination = 0;

    // act
    int result = umocktypes_copy_int64_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_int64_t */

/* Tests_SRS_UMOCKTYPES_STDINT_01_099: [ umocktypes_free_int64_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_int64_t_does_nothing)
{
    // arrange
    int64_t value = 0;

    // act
    umocktypes_free_int64_t(&value);

    // assert
    // no explicit assert
}

END_TEST_SUITE(umocktypes_stdint_unittests)
