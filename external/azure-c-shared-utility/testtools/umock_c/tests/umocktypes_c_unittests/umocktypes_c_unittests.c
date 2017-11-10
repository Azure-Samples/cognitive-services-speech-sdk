// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"
#include "umocktypes_c.h"

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

BEGIN_TEST_SUITE(umocktypes_c_unittests)

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

/* umocktypes_stringify_char */

/* Tests_SRS_UMOCKTYPES_C_01_002: [ umocktypes_stringify_char shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_char_with_0_value)
{
    // arrange
    char input = 0;

    // act
    char* result = umocktypes_stringify_char(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_002: [ umocktypes_stringify_char shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_char_with_min_value)
{
    // arrange
    char input = CHAR_MIN;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_char(&input);

    // assert
    (void)sprintf(expected_string, "%d", (int)CHAR_MIN);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_002: [ umocktypes_stringify_char shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_char_with_max_value)
{
    // arrange
    char input = CHAR_MAX;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_char(&input);

    // assert
    (void)sprintf(expected_string, "%d", (int)CHAR_MAX);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_003: [ If value is NULL, umocktypes_stringify_char shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_char_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_char(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_004: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_char shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_char_fails)
{
    // arrange
    char input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_char(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_char */

/* Tests_SRS_UMOCKTYPES_C_01_006: [ umocktypes_are_equal_char shall compare the 2 chars pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_008: [ If the values pointed to by left and right are equal, umocktypes_are_equal_char shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_char_with_2_equal_values_returns_1)
{
    // arrange
    char left = 0x42;
    char right = 0x42;

    // act
    int result = umocktypes_are_equal_char(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_009: [ If the values pointed to by left and right are different, umocktypes_are_equal_char shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_char_with_2_different_values_returns_0)
{
    // arrange
    char left = 0x42;
    char right = 0x43;

    // act
    int result = umocktypes_are_equal_char(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_char shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_char_with_NULL_left_fails)
{
    // arrange
    char right = 0x43;

    // act
    int result = umocktypes_are_equal_char(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_007: [ If any of the arguments is NULL, umocktypes_are_equal_char shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_char_with_NULL_right_fails)
{
    // arrange
    char left = 0x42;

    // act
    int result = umocktypes_are_equal_char(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_char */

/* Tests_SRS_UMOCKTYPES_C_01_010: [ umocktypes_copy_char shall copy the char value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_011: [ On success umocktypes_copy_char shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_char_succeeds)
{
    // arrange
    char destination = 0;
    char source = 0x42;

    // act
    int result = umocktypes_copy_char(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char, 0x42, destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_012: [ If source or destination are NULL, umocktypes_copy_char shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_char_with_NULL_destination_fails)
{
    // arrange
    char source = 0x42;

    // act
    int result = umocktypes_copy_char(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_012: [ If source or destination are NULL, umocktypes_copy_char shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_char_with_NULL_source_fails)
{
    // arrange
    char destination = 0;

    // act
    int result = umocktypes_copy_char(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_char */

/* Tests_SRS_UMOCKTYPES_C_01_013: [ umocktypes_free_char shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_char_does_nothing)
{
    // arrange
    char value = 0;

    // act
    umocktypes_free_char(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_unsignedchar */

/* Tests_SRS_UMOCKTYPES_C_01_014: [ umocktypes_stringify_unsignedchar shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedchar_with_0_value)
{
    // arrange
    unsigned char input = 0;

    // act
    char* result = umocktypes_stringify_unsignedchar(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_014: [ umocktypes_stringify_unsignedchar shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedchar_with_positive_value)
{
    // arrange
    unsigned char input = 255;

    // act
    char* result = umocktypes_stringify_unsignedchar(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "255", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_015: [ If value is NULL, umocktypes_stringify_unsignedchar shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedchar_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_unsignedchar(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_016: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedchar shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_unsignedchar_fails)
{
    // arrange
    unsigned char input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_unsignedchar(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_unsignedchar */

/* Tests_SRS_UMOCKTYPES_C_01_018: [ umocktypes_are_equal_unsignedchar shall compare the 2 unsigned chars pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_020: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedchar shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedchar_with_2_equal_values_returns_1)
{
    // arrange
    unsigned char left = 0x42;
    unsigned char right = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedchar(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_021: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedchar shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedchar_with_2_different_values_returns_0)
{
    // arrange
    unsigned char left = 0x42;
    unsigned char right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedchar(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_019: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedchar shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedchar_with_NULL_left_fails)
{
    // arrange
    unsigned char right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedchar(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_019: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedchar shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedchar_with_NULL_right_fails)
{
    // arrange
    unsigned char left = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedchar(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_unsignedchar */

/* Tests_SRS_UMOCKTYPES_C_01_022: [ umocktypes_copy_unsignedchar shall copy the unsigned char value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_023: [ On success umocktypes_copy_unsignedchar shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedchar_succeeds)
{
    // arrange
    unsigned char destination = 0;
    unsigned char source = 0x42;

    // act
    int result = umocktypes_copy_unsignedchar(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(uint8_t, 0x42, destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_024: [ If source or destination are NULL, umocktypes_copy_unsignedchar shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedchar_with_NULL_destination_fails)
{
    // arrange
    unsigned char source = 0x42;

    // act
    int result = umocktypes_copy_unsignedchar(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_024: [ If source or destination are NULL, umocktypes_copy_unsignedchar shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedchar_with_NULL_source_fails)
{
    // arrange
    unsigned char destination = 0;

    // act
    int result = umocktypes_copy_unsignedchar(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_unsignedchar */

/* Tests_SRS_UMOCKTYPES_C_01_025: [ umocktypes_free_unsignedchar shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_unsignedchar_does_nothing)
{
    // arrange
    unsigned char value = 0;

    // act
    umocktypes_free_unsignedchar(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_short */

