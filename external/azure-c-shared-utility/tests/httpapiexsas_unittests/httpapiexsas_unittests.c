// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>

#include <time.h>

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

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

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/httpapiexsas.h"

TEST_DEFINE_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_NULL_STRING_HANDLE (STRING_HANDLE)0x00
#define TEST_KEYNAME_HANDLE (STRING_HANDLE)0x48
#define TEST_KEY_HANDLE (STRING_HANDLE)0x49
#define TEST_URIRESOURCE_HANDLE (STRING_HANDLE)0x50
#define TEST_CLONED_KEYNAME_HANDLE  (STRING_HANDLE)0x51
#define TEST_CLONED_URIRESOURCE_HANDLE  (STRING_HANDLE)0x52
#define TEST_CLONED_KEY_HANDLE  (STRING_HANDLE)0x53
#define TEST_HTTPAPIEX_HANDLE (HTTPAPIEX_HANDLE)0x54
#define TEST_HTTPAPI_REQUEST_TYPE (HTTPAPI_REQUEST_TYPE)0x55
#define TEST_REQUEST_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE)0x56
#define TEST_REQUEST_CONTENT (BUFFER_HANDLE)0x57
#define TEST_RESPONSE_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE)0x58
#define TEST_RESPONSE_CONTENT (BUFFER_HANDLE)0x59
#define TEST_CONST_CHAR_STAR_NULL (const char*)NULL
#define TEST_SASTOKEN_HANDLE (STRING_HANDLE)0x60
#define TEST_EXPIRY ((size_t)7200)
#define TEST_TIME_T ((time_t)-1)

static const char TEST_CHAR_ARRAY[10] = "ABCD";

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

STRING_HANDLE my_STRING_clone(STRING_HANDLE handle)
{
    return (STRING_HANDLE)malloc(1);
}

void my_STRING_delete(STRING_HANDLE handle)
{
    free(handle);
}

STRING_HANDLE my_SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry)
{
    return (STRING_HANDLE)malloc(1);
}

static void setupSAS_Create_happy_path(void)
{
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_CLONED_URIRESOURCE_HANDLE);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEYNAME_HANDLE)).SetReturn(TEST_CLONED_KEYNAME_HANDLE);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_KEYNAME_HANDLE));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_URIRESOURCE_HANDLE));
}

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(httpapiexsas_unittests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT);
    REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(time_t*, void*);
    REGISTER_UMOCK_ALIAS_TYPE(time_t, int);
    REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(SASToken_Create, my_SASToken_Create);

    REGISTER_GLOBAL_MOCK_RETURN(STRING_c_str, TEST_CONST_CHAR_STAR_NULL);
    REGISTER_GLOBAL_MOCK_RETURN(STRING_length, 0);

    REGISTER_GLOBAL_MOCK_RETURN(HTTPAPIEX_ExecuteRequest, HTTPAPIEX_ERROR);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_FindHeaderValue, TEST_CONST_CHAR_STAR_NULL);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_ERROR);
    REGISTER_GLOBAL_MOCK_RETURN(get_time, TEST_TIME_T);
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
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);

    umock_c_reset_all_calls();

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;
}

TEST_FUNCTION(HTTPAPIEX_SAS_is_zero_the_epoch)
{
    time_t epoch_candidate = 0;
    struct tm broken_down_time;
    broken_down_time = *gmtime(&epoch_candidate);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_hour, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_min, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_sec, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_year, 70);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mon, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mday, 1);

}

/*Tests_SRS_HTTPAPIEXSAS_06_001: [If the parameter key is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_key_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_NULL_STRING_HANDLE, TEST_STRING_HANDLE, TEST_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_002: [If the parameter uriResource is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_uriResource_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_NULL_STRING_HANDLE, TEST_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_003: [If the parameter keyName is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_keyName_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_STRING_HANDLE, TEST_NULL_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_malloc_state_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);

    whenShallmalloc_fail = 1;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_STRING_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_first_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_second_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_third_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_CLONED_URIRESOURCE_HANDLE);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_CLONED_URIRESOURCE_HANDLE));
    STRICT_EXPECTED_CALL(STRING_clone(TEST_KEYNAME_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_006: [HTTAPIEX_SAS_Destroy shall deallocate any structures denoted by the parameter handle.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_succeeds)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    setupSAS_Create_happy_path();

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NOT_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_005: [If the parameter handle is NULL then HTTAPIEX_SAS_Destroy shall do nothing and return.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_destroy_with_null_succeeds)
{
    // arrange
    // act
    HTTPAPIEX_SAS_Destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_007: [If the parameter sasHandle is NULL then HTTPAPIEX_SAS_ExecuteRequest shall simply invoke HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments and shall return immediately with the result of that call as the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_with_null_sas_handle_succeeds)
{
    HTTPAPIEX_RESULT result = HTTPAPIEX_ERROR;
    unsigned int statusCode;

    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);
    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(NULL, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEXSAS_06_008: [if the parameter requestHttpHeadersHandle is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_with_null_request_http_headers_handle_succeeds)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, NULL, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, NULL, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_009: [HTTPHeaders_FindHeaderValue shall be invoked with the requestHttpHeadersHandle as its first argument and the string "Authorization" as its second argument.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_010: [If the return result of the invocation of HTTPHeaders_FindHeaderValue is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_findheadervalues_returns_null_succeeds)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CONST_CHAR_STAR_NULL);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_018: [A value of type time_t that shall be known as currentTime is obtained from calling get_time.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_019: [If the value of currentTime is (time_t)-1 is then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_get_time_fails)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(get_time(NULL)).SetReturn((time_t)-1);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_011: [SASToken_Create shall be invoked.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_012: [If the return result of SASToken_Create is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_sastoken_create_returns_null_succeeds)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_NULL_STRING_HANDLE);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_013: [HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (newSASToken) as its third argument.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_014: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_015: [STRING_delete(newSASToken) will be invoked.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_replace_header_name_value_pair_fails_succeeds)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_SASTOKEN_HANDLE);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_SASTOKEN_HANDLE));
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_SASTOKEN_HANDLE)).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization", TEST_CHAR_ARRAY)).SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_016: [HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments will be invoked and the result of that call is the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_replace_header_name_value_pair_succeeds_succeeds)
{

    HTTPAPIEX_RESULT result;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path();
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_SASTOKEN_HANDLE);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_SASTOKEN_HANDLE));
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_SASTOKEN_HANDLE)).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization", TEST_CHAR_ARRAY)).SetReturn(HTTP_HEADERS_OK);
    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

END_TEST_SUITE(httpapiexsas_unittests)
