// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"

static size_t my_malloc_count;

void* my_malloc(size_t size)
{
    my_malloc_count++;
    return malloc(size);
}

static size_t my_realloc_count;

void* my_realloc(void* ptr, size_t size)
{
    my_realloc_count++;
    return realloc(ptr, size);
}

static size_t my_free_count;

void my_free(void* ptr)
{
    my_free_count++;
    free(ptr);
}

#ifdef _CRTDBG_MAP_ALLOC
#define _malloc_dbg(size, ...) my_malloc(size)
#define _realloc_dbg(ptr, size, ...) my_realloc(ptr, size)
#define _free_dbg(ptr, ...) my_free(ptr)
#else
#define malloc my_malloc
#define realloc my_realloc
#define free my_free
#endif

#define ENABLE_MOCKS

#include "umock_c.h"
#include "umocktypes_charptr.h"

static void test_on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

MOCK_FUNCTION_WITH_CODE(, int, function1, int, a)
MOCK_FUNCTION_END(42)

BEGIN_TEST_SUITE(umock_c_malloc_hook_integrationtests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(global_mutex);

    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);

    result = umock_c_init(test_on_umock_c_error);
    ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(test_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(global_mutex);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    int mutex_acquire_result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, mutex_acquire_result);

    umock_c_reset_all_calls();
    my_malloc_count = 0;
    my_realloc_count = 0;
    my_free_count = 0;
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    TEST_MUTEX_RELEASE(test_mutex);
}

TEST_FUNCTION(when_malloc_is_hooked_no_calls_are_made_to_it)
{
    // arrange
    STRICT_EXPECTED_CALL(function1(42));

    // act
    function1(42);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, my_malloc_count);
    ASSERT_ARE_EQUAL(size_t, 0, my_realloc_count);
    ASSERT_ARE_EQUAL(size_t, 0, my_free_count);
}

END_TEST_SUITE(umock_c_malloc_hook_integrationtests)