/* Tests_SRS_UMOCKTYPES_C_01_026: [ umocktypes_stringify_short shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_short_with_0_value)
{
    // arrange
    short input = 0;

    // act
    char* result = umocktypes_stringify_short(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_026: [ umocktypes_stringify_short shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_short_with_negative_value)
{
    // arrange
    short input = -127-1;

    // act
    char* result = umocktypes_stringify_short(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-128", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_026: [ umocktypes_stringify_short shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_short_with_positive_value)
{
    // arrange
    short input = 127;

    // act
    char* result = umocktypes_stringify_short(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_027: [ If value is NULL, umocktypes_stringify_short shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_short_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_short(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_028: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_short shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_short_fails)
{
    // arrange
    short input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_short(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_short */

/* Tests_SRS_UMOCKTYPES_C_01_030: [ umocktypes_are_equal_short shall compare the 2 shorts poshorted to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_032: [ If the values poshorted to by left and right are equal, umocktypes_are_equal_short shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_short_with_2_equal_values_returns_1)
{
    // arrange
    short left = 0x42;
    short right = 0x42;

    // act
    int result = umocktypes_are_equal_short(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_033: [ If the values poshorted to by left and right are different, umocktypes_are_equal_short shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_short_with_2_different_values_returns_0)
{
    // arrange
    short left = 0x42;
    short right = 0x43;

    // act
    int result = umocktypes_are_equal_short(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_031: [ If any of the arguments is NULL, umocktypes_are_equal_short shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_short_with_NULL_left_fails)
{
    // arrange
    short right = 0x43;

    // act
    int result = umocktypes_are_equal_short(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_031: [ If any of the arguments is NULL, umocktypes_are_equal_short shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_short_with_NULL_right_fails)
{
    // arrange
    short left = 0x42;

    // act
    int result = umocktypes_are_equal_short(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_short */

/* Tests_SRS_UMOCKTYPES_C_01_034: [ umocktypes_copy_short shall copy the short value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_035: [ On success umocktypes_copy_short shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_short_succeeds)
{
    // arrange
    short destination = 0;
    short source = 0x42;

    // act
    int result = umocktypes_copy_short(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(short, 0x42, destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_036: [ If source or destination are NULL, umocktypes_copy_short shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_short_with_NULL_destination_fails)
{
    // arrange
    short source = 0x42;

    // act
    int result = umocktypes_copy_short(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_036: [ If source or destination are NULL, umocktypes_copy_short shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_short_with_NULL_source_fails)
{
    // arrange
    short destination = 0;

    // act
    int result = umocktypes_copy_short(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_short */

/* Tests_SRS_UMOCKTYPES_C_01_037: [ umocktypes_free_short shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_short_does_nothing)
{
    // arrange
    short value = 0;

    // act
    umocktypes_free_short(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_unsignedshort */

/* Tests_SRS_UMOCKTYPES_C_01_038: [ umocktypes_stringify_unsignedshort shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedshort_with_0_value)
{
    // arrange
    unsigned short input = 0;

    // act
    char* result = umocktypes_stringify_unsignedshort(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_038: [ umocktypes_stringify_unsignedshort shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedshort_with_positive_value)
{
    // arrange
    unsigned short input = 255;

    // act
    char* result = umocktypes_stringify_unsignedshort(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "255", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_039: [ If value is NULL, umocktypes_stringify_unsignedshort shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedshort_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_unsignedshort(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_040: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedshort shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_unsignedshort_fails)
{
    // arrange
    unsigned short input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_unsignedshort(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_unsignedshort */

/* Tests_SRS_UMOCKTYPES_C_01_042: [ umocktypes_are_equal_unsignedshort shall compare the 2 unsigned shorts pounsigned shorted to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_044: [ If the values pounsigned shorted to by left and right are equal, umocktypes_are_equal_unsignedshort shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedshort_with_2_equal_values_returns_1)
{
    // arrange
    unsigned short left = 0x42;
    unsigned short right = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedshort(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_045: [ If the values pounsigned shorted to by left and right are different, umocktypes_are_equal_unsignedshort shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedshort_with_2_different_values_returns_0)
{
    // arrange
    unsigned short left = 0x42;
    unsigned short right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedshort(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_043: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedshort shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedshort_with_NULL_left_fails)
{
    // arrange
    unsigned short right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedshort(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_043: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedshort shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedshort_with_NULL_right_fails)
{
    // arrange
    unsigned short left = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedshort(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_unsignedshort */

/* Tests_SRS_UMOCKTYPES_C_01_046: [ umocktypes_copy_unsignedshort shall copy the unsigned short value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_047: [ On success umocktypes_copy_unsignedshort shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedshort_succeeds)
{
    // arrange
    unsigned short destination = 0;
    unsigned short source = 0x42;

    // act
    int result = umocktypes_copy_unsignedshort(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0x42, (int)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_048: [ If source or destination are NULL, umocktypes_copy_unsignedshort shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedshort_with_NULL_destination_fails)
{
    // arrange
    unsigned short source = 0x42;

    // act
    int result = umocktypes_copy_unsignedshort(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_048: [ If source or destination are NULL, umocktypes_copy_unsignedshort shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedshort_with_NULL_source_fails)
{
    // arrange
    unsigned short destination = 0;

    // act
    int result = umocktypes_copy_unsignedshort(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_unsignedshort */

/* Tests_SRS_UMOCKTYPES_C_01_049: [ umocktypes_free_unsignedshort shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_unsignedshort_does_nothing)
{
    // arrange
    unsigned short value = 0;

    // act
    umocktypes_free_unsignedshort(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_int */

