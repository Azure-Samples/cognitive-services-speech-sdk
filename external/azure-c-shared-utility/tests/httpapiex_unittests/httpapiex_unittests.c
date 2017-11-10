// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

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
#include "azure_c_shared_utility/macro_utils.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapi.h"

static size_t currentHTTPAPI_SaveOption_call;
static size_t whenShallHTTPAPI_SaveOption_fail;

static size_t currentBUFFER_content_call;
static size_t whenShallBUFFER_content_fail;

static size_t currentBUFFER_size_call;
static size_t whenShallBUFFER_size_fail;

#define N_MAX_FAILS 5
static size_t currentHTTPAPI_CreateConnection_call;
static size_t whenShallHTTPAPI_CreateConnection_fail[N_MAX_FAILS];

static size_t currentHTTPAPI_Init_call;
static size_t whenShallHTTPAPI_Init_fail[N_MAX_FAILS];
static size_t HTTPAPI_Init_calls;

STRING_HANDLE my_STRING_construct(const char* psz)
{
    return (STRING_HANDLE)malloc(1);
}

void my_STRING_delete(STRING_HANDLE handle)
{
    free(handle);
}

HTTP_HEADERS_HANDLE my_HTTPHeaders_Alloc(void)
{
    return (HTTP_HEADERS_HANDLE)malloc(1);
}

void my_HTTPHeaders_Free(HTTP_HEADERS_HANDLE handle)
{
    free(handle);
}

BUFFER_HANDLE my_BUFFER_new(void)
{
    return (BUFFER_HANDLE)malloc(1);
}

void my_BUFFER_delete(BUFFER_HANDLE handle)
{
    free(handle);
}

HTTPAPI_RESULT my_HTTPAPI_Init(void)
{
    HTTPAPI_RESULT result2;
    currentHTTPAPI_Init_call++;
    size_t i;
    for (i = 0; i < N_MAX_FAILS; i++)
    {
        if (whenShallHTTPAPI_Init_fail[i] > 0)
        {
            if (currentHTTPAPI_Init_call == whenShallHTTPAPI_Init_fail[i])
            {
                break;
            }
        }
    }

    if (i == N_MAX_FAILS)
    {
        HTTPAPI_Init_calls++;
        result2 = HTTPAPI_OK;
    }
    else
    {
        result2 = HTTPAPI_ERROR;
    }
    return result2;
}

void my_HTTPAPI_Deinit(void)
{
    HTTPAPI_Init_calls--;
}

HTTP_HANDLE my_HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE result2;
    currentHTTPAPI_CreateConnection_call++;
    size_t i;
    for (i = 0; i < N_MAX_FAILS; i++)
    {
        if (whenShallHTTPAPI_CreateConnection_fail[i] > 0)
        {
            if (currentHTTPAPI_CreateConnection_call == whenShallHTTPAPI_CreateConnection_fail[i])
            {
                result2 = (HTTP_HANDLE)NULL;
                break;
            }
        }
    }

    if (i == N_MAX_FAILS)
    {
        result2 = (HTTP_HANDLE)malloc(1);
    }
    return result2;
}

void my_HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    free(handle);
}

HTTPAPI_RESULT my_HTTPAPI_CloneOption(const char* optionName, const void* value, const void** savedValue)
{
    HTTPAPI_RESULT result2;
    currentHTTPAPI_SaveOption_call++;
    if (currentHTTPAPI_SaveOption_call == whenShallHTTPAPI_SaveOption_fail)
    {
        result2 = HTTPAPI_ERROR;
    }
    else
    {
        result2 = HTTPAPI_OK;
        if (strcmp("someOption", optionName) == 0)
        {
            char* temp;
            temp = (char *)malloc(strlen((const char*)value) + 1);
            strcpy(temp, (const char*)value);
            *savedValue = temp;
        }
        else if (strcmp("someOption1", optionName) == 0)
        {
            char* temp;
            temp = (char *)malloc(strlen((const char*)value) + 1);
            strcpy(temp, (const char*)value);
            *savedValue = temp;
        }
        else if (strcmp("someOption2", optionName) == 0)
        {
            char* temp;
            temp = (char *)malloc(strlen((const char*)value) + 1);
            strcpy(temp, (const char*)value);
            *savedValue = temp;
        }
        else
        {
            result2 = HTTPAPI_INVALID_ARG;
        }
    }
    return result2;
}

