// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
//#ifdef _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif

static size_t g_currentmalloc_call;
static size_t g_whenShallmalloc_fail;
static size_t g_buffer_size;

void* my_gballoc_malloc(size_t size)
{
    void* result;
    g_currentmalloc_call++;
    if (g_whenShallmalloc_fail > 0)
    {
        if (g_currentmalloc_call == g_whenShallmalloc_fail)
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
#include "azure_c_shared_utility/macro_utils.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/buffer_.h"
/* after this point malloc is gballoc */
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#undef ENABLE_MOCKS
#include "azure_c_shared_utility/uhttp.h"

int my_BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size)
{
    g_buffer_size += size;
    return 0;
}

size_t my_BUFFER_length(BUFFER_HANDLE handle)
{
    return g_buffer_size;
}

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;
static ON_BYTES_RECEIVED g_onBytesRecv = NULL;
static void* g_onBytesRecv_ctx = NULL;
static ON_IO_ERROR g_on_io_error = NULL;
static void* g_on_io_error_ctx = NULL;
static bool g_msgRecvSuccessful = false;

static HTTP_HEADERS_HANDLE TEST_HTTP_HEADERS_HANDLE = (HTTP_HEADERS_HANDLE)0x1211;
static XIO_HANDLE TEST_XIO_HANDLE = (XIO_HANDLE)0x1212;
//static HTTP_HEADERS_HANDLE TEST_HTTP_HEADER_HANDLE = (HTTP_HEADERS_HANDLE)0x1213;
static BUFFER_HANDLE TEST_BUFFER_HANDLE = (BUFFER_HANDLE)0x1214;
static STRING_HANDLE TEST_STRING_HANDLE = (STRING_HANDLE)0x1215;

static const char* TEST_HOST_NAME = "HTTP_HOST";
static const char* TEST_HEADER_STRING = "HEADER_VALUE";
static const char* TEST_HTTP_CONTENT = "grant_type=client_credentials&client_id=d14d2b5&client_secret=shhhhhh";
static const unsigned char TEST_HTTP_CONTENT_CHUNK[] = { 0x52, 0x49, 0x46, 0x46, 0x0A, 0x31, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74 };
static size_t TEST_HTTP_CONTENT_CHUNK_LEN = 15;
static size_t HEADER_COUNT = 9;
static const char* TEST_HTTP_EXAMPLE[] = 
{
    "HTTP/1.1 200 OK\r\nDate: Mon, 23 May 2005 22:38:34 GMT\r\nContent-Type: tex",
    "t/html; charset=UTF-8\r\nContent-Encoding: UTF-8\r\nContent-Leng",
    "th: 118\r\nLast-Modified: Wed, 08 Jan 2003 23:11:55 GMT\r\nServer: Apache/1.3.3.7 (Unix)(Red-Hat/Linux)\r\n",
    "ETag: \"3f80f-1b6-3e1cb03b\"\r\nAccept-",
    "Ranges: bytes\r\nConnection: close\r\n\r\n<html><head><title>An Example Page</title>",
    "</head><body>Hello World, this is a very simple HTML document.</body></html>" 
};
static const char* TEST_HTTP_NO_CONTENT_EXAMPLE[] =
{
    "HTTP/1.1 204 No Content\r\nContent-Length:",
    "0\r\nServer: Microsoft-HTTPAPI/2.0\r\nDate: Wed",
    ", 25 May 2016 00:07:51 GMT\r\n\r\n"
};
static const char* TEST_HTTP_BODY = "<html><head><title>An Example Page</title></head><body>Hello World, this is a very simple HTML document.</body></html>";

int my_xio_open_fn(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    g_onBytesRecv = on_bytes_received;
    g_onBytesRecv_ctx = on_bytes_received_context;
    g_on_io_error = on_io_error;
    g_on_io_error_ctx = on_io_error_context;
    return 0;
}

TEST_DEFINE_ENUM_TYPE(HTTP_CLIENT_RESULT, HTTP_CLIENT_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_CLIENT_RESULT, HTTP_CLIENT_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(uhttp_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);

    REGISTER_UMOCK_ALIAS_TYPE(HTTP_CLIENT_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(XIO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
   
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(xio_open, my_xio_open_fn);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_build, my_BUFFER_build);
    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_length, my_BUFFER_length);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(method_init)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
    {
        ASSERT_FAIL("Could not acquire test serialization mutex.");
    }

    umock_c_reset_all_calls();

    g_currentmalloc_call = 0;
    g_whenShallmalloc_fail = 0;
    g_msgRecvSuccessful = false;
    g_onBytesRecv = NULL;
    g_onBytesRecv_ctx = NULL;
    g_on_io_error = NULL;
    g_on_io_error_ctx = NULL;
}

TEST_FUNCTION_CLEANUP(method_cleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

static void OnErrorCB(HTTP_CLIENT_HANDLE handle, void* callbackCtx)
{
    (void)handle;
    (void)callbackCtx;
}

static void OnConnectedCallback(HTTP_CLIENT_HANDLE handle, void* callbackCtx)
{
    (void)handle;
    (void)callbackCtx;
}

static void OnMsgRecvCb(HTTP_CLIENT_HANDLE handle, void* callback_ctx, const unsigned char* content, size_t contentLen, unsigned int statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle)
{
    const char** expected_content = (const char**)callback_ctx;
    if (expected_content != NULL)
    {
        g_msgRecvSuccessful = true;
    }
}

static void OnChunkReply(HTTP_CLIENT_HANDLE handle, void* callback_ctx, const unsigned char* content, size_t contentLen, unsigned int statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, bool replyComplete)
{
    const char** expected_content = (const char**)callback_ctx;
    if (expected_content != NULL)
    {
        g_msgRecvSuccessful = true;
    }
}

/* http_client_create */
TEST_FUNCTION(uhttp_client_create_xio_handle_NULL_fails)
{
    // arrange

    // act
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(NULL, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(clientHandle);
}

/* Tests_SRS_UHTTP_07_002: [If msgCb is NULL then http_client_create shall return NULL] */
TEST_FUNCTION(uhttp_client_create_msg_recv_callback_NULL_fails)
{
    // arrange

    // act
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, NULL, OnErrorCB, OnChunkReply, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(clientHandle);
}

/* Tests_SRS_UHTTP_07_003: [If http_client_create encounters any error then it shall return NULL] */
TEST_FUNCTION(uhttp_client_create_memory_NULL_fails)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).SetFailReturn((void*)NULL);

    g_whenShallmalloc_fail = 1;
    // act
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(clientHandle);
}

/* Tests_SRS_UHTTP_07_001: [http_client_create shall return an initialize the http client.] */
TEST_FUNCTION(uhttp_client_create_succeeds)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    // act
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NOT_NULL(clientHandle);

    // Cleanup
    http_client_destroy(clientHandle);
}