/* Tests_SRS_UMOCKTYPES_C_01_050: [ umocktypes_stringify_int shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int_with_0_value)
{
    // arrange
    int input = 0;

    // act
    char* result = umocktypes_stringify_int(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_050: [ umocktypes_stringify_int shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int_with_negative_value)
{
    // arrange
    int input = -127-1;

    // act
    char* result = umocktypes_stringify_int(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-128", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_050: [ umocktypes_stringify_int shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_int_with_positive_value)
{
    // arrange
    int input = 127;

    // act
    char* result = umocktypes_stringify_int(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_051: [ If value is NULL, umocktypes_stringify_int shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_int_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_int(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_052: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_int shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_int_fails)
{
    // arrange
    int input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_int(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_int */

/* Tests_SRS_UMOCKTYPES_C_01_054: [ umocktypes_are_equal_int shall compare the 2 ints pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_056: [ If the values pointed to by left and right are equal, umocktypes_are_equal_int shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int_with_2_equal_values_returns_1)
{
    // arrange
    int left = 0x42;
    int right = 0x42;

    // act
    int result = umocktypes_are_equal_int(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_057: [ If the values pointed to by left and right are different, umocktypes_are_equal_int shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_int_with_2_different_values_returns_0)
{
    // arrange
    int left = 0x42;
    int right = 0x43;

    // act
    int result = umocktypes_are_equal_int(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_055: [ If any of the arguments is NULL, umocktypes_are_equal_int shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int_with_NULL_left_fails)
{
    // arrange
    int right = 0x43;

    // act
    int result = umocktypes_are_equal_int(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_055: [ If any of the arguments is NULL, umocktypes_are_equal_int shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_int_with_NULL_right_fails)
{
    // arrange
    int left = 0x42;

    // act
    int result = umocktypes_are_equal_int(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_int */

/* Tests_SRS_UMOCKTYPES_C_01_058: [ umocktypes_copy_int shall copy the int value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_059: [ On success umocktypes_copy_int shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_int_succeeds)
{
    // arrange
    int destination = 0;
    int source = 0x42;

    // act
    int result = umocktypes_copy_int(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0x42, (int)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_060: [ If source or destination are NULL, umocktypes_copy_int shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int_with_NULL_destination_fails)
{
    // arrange
    int source = 0x42;

    // act
    int result = umocktypes_copy_int(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_060: [ If source or destination are NULL, umocktypes_copy_int shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_int_with_NULL_source_fails)
{
    // arrange
    int destination = 0;

    // act
    int result = umocktypes_copy_int(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_int */

/* Tests_SRS_UMOCKTYPES_C_01_061: [ umocktypes_free_int shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_int_does_nothing)
{
    // arrange
    int value = 0;

    // act
    umocktypes_free_int(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_unsignedint */

/* Tests_SRS_UMOCKTYPES_C_01_062: [ umocktypes_stringify_unsignedint shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedint_with_0_value)
{
    // arrange
    unsigned int input = 0;

    // act
    char* result = umocktypes_stringify_unsignedint(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_062: [ umocktypes_stringify_unsignedint shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedint_with_positive_value)
{
    // arrange
    unsigned int input = 127;

    // act
    char* result = umocktypes_stringify_unsignedint(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_063: [ If value is NULL, umocktypes_stringify_unsignedint shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedint_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_unsignedint(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_064: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedint shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_unsignedint_fails)
{
    // arrange
    unsigned int input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_unsignedint(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_unsignedint */

/* Tests_SRS_UMOCKTYPES_C_01_066: [ umocktypes_are_equal_unsignedint shall compare the 2 unsigned ints pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_068: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedint shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedint_with_2_equal_values_returns_1)
{
    // arrange
    unsigned int left = 0x42;
    unsigned int right = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedint(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_069: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedint shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedint_with_2_different_values_returns_0)
{
    // arrange
    unsigned int left = 0x42;
    unsigned int right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedint(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_067: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedint shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedint_with_NULL_left_fails)
{
    // arrange
    unsigned int right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedint(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_067: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedint shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedint_with_NULL_right_fails)
{
    // arrange
    unsigned int left = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedint(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_unsignedint */

/* Tests_SRS_UMOCKTYPES_C_01_070: [ umocktypes_copy_unsignedint shall copy the unsigned int value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_071: [ On success umocktypes_copy_unsignedint shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedint_succeeds)
{
    // arrange
    unsigned int destination = 0;
    unsigned int source = 0x42;

    // act
    int result = umocktypes_copy_unsignedint(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (unsigned int)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_072: [ If source or destination are NULL, umocktypes_copy_unsignedint shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedint_with_NULL_destination_fails)
{
    // arrange
    unsigned int source = 0x42;

    // act
    int result = umocktypes_copy_unsignedint(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_072: [ If source or destination are NULL, umocktypes_copy_unsignedint shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedint_with_NULL_source_fails)
{
    // arrange
    unsigned int destination = 0;

    // act
    int result = umocktypes_copy_unsignedint(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_unsignedint */

/* Tests_SRS_UMOCKTYPES_C_01_073: [ umocktypes_free_unsignedint shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_unsignedint_does_nothing)
{
    // arrange
    unsigned int value = 0;

    // act
    umocktypes_free_unsignedint(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_long */

/* Tests_SRS_UMOCKTYPES_C_01_074: [ umocktypes_stringify_long shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_long_with_0_value)
{
    // arrange
    long input = 0;

    // act
    char* result = umocktypes_stringify_long(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_074: [ umocktypes_stringify_long shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_long_with_negative_value)
{
    // arrange
    long input = -127 - 1;

    // act
    char* result = umocktypes_stringify_long(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-128", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_074: [ umocktypes_stringify_long shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_long_with_positive_value)
{
    // arrange
    long input = 127;

    // act
    char* result = umocktypes_stringify_long(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_075: [ If value is NULL, umocktypes_stringify_long shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_long_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_long(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_076: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_long shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_long_fails)
{
    // arrange
    long input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_long(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_long */

