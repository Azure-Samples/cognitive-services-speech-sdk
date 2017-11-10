// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"

typedef struct umocktypename_normalize_CALL_TAG
{
    char* type_name;
} umocktypename_normalize_CALL;

static umocktypename_normalize_CALL* umocktypename_normalize_calls;
static size_t umocktypename_normalize_call_count;
static const char* umocktypename_normalize_call_result;

typedef struct test_stringify_func_testtype_CALL_TAG
{
    const void* value;
} test_stringify_func_testtype_CALL;

static test_stringify_func_testtype_CALL* test_stringify_func_testtype_calls;
static size_t test_stringify_func_testtype_call_count;
static const char* test_stringify_func_testtype_call_result;

typedef struct test_are_equal_func_testtype_CALL_TAG
{
    const void* left;
    const void* right;
} test_are_equal_func_testtype_CALL;

static test_are_equal_func_testtype_CALL* test_are_equal_func_testtype_calls;
static size_t test_are_equal_func_testtype_call_count;
static int test_are_equal_func_testtype_call_result;

typedef struct test_copy_func_testtype_CALL_TAG
{
    const void* destination;
    const void* source;
} test_copy_func_testtype_CALL;

static test_copy_func_testtype_CALL* test_copy_func_testtype_calls;
static size_t test_copy_func_testtype_call_count;
static int test_copy_func_testtype_call_result;

typedef struct test_free_func_testtype_CALL_TAG
{
    void* value;
} test_free_func_testtype_CALL;

static test_free_func_testtype_CALL* test_free_func_testtype_calls;
static size_t test_free_func_testtype_call_count;

static size_t malloc_call_count;
static size_t calloc_call_count;
static size_t realloc_call_count;

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
        free(ptr);
    }

    char* umocktypename_normalize(const char* type_name)
    {
        char* result;

        umocktypename_normalize_CALL* new_calls = (umocktypename_normalize_CALL*)realloc(umocktypename_normalize_calls, sizeof(umocktypename_normalize_CALL) * (umocktypename_normalize_call_count + 1));
        if (new_calls != NULL)
        {
            size_t type_name_length = strlen(type_name);
            umocktypename_normalize_calls = new_calls;
            umocktypename_normalize_calls[umocktypename_normalize_call_count].type_name = (char*)malloc(type_name_length + 1);
            (void)memcpy(umocktypename_normalize_calls[umocktypename_normalize_call_count].type_name, type_name, type_name_length + 1);
            umocktypename_normalize_call_count++;
        }

        if (umocktypename_normalize_call_result != NULL)
        {
            size_t result_length = strlen(umocktypename_normalize_call_result);
            result = (char*)malloc(result_length + 1);
            (void)memcpy(result, umocktypename_normalize_call_result, result_length + 1);
        }
        else
        {
            result = NULL;
        }

        return result;
    }

#ifdef __cplusplus
}
#endif

static char* test_stringify_func_testtype(const void* value)
{
    char* result;
    test_stringify_func_testtype_CALL* new_calls = (test_stringify_func_testtype_CALL*)realloc(test_stringify_func_testtype_calls, sizeof(test_stringify_func_testtype_CALL) * (test_stringify_func_testtype_call_count + 1));
    if (new_calls != NULL)
    {
        test_stringify_func_testtype_calls = new_calls;
        test_stringify_func_testtype_calls[test_stringify_func_testtype_call_count].value = value;
        test_stringify_func_testtype_call_count++;
    }

    if (test_stringify_func_testtype_call_result != NULL)
    {
        size_t result_length = strlen(test_stringify_func_testtype_call_result);
        result = (char*)malloc(result_length + 1);
        (void)memcpy(result, test_stringify_func_testtype_call_result, result_length + 1);
    }
    else
    {
        result = NULL;
    }

    return result;
}

static int test_copy_func_testtype(void* destination, const void* source)
{
    test_copy_func_testtype_CALL* new_calls = (test_copy_func_testtype_CALL*)realloc(test_copy_func_testtype_calls, sizeof(test_copy_func_testtype_CALL) * (test_copy_func_testtype_call_count + 1));
    if (new_calls != NULL)
    {
        test_copy_func_testtype_calls = new_calls;
        test_copy_func_testtype_calls[test_copy_func_testtype_call_count].destination = destination;
        test_copy_func_testtype_calls[test_copy_func_testtype_call_count].source = source;
        test_copy_func_testtype_call_count++;
    }

    return test_copy_func_testtype_call_result;
}

void test_free_func_testtype(void* value)
{
    test_free_func_testtype_CALL* new_calls = (test_free_func_testtype_CALL*)realloc(test_free_func_testtype_calls, sizeof(test_free_func_testtype_CALL) * (test_free_func_testtype_call_count + 1));
    if (new_calls != NULL)
    {
        test_free_func_testtype_calls = new_calls;
        test_free_func_testtype_calls[test_free_func_testtype_call_count].value = value;
        test_free_func_testtype_call_count++;
    }
}

int test_are_equal_func_testtype(const void* left, const void* right)
{
    test_are_equal_func_testtype_CALL* new_calls = (test_are_equal_func_testtype_CALL*)realloc(test_are_equal_func_testtype_calls, sizeof(test_are_equal_func_testtype_CALL) * (test_are_equal_func_testtype_call_count + 1));
    if (new_calls != NULL)
    {
        test_are_equal_func_testtype_calls = new_calls;
        test_are_equal_func_testtype_calls[test_are_equal_func_testtype_call_count].left = left;
        test_are_equal_func_testtype_calls[test_are_equal_func_testtype_call_count].right = right;
        test_are_equal_func_testtype_call_count++;
    }

    return test_are_equal_func_testtype_call_result;
}

