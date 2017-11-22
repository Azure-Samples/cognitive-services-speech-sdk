// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdint.h>
#include "testrunnerswitcher.h"
#include "umocktypes.h"

typedef struct umocktypename_normalize_CALL_TAG
{
    char* type_name;
} umocktypename_normalize_CALL;

#define umocktypename_normalize_max_calls 5

static umocktypename_normalize_CALL* umocktypename_normalize_calls;
static size_t umocktypename_normalize_call_count;
static const char* umocktypename_normalize_call_result[umocktypename_normalize_max_calls];

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

        if (umocktypename_normalize_call_result[umocktypename_normalize_call_count - 1] != NULL)
        {
            size_t result_length = strlen(umocktypename_normalize_call_result[umocktypename_normalize_call_count - 1]);
            result = (char*)malloc(result_length + 1);
            (void)memcpy(result, umocktypename_normalize_call_result[umocktypename_normalize_call_count - 1], result_length + 1);
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

static char* test_stringify_func_testtype_2(const void* value)
{
    (void)value;
    return NULL;
}

static int test_copy_func_testtype_2(void* destination, const void* source)
{
    (void)destination, source;
    return 0;
}

void test_free_func_testtype_2(void* value)
{
    (void)value;
}

int test_are_equal_func_testtype_2(const void* left, const void* right)
{
    (void)left, right;
    return 0;
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

BEGIN_TEST_SUITE(umocktypes_unittests)

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
    size_t i;

    int mutex_acquire_result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, mutex_acquire_result);

    umocktypename_normalize_calls = NULL;
    umocktypename_normalize_call_count = 0;
    for (i = 0; i < umocktypename_normalize_max_calls; i++)
    {
        umocktypename_normalize_call_result[i] = NULL;
    }

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

/* umocktypes_init */

/* Tests_SRS_UMOCKTYPES_01_001: [ umocktypes_init shall initialize the umocktypes module. ] */
/* Tests_SRS_UMOCKTYPES_01_003: [ On success umocktypes_init shall return 0. ]*/
TEST_FUNCTION(umocktypes_init_initializes_the_module)
{
    // arrange

    // act
    int result = umocktypes_init();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_01_002: [ After initialization the list of registered type shall be empty. ] */
TEST_FUNCTION(after_umocktypes_init_no_type_shall_be_registered)
{
    // arrange
    (void)umocktypes_init();
    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    umocktypes_deinit();
    (void)umocktypes_init();

    // act
    int result = umocktypes_are_equal("testtype", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
}

/* Tests_SRS_UMOCKTYPES_01_004: [ umocktypes_init after another umocktypes_init without deinitializing the module shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_init_after_umocktypes_init_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_init();

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_deinit */

/* Tests_SRS_UMOCKTYPES_01_005: [ umocktypes_deinit shall free all resources associated with the registered types and shall leave the module in a state where another init is possible. ]*/
TEST_FUNCTION(umocktypes_deinit_after_umocktypes_init_deinitializes_the_module)
{
    // arrange
    (void)umocktypes_init();
    (void)umocktypes_register_type("testtype1", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    (void)umocktypes_register_type("testtype2", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // act
    umocktypes_deinit();

    // assert
    // no explicit assert
}

/* Tests_SRS_UMOCKTYPES_01_005: [ umocktypes_deinit shall free all resources associated with the registered types and shall leave the module in a state where another init is possible. ]*/
TEST_FUNCTION(umocktypes_deinit_after_umocktypes_init_when_no_types_registered_deinitializes_the_module)
{
    // arrange
    (void)umocktypes_init();

    // act
    umocktypes_deinit();

    // assert
    // no explicit assert
}

/* Tests_SRS_UMOCKTYPES_01_040: [ An umocktypes_init call after deinit shall succeed provided all underlying calls succeed. ]*/
TEST_FUNCTION(umocktypes_init_after_umocktypes_deinit_succeeds)
{
    // arrange
    (void)umocktypes_init();
    (void)umocktypes_register_type("testtype1", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    (void)umocktypes_register_type("testtype2", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    umocktypes_deinit();

    // act
    int result = umocktypes_init();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

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

/* Tests_SRS_UMOCKTYPES_01_007: [ umocktypes_register_type shall register an interface made out of the stringify, are equal, copy and free functions for the type identified by the argument type. ] */
/* Tests_SRS_UMOCKTYPES_01_008: [ On success umocktypes_register_type shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_01_034: [ Before registering, the type string shall be normalized. ] */
TEST_FUNCTION(umocktypes_register_type_when_module_is_initialized_succeeds)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_009: [ If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_with_NULL_type_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_register_type(NULL, test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_009: [ If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_with_NULL_stringify_function_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_register_type("testtype", NULL, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_009: [ If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_with_NULL_are_equal_function_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, NULL, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_009: [ If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_with_NULL_copy_function_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, NULL, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_009: [ If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_with_NULL_free_function_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_010: [ If the type has already been registered with the same function pointers then umocktypes_register_type shall succeed and return 0. ]*/
TEST_FUNCTION(umocktypes_register_type_2_times_on_the_same_type_with_the_same_functions_succeeds)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_011: [ If the type has already been registered but at least one of the function pointers is different, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_2_times_on_the_same_type_with_different_stringify_function_fails)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype_2, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_011: [ If the type has already been registered but at least one of the function pointers is different, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_2_times_on_the_same_type_with_different_are_equal_function_fails)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype_2, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_011: [ If the type has already been registered but at least one of the function pointers is different, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_2_times_on_the_same_type_with_different_copy_function_fails)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype_2, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_011: [ If the type has already been registered but at least one of the function pointers is different, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_2_times_on_the_same_type_with_different_free_function_fails)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = "testtype";

    (void)umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    int result = umocktypes_register_type("testtype", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype_2);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "testtype", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_045: [ If normalizing the typename fails, umocktypes_register_type shall fail and return a non-zero value. ] */
TEST_FUNCTION(when_normalizing_the_type_fails_umocktypes_register_type_fails)
{
    // arrange
    (void)umocktypes_init();

    umocktypename_normalize_call_result[0] = NULL;

    // act
    int result = umocktypes_register_type("char  *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char  *", umocktypename_normalize_calls[0].type_name);
}

/* Tests_SRS_UMOCKTYPES_01_050: [ If umocktypes_register_type is called when the module is not initialized, umocktypes_register_type shall fail and return a non zero value. ]*/
TEST_FUNCTION(umocktypes_register_type_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result[0] = "char*";

    // act
    int result = umocktypes_register_type("char  *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_01_012: [ If an error occurs allocating memory for the newly registered type, umocktypes_register_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_allocating_memory_fails_umocktypes_register_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";

    when_shall_realloc_fail = 1;

    // act
    int result = umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* umocktypes_register_alias_type */

/* Tests_SRS_UMOCKTYPES_01_053: [ umocktypes_register_alias_type shall register a new alias type for the type "is_type". ]*/
/* Tests_SRS_UMOCKTYPES_01_054: [ On success, umocktypes_register_alias_type shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_01_058: [ Before looking it up, is_type shall be normalized by using umocktypename_normalize. ] */
/* Tests_SRS_UMOCKTYPES_01_059: [ Before adding it as alias, type shall be normalized by using umocktypename_normalize. ]*/
TEST_FUNCTION(umocktypes_register_alias_type_registers_an_alias_type)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    umocktypes_register_type("char*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    umocktypename_normalize_call_result[1] = "char*";
    umocktypename_normalize_call_result[2] = "PSTR";

    // act
    int result = umocktypes_register_alias_type("PSTR", "char*");

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 3, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_055: [ If any of the arguments is NULL, umocktypes_register_alias_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_type_is_NULL_umocktypes_register_alias_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    umocktypes_register_type("char*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // act
    int result = umocktypes_register_alias_type(NULL, "char*");

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_01_055: [ If any of the arguments is NULL, umocktypes_register_alias_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_is_type_is_NULL_umocktypes_register_alias_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    umocktypes_register_type("char*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    // act
    int result = umocktypes_register_alias_type("PSTR", NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_UMOCKTYPES_01_057: [ If is_type was not already registered, umocktypes_register_alias_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_is_type_was_not_registered_umocktypes_register_alias_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";

    // act
    int result = umocktypes_register_alias_type("PSTR", "char*");

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 1, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_060: [ If umocktypename_normalize fails, umocktypes_register_alias_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_umocktypename_normalize_fails_for_is_type_then_umocktypes_register_alias_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    umocktypes_register_type("char*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    umocktypename_normalize_call_result[1] = NULL;

    // act
    int result = umocktypes_register_alias_type("PSTR", "char*");

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 2, umocktypename_normalize_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_060: [ If umocktypename_normalize fails, umocktypes_register_alias_type shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_umocktypename_normalize_fails_for_type_then_umocktypes_register_alias_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    umocktypes_register_type("char*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);

    umocktypename_normalize_call_result[1] = "char*";
    umocktypename_normalize_call_result[2] = NULL;

    // act
    int result = umocktypes_register_alias_type("PSTR", "char*");

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(size_t, 3, umocktypename_normalize_call_count);
}

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

/* Tests_SRS_UMOCKTYPES_01_013: [ umocktypes_stringify shall return a char\* with the string representation of the value argument. ]*/
/* Tests_SRS_UMOCKTYPES_01_014: [ The string representation shall be obtained by calling the stringify function registered for the type identified by the argument type. ]*/
/* Tests_SRS_UMOCKTYPES_01_015: [ On success umocktypes_stringify shall return the char\* produced by the underlying stringify function for type (passed in umocktypes_register_type). ]*/
/* Tests_SRS_UMOCKTYPES_01_035: [ Before looking it up, the type string shall be normalized by calling umocktypename_normalize. ]*/
TEST_FUNCTION(umocktypes_stringify_calls_the_underlying_stringify)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    test_stringify_func_testtype_call_result = "blahblah";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    char* result = umocktypes_stringify("char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "blahblah", result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_stringify_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_stringify_func_testtype_calls[0].value);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_01_016: [ If any of the arguments is NULL, umocktypes_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_with_NULL_value_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    char* result = umocktypes_stringify("char *", NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_01_016: [ If any of the arguments is NULL, umocktypes_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_with_NULL_type_fails)
{
    // arrange
    (void)umocktypes_init();

    // act
    char* result = umocktypes_stringify(NULL, test_value_1);

    // assert
    ASSERT_IS_NULL(result);

}

/* Tests_SRS_UMOCKTYPES_01_017: [ If type can not be found in the registered types list maintained by the module, umocktypes_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_with_a_type_that_is_not_registered_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";

    // act
    char* result = umocktypes_stringify("const char  *", test_value_1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_01_044: [ If normalizing the typename fails, umocktypes_stringify shall fail and return NULL. ]*/
TEST_FUNCTION(when_normalizing_the_type_fails_umocktypes_stringify_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = NULL;

    // act
    char* result = umocktypes_stringify("char  *", test_value_1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_01_049: [ If umocktypes_stringify is called when the module is not initialized, umocktypes_stringify shall return NULL. ]*/
TEST_FUNCTION(umocktypes_stringify_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result[0] = "char*";
    test_stringify_func_testtype_call_result = "blahblah";

    // act
    char* result = umocktypes_stringify("char  *", test_value_1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPES_01_063: [ If type is a pointer type and type was not registered then umocktypes_stringify shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_stringify_with_an_unregistered_pointer_type_defaults_to_void_ptr)
{
    // arrange
    umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "char*";
    test_stringify_func_testtype_call_result = "blahblah";

    // act
    char* result = umocktypes_stringify("char  *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "blahblah", result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char  *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_stringify_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_stringify_func_testtype_calls[0].value);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPES_01_063: [ If type is a pointer type and type was not registered then umocktypes_stringify shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_stringify_with_an_unregistered_non_pointer_type_fails)
{
    // arrange
    umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "char";
    test_stringify_func_testtype_call_result = "blahblah";

    // act
    char* result = umocktypes_stringify("char", test_value_1);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_stringify_func_testtype_call_count);
}

/* umocktypes_are_equal */

/* Tests_SRS_UMOCKTYPES_01_018: [ umocktypes_are_equal shall evaluate whether 2 values are equal. ]*/
/* Tests_SRS_UMOCKTYPES_01_019: [ umocktypes_are_equal shall call the underlying are_equal function for the type identified by the argument type (passed in umocktypes_register_type). ] */
/* Tests_SRS_UMOCKTYPES_01_021: [ If the underlying are_equal function indicates the types are equal, umocktypes_are_equal shall return 1. ]*/
/* Tests_SRS_UMOCKTYPES_01_036: [ Before looking it up, the type string shall be normalized by calling umocktypename_normalize. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_2_equal_values_returns_1)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 1;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_are_equal_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_are_equal_func_testtype_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_value_2, test_are_equal_func_testtype_calls[0].right);
}

/* Tests_SRS_UMOCKTYPES_01_018: [ umocktypes_are_equal shall evaluate whether 2 values are equal. ]*/
/* Tests_SRS_UMOCKTYPES_01_019: [ umocktypes_are_equal shall call the underlying are_equal function for the type identified by the argument type (passed in umocktypes_register_type). ] */
/* Tests_SRS_UMOCKTYPES_01_022: [ If the underlying are_equal function indicates the types are not equal, umocktypes_are_equal shall return 0. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_2_different_values_returns_0)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_are_equal_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_are_equal_func_testtype_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_value_2, test_are_equal_func_testtype_calls[0].right);
}

/* Tests_SRS_UMOCKTYPES_01_023: [ If any of the arguments is NULL, umocktypes_are_equal shall fail and return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_NULL_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal(NULL, test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_023: [ If any of the arguments is NULL, umocktypes_are_equal shall fail and return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_NULL_left_argument_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("char *", NULL, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_023: [ If any of the arguments is NULL, umocktypes_are_equal shall fail and return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_NULL_right_argument_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_024: [ If type can not be found in the registered types list maintained by the module, umocktypes_are_equal shall fail and return -1. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_a_not_registered_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";
    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("const char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_043: [ If normalizing the typename fails, umocktypes_are_equal shall fail and return -1. ]*/
TEST_FUNCTION(when_normalizing_the_typename_fails_umocktypes_are_equal_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = NULL;
    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("const char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_046: [ If umocktypes_are_equal is called when the module is not initialized, umocktypes_are_equal shall return -1. ] */
TEST_FUNCTION(umocktypes_are_equal_when_the_module_is_not_initialized_fails)
{
    // arrange
    umocktypename_normalize_call_result[0] = "char*";
    test_are_equal_func_testtype_call_result = 0;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_020: [ If the underlying are_equal function fails,, umocktypes_are_equal shall fail and return -1. ] */
TEST_FUNCTION(when_the_underlying_are_equal_call_fails_with_minus_1_umocktypes_are_equal_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = -1;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_are_equal_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_are_equal_func_testtype_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_value_2, test_are_equal_func_testtype_calls[0].right);
}

/* Tests_SRS_UMOCKTYPES_01_020: [ If the underlying are_equal function fails,, umocktypes_are_equal shall fail and return -1. ] */
TEST_FUNCTION(when_the_underlying_are_equal_call_fails_with_2_umocktypes_are_equal_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 2;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_2);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_are_equal_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_are_equal_func_testtype_calls[0].left);
    ASSERT_ARE_EQUAL(void_ptr, test_value_2, test_are_equal_func_testtype_calls[0].right);
}

/* Tests_SRS_UMOCKTYPES_01_051: [ If the pointer values for left and right are equal, umocktypes_are_equal shall return 1 without calling the underlying are_equal function. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_2_equal_pointers_returns_1)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_are_equal_func_testtype_call_result = 1;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_064: [ If type is a pointer type and type was not registered then umocktypes_are_equal shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_a_pointer_type_that_was_not_registered_defaults_to_void_ptr)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "char*";
    test_are_equal_func_testtype_call_result = 1;

    // act
    int result = umocktypes_are_equal("char *", test_value_1, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_064: [ If type is a pointer type and type was not registered then umocktypes_are_equal shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_are_equal_with_a_non_pointer_type_that_was_not_registered_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "char";
    test_are_equal_func_testtype_call_result = 1;

    // act
    int result = umocktypes_are_equal("char", test_value_1, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, -1, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_are_equal_func_testtype_call_count);
}

/* umocktypes_copy */

/* Tests_SRS_UMOCKTYPES_01_025: [ umocktypes_copy shall copy the value of the source into the destination argument. ]*/
/* Tests_SRS_UMOCKTYPES_01_026: [ The copy shall be done by calling the underlying copy function (passed in umocktypes_register_type) for the type identified by the type argument. ]*/
/* Tests_SRS_UMOCKTYPES_01_052: [ On success, umocktypes_copy shall return 0. ]*/
/* Tests_SRS_UMOCKTYPES_01_037: [ Before looking it up, the type string shall be normalized by calling umocktypename_normalize. ]*/
TEST_FUNCTION(umocktypes_copy_calls_the_underlying_copy_function)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("char *", destination, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_copy_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, destination, test_copy_func_testtype_calls[0].destination);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_copy_func_testtype_calls[0].source);
}

/* Tests_SRS_UMOCKTYPES_01_027: [ If any of the arguments is NULL, umocktypes_copy shall return -1. ]*/
TEST_FUNCTION(umocktypes_copy_with_NULL_type_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy(NULL, destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_027: [ If any of the arguments is NULL, umocktypes_copy shall return -1. ]*/
TEST_FUNCTION(umocktypes_copy_with_NULL_destination_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("char *", NULL, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_027: [ If any of the arguments is NULL, umocktypes_copy shall return -1. ]*/
TEST_FUNCTION(umocktypes_copy_with_NULL_source_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("char *", destination, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_028: [ If the underlying copy fails, umocktypes_copy shall return -1. ]*/
TEST_FUNCTION(when_the_underlying_copy_fails_then_umocktypes_copy_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    test_copy_func_testtype_call_result = -1;

    // act
    int result = umocktypes_copy("char *", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_copy_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, destination, test_copy_func_testtype_calls[0].destination);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_copy_func_testtype_calls[0].source);
}

/* Tests_SRS_UMOCKTYPES_01_029: [ If type can not be found in the registered types list maintained by the module, umocktypes_copy shall fail and return -1. ]*/
TEST_FUNCTION(when_the_type_is_not_found_in_the_registered_types_list_umocktypes_copy_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char *", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_042: [ If normalizing the typename fails, umocktypes_copy shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_normalizing_the_type_fails_then_umocktypes_copy_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = NULL;
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char *", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_047: [ If umocktypes_copy is called when the module is not initialized, umocktypes_copy shall fail and return a non zero value. ]*/
TEST_FUNCTION(when_the_module_is_not_initialized_then_umocktypes_copy_fails)
{
    // arrange
    void* destination = (void*)0x4245;

    umocktypename_normalize_call_result[0] = NULL;
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char *", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_065: [ If type is a pointer type and type was not registered then umocktypes_copy shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_copy_with_a_pointer_type_that_was_not_registered_defaults_to_void_ptr)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char *", destination, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_copy_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, destination, test_copy_func_testtype_calls[0].destination);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_copy_func_testtype_calls[0].source);
}

/* Tests_SRS_UMOCKTYPES_01_065: [ If type is a pointer type and type was not registered then umocktypes_copy shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_copy_with_a_non_pointer_type_that_was_not_registered_fails)
{
    // arrange
    void* destination = (void*)0x4245;
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char";
    test_copy_func_testtype_call_result = 0;

    // act
    int result = umocktypes_copy("const char", destination, test_value_1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_copy_func_testtype_call_count);
}

/* umocktypes_free */

/* Tests_SRS_UMOCKTYPES_01_030: [ umocktypes_free shall free a value previously allocated with umocktypes_copy. ]*/
/* Tests_SRS_UMOCKTYPES_01_033: [ The free shall be done by calling the underlying free function (passed in umocktypes_register_type) for the type identified by the type argument. ]*/
/* Tests_SRS_UMOCKTYPES_01_038: [ Before looking it up, the type string shall be normalized by calling umocktypename_normalize. ]*/
TEST_FUNCTION(umocktypes_free_calls_the_underlying_free_function)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    umocktypes_free("char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_free_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_free_func_testtype_calls[0].value);
}

/* Tests_SRS_UMOCKTYPES_01_031: [ If any of the arguments is NULL, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_with_NULL_type_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    umocktypes_free(NULL, test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_031: [ If any of the arguments is NULL, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_with_NULL_value_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    // act
    umocktypes_free("char *", NULL);

    // assert
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_032: [ If type can not be found in the registered types list maintained by the module, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(when_the_type_is_not_found_in_the_registered_type_list_umocktypes_free_fails)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";

    // act
    umocktypes_free("const char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_041: [ If normalizing the typename fails, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(when_normalizing_the_type_fails_then_umocktypes_free_does_not_free_anything)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "char*";
    (void)umocktypes_register_type("char *", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = NULL;

    // act
    umocktypes_free("const char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_048: [ If umocktypes_free is called when the module is not initialized, umocktypes_free shall do nothing. ]*/
TEST_FUNCTION(umocktypes_free_when_the_module_is_not_initialized_does_not_free_anything)
{
    // arrange
    umocktypename_normalize_call_result[0] = "char *";

    // act
    umocktypes_free("char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

/* Tests_SRS_UMOCKTYPES_01_066: [ If type is a pointer type and type was not registered then umocktypes_free shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_free_with_a_pointer_type_that_is_not_registered_defaults_to_void_ptr)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char*";

    // act
    umocktypes_free("const char *", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char *", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 1, test_free_func_testtype_call_count);
    ASSERT_ARE_EQUAL(void_ptr, test_value_1, test_free_func_testtype_calls[0].value);
}

/* Tests_SRS_UMOCKTYPES_01_066: [ If type is a pointer type and type was not registered then umocktypes_free shall execute as if type is void*. ]*/
TEST_FUNCTION(umocktypes_free_with_a_non_pointer_type_that_is_not_registered_does_not_free_anything)
{
    // arrange
    (void)umocktypes_init();
    umocktypename_normalize_call_result[0] = "void*";
    (void)umocktypes_register_type("void*", test_stringify_func_testtype, test_are_equal_func_testtype, test_copy_func_testtype, test_free_func_testtype);
    reset_umocktypename_normalize_calls();

    umocktypename_normalize_call_result[0] = "const char";

    // act
    umocktypes_free("const char", test_value_1);

    // assert
    ASSERT_ARE_EQUAL(int, 1, umocktypename_normalize_call_count);
    ASSERT_ARE_EQUAL(char_ptr, "const char", umocktypename_normalize_calls[0].type_name);
    ASSERT_ARE_EQUAL(int, 0, test_free_func_testtype_call_count);
}

END_TEST_SUITE(umocktypes_unittests)