/* Tests_SRS_UMOCKTYPES_C_01_078: [ umocktypes_are_equal_long shall compare the 2 longs pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_080: [ If the values pointed to by left and right are equal, umocktypes_are_equal_long shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_long_with_2_equal_values_returns_1)
{
    // arrange
    long left = 0x42;
    long right = 0x42;

    // act
    int result = umocktypes_are_equal_long(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_081: [ If the values pointed to by left and right are different, umocktypes_are_equal_long shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_long_with_2_different_values_returns_0)
{
    // arrange
    long left = 0x42;
    long right = 0x43;

    // act
    int result = umocktypes_are_equal_long(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_079: [ If any of the arguments is NULL, umocktypes_are_equal_long shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_long_with_NULL_left_fails)
{
    // arrange
    long right = 0x43;

    // act
    int result = umocktypes_are_equal_long(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_079: [ If any of the arguments is NULL, umocktypes_are_equal_long shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_long_with_NULL_right_fails)
{
    // arrange
    long left = 0x42;

    // act
    int result = umocktypes_are_equal_long(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_long */

/* Tests_SRS_UMOCKTYPES_C_01_082: [ umocktypes_copy_long shall copy the long value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_083: [ On success umocktypes_copy_long shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_long_succeeds)
{
    // arrange
    long destination = 0;
    long source = 0x42;

    // act
    int result = umocktypes_copy_long(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_084: [ If source or destination are NULL, umocktypes_copy_long shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_long_with_NULL_destination_fails)
{
    // arrange
    long source = 0x42;

    // act
    int result = umocktypes_copy_long(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_084: [ If source or destination are NULL, umocktypes_copy_long shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_long_with_NULL_source_fails)
{
    // arrange
    long destination = 0;

    // act
    int result = umocktypes_copy_long(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_long */

/* Tests_SRS_UMOCKTYPES_C_01_085: [ umocktypes_free_long shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_long_does_nothing)
{
    // arrange
    long value = 0;

    // act
    umocktypes_free_long(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_unsignedlong */

/* Tests_SRS_UMOCKTYPES_C_01_086: [ umocktypes_stringify_unsignedlong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlong_with_0_value)
{
    // arrange
    unsigned long input = 0;

    // act
    char* result = umocktypes_stringify_unsignedlong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_086: [ umocktypes_stringify_unsignedlong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlong_with_positive_value)
{
    // arrange
    unsigned long input = 127;

    // act
    char* result = umocktypes_stringify_unsignedlong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_087: [ If value is NULL, umocktypes_stringify_unsignedlong shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlong_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_unsignedlong(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_088: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlong shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_unsignedlong_fails)
{
    // arrange
    unsigned long input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_unsignedlong(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_unsignedlong */

/* Tests_SRS_UMOCKTYPES_C_01_090: [ umocktypes_are_equal_unsignedlong shall compare the 2 unsigned longs pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_092: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlong shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlong_with_2_equal_values_returns_1)
{
    // arrange
    unsigned long left = 0x42;
    unsigned long right = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedlong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_093: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlong shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlong_with_2_different_values_returns_0)
{
    // arrange
    unsigned long left = 0x42;
    unsigned long right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedlong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_091: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlong_with_NULL_left_fails)
{
    // arrange
    unsigned long right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedlong(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_091: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlong_with_NULL_right_fails)
{
    // arrange
    unsigned long left = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedlong(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_unsignedlong */

/* Tests_SRS_UMOCKTYPES_C_01_094: [ umocktypes_copy_unsignedlong shall copy the unsigned long value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_095: [ On success umocktypes_copy_unsignedlong shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlong_succeeds)
{
    // arrange
    unsigned long destination = 0;
    unsigned long source = 0x42;

    // act
    int result = umocktypes_copy_unsignedlong(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_096: [ If source or destination are NULL, umocktypes_copy_unsignedlong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlong_with_NULL_destination_fails)
{
    // arrange
    unsigned long source = 0x42;

    // act
    int result = umocktypes_copy_unsignedlong(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_096: [ If source or destination are NULL, umocktypes_copy_unsignedlong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlong_with_NULL_source_fails)
{
    // arrange
    unsigned long destination = 0;

    // act
    int result = umocktypes_copy_unsignedlong(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_unsignedlong */

/* Tests_SRS_UMOCKTYPES_C_01_097: [ umocktypes_free_unsignedlong shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_unsignedlong_does_nothing)
{
    // arrange
    unsigned long value = 0;

    // act
    umocktypes_free_unsignedlong(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_longlong */

/* Tests_SRS_UMOCKTYPES_C_01_098: [ umocktypes_stringify_longlong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longlong_with_0_value)
{
    // arrange
    long long input = 0;

    // act
    char* result = umocktypes_stringify_longlong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_098: [ umocktypes_stringify_longlong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longlong_with_negative_value)
{
    // arrange
    long long input = -127 - 1;

    // act
    char* result = umocktypes_stringify_longlong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "-128", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_098: [ umocktypes_stringify_longlong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longlong_with_positive_value)
{
    // arrange
    long long input = 127;

    // act
    char* result = umocktypes_stringify_longlong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_099: [ If value is NULL, umocktypes_stringify_longlong shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_longlong_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_longlong(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_100: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_longlong shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_longlong_fails)
{
    // arrange
    long long input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_longlong(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_longlong */

/* Tests_SRS_UMOCKTYPES_C_01_102: [ umocktypes_are_equal_longlong shall compare the 2 long longs pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_104: [ If the values pointed to by left and right are equal, umocktypes_are_equal_longlong shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longlong_with_2_equal_values_returns_1)
{
    // arrange
    long long left = 0x42;
    long long right = 0x42;

    // act
    int result = umocktypes_are_equal_longlong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_105: [ If the values pointed to by left and right are different, umocktypes_are_equal_longlong shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_longlong_with_2_different_values_returns_0)
{
    // arrange
    long long left = 0x42;
    long long right = 0x43;

    // act
    int result = umocktypes_are_equal_longlong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_103: [ If any of the arguments is NULL, umocktypes_are_equal_longlong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longlong_with_NULL_left_fails)
{
    // arrange
    long long right = 0x43;

    // act
    int result = umocktypes_are_equal_longlong(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_103: [ If any of the arguments is NULL, umocktypes_are_equal_longlong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longlong_with_NULL_right_fails)
{
    // arrange
    long long left = 0x42;

    // act
    int result = umocktypes_are_equal_longlong(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_longlong */