#ifdef __cplusplus
extern "C"
{
#endif

    VECTOR_HANDLE real_VECTOR_create(size_t elementSize);
    void real_VECTOR_destroy(VECTOR_HANDLE handle);

    /* insertion */
    int real_VECTOR_push_back(VECTOR_HANDLE handle, const void* elements, size_t numElements);

    /* removal */
    void real_VECTOR_erase(VECTOR_HANDLE handle, void* elements, size_t numElements);
    void real_VECTOR_clear(VECTOR_HANDLE handle);

    /* access */
    void* real_VECTOR_element(const VECTOR_HANDLE handle, size_t index);
    void* real_VECTOR_front(const VECTOR_HANDLE handle);
    void* real_VECTOR_back(const VECTOR_HANDLE handle);
    void* real_VECTOR_find_if(const VECTOR_HANDLE handle, PREDICATE_FUNCTION pred, const void* value);

    /* capacity */
    size_t real_VECTOR_size(const VECTOR_HANDLE handle);

    int real_mallocAndStrcpy_s(char** destination, const char* source);
    int real_size_tToString(char* destination, size_t destinationSize, size_t value);

#ifdef __cplusplus
}
#endif

#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/httpapiex.h"

TEST_DEFINE_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);

#define TEST_HOSTNAME "aaa"
#define TEST_RELATIVE_PATH "nothing/to/see/here/devices"
#define TEST_REQUEST_HTTP_HEADERS (HTTP_HEADERS_HANDLE) 0x42
#define TEST_REQUEST_BODY (BUFFER_HANDLE) 0x43
#define TEST_RESPONSE_HTTP_HEADERS (HTTP_HEADERS_HANDLE) 0x45
#define TEST_RESPONSE_BODY (BUFFER_HANDLE) 0x46
#define TEST_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE) 0x47
#define TEST_BUFFER_REQ_BODY    (BUFFER_HANDLE) 0x48
#define TEST_BUFFER_RESP_BODY   (BUFFER_HANDLE) 0x49
unsigned char* TEST_BUFFER = (unsigned char*)"333333";
#define TEST_BUFFER_SIZE 6

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static void createHttpObjects(HTTP_HEADERS_HANDLE* requestHttpHeaders, HTTP_HEADERS_HANDLE* responseHttpHeaders)
{
    /*assumed to never fail*/
    *requestHttpHeaders = HTTPHeaders_Alloc();
    *responseHttpHeaders = HTTPHeaders_Alloc();
    if (
        (*requestHttpHeaders == NULL) ||
        (*responseHttpHeaders == NULL)
        )
    {
        ASSERT_FAIL("unable to build test prerequisites");
    }
}

static void destroyHttpObjects(HTTP_HEADERS_HANDLE* requestHttpHeaders, HTTP_HEADERS_HANDLE* responseHttpHeaders)
{
    HTTPHeaders_Free(*requestHttpHeaders);
    *requestHttpHeaders = NULL;
    HTTPHeaders_Free(*responseHttpHeaders);
    *responseHttpHeaders = NULL;
}

/*this function takes care of setting expected call for httpapiex_executerequest assuming all the parameters are non-null*/
static void setupAllCallBeforeHTTPsequence(void)
{
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);
}

static void setupAllCallForHTTPsequence(const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeaders, BUFFER_HANDLE requestHttpBody, HTTP_HEADERS_HANDLE responseHttpHeaders, BUFFER_HANDLE responseHttpBody)
{
    STRICT_EXPECTED_CALL(HTTPAPI_Init());

    /*this is getting the hostname for the HTTAPI_connect call)*/
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(requestHttpBody))
        .SetReturn(TEST_BUFFER_SIZE);
    size_t requestHttpBodyLength = TEST_BUFFER_SIZE;

    STRICT_EXPECTED_CALL(BUFFER_u_char(requestHttpBody))
        .SetReturn(TEST_BUFFER);
    const unsigned char* requestHttpBodyContent = TEST_BUFFER;

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        relativePath,
        requestHttpHeaders,
        requestHttpBodyContent, 
        requestHttpBodyLength,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .IgnoreArgument(1)
        .IgnoreArgument(7)
        ;
}

