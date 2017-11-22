// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "testrunnerswitcher.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

void* my_gballoc_malloc(size_t size)
{
    void* result;
    currentmalloc_call++;
    if (whenShallmalloc_fail > 0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = malloc(size);
        }
    }
    else
    {
        result = malloc(size);
    }
    return result;
}

void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#define ENABLE_MOCKS

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/strings.h"

static const char TEST_STRING_VALUE []= "DataValueTest";
static const char INITAL_STRING_VALUE []= "Initial_";
static const char MULTIPLE_TEST_STRING_VALUE[] = "DataValueTestDataValueTest";
static const char* COMBINED_STRING_VALUE = "Initial_DataValueTest";
static const char* QUOTED_TEST_STRING_VALUE = "\"DataValueTest\"";
static const char* EMPTY_STRING = "";

#define NUMBER_OF_CHAR_TOCOPY               8

static TEST_MUTEX_HANDLE g_dllByDll;
static TEST_MUTEX_HANDLE g_testByTest;

static const struct JSONEncoding {
    const char* source;
    const char* expectedJSON;
} JSONtests[]= 
    {
        { "", "\"\"" }, /*empty string*/
        { "a", "\"a\"" }, /*a => "a"*/
        { "aaslkdjhfalksjh", "\"aaslkdjhfalksjh\"" }, /*aaslkdjhfalksjh => "aaslkdjhfalksjh"*/
        { "\x01", "\"\\u0001\"" },
        { "\x1F", "\"\\u001F\"" },
        { "\x1F", "\"\\u001F\"" },
        { "\"", "\"\\\"\""},
        { "\\", "\"\\\\\"" },
        { "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F some text\"\\a/a", 
          "\"\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\u0007\\u0008\\u0009\\u000A\\u000B\\u000C\\u000D\\u000E\\u000F\\u0010\\u0011\\u0012\\u0013\\u0014\\u0015\\u0016\\u0017\\u0018\\u0019\\u001A\\u001B\\u001C\\u001D\\u001E\\u001F some text\\\"\\\\a\\/a\"" },

    };

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(strings_unittests)

    TEST_SUITE_INITIALIZE(setsBufferTempSize)
    {
        int result;

        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        umock_c_init(on_umock_c_error);

        REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
        result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(a)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }

        umock_c_reset_all_calls();

        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(cleans)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* STRING_Tests BEGIN */
    /* Tests_SRS_STRING_07_001: [STRING_new shall allocate a new STRING_HANDLE pointing to an empty string.] */
    TEST_FUNCTION(STRING_new_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(1));

        ///act
        g_hString = STRING_new();

        ///assert
        ASSERT_IS_NOT_NULL(g_hString);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_007: [STRING_new_with_memory shall return a NULL STRING_HANDLE if the supplied char* is empty.] */
    TEST_FUNCTION(STRING_new_With_Memory_NULL_Memory_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;

        ///act
        g_hString = STRING_new_with_memory(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(g_hString);
    }

    /* Tests_SRS_STRING_07_006: [STRING_new_with_memory shall return a STRING_HANDLE by using the supplied char* memory.] */
    TEST_FUNCTION(STRING_new_With_Memory_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        size_t nLen = strlen(TEST_STRING_VALUE) + 1;
        char* szTestString = (char*)malloc(nLen);
        strncpy(szTestString, TEST_STRING_VALUE, nLen);
        umock_c_reset_all_calls();
        
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        g_hString = STRING_new_with_memory(szTestString);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_003: [STRING_construct shall allocate a new string with the value of the specified const char*.] */
    TEST_FUNCTION(STRING_construct_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_STRING_VALUE) + 1));

        ///act
        g_hString = STRING_construct(TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_005: [If the supplied const char* is NULL STRING_construct shall return a NULL value.] */
    TEST_FUNCTION(STRING_construct_With_NULL_HANDLE_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;

        ///act
        g_hString = STRING_construct(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(g_hString);
    }

    /* Tests_SRS_STRING_07_008: [STRING_new_quoted shall return a valid STRING_HANDLE Copying the supplied const char* value surrounded by quotes.] */
    TEST_FUNCTION(STRING_new_quoted_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(2 + strlen(TEST_STRING_VALUE) + 1));

        ///act
        g_hString = STRING_new_quoted(TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, QUOTED_TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_009: [STRING_new_quoted shall return a NULL STRING_HANDLE if the supplied const char* is NULL.] */
    TEST_FUNCTION(STRING_new_quoted_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;

        ///act
        g_hString = STRING_new_quoted(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(g_hString);
    }

    /* Tests_ */
    TEST_FUNCTION(STRING_Concat_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, strlen(INITAL_STRING_VALUE) + strlen(TEST_STRING_VALUE) + 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_concat(g_hString, TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, COMBINED_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if the STRING_HANDLE and const char* is NULL.] */
    TEST_FUNCTION(STRING_Concat_HANDLE_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_concat(NULL, TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);

    }

    /* Tests_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if the STRING_HANDLE and const char* is NULL.] */
    TEST_FUNCTION(STRING_Concat_CharPtr_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_concat(g_hString, NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if the STRING_HANDLE and const char* is NULL.] */
    TEST_FUNCTION(STRING_Concat_HANDLE_and_CharPtr_NULL_Fail)
    {
        ///arrange
        ///act
        
        int nResult = STRING_concat(NULL, TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);

        ///cleanup
    }

    /* Tests_SRS_STRING_07_013: [STRING_concat shall return a nonzero number if the STRING_HANDLE and const char* is NULL.] */
    TEST_FUNCTION(STRING_Concat_Copy_Multiple_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_new();
        STRING_copy(g_hString, TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_STRING_VALUE) + strlen(TEST_STRING_VALUE)+1))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        STRING_concat(g_hString, TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, MULTIPLE_TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_034: [String_Concat_with_STRING shall concatenate a given STRING_HANDLE variable with a source STRING_HANDLE.] */
    TEST_FUNCTION(STRING_Concat_With_STRING_SUCCEED)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        STRING_HANDLE hAppend = STRING_construct(TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, strlen(INITAL_STRING_VALUE) + strlen(TEST_STRING_VALUE) + 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_concat_with_STRING(g_hString, hAppend);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, COMBINED_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // Clean up
        STRING_delete(hAppend);
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_035: [String_Concat_with_STRING shall return a nonzero number if an error is encountered.] */
    TEST_FUNCTION(STRING_Concat_With_STRING_HANDLE_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE hAppend = STRING_construct(TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_concat_with_STRING(NULL, hAppend);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // Clean up
        STRING_delete(hAppend);
    }

    /* Tests_SRS_STRING_07_035: [String_Concat_with_STRING shall return a nonzero number if an error is encountered.] */
    TEST_FUNCTION(STRING_Concat_With_STRING_Append_HANDLE_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_concat_with_STRING(g_hString, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_035: [String_Concat_with_STRING shall return a nonzero number if an error is encountered.] */
    TEST_FUNCTION(STRING_Concat_With_STRING_All_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        int nResult = STRING_concat_with_STRING(NULL, NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_STRING_07_016: [STRING_copy shall copy the const char* into the supplied STRING_HANDLE.] */
    TEST_FUNCTION(STRING_Copy_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_STRING_VALUE) + 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_copy(g_hString, TEST_STRING_VALUE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_017: [STRING_copy shall return a nonzero value if any of the supplied parameters are NULL.] */
    TEST_FUNCTION(STRING_Copy_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_copy(g_hString, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_018: [STRING_copy_n shall copy the number of characters defined in size_t.] */
    TEST_FUNCTION(STRING_Copy_n_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();
        
        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, NUMBER_OF_CHAR_TOCOPY + 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_copy_n(g_hString, COMBINED_STRING_VALUE, NUMBER_OF_CHAR_TOCOPY);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, INITAL_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_019: [STRING_copy_n shall return a nonzero value if STRING_HANDLE or const char* is NULL.] */
    TEST_FUNCTION(STRING_Copy_n_With_HANDLE_NULL_Fail)
    {
        ///arrange

        ///act
        int nResult = STRING_copy_n(NULL, COMBINED_STRING_VALUE, NUMBER_OF_CHAR_TOCOPY);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_STRING_07_019: [STRING_copy_n shall return a nonzero value if STRING_HANDLE or const char* is NULL.] */
    TEST_FUNCTION(STRING_Copy_n_With_CONST_CHAR_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        int nResult = STRING_copy_n(g_hString, NULL, NUMBER_OF_CHAR_TOCOPY);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_018: [STRING_copy_n shall copy the number of characters defined in size_t.] */
    TEST_FUNCTION(STRING_Copy_n_With_Size_0_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(INITAL_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_copy_n(g_hString, COMBINED_STRING_VALUE, 0);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, EMPTY_STRING, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_014: [STRING_quote shall "quote" the supplied STRING_HANDLE and return 0 on success.] */
    TEST_FUNCTION(STRING_quote_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, 2 + strlen(TEST_STRING_VALUE) + 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_quote(g_hString);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, QUOTED_TEST_STRING_VALUE, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_015: [STRING_quote shall return a nonzero value if any of the supplied parameters are NULL.] */
    TEST_FUNCTION(STRING_quote_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        int nResult = STRING_quote(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_STRING_07_021: [STRING_c_str shall return NULL if the STRING_HANDLE is NULL.] */
    TEST_FUNCTION(STRING_c_str_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        const char* s = STRING_c_str(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(s);
    }

    /* Tests_SRS_STRING_07_020: [STRING_c_str shall return the const char* associated with the given STRING_HANDLE.] */
    TEST_FUNCTION(STRING_c_str_Success)
    {
        ///arrange
        STRING_HANDLE g_hString;

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_STRING_VALUE) + 1));

        ///act
        g_hString = STRING_construct(TEST_STRING_VALUE);
        const char* s = STRING_c_str(g_hString);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, s, TEST_STRING_VALUE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_022: [STRING_empty shall revert the STRING_HANDLE to an empty state.] */
    TEST_FUNCTION(STRING_empty_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, 1))
            .IgnoreArgument(1);

        ///act
        int nResult = STRING_empty(g_hString);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(char_ptr, EMPTY_STRING, STRING_c_str(g_hString) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    /* Tests_SRS_STRING_07_023: [STRING_empty shall return a nonzero value if the STRING_HANDLE is NULL.] */
    TEST_FUNCTION(STRING_empty_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        int nResult = STRING_empty(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_STRING_07_011: [STRING_delete will not attempt to free anything with a NULL STRING_HANDLE.] */
    TEST_FUNCTION(STRING_delete_NULL_Succeed)
    {
        ///arrange

        ///act
        STRING_delete(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_STRING_07_011: [STRING_delete will not attempt to free anything with a NULL STRING_HANDLE.] */
    TEST_FUNCTION(STRING_delete_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_new();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        STRING_delete(g_hString);
        
        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    TEST_FUNCTION(STRING_length_Succeed)
    {
        ///arrange
        STRING_HANDLE g_hString;
        g_hString = STRING_construct(TEST_STRING_VALUE);
        umock_c_reset_all_calls();

        ///act
        size_t nResult = STRING_length(g_hString);

        ///assert
        ASSERT_ARE_EQUAL(size_t, nResult, strlen(TEST_STRING_VALUE) );
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(g_hString);
    }

    TEST_FUNCTION(STRING_length_NULL_HANDLE_Fail)
    {
        ///arrange

        ///act
        size_t nResult = STRING_length(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(size_t, nResult, 0);
    }

    /*Tests_SRS_STRING_02_002: [If parameter handle is NULL then STRING_clone shall return NULL.]*/
    TEST_FUNCTION(STRING_clone_NULL_HANDLE_return_NULL)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_clone(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL( result);
    }

    /*Tests_SRS_STRING_02_001: [STRING_clone shall produce a new string having the same content as the handle string.]*/
    TEST_FUNCTION(STRING_clone_succeeds)
    {
        ///arrange
        STRING_HANDLE hSource = STRING_construct("aa");
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof("aa")));

        ///act
        STRING_HANDLE result = STRING_clone(hSource);

        ///assert
        ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
        ASSERT_ARE_NOT_EQUAL(void_ptr, STRING_c_str(hSource), STRING_c_str(result));
        ASSERT_ARE_EQUAL    (char_ptr, STRING_c_str(hSource), STRING_c_str(result));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(hSource);
        STRING_delete(result);
    }

    /*Codes_SRS_STRING_02_008: [If psz is NULL then STRING_construct_n shall return NULL*/
    TEST_FUNCTION(STRING_construct_n_with_NULL_argument_fails)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_construct_n(NULL, 3);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(result);
    }

    /*Codes_SRS_STRING_02_009: [If n is bigger than the size of the string psz, then STRING_construct_n shall return NULL.] */
    TEST_FUNCTION(STRING_construct_n_with_too_big_size_fails)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_construct_n("a", 2);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(result);
    }

    /*Codes_SRS_STRING_02_007: [STRING_construct_n shall construct a STRING_HANDLE from first "n" characters of the string pointed to by psz parameter.] */
    TEST_FUNCTION(STRING_construct_n_succeeds_with_2_char)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(3))
            .IgnoreArgument(1);

        ///act
        STRING_HANDLE result = STRING_construct_n("qq", 2);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, "qq", STRING_c_str(result));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(result);
    }

    /*Codes_SRS_STRING_02_007: [STRING_construct_n shall construct a STRING_HANDLE from first "n" characters of the string pointed to by psz parameter.] */
    TEST_FUNCTION(STRING_construct_n_succeeds_with_3_char_out_of_five)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_malloc(4))
            .IgnoreArgument(1);

        ///act
        STRING_HANDLE result = STRING_construct_n("12345", 3);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, "123", STRING_c_str(result));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(result);
    }

    /* Tests_SRS_STRING_07_036: [If h1 is NULL and h2 is nonNULL then STRING_compare shall return 1.] */
    TEST_FUNCTION(STRING_compare_s1_NULL)
    {
        ///arrange
        STRING_HANDLE h2 = STRING_construct("bb");
        umock_c_reset_all_calls();

        ///act
        int result = STRING_compare(NULL, h2);

        ///assert
        ASSERT_ARE_EQUAL(int, 1, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(h2);
    }

    /* Tests_SRS_STRING_07_037: [If h2 is NULL and h1 is nonNULL then STRING_compare shall return -1.] */
    TEST_FUNCTION(STRING_compare_s2_NULL)
    {
        ///arrange
        STRING_HANDLE h1 = STRING_construct("aa");
        umock_c_reset_all_calls();

        ///act
        int result = STRING_compare(h1, NULL);

        ///assert
        ASSERT_ARE_EQUAL(int, -1, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(h1);
    }

    /* Codes_SRS_STRING_07_035: [If h1 and h2 are both NULL then STRING_compare shall return 0.] */
    TEST_FUNCTION(STRING_compare_s1_s2_NULL)
    {
        ///arrange

        ///act
        int result = STRING_compare(NULL, NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(int, 0, result);

        ///cleanup
    }

    /* Tests_SRS_STRING_07_034: [STRING_compare returns an integer greater than, equal to, or less than zero, accordingly as the string pointed to by s1 is greater than, equal to, or less than the string s2.] */
    TEST_FUNCTION(STRING_compare_s1_first_SUCCEED)
    {
        ///arrange
        STRING_HANDLE h1 = STRING_construct("aa");
        STRING_HANDLE h2 = STRING_construct("bb");
        umock_c_reset_all_calls();

        ///act
        int result = STRING_compare(h1, h2);

        ///assert
        ASSERT_ARE_EQUAL(int, -1, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(h1);
        STRING_delete(h2);
    }

    /* Tests_SRS_STRING_07_034: [STRING_compare returns an integer greater than, equal to, or less than zero, accordingly as the string pointed to by s1 is greater than, equal to, or less than the string s2.] */
    TEST_FUNCTION(STRING_compare_s2_first_SUCCEED)
    {
        ///arrange
        STRING_HANDLE h1 = STRING_construct("aa");
        STRING_HANDLE h2 = STRING_construct("bb");
        umock_c_reset_all_calls();

        ///act
        int result = STRING_compare(h2, h1);

        ///assert
        ASSERT_ARE_EQUAL(int, 1, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(h1);
        STRING_delete(h2);
    }

    /* Tests_SRS_STRING_07_034: [STRING_compare returns an integer greater than, equal to, or less than zero, accordingly as the string pointed to by s1 is greater than, equal to, or less than the string s2.] */
    /* Tests_SRS_STRING_07_038: [STRING_compare shall compare the char s variable using the strcmp function.] */
    TEST_FUNCTION(STRING_compare_Equal_SUCCEED)
    {
        ///arrange
        STRING_HANDLE h1 = STRING_construct("a1234");
        STRING_HANDLE h2 = STRING_construct("a1234");
        umock_c_reset_all_calls();

        ///act
        int result = STRING_compare(h1, h2);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        STRING_delete(h1);
        STRING_delete(h2);
    }

    /*Tests_SRS_STRING_02_011: [If source is NULL then STRING_new_JSON shall return NULL.] */
    TEST_FUNCTION(STRING_new_JSON_with_NULL_input_returns_NULL)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_new_JSON(NULL);
        
        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(result);

        ///cleanup
    }

    /*Tests_SRS_STRING_02_012: [The string shall begin with the quote character.] */
    /*Tests_SRS_STRING_02_013: [The string shall copy the characters of source "as they are" (until the '\0' character) with the following exceptions:] */
    /*Tests_SRS_STRING_02_014: [If any character has the value outside [1...127] then STRING_new_JSON shall fail and return NULL.] */
    /*Tests_SRS_STRING_02_016: [If the character is " (quote) then it shall be repsented as \".] */
    /*Tests_SRS_STRING_02_017: [If the character is \ (backslash) then it shall represented as \\.]*/
    /*Tests_SRS_STRING_02_018: [If the character is / (slash) then it shall be represented as \/.] */
    /*Tests_SRS_STRING_02_019: [If the character code is less than 0x20 then it shall be represented as \\u00xx, where xx is the hex representation of the character code.]*/
    /*Tests_SRS_STRING_02_020: [The string shall end with " (quote).] */
    TEST_FUNCTION(STRING_new_JSON_succeeds)
    {
        for (size_t i = 0; i < sizeof(JSONtests) / sizeof(JSONtests[0]); i++)
        {
            ///arrange
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(gballoc_malloc(strlen(JSONtests[i].expectedJSON) + 1));

            ///act
            STRING_HANDLE result = STRING_new_JSON(JSONtests[i].source);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, JSONtests[i].expectedJSON, STRING_c_str(result));
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            STRING_delete(result);
        }
    }

    /*Codes_SRS_STRING_02_021: [If the complete JSON representation cannot be produced, then STRING_new_JSON shall fail and return NULL.] */
    TEST_FUNCTION(STRING_new_JSON_when_gballoc_fails_it_fails_1)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = 2;
        STRICT_EXPECTED_CALL(gballoc_malloc(strlen("ab") + 2+1));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        STRING_HANDLE result = STRING_new_JSON("ab");

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Codes_SRS_STRING_02_021: [If the complete JSON representation cannot be produced, then STRING_new_JSON shall fail and return NULL.] */
    TEST_FUNCTION(STRING_new_JSON_when_gballoc_fails_it_fails_2)
    {
        ///arrange
        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        STRING_HANDLE result = STRING_new_JSON("wwwa");

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_STRING_02_014: [If any character has the value outside [1...127] then STRING_new_JSON shall fail and return NULL.] */
    TEST_FUNCTION(STRING_new_JSON_when_character_not_ASCII_fails)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_new_JSON("a\xFF");

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_STRING_02_022: [ If source is NULL and size > 0 then STRING_from_BUFFER shall fail and return NULL. ]*/
    TEST_FUNCTION(STRING_from_byte_array_with_NULL_array_and_size_not_zero_fails)
    {
        ///arrange

        ///act
        STRING_HANDLE result = STRING_from_byte_array(NULL, 1);

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_STRING_02_023: [ Otherwise, STRING_from_BUFFER shall build a string that has the same content (byte-by-byte) as source and return a non-NULL handle. ]*/
    TEST_FUNCTION(STRING_from_byte_array_succeeds)
    {
        ///arrange

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        STRICT_EXPECTED_CALL(gballoc_malloc(1 + 1));

        ///act
        STRING_HANDLE result = STRING_from_byte_array((const unsigned char*)"a", 1);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(char_ptr, "a", STRING_c_str(result));

        ///cleanup
        STRING_delete(result);
    }

    /*Tests_SRS_STRING_02_023: [ Otherwise, STRING_from_BUFFER shall build a string that has the same content (byte-by-byte) as source and return a non-NULL handle. ]*/
    TEST_FUNCTION(STRING_from_byte_array_succeeds_2)
    {
        ///arrange

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        STRICT_EXPECTED_CALL(gballoc_malloc(1 + 0));

        ///act
        STRING_HANDLE result = STRING_from_byte_array(NULL, 0);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(char_ptr, "", STRING_c_str(result));

        ///cleanup
        STRING_delete(result);
    }

    /*Tests_SRS_STRING_02_024: [ If building the string fails, then STRING_from_BUFFER shall fail and return NULL. ]*/
    TEST_FUNCTION(STRING_from_byte_array_fails_1)
    {
        ///arrange

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        
        STRICT_EXPECTED_CALL(gballoc_malloc(1 + 1))
            .SetReturn(NULL);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        ///act
        STRING_HANDLE result = STRING_from_byte_array((const unsigned char*)"a", 1);

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_STRING_02_024: [ If building the string fails, then STRING_from_BUFFER shall fail and return NULL. ]*/
    TEST_FUNCTION(STRING_from_byte_array_fails_2)
    {
        ///arrange

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size()
            .SetReturn(NULL);

        ///act
        STRING_HANDLE result = STRING_from_byte_array((const unsigned char*)"a", 1);

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }
END_TEST_SUITE(strings_unittests)