/* Tests_SRS_UMOCKTYPES_C_01_106: [ umocktypes_copy_longlong shall copy the long long value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_107: [ On success umocktypes_copy_longlong shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_longlong_succeeds)
{
    // arrange
    long long destination = 0;
    long long source = 0x42;

    // act
    int result = umocktypes_copy_longlong(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_108: [ If source or destination are NULL, umocktypes_copy_longlong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_longlong_with_NULL_destination_fails)
{
    // arrange
    long long source = 0x42;

    // act
    int result = umocktypes_copy_longlong(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_108: [ If source or destination are NULL, umocktypes_copy_longlong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_longlong_with_NULL_source_fails)
{
    // arrange
    long long destination = 0;

    // act
    int result = umocktypes_copy_longlong(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_longlong */

/* Tests_SRS_UMOCKTYPES_C_01_109: [ umocktypes_free_longlong shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_longlong_does_nothing)
{
    // arrange
    long long value = 0;

    // act
    umocktypes_free_longlong(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_unsignedlonglong */

/* Tests_SRS_UMOCKTYPES_C_01_110: [ umocktypes_stringify_unsignedlonglong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlonglong_with_0_value)
{
    // arrange
    unsigned long long input = 0;

    // act
    char* result = umocktypes_stringify_unsignedlonglong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "0", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_110: [ umocktypes_stringify_unsignedlonglong shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlonglong_with_positive_value)
{
    // arrange
    unsigned long long input = 127;

    // act
    char* result = umocktypes_stringify_unsignedlonglong(&input);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "127", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_111: [ If value is NULL, umocktypes_stringify_unsignedlonglong shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_unsignedlonglong_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_unsignedlonglong(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_112: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlonglong shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_unsignedlonglong_fails)
{
    // arrange
    unsigned long long input = 127;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_unsignedlonglong(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_unsignedlonglong */

/* Tests_SRS_UMOCKTYPES_C_01_114: [ umocktypes_are_equal_unsignedlonglong shall compare the 2 unsigned long longs pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_116: [ If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlonglong shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlonglong_with_2_equal_values_returns_1)
{
    // arrange
    unsigned long long left = 0x42;
    unsigned long long right = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedlonglong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_117: [ If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlonglong shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlonglong_with_2_different_values_returns_0)
{
    // arrange
    unsigned long long left = 0x42;
    unsigned long long right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedlonglong(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_115: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlonglong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlonglong_with_NULL_left_fails)
{
    // arrange
    unsigned long long right = 0x43;

    // act
    int result = umocktypes_are_equal_unsignedlonglong(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_115: [ If any of the arguments is NULL, umocktypes_are_equal_unsignedlonglong shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_unsignedlonglong_with_NULL_right_fails)
{
    // arrange
    unsigned long long left = 0x42;

    // act
    int result = umocktypes_are_equal_unsignedlonglong(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_unsignedlonglong */

/* Tests_SRS_UMOCKTYPES_C_01_118: [ umocktypes_copy_unsignedlonglong shall copy the unsigned long long value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_119: [ On success umocktypes_copy_unsignedlonglong shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlonglong_succeeds)
{
    // arrange
    unsigned long long destination = 0;
    unsigned long long source = 0x42;

    // act
    int result = umocktypes_copy_unsignedlonglong(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_120: [ If source or destination are NULL, umocktypes_copy_unsignedlonglong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlonglong_with_NULL_destination_fails)
{
    // arrange
    unsigned long long source = 0x42;

    // act
    int result = umocktypes_copy_unsignedlonglong(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_120: [ If source or destination are NULL, umocktypes_copy_unsignedlonglong shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_unsignedlonglong_with_NULL_source_fails)
{
    // arrange
    unsigned long long destination = 0;

    // act
    int result = umocktypes_copy_unsignedlonglong(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_unsignedlonglong */

/* Tests_SRS_UMOCKTYPES_C_01_121: [ umocktypes_free_unsignedlonglong shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_unsignedlonglong_does_nothing)
{
    // arrange
    unsigned long long value = 0;

    // act
    umocktypes_free_unsignedlonglong(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_float */

/* Tests_SRS_UMOCKTYPES_C_01_122: [ umocktypes_stringify_float shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_float_with_0_0_value)
{
    // arrange
    float input = 0.0f;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_float(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_122: [ umocktypes_stringify_float shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_float_with_negative_value)
{
    // arrange
    float input = -1.42f;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_float(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_122: [ umocktypes_stringify_float shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_float_with_positive_value)
{
    // arrange
    float input = 2.42f;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_float(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_123: [ If value is NULL, umocktypes_stringify_float shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_float_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_float(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_124: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_float shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_float_fails)
{
    // arrange
    float input = 0.42f;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_float(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_float */

/* Tests_SRS_UMOCKTYPES_C_01_126: [ umocktypes_are_equal_float shall compare the 2 floats pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_128: [ If the values pointed to by left and right are equal, umocktypes_are_equal_float shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_float_with_2_equal_values_returns_1)
{
    // arrange
    float left = 0x42;
    float right = 0x42;

    // act
    int result = umocktypes_are_equal_float(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_129: [ If the values pointed to by left and right are different, umocktypes_are_equal_float shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_float_with_2_different_values_returns_0)
{
    // arrange
    float left = 0x42;
    float right = 0x43;

    // act
    int result = umocktypes_are_equal_float(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_127: [ If any of the arguments is NULL, umocktypes_are_equal_float shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_float_with_NULL_left_fails)
{
    // arrange
    float right = 0x43;

    // act
    int result = umocktypes_are_equal_float(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_127: [ If any of the arguments is NULL, umocktypes_are_equal_float shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_float_with_NULL_right_fails)
{
    // arrange
    float left = 0x42;

    // act
    int result = umocktypes_are_equal_float(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_float */