static void* test_value_1 = (void*)0x4242;
static void* test_value_2 = (void*)0x4243;

void reset_umocktypename_normalize_calls(void)
{
    if (umocktypename_normalize_calls != NULL)
    {
        size_t i;
        for (i = 0; i < umocktypename_normalize_call_count; i++)
        {
            free(umocktypename_normalize_calls[i].type_name);
        }

        free(umocktypename_normalize_calls);
        umocktypename_normalize_calls = NULL;
    }
    umocktypename_normalize_call_count = 0;
}

void reset_test_stringify_testtype_calls(void)
{
    if (test_stringify_func_testtype_calls != NULL)
    {
        free(test_stringify_func_testtype_calls);
        test_stringify_func_testtype_calls = NULL;
    }
    test_stringify_func_testtype_call_count = 0;
}

void reset_test_are_equal_testtype_calls(void)
{
    if (test_are_equal_func_testtype_calls != NULL)
    {
        free(test_are_equal_func_testtype_calls);
        test_are_equal_func_testtype_calls = NULL;
    }
    test_are_equal_func_testtype_call_count = 0;
}

void reset_test_copy_testtype_calls(void)
{
    if (test_copy_func_testtype_calls != NULL)
    {
        free(test_copy_func_testtype_calls);
        test_copy_func_testtype_calls = NULL;
    }
    test_copy_func_testtype_call_count = 0;
}

void reset_test_free_testtype_calls(void)
{
    if (test_free_func_testtype_calls != NULL)
    {
        free(test_free_func_testtype_calls);
        test_free_func_testtype_calls = NULL;
    }
    test_free_func_testtype_call_count = 0;
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

BEGIN_TEST_SUITE(umocktypes_without_init_unittests)

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

    umocktypename_normalize_calls = NULL;
    umocktypename_normalize_call_count = 0;
    umocktypename_normalize_call_result = NULL;

    test_stringify_func_testtype_calls = NULL;
    test_stringify_func_testtype_call_count = 0;
    test_stringify_func_testtype_call_result = NULL;

    test_are_equal_func_testtype_calls = NULL;
    test_are_equal_func_testtype_call_count = 0;
    test_are_equal_func_testtype_call_result = 1;

    test_copy_func_testtype_calls = NULL;
    test_copy_func_testtype_call_count = 0;
    test_copy_func_testtype_call_result = 1;

    test_free_func_testtype_calls = NULL;
    test_free_func_testtype_call_count = 0;

    malloc_call_count = 0;
    when_shall_malloc_fail = 0;
    calloc_call_count = 0;
    when_shall_calloc_fail = 0;
    realloc_call_count = 0;
    when_shall_realloc_fail = 0;
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    reset_umocktypename_normalize_calls();
    reset_test_stringify_testtype_calls();
    reset_test_are_equal_testtype_calls();
    reset_test_copy_testtype_calls();
    reset_test_free_testtype_calls();

    umocktypes_deinit();

    TEST_MUTEX_RELEASE(test_mutex);
}

/* umocktypes_deinit */

/* Tests_SRS_UMOCKTYPES_01_006: [ If the module was not initialized, umocktypes_deinit shall do nothing. ]*/
TEST_FUNCTION(umocktypes_deinit_if_the_module_was_not_initialized_shall_do_nothing)
{
    // arrange

    // act
    umocktypes_deinit();

    // assert
    // no explicit assert
}

/* umocktypes_register_type */

/* Tests_SRS_UMOCKTYPES_01_050: [ If umocktypes_register_type is called when the module is not initialized, umocktypes_register_type shall fail and return a non zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result = "char*";

    // act
    int result = umocktypes_register_type("char  *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_register_alias_type */

/* Tests_SRS_UMOCKTYPES_01_061: [ If umocktypes_register_alias_type is called when the module is not initialized, umocktypes_register_type shall fail and return a non zero value. ] */
TEST_FUNCTION(umocktypes_register_alias_when_module_is_not_initialized_fails)
{
    // arrange

    // act
    int result = umocktypes_register_alias_type("PSTR", "char*");

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 0, umocktypename_normalize_call_count);
}

/* umocktypes_stringify */

/* Tests_SRS_UMOCKTYPES_01_049: [ If umocktypes_stringify is called when the module is not initialized, umocktypes_stringify shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result = "char*";
    test_stringify_func_testtype_call_result = "blahblah";

    // act
    char* result = umocktypes_stringify("char  *", test_value_1);

    // assert
    ASSERT_IS_NULL(result);
}

/* umocktypes_are_equal */

/* Tests_SRS_UMOCKTYPES_01_046: [ If umocktypes_are_equal is called when the module is not initialized, umocktypes_are_equal shall return -1. ] */
TEST_FUNCTION(umocktypes_are_equal_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result = "char*";
    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* umocktypes_copy */

/* Tests_SRS_UMOCKTYPES_01_047: [ If umocktypes_copy is called when the module is not initialized, umocktypes_copy shall fail and return a non zero value. ]*/
TEST_FUNCTION(when_the_module_is_not_initialized_then_umocktypes_copy_fails)
{
    // arrange
    void* destination = (void*)0x4245;

    umocktypename_normalize_call_result = NULL;
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char *", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* umocktypes_free */

/* Tests_SRS_UMOCKTYPES_01_048: [ If umocktypes_free is called when the module is not initialized, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_when_the_module_is_not_initialized_does_not_free_anything)
{
    // arrange
    umocktypename_normalize_call_result = "char *";

    // act
    umocktypes_free("char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

END_TEST_SUITE(umocktypes_without_init_unittests)
