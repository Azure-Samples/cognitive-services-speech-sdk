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
#include "azure_c_shared_utility/strings.h"

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

#include "azure_c_shared_utility/string_tokenizer.h"

#define ENABLE_MOCKS
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "azure_c_shared_utility/gballoc.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

static TEST_MUTEX_HANDLE g_dllByDll;
static TEST_MUTEX_HANDLE g_testByTest;

#define TEST_STRING_HANDLE (STRING_HANDLE)0x42
#define FAKE_LOCK_HANDLE (LOCK_HANDLE)0x4f

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(string_tokenizer_unittests)

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

    /* STRING_TOKENIZER_Tests BEGIN */

    /* STRING_TOKENIZER_CREATE */
    /* Tests_SRS_STRING_04_001: [STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER_HANDLE if parameter handle is NULL]*/
    TEST_FUNCTION(STRING_TOKENIZER_create_with_null_handle_fail)
    {
        ///arrange

        ///act
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NULL(t);
    }

    /* Tests_SRS_STRING_04_002: [STRING_TOKENIZER_create shall allocate a new STRING_TOKENIZER _HANDLE having the content of the STRING_HANDLE copied and current position pointing at the beginning of the string] */
    TEST_FUNCTION(STRING_TOKENIZER_create_succeed)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        umock_c_reset_all_calls();
        ///act

        STRICT_EXPECTED_CALL(gballoc_malloc(0))  //Token Allocation.
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(0))  //Token Content Allocation.
            .IgnoreArgument(1);


        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///assert
        ASSERT_IS_NOT_NULL(t);

        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
    }

    TEST_FUNCTION(STRING_TOKENIZER_create_from_char_input_NULL_fail)
    {
        ///arrange

        ///act

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create_from_char(NULL);

        ///assert
        ASSERT_IS_NULL(t);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //Cleanup
        STRING_TOKENIZER_destroy(t);
    }

    TEST_FUNCTION(STRING_TOKENIZER_create_from_char_succeed)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        umock_c_reset_all_calls();
        ///act

        STRICT_EXPECTED_CALL(gballoc_malloc(0))  //Token Allocation.
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(0))  //Token Content Allocation.
            .IgnoreArgument(1);


        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create_from_char(inputString);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NOT_NULL(t);

        //Cleanup
        STRING_TOKENIZER_destroy(t);
    }

    /* Test_SRS_STRING_04_003: [STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER _HANDLE on any error that is encountered] */
    TEST_FUNCTION(STRING_TOKENIZER_create_when_malloc_fails_then_string_tokenizer_create_fails)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        umock_c_reset_all_calls();
        ///act

        
        EXPECTED_CALL(gballoc_malloc(0));  //Token Allocation.


        whenShallmalloc_fail = currentmalloc_call + 1;
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        ///assert
        ASSERT_IS_NULL(t);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //Cleanup
        STRING_delete(input_string_handle);
    }

    /* STRING_TOKENIZER_get_next_token */
    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_handle_NULL_Fail)
    {
        ///arrange
        STRING_HANDLE output_string_handle = STRING_new();

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(NULL, output_string_handle, "m");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_Delimiter_empty_Fail)
    {
        ///arrange
        STRING_HANDLE output_string_handle = STRING_new();

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(NULL, output_string_handle, "");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //Cleanup
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_output_NULL_Fail)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, NULL, "m");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
    }

    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_delimiters_NULL_Fail)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_new();

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, NULL);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_PIRLI_Succeed)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "m");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "Pirli", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_Start_With_Delimiter_Succeed)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "irlimpimpim", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }
    
    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    /* Tests_SRS_STRING_04_008: [STRING_TOKENIZER_get_next_token than searches from the start of a token for a character that is contained in the delimiters string.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_Start_And_End_With_Delimiter_Succeed)
    {
        ///arrange
        const char* inputString = "PirlimP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "irlim", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_with_All_token_String_Fail)
    {
        ///arrange
        const char* inputString = "PPPPPP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_2Times_With_Just_1Token_SecondCall_Fail)
    {
        ///arrange
        const char* inputString = "TestP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "Test", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_007: [If such a character is found, STRING_TOKENIZER_get_next_token consider it as the start of a token.] */
    /* Tests_SRS_STRING_04_009: [If no such character is found, STRING_TOKENIZER_get_next_token extends the current token to the end of the string inside t, copies the token to output and returns 0.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_2charactersToken_at_begin_of_input_call_1_Time_Succeed)
    {
        ///arrange
        const char* inputString = "??This is a Test";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "This is a Test", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_stringWith3Tokens_Call3Times_Succeed)
    {
        ///arrange
        const char* inputString = "Test1PTest2PTest3";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result1. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result2. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result3. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");
        
        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "Test1", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr,"Test2", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr,"Test3", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_TOKENIZER_04_014: [STRING_TOKENIZER_get_next_token shall return nonzero value if t contains an empty string.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputEmptyString_Fail)
    {
        ///arrange
        const char* inputString = "";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_multipleCalls_DifferentToken_Succeed)
    {
        ///arrange
        const char* inputString = "?a???b,,,#c";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act1
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"a", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, ",");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr,"??b", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "#,");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr,"c", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act4
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert4
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_SingleCharacter_call2Times_succeed)
    {
        ///arrange
        const char* inputString = "c";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act1
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"c", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert2
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_NULL_in_the_Middle_succeed)
    {
        ///arrange
        const char* inputString = "This is a Test \0 1234";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act1
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "1");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"This is a Test ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_specialCharacters_succeed)
    {
        ///arrange
        const char* inputString = "This is a Test \r\n 1234 \r\n\t 12345";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();

        ///act1
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"This is a Test ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr," 1234 ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        EXPECTED_CALL(gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n\t");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr," 12345", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }
    /* STRING_TOKENIZER_delete */
    /*Test_SRS_STRING_TOKENIZER_04_012: [STRING_TOKENIZER_destroy shall free the memory allocated by the STRING_TOKENIZER_create ] */
    TEST_FUNCTION(STRING_TOKENIZER_DESTROY_Succeed)
    {
        ///arrange
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        umock_c_reset_all_calls();
        ///act

        EXPECTED_CALL(gballoc_free(0))  //Free Token content.
            .IgnoreArgument(1);

        EXPECTED_CALL(gballoc_free(0))  //Free Token.
            .IgnoreArgument(1);


        STRING_TOKENIZER_destroy(t);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        //Cleanup
        STRING_delete(input_string_handle);
    }

    /*Tests_SRS_STRING_TOKENIZER_04_013: [When the t argument is NULL, then STRING_TOKENIZER_destroy shall not attempt to free] */
    TEST_FUNCTION(STRING_TOKENIZER_DESTROY_WITH_NULL_HANDLE_NO_FREE)
    {
        ///arrange

        // act
        STRING_TOKENIZER_destroy(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

END_TEST_SUITE(string_tokenizer_unittests)