/* Tests_SRS_UMOCKTYPES_C_01_130: [ umocktypes_copy_float shall copy the float value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_131: [ On success umocktypes_copy_float shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_float_succeeds)
{
    // arrange
    float destination = 0;
    float source = 0x42;

    // act
    int result = umocktypes_copy_float(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_132: [ If source or destination are NULL, umocktypes_copy_float shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_float_with_NULL_destination_fails)
{
    // arrange
    float source = 0x42;

    // act
    int result = umocktypes_copy_float(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_132: [ If source or destination are NULL, umocktypes_copy_float shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_float_with_NULL_source_fails)
{
    // arrange
    float destination = 0;

    // act
    int result = umocktypes_copy_float(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_float */

/* Tests_SRS_UMOCKTYPES_C_01_133: [ umocktypes_free_float shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_float_does_nothing)
{
    // arrange
    float value = 0;

    // act
    umocktypes_free_float(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_double */

/* Tests_SRS_UMOCKTYPES_C_01_134: [ umocktypes_stringify_double shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_double_with_0_0_value)
{
    // arrange
    double input = 0.0;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_double(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_134: [ umocktypes_stringify_double shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_double_with_negative_value)
{
    // arrange
    double input = -1.42;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_double(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_134: [ umocktypes_stringify_double shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_double_with_positive_value)
{
    // arrange
    double input = 2.42;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_double(&input);

    // assert
    (void)sprintf(expected_string, "%f", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_135: [ If value is NULL, umocktypes_stringify_double shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_double_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_double(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_136: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_double shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_double_fails)
{
    // arrange
    double input = 0.42;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_double(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_double */

/* Tests_SRS_UMOCKTYPES_C_01_138: [ umocktypes_are_equal_double shall compare the 2 doubles pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_140: [ If the values pointed to by left and right are equal, umocktypes_are_equal_double shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_double_with_2_equal_values_returns_1)
{
    // arrange
    double left = 0x42;
    double right = 0x42;

    // act
    int result = umocktypes_are_equal_double(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_141: [ If the values pointed to by left and right are different, umocktypes_are_equal_double shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_double_with_2_different_values_returns_0)
{
    // arrange
    double left = 0x42;
    double right = 0x43;

    // act
    int result = umocktypes_are_equal_double(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_139: [ If any of the arguments is NULL, umocktypes_are_equal_double shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_double_with_NULL_left_fails)
{
    // arrange
    double right = 0x43;

    // act
    int result = umocktypes_are_equal_double(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_139: [ If any of the arguments is NULL, umocktypes_are_equal_double shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_double_with_NULL_right_fails)
{
    // arrange
    double left = 0x42;

    // act
    int result = umocktypes_are_equal_double(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_double */

/* Tests_SRS_UMOCKTYPES_C_01_142: [ umocktypes_copy_double shall copy the double value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_143: [ On success umocktypes_copy_double shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_double_succeeds)
{
    // arrange
    double destination = 0;
    double source = 0x42;

    // act
    int result = umocktypes_copy_double(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_144: [ If source or destination are NULL, umocktypes_copy_double shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_double_with_NULL_destination_fails)
{
    // arrange
    double source = 0x42;

    // act
    int result = umocktypes_copy_double(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_144: [ If source or destination are NULL, umocktypes_copy_double shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_double_with_NULL_source_fails)
{
    // arrange
    double destination = 0;

    // act
    int result = umocktypes_copy_double(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_double */

/* Tests_SRS_UMOCKTYPES_C_01_145: [ umocktypes_free_double shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_double_does_nothing)
{
    // arrange
    double value = 0;

    // act
    umocktypes_free_double(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_longdouble */

/* Tests_SRS_UMOCKTYPES_C_01_146: [ umocktypes_stringify_longdouble shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longdouble_with_0_0_value)
{
    // arrange
    long double input = 0.0;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_longdouble(&input);

    // assert
    (void)sprintf(expected_string, "%Lf", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_146: [ umocktypes_stringify_longdouble shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longdouble_with_negative_value)
{
    // arrange
    long double input = -1.42;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_longdouble(&input);

    // assert
    (void)sprintf(expected_string, "%Lf", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_146: [ umocktypes_stringify_longdouble shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_longdouble_with_positive_value)
{
    // arrange
    long double input = 2.42;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_longdouble(&input);

    // assert
    (void)sprintf(expected_string, "%Lf", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_147: [ If value is NULL, umocktypes_stringify_longdouble shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_longdouble_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_longdouble(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_148: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_longdouble shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_longdouble_fails)
{
    // arrange
    long double input = 0.42;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_longdouble(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_longdouble */

/* Tests_SRS_UMOCKTYPES_C_01_150: [ umocktypes_are_equal_longdouble shall compare the 2 long doubles pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_152: [ If the values pointed to by left and right are equal, umocktypes_are_equal_longdouble shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longdouble_with_2_equal_values_returns_1)
{
    // arrange
    long double left = 0x42;
    long double right = 0x42;

    // act
    int result = umocktypes_are_equal_longdouble(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_153: [ If the values pointed to by left and right are different, umocktypes_are_equal_longdouble shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_longdouble_with_2_different_values_returns_0)
{
    // arrange
    long double left = 0x42;
    long double right = 0x43;

    // act
    int result = umocktypes_are_equal_longdouble(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_151: [ If any of the arguments is NULL, umocktypes_are_equal_longdouble shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longdouble_with_NULL_left_fails)
{
    // arrange
    long double right = 0x43;

    // act
    int result = umocktypes_are_equal_longdouble(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_151: [ If any of the arguments is NULL, umocktypes_are_equal_longdouble shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_longdouble_with_NULL_right_fails)
{
    // arrange
    long double left = 0x42;

    // act
    int result = umocktypes_are_equal_longdouble(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_longdouble */