/*every time HttpApi_Execute request is executed several things will be auto-aupdated by the code*/
/*request headers to match the content-length, host to match hostname*/
static void prepareHTTPAPIEX_ExecuteRequest(unsigned int *asGivenByHttpApi, HTTP_HEADERS_HANDLE requestHttpHeaders, HTTP_HEADERS_HANDLE responseHttpHeaders, BUFFER_HANDLE responseHttpBody, HTTPAPI_RESULT resultToBeUsed)
{
    /*this is building the host and content-length for the http request headers, this happens every time*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER);
    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, asGivenByHttpApi, sizeof(*asGivenByHttpApi))
        .SetReturn(resultToBeUsed);
}

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(httpapiex_unittests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT);
    REGISTER_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT);
    REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);
    REGISTER_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE);
    REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const unsigned char*, void*);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
    REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);
    REGISTER_GLOBAL_MOCK_RETURN(STRING_c_str, TEST_HOSTNAME);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Alloc, my_HTTPHeaders_Alloc);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Free, my_HTTPHeaders_Free);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_OK);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_OK);
    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_new, my_BUFFER_new);
    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_delete, my_BUFFER_delete);
    REGISTER_GLOBAL_MOCK_RETURN(BUFFER_u_char, TEST_BUFFER);
    REGISTER_GLOBAL_MOCK_RETURN(BUFFER_length, TEST_BUFFER_SIZE);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPI_Init, my_HTTPAPI_Init);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPI_Deinit, my_HTTPAPI_Deinit);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPI_CreateConnection, my_HTTPAPI_CreateConnection);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPI_CloseConnection, my_HTTPAPI_CloseConnection);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPAPI_ExecuteRequest, HTTPAPI_OK);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPAPI_SetOption, HTTPAPI_OK);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPI_CloneOption, my_HTTPAPI_CloneOption);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_create, real_VECTOR_create);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_destroy, real_VECTOR_destroy);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_push_back, real_VECTOR_push_back);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_erase, real_VECTOR_erase);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_clear, real_VECTOR_clear);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_element, real_VECTOR_element);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_front, real_VECTOR_front);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_back, real_VECTOR_back);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_find_if, real_VECTOR_find_if);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_size, real_VECTOR_size);
    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, real_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_HOOK(size_tToString, real_size_tToString);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

    currentrealloc_call = 0;
    whenShallrealloc_fail = 0;

    currentHTTPAPI_SaveOption_call = 0;
    whenShallHTTPAPI_SaveOption_fail = 0;

    currentBUFFER_content_call = 0;
    whenShallBUFFER_content_fail = 0;

    currentBUFFER_size_call = 0;
    whenShallBUFFER_size_fail = 0;
    
    HTTPAPI_Init_calls = 0;

    currentHTTPAPI_CreateConnection_call = 0;
    for(size_t i=0;i<N_MAX_FAILS;i++) whenShallHTTPAPI_CreateConnection_fail[i] = 0;

    currentHTTPAPI_Init_call = 0;
    for (size_t i = 0; i<N_MAX_FAILS; i++) whenShallHTTPAPI_Init_fail[i] = 0;

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

/*Tests_SRS_HTTPAPIEX_02_001: [If parameter hostName is NULL then HTTPAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_with_NULL_name_fails)
{
    /// arrange

    /// act
    HTTPAPIEX_HANDLE result = HTTPAPIEX_Create(NULL);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEX_02_002: [Parameter hostName shall be saved.] */
/*Tests_SRS_HTTPAPIEX_02_004: [Otherwise, HTTPAPIEX_Create shall return a HTTAPIEX_HANDLE suitable for further calls to the module.]*/
TEST_FUNCTION(HTTPAPIEX_Create_succeeds)
{
    /// arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));

    STRICT_EXPECTED_CALL(VECTOR_create(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_HANDLE result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(result);
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_1) /*this is destroy after created*/
{
    /// arrange
    HTTPAPIEX_HANDLE handle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is the copy of the hostName*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(handle)); /*this is handle data*/

    /// act
    HTTPAPIEX_Destroy(handle);
    
    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_2) /*this is destroy after setting options*/
{
    /// arrange
    HTTPAPIEX_HANDLE handle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(handle, "notAnOption", "notAValue"); /*this is not something handled*/
    (void)HTTPAPIEX_SetOption(handle, "someOption1", "someValue"); /*this is something handled*/
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is the copy of the hostName*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is "someValue"*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is "someOption"*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(handle)); /*this is handle data*/

    /// act
    HTTPAPIEX_Destroy(handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_3) /*this is destroy after having a sequence build*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence(TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG)) /*closing the conenction*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_Deinit()); /*deinit */
        
    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is hostname*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(httpapiexhandle)); /*this is the handle*/

    /// act
    HTTPAPIEX_Destroy(httpapiexhandle);

    ///assert

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    
}

