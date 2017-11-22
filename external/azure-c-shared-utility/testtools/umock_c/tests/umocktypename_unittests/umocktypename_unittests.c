// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"
#include "umocktypename.h"

static size_t malloc_call_count;
static size_t realloc_call_count;

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
        free(ptr);
    }

#ifdef __cplusplus
}
#endif

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

BEGIN_TEST_SUITE(umocktypename_unittests)

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

    malloc_call_count = 0;
    when_shall_malloc_fail = 0;
    realloc_call_count = 0;
    when_shall_realloc_fail = 0;
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    TEST_MUTEX_RELEASE(test_mutex);
}

/* umocktypename_normalize */

/* Tests_SRS_UMOCKTYPENAME_01_001: [ umocktypename_normalize shall return a char\* with a newly allocated string that contains the normalized typename. ]*/
TEST_FUNCTION(umocktypename_normalize_returns_the_same_string_when_already_normalized)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_005: [ If typename is NULL, then umocktypename_normalize shall fail and return NULL. ]*/
TEST_FUNCTION(umocktypename_normalize_with_NULL_returns_NULL)
{
    // arrange

    // act
    char* result = umocktypename_normalize(NULL);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_004: [ umocktypename_normalize shall remove all extra spaces (more than 1 space) between elements that are part of the typename. ]*/
TEST_FUNCTION(umocktypename_normalize_removes_1_extra_space_between_2_words)
{
    // arrange

    // act
    char* result = umocktypename_normalize("const  char");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "const char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_004: [ umocktypename_normalize shall remove all extra spaces (more than 1 space) between elements that are part of the typename. ]*/
TEST_FUNCTION(umocktypename_normalize_removes_2_extra_spaces_between_2_words)
{
    // arrange

    // act
    char* result = umocktypename_normalize("const   char");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "const char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_006: [ No space shall exist between any other token and a star. ]*/
TEST_FUNCTION(umocktypename_normalize_removes_the_space_before_a_star)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char *");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char*", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_006: [ No space shall exist between any other token and a star. ]*/
TEST_FUNCTION(umocktypename_normalize_removes_the_space_after_a_star)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char* const");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char*const", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_002: [** umocktypename_normalize shall remove all spaces at the beginning of the typename. ] ]*/
TEST_FUNCTION(umocktypename_normalize_removes_1_space_at_the_beginning)
{
    // arrange

    // act
    char* result = umocktypename_normalize(" char");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_002: [** umocktypename_normalize shall remove all spaces at the beginning of the typename. ] ]*/
TEST_FUNCTION(umocktypename_normalize_removes_2_spaces_at_the_beginning)
{
    // arrange

    // act
    char* result = umocktypename_normalize("  char");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_007: [ If the length of the normalized typename is 0, umocktypename_normalize shall return NULL. ]*/
TEST_FUNCTION(umocktypename_normalize_for_a_zero_length_normalized_typename_returns_NULL)
{
    // arrange

    // act
    char* result = umocktypename_normalize(" ");

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_003: [ umocktypename_normalize shall remove all spaces at the end of the typename. ] */
TEST_FUNCTION(umocktypename_normalize_removes_1_space_at_the_end)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char ");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_003: [ umocktypename_normalize shall remove all spaces at the end of the typename. ] */
TEST_FUNCTION(umocktypename_normalize_removes_2_spaces_at_the_end)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char ");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_001: [ umocktypename_normalize shall return a char\* with a newly allocated string that contains the normalized typename. ]*/
TEST_FUNCTION(umocktypename_normalize_succeeds_with_a_type_that_ends_in_star)
{
    // arrange

    // act
    char* result = umocktypename_normalize("char*");

    // assert
    ASSERT_ARE_EQUAL(char_ptr, "char*", result);

    // cleanup
    free(result);
}

/* Tests_SRS_UMOCKTYPENAME_01_008: [ If allocating memory fails, umocktypename_normalize shall fail and return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_Fails_umocktypename_normalize_fails)
{
    // arrange
    when_shall_malloc_fail = 1;

    // act
    char* result = umocktypename_normalize("char*");

    // assert
    ASSERT_IS_NULL(result);
}

END_TEST_SUITE(umocktypename_unittests)