/* Tests_SRS_UMOCKTYPES_C_01_154: [ umocktypes_copy_longdouble shall copy the long double value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_155: [ On success umocktypes_copy_longdouble shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_longdouble_succeeds)
{
    // arrange
    long double destination = 0;
    long double source = 0x42;

    // act
    int result = umocktypes_copy_longdouble(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_156: [ If source or destination are NULL, umocktypes_copy_longdouble shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_longdouble_with_NULL_destination_fails)
{
    // arrange
    long double source = 0x42;

    // act
    int result = umocktypes_copy_longdouble(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_156: [ If source or destination are NULL, umocktypes_copy_longdouble shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_longdouble_with_NULL_source_fails)
{
    // arrange
    long double destination = 0;

    // act
    int result = umocktypes_copy_longdouble(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_longdouble */

/* Tests_SRS_UMOCKTYPES_C_01_157: [ umocktypes_free_longdouble shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_longdouble_does_nothing)
{
    // arrange
    long double value = 0;

    // act
    umocktypes_free_longdouble(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_size_t */

/* Tests_SRS_UMOCKTYPES_C_01_158: [ umocktypes_stringify_size_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_size_t_with_0_0_value)
{
    // arrange
    size_t input = 0;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_size_t(&input);

    // assert
    (void)sprintf(expected_string, "%zu", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_158: [ umocktypes_stringify_size_t shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_size_t_with_max_value)
{
    // arrange
    size_t input = (size_t)-1;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_size_t(&input);

    // assert
    (void)sprintf(expected_string, "%zu", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_159: [ If value is NULL, umocktypes_stringify_size_t shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_size_t_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_size_t(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_160: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_size_t shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_size_t_fails)
{
    // arrange
    size_t input = 42;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_size_t(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_size_t */

/* Tests_SRS_UMOCKTYPES_C_01_162: [ umocktypes_are_equal_size_t shall compare the 2 size_ts pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_164: [ If the values pointed to by left and right are equal, umocktypes_are_equal_size_t shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_size_t_with_2_equal_values_returns_1)
{
    // arrange
    size_t left = 0x42;
    size_t right = 0x42;

    // act
    int result = umocktypes_are_equal_size_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_165: [ If the values pointed to by left and right are different, umocktypes_are_equal_size_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_size_t_with_2_different_values_returns_0)
{
    // arrange
    size_t left = 0x42;
    size_t right = 0x43;

    // act
    int result = umocktypes_are_equal_size_t(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_163: [ If any of the arguments is NULL, umocktypes_are_equal_size_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_size_t_with_NULL_left_fails)
{
    // arrange
    size_t right = 0x43;

    // act
    int result = umocktypes_are_equal_size_t(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_163: [ If any of the arguments is NULL, umocktypes_are_equal_size_t shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_size_t_with_NULL_right_fails)
{
    // arrange
    size_t left = 0x42;

    // act
    int result = umocktypes_are_equal_size_t(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_size_t */

/* Tests_SRS_UMOCKTYPES_C_01_166: [ umocktypes_copy_size_t shall copy the size_t value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_167: [ On success umocktypes_copy_size_t shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_size_t_succeeds)
{
    // arrange
    size_t destination = 0;
    size_t source = 0x42;

    // act
    int result = umocktypes_copy_size_t(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(long, 0x42, (long)destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_168: [ If source or destination are NULL, umocktypes_copy_size_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_size_t_with_NULL_destination_fails)
{
    // arrange
    size_t source = 0x42;

    // act
    int result = umocktypes_copy_size_t(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_168: [ If source or destination are NULL, umocktypes_copy_size_t shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_size_t_with_NULL_source_fails)
{
    // arrange
    size_t destination = 0;

    // act
    int result = umocktypes_copy_size_t(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_size_t */

/* Tests_SRS_UMOCKTYPES_C_01_169: [ umocktypes_free_size_t shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_size_t_does_nothing)
{
    // arrange
    size_t value = 0;

    // act
    umocktypes_free_size_t(&value);

    // assert
    // no explicit assert
}

/* umocktypes_stringify_void_ptr */

/* Tests_SRS_UMOCKTYPES_C_01_170: [ umocktypes_stringify_void_ptr shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_void_ptr_with_NULL_value)
{
    // arrange
    void* input = NULL;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_void_ptr(&input);

    // assert
    (void)sprintf(expected_string, "%p", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_170: [ umocktypes_stringify_void_ptr shall return the string representation of value. ]*/
