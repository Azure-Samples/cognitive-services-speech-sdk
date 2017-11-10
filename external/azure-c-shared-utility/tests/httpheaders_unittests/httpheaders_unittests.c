// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <limits.h>

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

static size_t currentrealloc_call = 0;
static size_t whenShallrealloc_fail = 0;

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
    void* result;
    currentrealloc_call++;
    if (whenShallrealloc_fail > 0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
        }
    }
    else
    {
        result = realloc(ptr, size);
    }

    return result;
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/map.h"

MAP_HANDLE my_Map_Create(MAP_FILTER_CALLBACK mapFilterFunc)
{
    return (MAP_HANDLE)malloc(1);
}

MAP_HANDLE my_Map_Clone(MAP_HANDLE handle)
{
    return (MAP_HANDLE)malloc(1);
}

void my_Map_Destroy(MAP_HANDLE handle)
{
    free(handle);
}

#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/httpheaders.h"

/*Below tags exists for traceability reasons only, they canot be really tested by automated means, except "this file compiles"*/
/*Tests_SRS_HTTP_HEADERS_99_001:[ HttpHeaders shall have the following interface]*/

static TEST_MUTEX_HANDLE g_testByTest;

TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(MAP_RESULT, MAP_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(MAP_RESULT, MAP_RESULT_VALUES);

/*test assets*/
#define NAME1 "name1"
#define VALUE1 "value1"
#define HEADER1 NAME1 ": " VALUE1
static const char *NAME1_TRICK1 = "name1:";
static const char *NAME1_TRICK2 = "name1: ";
static const char *NAME1_TRICK3 = "name1: value1";

#define NAME2 "name2"
#define VALUE2 "value2"
#define HEADER2 NAME2 ": " VALUE2

#define TEMP_BUFFER_SIZE 1024
static char tempBuffer[TEMP_BUFFER_SIZE];

#define MAX_NAME_VALUE_PAIR 100

static TEST_MUTEX_HANDLE g_dllByDll;

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(HTTPHeaders_UnitTests)

        TEST_SUITE_INITIALIZE(TestClassInitialize)
        {
            int result;

            TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

            g_testByTest = TEST_MUTEX_CREATE();
            ASSERT_IS_NOT_NULL(g_testByTest);

            umock_c_init(on_umock_c_error);

            result = umocktypes_charptr_register_types();
            ASSERT_ARE_EQUAL(int, 0, result);

            REGISTER_TYPE(MAP_RESULT, MAP_RESULT);
            REGISTER_UMOCK_ALIAS_TYPE(MAP_FILTER_CALLBACK, void*);
            REGISTER_UMOCK_ALIAS_TYPE(MAP_HANDLE, void*);

            REGISTER_GLOBAL_MOCK_HOOK(Map_Create, my_Map_Create);
            REGISTER_GLOBAL_MOCK_HOOK(Map_Clone, my_Map_Clone);
            REGISTER_GLOBAL_MOCK_HOOK(Map_Destroy, my_Map_Destroy);
            REGISTER_GLOBAL_MOCK_RETURN(Map_AddOrUpdate, MAP_OK);
            REGISTER_GLOBAL_MOCK_RETURN(Map_GetValueFromKey, VALUE1);
            REGISTER_GLOBAL_MOCK_RETURN(Map_GetInternals, MAP_OK);

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

        TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
        {
            if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
            {
                ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
            }

            umock_c_reset_all_calls();

            currentmalloc_call = 0;
            whenShallmalloc_fail = 0;

            currentrealloc_call = 0;
            whenShallrealloc_fail = 0;
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            TEST_MUTEX_RELEASE(g_testByTest);
        }


        /*Tests_SRS_HTTP_HEADERS_99_002:[ This API shall produce a HTTP_HANDLE that can later be used in subsequent calls to the module.]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_happy_path_succeeds)
        {
            ///arrange
            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(Map_Create(IGNORED_PTR_ARG));

            ///act
            HTTP_HEADERS_HANDLE handle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NOT_NULL(handle);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            /// cleanup
            HTTPHeaders_Free(handle);
        }


        /*Tests_SRS_HTTP_HEADERS_99_003:[ The function shall return NULL when the function cannot execute properly]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_fails_when_malloc_fails)
        {
            ///arrange
            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NULL(httpHandle);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_02_001: [If httpHeadersHandle is NULL then HTTPHeaders_Free shall perform no action.] */
        TEST_FUNCTION(HTTPHeaders_Free_with_NULL_handle_does_nothing)
        {
            ///arrange

            ///act
            HTTPHeaders_Free(NULL);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        }

        /*Tests_SRS_HTTP_HEADERS_99_005:[ Calling this API shall de-allocate the data structures allocated by previous API calls to the same handle.]*/
        TEST_FUNCTION(HTTPHeaders_Free_with_valid_handle_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE handle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTPHeaders_Free(handle);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }


        /*Tests_SRS_HTTP_HEADERS_99_003:[ The function shall return NULL when the function cannot execute properly]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_fails_when_Map_Create_fails)
        {
            ///arrange
            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(Map_Create(IGNORED_PTR_ARG))
                .SetReturn(NULL);

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NULL(httpHandle);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_99_004:[ After a successful init, HTTPHeaders_GetHeaderCount shall report 0 existing headers.]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_succeeds_and_GetHeaderCount_returns_0)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            size_t nHeaders;
            size_t zero = 0;
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .CopyOutArgumentBuffer(4, &zero, sizeof(zero));

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, 0, nHeaders);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_012:[ Calling this API shall record a header from name and value parameters.]*/
        /*Tests_SRS_HTTP_HEADERS_99_013:[ The function shall return HTTP_HEADERS_OK when execution is successful.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_happy_path_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Codes_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_fails_when_Map_AddOrUpdate_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/


            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1)
                .SetReturn(MAP_ERROR);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ALLOC_FAILED, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_handle_fails)
        {
            ///arrange

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(NULL, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_name_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NULL, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_value_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_017:[ If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_appends_to_existing_value_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1 ", " VALUE1))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_appends_fails_when_Map_AddOrUpdate_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1 ", " VALUE1))
                .IgnoreArgument(1)
                .SetReturn(MAP_ERROR);

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_fails_when_gballoc_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ALLOC_FAILED, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_012:[ Calling this API shall record a header from name and value parameters.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_add_two_headers_produces_two_headers)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME2, VALUE2))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_017:[ If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.]*/
        TEST_FUNCTION(HTTPHeaders_When_Second_Added_Header_Is_A_Substring_Of_An_Existing_Header_2_Headers_Are_Added)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "ab"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "ab", VALUE1);
            umock_c_reset_all_calls();
            
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, "a", VALUE1))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT result = HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_022:[ The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_NULL_handle_returns_NULL)
        {
            ///arrange

            ///act
            const char* res = HTTPHeaders_FindHeaderValue(NULL, NAME1);

            ///assert
            ASSERT_IS_NULL(res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_99_022:[ The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_NULL_name_returns_NULL)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            ///act
            const char* res = HTTPHeaders_FindHeaderValue(httpHandle, NULL);

            ///assert
            ASSERT_IS_NULL(res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_previously_stored_value_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1);

            ///act
            const char* res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, VALUE1, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_previously_stored_value_for_two_headers_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn(VALUE2);

            ///act
            const char* res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);
            const char* res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME2);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, VALUE1, res1);
            ASSERT_ARE_EQUAL(char_ptr, VALUE2, res2);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_concatenation_of_previously_stored_values_for_header_name_succeeds)
        {
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1); /*this key exists, was added above*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE2);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1 ", " VALUE2);

            ///act
            const char* res = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr,VALUE1 ", " VALUE2, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_020:[ The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.]*/
        /*actually we are trying to see that finding a nonexisting value produces NULL*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_returns_NULL_for_nonexistent_value)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            ///act
            const char* res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME2);

            ///assert
            ASSERT_IS_NULL(res2);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*trying to catch some errors here*/
        /*Tests_SRS_HTTP_HEADERS_99_020:[ The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_nonexistent_header_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK3))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            ///act
            const char* res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK1);
            const char* res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK2);
            const char* res3 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK3);

            ///assert
            ASSERT_IS_NULL(res1);
            ASSERT_IS_NULL(res2);
            ASSERT_IS_NULL(res3);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /* Tests_SRS_HTTP_HEADERS_06_001: [This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.] */
        TEST_FUNCTION(HTTPHeaders_ReplaceHeaderNameValuePair_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE2))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_ReplaceHeaderNameValuePair(httpHandle, NAME1, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /* Tests_SRS_HTTP_HEADERS_06_001: [This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.] */
        TEST_FUNCTION(HTTPHeaders_ReplaceHeaderNameValuePair_for_none_existing_header_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE2))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_ReplaceHeaderNameValuePair(httpHandle, NAME1, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_024:[ The function shall return HTTP_HEADERS_INVALID_ARG when an invalid handle is passed.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_NULL_handle_fails)
        {
            ///arrange
            size_t nHeaders;

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeaderCount(NULL, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_99_025:[ The function shall return HTTP_HEADERS_INVALID_ARG when headersCount is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_NULL_headersCount_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeaderCount(httpHandle, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_026:[ The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK]*/
        /*Tests_SRS_HTTP_HEADERS_99_023:[ Calling this API shall provide the number of stored headers.]*/
        /*Tests_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_1_header_produces_1)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            size_t nHeaders;
            const char* keys[] = { "NAME1" }; 
            const char* values[] = { "VALUE1" }; 
            const char* const ** pKeys = (const char* const **)&keys; 
            const char* const ** pValues = (const char* const **)&values;
            const size_t one = 1;
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));


            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);
            
            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, (size_t)1, nHeaders);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_037:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_fails_when_Map_GetInternals_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            size_t nHeaders;
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(MAP_ERROR);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());


            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_026:[ The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK]*/
        /*Tests_SRS_HTTP_HEADERS_99_023:[ Calling this API shall provide the number of stored headers.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_2_header_produces_2)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);
            size_t nHeaders;
            const char* keys[2] = { "NAME1", "NAME2" };
            const char* values[2] = { "VALUE1", "VALUE2" };
            const char* const ** pKeys = (const char* const **)&keys;
            const char* const ** pValues = (const char* const **)&values;

            const size_t two = 2;
            umock_c_reset_all_calls();


            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &two, sizeof(two));

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, 2, nHeaders);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls()); 

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_028:[ The function shall return NULL if the handle is invalid.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_NULL_handle_fails)
        {
            ///arrange
            char* headerValue;

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeader(NULL, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        }

        /*Tests_SRS_HTTP_HEADERS_99_032:[ The function shall return HTTP_HEADERS_INVALID_ARG if the destination  is NULL]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_NULL_buffer_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            umock_c_reset_all_calls();

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_GetHeader(httpHandle, 0, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
        /*Tests that not adding something to the collection fails tso retrieve for index 0*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_index_too_big_fails_1)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            char* headerValue;
            const char* thisIsNULL = NULL;
            const char* zero = 0;
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &thisIsNULL, sizeof(thisIsNULL))
                .CopyOutArgumentBuffer(3, &thisIsNULL, sizeof(thisIsNULL))
                .CopyOutArgumentBuffer(4, &zero, sizeof(zero));

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
        /*Tests that not adding something to the collection fails tso retrieve for index 0*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_index_too_big_fails_2)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            const char* keys[1] = { "NAME1" };
            const char* values[1] = { "VALUE1" };
            const char* const ** pKeys = (const char* const **)&keys;
            const char* const ** pValues = (const char* const **)&values;
            const size_t one = 1;
            umock_c_reset_all_calls();
            char* headerValue;

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 1, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_027:[ Calling this API shall produce the string value+": "+pair) for the index header in the buffer pointed to by buffer.]*/
        /*Tests_SRS_HTTP_HEADERS_99_035:[ The function shall return HTTP_HEADERS_OK when the function executed without error.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_succeeds_1)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            umock_c_reset_all_calls();
            char* headerValue;
            const char* keys[1] = { "a" };
            const char** pKeys = &keys[0];
            const char* values[1] = { "b" };
            const char** pValues = &values[0];
            const size_t one = 1;

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res1);
            ASSERT_ARE_EQUAL(char_ptr, "a: b", headerValue);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);
        }

        /*Tests_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_fails_when_Map_GetInternals_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            umock_c_reset_all_calls();
            char* headerValue;
            const char* keys[1] = { "a" };
            const char** pKeys = &keys[0];
            const char* values[1] = { "b" };
            const char** pValues = &values[0];
            const size_t one = 1;

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one))
                .SetReturn(MAP_ERROR);

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_succeeds_fails_when_malloc_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            umock_c_reset_all_calls();
            char* headerValue;
            const char* keys[1] = { "a" };
            const char** pKeys = &keys[0];
            const char* values[1] = { "b" };
            const char** pValues = &values[0];
            const size_t one = 1;

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res1);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);
        }

        /*Tests_SRS_HTTP_HEADERS_99_031:[ If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_colon_in_name_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a:", "b");

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_031:[ If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_colon_in_value_succeeds_1)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            char* headerValue;
            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", ":");
            const char* keys[1] = { "a" };
            const char** pKeys = &keys[0];
            const char* values[1] = { ":" };
            const char** pValues = &values[0];
            const size_t one = 1;
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res1);
            ASSERT_ARE_EQUAL(char_ptr, "a: :", headerValue);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);
        }

        /*Tests_SRS_HTTP_HEADERS_99_036:[ If name contains the characters outside character codes 33 to 126 then the return value shall be HTTP_HEADERS_INVALID_ARG]*/
        /*so says http://tools.ietf.org/html/rfc822#section-3.1*/
        TEST_FUNCTION(HTTP_HEADERS_AddHeaderNameValuePair_fails_for_every_invalid_character)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            char unacceptableString[2]={'\0', '\0'};
            
            for(int c=SCHAR_MIN;c <=SCHAR_MAX; c++)
            {
                if(c=='\0') continue;

                if((c<33) ||( 126<c)|| (c==':'))
                {
                    /*so it is an unacceptable character*/
                    unacceptableString[0]=(char)c;

                    ///act
                    HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, unacceptableString, VALUE1);

                    ///assert
                    ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
                }
            }

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_02_002: [The LWS from the beginning of the value shall not be stored.] */
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_LWS_value_stores_without_LWS_characters_succeeds)
        {
            ///arrange
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, " \r\t\n" VALUE1); /*notice how there are some LWS characters in the value*/

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_02_003: [If handle is NULL then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_with_NULL_parameter_returns_NULL)
        {
            ///arrange
            ///act
            HTTP_HEADERS_HANDLE result= HTTPHeaders_Clone(NULL);

            ///assert
            ASSERT_IS_NULL(result);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
            ///cleanup
        }

        /*Tests_SRS_HTTP_HEADERS_02_004: [Otherwise HTTPHeaders_Clone shall clone the content of handle to a new handle.*/
        TEST_FUNCTION(HTTPHEADERS_Clone_happy_path)
        {
            ///arrange
            HTTP_HEADERS_HANDLE source = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(Map_Clone(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_HANDLE result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NOT_NULL(result);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }

        /*Tests_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_fails_when_map_clone_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE source = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(Map_Clone(IGNORED_PTR_ARG))
                .IgnoreArgument(1).SetReturn(NULL);

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_HANDLE result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NULL(result);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }

        /*Tests_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_fails_when_gballoc_fails)
        {
            ///arrange
            HTTP_HEADERS_HANDLE source = HTTPHeaders_Alloc();
            umock_c_reset_all_calls();

            STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1).SetReturn(NULL);

            ///act
            HTTP_HEADERS_HANDLE result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NULL(result);
            ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }


END_TEST_SUITE(HTTPHeaders_UnitTests)