/*Tests_SRS_HTTPAPIEX_02_005: [If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_malloc_fails)
{
    /// arrange
    whenShallmalloc_fail = currentmalloc_call + 1;
    STRICT_EXPECTED_CALL(gballoc_malloc(0))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_HANDLE result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_003: [If saving the parameter hostName fails for any reason, then HTTPAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_STRING_construct_fails)
{
    /// arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(0))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME))
        .SetReturn(NULL);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_HANDLE result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_005: [If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_VECTOR_create_fails)
{
    /// arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(0))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));

    STRICT_EXPECTED_CALL(VECTOR_create(IGNORED_NUM_ARG))
        .IgnoreArgument(1).SetReturn(NULL);

    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_HANDLE result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_006: [If parameter handle is NULL then HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_handle_fails)
{
    ///arrange
    unsigned int httpStatusCode;

    ///act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(NULL, HTTPAPI_REQUEST_POST, TEST_RELATIVE_PATH, TEST_REQUEST_HTTP_HEADERS, TEST_REQUEST_BODY, &httpStatusCode, TEST_RESPONSE_HTTP_HEADERS, TEST_RESPONSE_BODY);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_HTTPAPIEX_02_007: [If parameter requestType does not indicate a valid request, HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_fails_with_invalid_request_type)
{
    /// arrange
    unsigned int httpStatusCode;

    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, (HTTPAPI_REQUEST_TYPE)(COUNT_ARG(HTTPAPI_REQUEST_TYPE_VALUES)), TEST_RELATIVE_PATH, TEST_REQUEST_HTTP_HEADERS, TEST_REQUEST_BODY, &httpStatusCode, TEST_RESPONSE_HTTP_HEADERS, TEST_RESPONSE_BODY);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_023: [HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_happy_path_with_all_non_NULL_parameters)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence(TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_008: [If parameter relativePath is NULL then HTTPAPIEX_INVALID_ARG shall not assume a relative path - that is, it will assume an empty path ("").] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_relativePath_uses_empty)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence("", requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, NULL, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_009: [If parameter requestHttpHeadersHandle is NULL then HTTPAPIEX_ExecuteRequest shall allocate a temporary internal instance of HTTPHEADERS, shall add to that instance the following headers
    Host:{hostname} - as it was indicated by the call to HTTPAPIEX_Create API call
    Content-Length:the size of the requestContent parameter, and use this instance to all the subsequent calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.] 
*/
/*Tests_SRS_HTTPAPIEX_02_013: [If requestContent is NULL then HTTPAPIEX_ExecuteRequest shall behave as if a buffer of zero size would have been used, that is, it shall call HTTPAPI_ExecuteRequest with parameter content = NULL and contentLength = 0.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(NULL);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,
        0,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        ;

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_3)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc())
        .SetReturn(NULL); /*because it makes fakes request headers*/

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_011: [If parameter requestHttpHeadersHandle is not NULL then HTTPAPIEX_ExecuteRequest shall create or update the following headers of the request:
    Host:{hostname}
    Content-Length:the size of the requestContent parameter, and shall use the so constructed HTTPHEADERS object to all calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.]