/* http_client_destroy */
/* Tests_SRS_UHTTP_07_004: [if hanlde is NULL then http_client_destroy shall do nothing] */
TEST_FUNCTION(uhttp_client_destroy_http_handle_NULL_succeeds)
{
    // arrange

    // act
    http_client_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_UHTTP_07_005: [http_client_destroy shall free any memory that is allocated in this translation unit] */
TEST_FUNCTION(uhttp_client_destroy_succeeds)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    umock_c_reset_all_calls();

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    http_client_destroy(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_open shall return HTTP_CLIENT_INVALID_ARG] */
TEST_FUNCTION(uhttp_client_open_HTTP_CLIENT_HANDLE_NULL_fail)
{
    // arrange

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_open(NULL, TEST_HOST_NAME);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);
}

/* Tests_SRS_UHTTP_07_007: [http_client_open shall attempt to open the xioHandle and shall return HTTP_CLIENT_OPEN_REQUEST_FAILED if a failure is encountered] */
/*TEST_FUNCTION(uhttp_client_open_XIO_open_NULL_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_open(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(__LINE__);

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_open(clientHandle, TEST_HOST_NAME);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OPEN_REQUEST_FAILED, httpResult);

    // Cleanup
    http_client_destroy(clientHandle);
}*/

/* Tests_SRS_UHTTP_07_008: [If http_client_open succeeds then it shall return HTTP_CLIENT_OK] */
TEST_FUNCTION(uhttp_client_open_succeed)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_open(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    /*STRICT_EXPECTED_CALL(xio_open(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .IgnoreArgument(7);*/

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_open(clientHandle, TEST_HOST_NAME);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls() );
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_009: [If handle is NULL then http_client_close shall do nothing] */
TEST_FUNCTION(uhttp_client_close_HTTP_CLIENT_HANDLE_NULL_fail)
{
    // arrange

    // act
    http_client_close(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_UHTTP_07_010: [If the xio_handle is NOT NULL http_client_close shall call xio_close to close the handle] */
TEST_FUNCTION(uhttp_client_close_xio_handle_NULL_succeed)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    umock_c_reset_all_calls();

    // act
    http_client_close(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_011: [http_client_close shall free any HTTPHeader object that has not been freed] */
TEST_FUNCTION(uhttp_client_close_succeed)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_close(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    http_client_close(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_close_free_headers_succeed)
{
    size_t headerCount = 0;

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", NULL, NULL, 0);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_close(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    http_client_close(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_012: [If handle, relativePath, or httpHeadersHandle is NULL then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG] */
TEST_FUNCTION(uhttp_client_execute_request_http_client_handle_NULL_fail)
{
    // arrange
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(NULL, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
}

/* Tests_SRS_UHTTP_07_012: [If handle, relativePath, or httpHeadersHandle is NULL then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG] */
TEST_FUNCTION(uhttp_client_execute_request_relativePath_NULL_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, NULL, TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_httpheaders_alloc_null_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    EXPECTED_CALL(BUFFER_new()).SetReturn(NULL);
    EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    EXPECTED_CALL(BUFFER_delete(IGNORED_NUM_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_buffer_new_null_fail)
{
    int headerCount = 0;
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(NULL);
    EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_NUM_ARG));
    EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_014: [If the requestType is not a valid request http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_invalid_method_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, (HTTP_CLIENT_REQUEST_TYPE)256, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_method_alloc_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).SetFailReturn((void*)NULL);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG));
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG));

    g_whenShallmalloc_fail = 2;

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_STRING_construct_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG));
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_httpheader_getheadercount_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(HTTP_HEADERS_ERROR);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_httpheaders_getheader_null_fail)
{
    size_t headerCount = 1;

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).SetReturn(HTTP_HEADERS_ERROR);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
TEST_FUNCTION(uhttp_client_execute_request_string_concat_fail)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).CopyOutArgumentBuffer(3, &headerValue, sizeof(headerValue));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(__LINE__);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_ERROR, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Codes_SRS_UHTTP_07_016: [http_client_execute_request shall transmit the http headers data through a call to xio_send;]*/
TEST_FUNCTION(uhttp_client_execute_request_no_content_succeed)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t) );
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).CopyOutArgumentBuffer(3, &headerValue, sizeof(headerValue));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(STRING_length(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_GET, "/", TEST_HTTP_HEADERS_HANDLE, NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_016: [http_client_execute_request shall transmit the http headers data through a call to xio_send;]*/
/* Tests_SRS_UHTTP_07_018: [upon success http_client_execute_request shall then transmit the content data, if supplied, through a call to xio_send.] */
/* Tests_SRS_UHTTP_07_015: [http_client_execute_request shall add the Content-Length http header item to the request if the contentLength is > 0] */
TEST_FUNCTION(uhttp_client_execute_request_with_content_no_header_succeed)
{
    size_t headerCount = 0;

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(STRING_length(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, TEST_HTTP_CONTENT, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", NULL, (const unsigned char*)TEST_HTTP_CONTENT, strlen(TEST_HTTP_CONTENT));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/* Tests_SRS_UHTTP_07_016: [http_client_execute_request shall transmit the http headers data through a call to xio_send;]*/
/* Tests_SRS_UHTTP_07_018: [upon success http_client_execute_request shall then transmit the content data, if supplied, through a call to xio_send.] */
/* Tests_SRS_UHTTP_07_015: [http_client_execute_request shall add the Content-Length http header item to the request if the contentLength is > 0] */
TEST_FUNCTION(uhttp_client_execute_request_with_content_succeed)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).CopyOutArgumentBuffer(3, &headerValue, sizeof(headerValue));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(STRING_length(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, TEST_HTTP_CONTENT, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_execute_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE, (const unsigned char*)TEST_HTTP_CONTENT, strlen(TEST_HTTP_CONTENT) );

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_start_chunk_request_http_client_handle_null_fail)
{
    // arrange

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(NULL, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
}

TEST_FUNCTION(uhttp_client_start_chunk_request_relativePath_null_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, NULL, TEST_HTTP_HEADERS_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_start_chunk_request_httpheaderhandle_null_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

/*TEST_FUNCTION(uhttp_client_start_chunk_request_xio_send_fail)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).CopyOutArgumentBuffer(3, &headerValue, sizeof(headerValue));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(STRING_length(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, 4, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(4).IgnoreArgument(5).SetReturn(__LINE__);
    //EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, TEST_HTTP_CONTENT, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    //EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE, (const unsigned char*)TEST_HTTP_CONTENT, strlen(TEST_HTTP_CONTENT));

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_SEND_REQUEST_FAILED, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}*/

TEST_FUNCTION(uhttp_client_start_chunk_request_httpheader_alloc_null_fail)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_HTTP_HEADERS_FAILED, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_start_chunk_request_buffer_new_null_fail)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new());
    STRICT_EXPECTED_CALL(HTTPHeaders_Free(TEST_HTTP_HEADERS_HANDLE));
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_HTTP_HEADERS_FAILED, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_start_chunk_request_succeed)
{
    size_t headerCount = 1;
    // Will be freed in the ConstructHttpHeader function
    size_t len = strlen(TEST_HEADER_STRING);
    char* headerValue = (char*)malloc(len+1);
    strcpy(headerValue, TEST_HEADER_STRING);

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));
    STRICT_EXPECTED_CALL(HTTPHeaders_GetHeader(TEST_HTTP_HEADERS_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).CopyOutArgumentBuffer(3, &headerValue, sizeof(headerValue));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_concat(TEST_STRING_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(STRING_length(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, TEST_HTTP_CONTENT, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_send_chunk_request_http_client_handle_fail)
{
    // arrange
 
    // act
    HTTP_CLIENT_RESULT httpResult = http_client_send_chunk_request(NULL, TEST_HTTP_CONTENT_CHUNK, TEST_HTTP_CONTENT_CHUNK_LEN);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
}

TEST_FUNCTION(uhttp_client_send_chunk_request_content_null_fail)
{
    size_t headerCount = 0;

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_send_chunk_request(clientHandle, NULL, TEST_HTTP_CONTENT_CHUNK_LEN);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_send_chunk_request_chunk_len_fail)
{
    size_t headerCount = 0;

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_send_chunk_request(clientHandle, TEST_HTTP_CONTENT_CHUNK, 0);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_send_chunk_request_invalid_chunk_fail)
{
    size_t headerCount = 0;

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_send_chunk_request(clientHandle, TEST_HTTP_CONTENT_CHUNK, TEST_HTTP_CONTENT_CHUNK_LEN);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_CHUNK_REQUEST, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_send_chunk_request_succeed)
{
    size_t headerCount = 0;

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    (void)http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_send_chunk_request(clientHandle, TEST_HTTP_CONTENT_CHUNK, TEST_HTTP_CONTENT_CHUNK_LEN);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_end_chunk_request_http_client_handle_fail)
{
    // arrange

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_end_chunk_request(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_ARG, httpResult);

    // Cleanup
}

TEST_FUNCTION(uhttp_client_end_chunk_request_invalid_chunk_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_end_chunk_request(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_INVALID_CHUNK_REQUEST, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_end_chunk_request_succeed)
{
    size_t headerCount = 0;

    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_HANDLE);
    EXPECTED_CALL(HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(2, &headerCount, sizeof(size_t));

    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    (void)http_client_start_chunk_request(clientHandle, HTTP_CLIENT_REQUEST_POST, "/", TEST_HTTP_HEADERS_HANDLE);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_send(TEST_XIO_HANDLE, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    HTTP_CLIENT_RESULT httpResult = http_client_end_chunk_request(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(HTTP_CLIENT_RESULT, HTTP_CLIENT_OK, httpResult);

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_dowork_http_client_handle_null_fail)
{
    // arrange

    // act
    http_client_dowork(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
}

TEST_FUNCTION(uhttp_client_dowork_succeed)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();

    EXPECTED_CALL(xio_dowork(TEST_XIO_HANDLE));

    // act
    http_client_dowork(clientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

static void SetupProcessHeader()
{
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(TEST_HTTP_HEADERS_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3);
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
}

TEST_FUNCTION(uhttp_client_onBytesReceived_succeed)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, (void*)TEST_HTTP_EXAMPLE, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();
    ASSERT_IS_NOT_NULL(g_onBytesRecv);

    // act
    STRICT_EXPECTED_CALL(BUFFER_new()).SetReturn(TEST_BUFFER_HANDLE);
    STRICT_EXPECTED_CALL(HTTPHeaders_Alloc()).SetReturn(TEST_HTTP_HEADERS_HANDLE);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    SetupProcessHeader();
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    SetupProcessHeader();
    SetupProcessHeader();
    SetupProcessHeader();
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    SetupProcessHeader();
    SetupProcessHeader();
    SetupProcessHeader();
    SetupProcessHeader();
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
    SetupProcessHeader();
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    size_t count = sizeof(TEST_HTTP_EXAMPLE)/sizeof(TEST_HTTP_EXAMPLE[0]);
    for (size_t index = 0; index < count; index++)
    {
        g_onBytesRecv(g_onBytesRecv_ctx, (const unsigned char*)TEST_HTTP_EXAMPLE[index], strlen(TEST_HTTP_EXAMPLE[index]));
    }

    // assert
    ASSERT_IS_TRUE(g_msgRecvSuccessful);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_onBytesReceived_buffer_null_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();
    ASSERT_IS_NOT_NULL(g_onBytesRecv);

    // act
    g_onBytesRecv(g_onBytesRecv_ctx, NULL, strlen(TEST_HTTP_EXAMPLE[0]));

    // assert
    ASSERT_IS_FALSE(g_msgRecvSuccessful);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

TEST_FUNCTION(uhttp_client_onBytesReceived_len_0_fail)
{
    // arrange
    HTTP_CLIENT_HANDLE clientHandle = http_client_create(TEST_XIO_HANDLE, OnConnectedCallback, OnMsgRecvCb, OnErrorCB, OnChunkReply, NULL, NULL);
    (void)http_client_open(clientHandle, TEST_HOST_NAME);
    umock_c_reset_all_calls();
    ASSERT_IS_NOT_NULL(g_onBytesRecv);

    // act
    g_onBytesRecv(g_onBytesRecv_ctx, (const unsigned char*)TEST_HTTP_EXAMPLE[0], 0);

    // assert
    ASSERT_IS_FALSE(g_msgRecvSuccessful);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // Cleanup
    http_client_close(clientHandle);
    http_client_destroy(clientHandle);
}

END_TEST_SUITE(uhttp_unittests)