TEST_FUNCTION(umocktypes_stringify_void_ptr_with_a_pointer)
{
    // arrange
    void* input = (void*)0x4242;
    char expected_string[32];

    // act
    char* result = umocktypes_stringify_void_ptr(&input);

    // assert
    (void)sprintf(expected_string, "%p", input);
    ASSERT_ARE_EQUAL(char_ptr, expected_string, result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_171: [ If value is NULL, umocktypes_stringify_void_ptr shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_void_ptr_with_NULL_fails)
{
    // arrange

    // act
    char* result = umocktypes_stringify_void_ptr(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_C_01_172: [ If allocating a new string to hold the string representation fails, umocktypes_stringify_void_ptr shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_stringify_void_ptr_fails)
{
    // arrange
    void* input = (void*)0x4242;
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypes_stringify_void_ptr(&input);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal_void_ptr */

/* Tests_SRS_UMOCKTYPES_C_01_174: [ umocktypes_are_equal_void_ptr shall compare the 2 void_ptrs pointed to by left and right. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_176: [ If the values pointed to by left and right are equal, umocktypes_are_equal_void_ptr shall return 1. ]*/
TEST_FUNCTION(umocktypes_are_equal_void_ptr_with_2_equal_values_returns_1)
{
    // arrange
    void* left = (void*)0x4242;
    void* right = (void*)0x4242;

    // act
    int result = umocktypes_are_equal_void_ptr(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_177: [ If the values pointed to by left and right are different, umocktypes_are_equal_void_ptr shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_void_ptr_with_2_different_values_returns_0)
{
    // arrange
    void* left = (void*)0x4242;
    void* right = (void*)0x4243;

    // act
    int result = umocktypes_are_equal_void_ptr(&left, &right);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_175: [ If any of the arguments is NULL, umocktypes_are_equal_void_ptr shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_void_ptr_with_NULL_left_fails)
{
    // arrange
    void* right = (void*)0x4243;

    // act
    int result = umocktypes_are_equal_void_ptr(NULL, &right);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_175: [ If any of the arguments is NULL, umocktypes_are_equal_void_ptr shall return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_void_ptr_with_NULL_right_fails)
{
    // arrange
    void* left = (void*)0x4242;

    // act
    int result = umocktypes_are_equal_void_ptr(&left, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* umocktypes_copy_void_ptr */

/* Tests_SRS_UMOCKTYPES_C_01_178: [ umocktypes_copy_void_ptr shall copy the void_ptr value from source to destination. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_179: [ On success umocktypes_copy_void_ptr shall return 0. ]*/
TEST_FUNCTION(umocktypes_copy_void_ptr_succeeds)
{
    // arrange
    void* destination = NULL;
    void* source = (void*)0x4242;

    // act
    int result = umocktypes_copy_void_ptr(&destination, &source);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(void_ptr, (void*)0x4242, destination);
}

/* Tests_SRS_UMOCKTYPES_C_01_180: [ If source or destination are NULL, umocktypes_copy_void_ptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_void_ptr_with_NULL_destination_fails)
{
    // arrange
    void* source = (void*)0x4242;

    // act
    int result = umocktypes_copy_void_ptr(NULL, &source);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_C_01_180: [ If source or destination are NULL, umocktypes_copy_void_ptr shall return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_copy_void_ptr_with_NULL_source_fails)
{
    // arrange
    void* destination = NULL;

    // act
    int result = umocktypes_copy_void_ptr(&destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_free_void_ptr */

/* Tests_SRS_UMOCKTYPES_C_01_181: [ umocktypes_free_void_ptr shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_void_ptr_does_nothing)
{
    // arrange
    void* value = NULL;

    // act
    umocktypes_free_void_ptr(&value);

    // assert
    // no explicit assert
}

/* umocktypes_c_register_types */

/* Tests_SRS_UMOCKTYPES_C_01_001: [ umocktypes_c_register_types shall register support for all the types in the module. ]*/
/* Tests_SRS_UMOCKTYPES_C_01_170: [ On success, umocktypes_c_register_types shall return 0. ]*/
TEST_FUNCTION(umocktypes_c_register_types_registers_all_types)
{
    // arrange
    size_t i;

    umocktypes_register_type_fail_call_result = 0;

    // act
    int result = umocktypes_c_register_types();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 16, umocktypes_register_type_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char", umocktypes_register_type_calls[0].type);
    ASSERT_ARE_EQUAL(char_ptr, "unsigned char", umocktypes_register_type_calls[1].type);
    ASSERT_ARE_EQUAL(char_ptr, "short", umocktypes_register_type_calls[2].type);
    ASSERT_ARE_EQUAL(char_ptr, "unsigned short", umocktypes_register_type_calls[3].type);
    ASSERT_ARE_EQUAL(char_ptr, "int", umocktypes_register_type_calls[4].type);
    ASSERT_ARE_EQUAL(char_ptr, "unsigned int", umocktypes_register_type_calls[5].type);
    ASSERT_ARE_EQUAL(char_ptr, "long", umocktypes_register_type_calls[6].type);
    ASSERT_ARE_EQUAL(char_ptr, "unsigned long", umocktypes_register_type_calls[7].type);
    ASSERT_ARE_EQUAL(char_ptr, "long long", umocktypes_register_type_calls[8].type);
    ASSERT_ARE_EQUAL(char_ptr, "unsigned long long", umocktypes_register_type_calls[9].type);
    ASSERT_ARE_EQUAL(char_ptr, "float", umocktypes_register_type_calls[10].type);
    ASSERT_ARE_EQUAL(char_ptr, "double", umocktypes_register_type_calls[11].type);
    ASSERT_ARE_EQUAL(char_ptr, "long double", umocktypes_register_type_calls[12].type);
    ASSERT_ARE_EQUAL(char_ptr, "size_t", umocktypes_register_type_calls[13].type);
    ASSERT_ARE_EQUAL(char_ptr, "void*", umocktypes_register_type_calls[14].type);
    ASSERT_ARE_EQUAL(char_ptr, "const void*", umocktypes_register_type_calls[15].type);
    
    for (i = 0; i < 16; i++)
    {
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].stringify_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].are_equal_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].copy_func);
        ASSERT_IS_NOT_NULL(umocktypes_register_type_calls[i].free_func);
    }
}

/* Tests_SRS_UMOCKTYPES_C_01_171: [ If registering any of the types fails, umocktypes_c_register_types shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_the_underlying_register_fails_umocktypes_c_register_types_fails)
{
    size_t i;

    for (i = 0; i < 14; i++)
    {
        // arrange
        reset_umocktypes_register_type_calls();
        umocktypes_register_type_fail_call_result = 1;
        when_shall_umocktypes_register_typecall_fail = i + 1;

        // act
        int result = umocktypes_c_register_types();

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(int, i + 1, umocktypes_register_type_call_count);
    }
}

END_TEST_SUITE(umocktypes_c_unittests)