*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(NULL);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        0,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        ;

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_1)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_fake_requestbody_creation_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new())
        .SetReturn(NULL); /*because it makes a fake buffer*/

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_1)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_015: [If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall not write in statusCode the HTTP status code, and it will use a temporary internal int for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_statusCode_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, NULL, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_016: [If statusCode is not NULL then If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall use it for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_statusCode_returns_that_code_to_the_caller_suceeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_cleans_when_originalhttpbody_is_NULL_and_response_headers_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*Because it is creating fake response headers*/
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc())
        .SetReturn(NULL);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_cleans_when_responsehttpbody_is_NULL_and_creating_a_fake_one_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(0);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, 0))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*Because it is creating fake response headers*/
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());

    STRICT_EXPECTED_CALL(BUFFER_new())
        .SetReturn(NULL); /*because it makes a fake buffer*/

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, NULL, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_017: [If responseHeaders handle is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal instance of HTTPHEADERS object and use that for responseHeaders parameter of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_headers_suceeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*Because it is creating fake response headers*/
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
        ;

    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_018: [If creating the temporary http headers in SRS_HTTPAPIEX_02_017 fails then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_headers_fails_when_HTTPHeaders_Alloc_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*Because it is creating fake response headers*/
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc())
        .SetReturn(NULL);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_019: [If responseHeaders is not NULL, then then HTTPAPIEX_ExecuteRequest shall use that object as parameter responseHeaders of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_response_headers_suceeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_020: [If responseContent is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal BUFFER object and use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_body_suceeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_new()); /*because it makes a fake response buffer*/

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER_SIZE);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1).SetReturn(TEST_BUFFER);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(9)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
        ;

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_021: [If creating the BUFFER_HANDLE in SRS_HTTPAPIEX_02_020 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_fails_when_BUFFER_new_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_new())
        .SetReturn(NULL); /*because it makes a fake response buffer*/

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_022: [If responseContent is not NULL then HTTPAPIEX_ExecuteRequest use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_response_body_suceeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    umock_c_reset_all_calls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(size_tToString(IGNORED_PTR_ARG, IGNORED_NUM_ARG, TEST_BUFFER_SIZE))
        .IgnoreArgument(1).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_Init());
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_023: [HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:]*/
/*Tests_SRS_HTTPAPIEX_02_024: [If any point in the sequence fails, HTTPAPIEX_ExecuteRequest shall attempt to recover by going back to the previous step and retrying that step.]*/
/*Tests_SRS_HTTPAPIEX_02_025: [If the first step fails, then the sequence fails.]*/
/*Tests_SRS_HTTPAPIEX_02_026: [A step shall be retried at most once.]*/
/*Tests_SRS_HTTPAPIEX_02_027: [If a step has been retried then all subsequent steps shall be retried too.]*/
/*Tests_SRS_HTTPAPIEX_02_028: [HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_OK when a call to HTTPAPI_ExecuteRequest has been completed successfully.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S1) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_OK);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S2) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            ;
    }
    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S3) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR)
            ;
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init())
            .SetReturn(HTTPAPI_OK);
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            ;
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S4) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init())
            .SetReturn(HTTPAPI_OK);
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            ;
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*this and all subsequent tests... */
/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F1) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        whenShallHTTPAPI_Init_fail[0] = currentHTTPAPI_Init_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F2) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[1] = currentHTTPAPI_CreateConnection_call + 2;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));

    }

    /*this is post final fail - sequence is reset to a clean start*/
    {        
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F3) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR);
            ;
    }

    /*this is post final fail - sequence is reset to a clean start*/
    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F4) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());

        whenShallHTTPAPI_Init_fail[0] = currentHTTPAPI_Init_call + 1;
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F5) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1); 
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[1] = currentHTTPAPI_CreateConnection_call + 2;
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F6) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenByHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    prepareHTTPAPIEX_ExecuteRequest(&asGivenByHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG, 
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenByHttpApi, sizeof(asGivenByHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPI_Deinit());
    }

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_032: [If parameter handle is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_handle)
{
    /// arrange
    
    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(NULL, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_033: [If parameter optionName is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_optionName_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, NULL, (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_034: [If parameter value is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_value_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_037: [HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE] */
/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_without_httpapi_handle_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption", "333", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")); /*this is looking for optionName*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is adding the optionName, value*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "333");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_037: [HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE] */
/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_without_httpapi_handle_can_save_2_options_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption1", (void*)"3", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption1")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption1"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption2"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /// act
    HTTPAPIEX_RESULT result1 = HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    HTTPAPIEX_RESULT result2 = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result1);
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result2);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_035: [HTTPAPIEX_ExecuteRequest shall pass all the saved options (see HTTPAPIEX_SetOption) to the newly create HTTPAPI_HANDLE in step 2 by calling HTTPAPI_SetOption.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_httpapi_succeeds)
{
    /// arrange
    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);

    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");
    umock_c_reset_all_calls();

    setupAllCallBeforeHTTPsequence();
    STRICT_EXPECTED_CALL(HTTPAPI_Init());

    /*this is getting the hostname for the HTTAPI_connect call)*/
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption1", (void*)"3"))
        .IgnoreArgument(1)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption2", (void*)"33"))
        .IgnoreArgument(1)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(BUFFER_length(requestHttpBody))
        .SetReturn(TEST_BUFFER_SIZE);
    size_t requestHttpBodyLength = TEST_BUFFER_SIZE;

    STRICT_EXPECTED_CALL(BUFFER_u_char(requestHttpBody))
        .SetReturn(TEST_BUFFER);
    const unsigned char* requestHttpBodyContent = TEST_BUFFER;

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        requestHttpBodyContent,
        requestHttpBodyLength,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .IgnoreArgument(1)
        .IgnoreArgument(7)
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_036: [If setting the option fails, then the failure shall be ignored.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_httpapi_ignores_errors_succeeds)
{
    /// arrange
    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);

    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");
    umock_c_reset_all_calls();

    setupAllCallBeforeHTTPsequence();
    STRICT_EXPECTED_CALL(HTTPAPI_Init());

    /*this is getting the hostname for the HTTAPI_connect call)*/
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption1", (void*)"3"))
        .IgnoreArgument(1)
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_ERROR);

    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption2", (void*)"33"))
        .IgnoreArgument(1)
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_ERROR);

    STRICT_EXPECTED_CALL(BUFFER_length(requestHttpBody))
        .SetReturn(TEST_BUFFER_SIZE);
    size_t requestHttpBodyLength = TEST_BUFFER_SIZE;

    STRICT_EXPECTED_CALL(BUFFER_u_char(requestHttpBody))
        .SetReturn(TEST_BUFFER);
    const unsigned char* requestHttpBodyContent = TEST_BUFFER;

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        requestHttpBodyContent,
        requestHttpBodyLength,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .IgnoreArgument(1)
        .IgnoreArgument(7)
        ;

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_VECTOR_push_back_fails)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption2"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the clone created by HTTPAPI_CloneOption*/
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_gballoc_fails_1)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption2"))
        .IgnoreArgument(1).SetReturn(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the clone created by HTTPAPI_CloneOption*/
        .IgnoreArgument(1);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
/*Tests_SRS_HTTPAPIEX_02_040: [For all other return values of HTTPAPI_SetOption, HTTPIAPIEX_SetOption shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_HTTPAPI_SaveOption_fails_3)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();
    
    whenShallHTTPAPI_SaveOption_fail = currentHTTPAPI_SaveOption_call + 1;
    EXPECTED_CALL(HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .SetReturn(HTTPAPI_ALREADY_INIT); /*some random error code*/

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_038: [If HTTPAPI_CloneOption returns HTTPAPI_INVALID_ARG then HTTPAPIEX shall return HTTPAPIEX_INVALID_ARG.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_HTTPAPI_SaveOption_fails_4)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    umock_c_reset_all_calls();

    whenShallHTTPAPI_SaveOption_fail = currentHTTPAPI_SaveOption_call + 1;
    EXPECTED_CALL(HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .SetReturn(HTTPAPI_INVALID_ARG);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_succeeds)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);

    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence(TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"));

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_fails_1)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);

    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence(TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"))
        .SetReturn(HTTPAPI_INVALID_ARG);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_fails_2)
{
    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody = TEST_BUFFER_REQ_BODY;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody = TEST_BUFFER_RESP_BODY;
    createHttpObjects(&requestHttpHeaders, &responseHttpHeaders);

    setupAllCallBeforeHTTPsequence();
    setupAllCallForHTTPsequence(TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")); /*this is looking for the option to device between update / create*/

    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "someOption"))
        .IgnoreArgument(1); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    EXPECTED_CALL(HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"))
        .SetReturn(HTTPAPI_ALLOC_FAILED);

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");
    
    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &responseHttpHeaders);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_on_update_option_value_succeeds)
{
    const char* OPTION_NAME = "someOption";

    /// arrange
    HTTPAPIEX_HANDLE httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    HTTPAPIEX_SetOption(httpapiexhandle, OPTION_NAME, "333");
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPAPI_CloneOption(OPTION_NAME, "4", IGNORED_PTR_ARG));  /*this asks lower HTTPAPI to create a clone of the option*/

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, OPTION_NAME)); /*this is looking for the option to device between update / create*/

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)); /*this is free-ing the previos value*/

    /// act
    HTTPAPIEX_RESULT result = HTTPAPIEX_SetOption(httpapiexhandle, OPTION_NAME, "4");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_043: [If parameter handle is NULL then HTTPAPIEX_Destroy shall take no action.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_with_NULL_argument_does_nothing)
{
    /// arrange
    /// act
    HTTPAPIEX_Destroy(NULL);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///destroy
}

END_TEST_SUITE(httpapiex_unittests)
