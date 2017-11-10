// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include "testrunnerswitcher.h"

#undef DECLSPEC_IMPORT

#pragma warning(disable: 4273)

#include <winsock2.h>
#include <mstcpip.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "azure_c_shared_utility/socketio.h"

#define ENABLE_MOCKS

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "azure_c_shared_utility/list.h"

static bool g_addrinfo_call_fail;
//static int g_socket_send_size_value;
static int g_socket_recv_size_value;

static const LIST_HANDLE TEST_LIST_HANDLE = (LIST_HANDLE)0x4242;
static const LIST_ITEM_HANDLE TEST_LIST_ITEM_HANDLE = (LIST_ITEM_HANDLE)0x11;
static const void** list_items = NULL;
static size_t list_item_count = 0;
static SOCKET test_socket = (SOCKET)0x4243;
static size_t list_head_count = 0;
static int PORT_NUM = 80;
static bool list_add_called = false;
static const char* HOSTNAME_ARG = "hostname";
static size_t callbackContext = 11;
static const struct sockaddr test_sock_addr = { 0 };
static ADDRINFO TEST_ADDR_INFO = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)&test_sock_addr, NULL };

static const char* TEST_BUFFER_VALUE = "test_buffer_value";
#define TEST_BUFFER_SIZE    17
#define TEST_CALLBACK_CONTEXT   0x951753

static struct tcp_keepalive persisted_tcp_keepalive;

MOCK_FUNCTION_WITH_CODE(WSAAPI, SOCKET, socket, int, af, int, type, int, protocol)
MOCK_FUNCTION_END(test_socket)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, closesocket, SOCKET, s)
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, connect, SOCKET, s, const struct sockaddr*, name, int, namelen)
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, recv, SOCKET, s, char*, buf, int, len, int, flags)
    if (g_socket_recv_size_value >= 0)
    {
        len = g_socket_recv_size_value;
    }
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, send, SOCKET, s, const char*, buf, int, len, int, flags)
/*if (g_socket_send_size_value >= 0)
{
len = g_socket_send_size_value;
}*/
MOCK_FUNCTION_END(len)
MOCK_FUNCTION_WITH_CODE(WSAAPI, INT, getaddrinfo, PCSTR, pNodeName, PCSTR, pServiceName, const ADDRINFOA*, pHints, PADDRINFOA*, ppResult)
int callFail;
if (!g_addrinfo_call_fail)
{
    *ppResult = (PADDRINFOA)malloc(sizeof(ADDRINFOA));
    memcpy(*ppResult, &TEST_ADDR_INFO, sizeof(ADDRINFOA));
    callFail = 0;
}
else
{
    *ppResult = NULL;
    callFail = __LINE__;
}
MOCK_FUNCTION_END(callFail)
MOCK_FUNCTION_WITH_CODE(WSAAPI, void, freeaddrinfo, PADDRINFOA, pResult)
if (pResult != NULL)
{
    free(pResult);
}
MOCK_FUNCTION_END()
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, WSAGetLastError)
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, ioctlsocket, SOCKET, s, long, cmd, u_long FAR*, argp)
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(WSAAPI, int, WSAIoctl, SOCKET, s, DWORD, dwIoControlCode, LPVOID, lpvInBuffer, DWORD, cbInBuffer, LPVOID, lpvOutBuffer, DWORD, cbOutBuffer, LPDWORD, lpcbBytesReturned, LPWSAOVERLAPPED, lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE, lpCompletionRoutine)
memcpy(&persisted_tcp_keepalive, lpvInBuffer, sizeof(struct tcp_keepalive));
MOCK_FUNCTION_END(0)

LIST_ITEM_HANDLE my_list_get_head_item(LIST_HANDLE list)
{
    LIST_ITEM_HANDLE listHandle = NULL;
    if (list_item_count > 0)
    {
        listHandle = (LIST_ITEM_HANDLE)list_items[0];
        list_item_count--;
    }
    return listHandle;
}

LIST_ITEM_HANDLE my_list_add(LIST_HANDLE list, const void* item)
{
    const void** items = (const void**)realloc((void*)list_items, (list_item_count + 1) * sizeof(item));
    if (items != NULL)
    {
        list_items = items;
        list_items[list_item_count++] = item;
    }
    list_add_called = true;
    return (LIST_ITEM_HANDLE)list_item_count;
}

const void* my_list_item_get_value(LIST_ITEM_HANDLE item_handle)
{
    const void* resultPtr = NULL;
    if (list_add_called)
    {
        resultPtr = item_handle;
    }

    return (const void*)resultPtr;
}

LIST_ITEM_HANDLE my_list_find(LIST_HANDLE handle, LIST_MATCH_FUNCTION match_function, const void* match_context)
{
    size_t i;
    const void* found_item = NULL;
    for (i = 0; i < list_item_count; i++)
    {
        if (match_function((LIST_ITEM_HANDLE)list_items[i], match_context))
        {
            found_item = list_items[i];
            break;
        }
    }

    return (LIST_ITEM_HANDLE)found_item;
}

void my_list_destroy(LIST_HANDLE handle)
{
    free((void*)list_items);
    list_items = NULL;
}

static void test_on_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    (void)context;
    (void)buffer;
    (void)size;
}

static void test_on_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    (void)context;
    (void)open_result;
}

static void test_on_io_close_complete(void* context)
{
    (void)context;
}

static void test_on_io_error(void* context)
{
    (void)context;
}

char* umocktypes_stringify_const_ADDRINFOA_ptr(const ADDRINFOA** value)
{
    char* result = NULL;
    char temp_buffer[256];
    int length;

    length = sprintf(temp_buffer, "{ ai_flags = %d, ai_family = %d, ai_socktype = %d, ai_protocol = %d, ai_addrlen = %u, ai_canonname = %s", (*value)->ai_flags, (*value)->ai_family, (*value)->ai_socktype, (*value)->ai_protocol, (unsigned int)((*value)->ai_addrlen), (*value)->ai_canonname);
    if (length > 0)
    {
        result = (char*)malloc(strlen(temp_buffer) + 1);
        if (result != NULL)
        {
            (void)memcpy(result, temp_buffer, strlen(temp_buffer) + 1);
        }
    }

    return result;
}

int umocktypes_are_equal_const_ADDRINFOA_ptr(const ADDRINFOA** left, const ADDRINFOA** right)
{
    int result = 1;
    if (((*left)->ai_flags != (*right)->ai_flags) ||
        ((*left)->ai_family != (*right)->ai_family) ||
        ((*left)->ai_socktype != (*right)->ai_socktype) ||
        ((*left)->ai_protocol != (*right)->ai_protocol) ||
        ((((*left)->ai_canonname == NULL) || ((*right)->ai_canonname == NULL)) && ((*left)->ai_canonname != (*right)->ai_canonname)) ||
        (strcmp((*left)->ai_canonname, (*right)->ai_canonname) != 0))
    {
        result = 0;
    }

    return result;
}

int umocktypes_copy_const_ADDRINFOA_ptr(ADDRINFOA** destination, const ADDRINFOA** source)
{
    int result;

    *destination = (ADDRINFOA*)malloc(sizeof(ADDRINFOA));
    if (*destination == NULL)
    {
        result = __LINE__;
    }
    else
    {
        (*destination)->ai_flags = (*source)->ai_flags;
        (*destination)->ai_family = (*source)->ai_family;
        (*destination)->ai_socktype = (*source)->ai_socktype;
        (*destination)->ai_protocol = (*source)->ai_protocol;
        (*destination)->ai_canonname = (*source)->ai_canonname;

        result = 0;
    }

    return result;
}

void umocktypes_free_const_ADDRINFOA_ptr(ADDRINFOA** value)
{
    free(*value);
}

char* umocktypes_stringify_const_struct_sockaddr_ptr(const struct sockaddr** value)
{
    char* result = NULL;
    char temp_buffer[256];
    int length;

    length = sprintf(temp_buffer, "{ sa_family = %u, sa_data = ... }", (unsigned int)((*value)->sa_family));
    if (length > 0)
    {
        result = (char*)malloc(strlen(temp_buffer) + 1);
        if (result != NULL)
        {
            (void)memcpy(result, temp_buffer, strlen(temp_buffer) + 1);
        }
    }

    return result;
}

int umocktypes_are_equal_const_struct_sockaddr_ptr(const struct sockaddr** left, const struct sockaddr** right)
{
    int result = 1;
    if (((*left)->sa_family != (*left)->sa_family) ||
        (memcmp((*left)->sa_data, (*right)->sa_data, sizeof((*left)->sa_data) != 0)))
    {
        result = 0;
    }

    return result;
}

int umocktypes_copy_const_struct_sockaddr_ptr(struct sockaddr** destination, const struct sockaddr** source)
{
    int result;

    *destination = (struct sockaddr*)malloc(sizeof(struct sockaddr));
    if (*destination == NULL)
    {
        result = __LINE__;
    }
    else
    {
        (*destination)->sa_family = (*source)->sa_family;
        memcpy((*destination)->sa_data, (*source)->sa_data, sizeof((*source)->sa_data));

        result = 0;
    }

    return result;
}

void umocktypes_free_const_struct_sockaddr_ptr(struct sockaddr** value)
{
    free(*value);
}

/* after this point malloc is gballoc */
#include "azure_c_shared_utility/gballoc.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(socketio_win32_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(CONCRETE_IO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LIST_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LIST_ITEM_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(SOCKET, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PCSTR, char*);
    REGISTER_TYPE(const ADDRINFOA*, const_ADDRINFOA_ptr);
    REGISTER_UMOCK_ALIAS_TYPE(PADDRINFOA, const ADDRINFOA*);
    REGISTER_UMOCK_ALIAS_TYPE(DWORD, unsigned long);
    REGISTER_UMOCK_ALIAS_TYPE(LPVOID, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LPDWORD, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LPWSAOVERLAPPED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LPWSAOVERLAPPED_COMPLETION_ROUTINE, void*);

    REGISTER_GLOBAL_MOCK_RETURN(list_remove, 0);
    REGISTER_GLOBAL_MOCK_RETURN(list_create, TEST_LIST_HANDLE);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(list_get_head_item, my_list_get_head_item);
    REGISTER_GLOBAL_MOCK_HOOK(list_add, my_list_add);
    REGISTER_GLOBAL_MOCK_HOOK(list_item_get_value, my_list_item_get_value);
    REGISTER_GLOBAL_MOCK_HOOK(list_find, my_list_find);
    REGISTER_GLOBAL_MOCK_HOOK(list_destroy, my_list_destroy);
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

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;
    list_head_count = 0;
    list_add_called = false;
    g_addrinfo_call_fail = false;
    //g_socket_send_size_value = -1;
    g_socket_recv_size_value = -1;
}

TEST_FUNCTION_CLEANUP(method_cleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

static void OnSendComplete(void* context, IO_SEND_RESULT send_result)
{
    (void)context;
    (void)send_result;
}

/* socketio_win32_create */
TEST_FUNCTION(socketio_create_io_create_parameters_NULL_fails)
{
    // arrange

    // act
    CONCRETE_IO_HANDLE ioHandle = socketio_create(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(ioHandle);
}

TEST_FUNCTION(socketio_create_list_create_fails)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(list_create()).SetReturn((LIST_HANDLE)NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };

    // act
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(ioHandle);
}

TEST_FUNCTION(socketio_create_succeeds)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(list_create());
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };

    // act
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    // assert
    ASSERT_IS_NOT_NULL(ioHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    socketio_destroy(ioHandle);
}

// socketio_win32_destroy 
TEST_FUNCTION(socketio_destroy_socket_io_NULL_succeeds)
{
    // arrange

    // act
    socketio_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(socketio_destroy_socket_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();
    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(send(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG))
        .SetReturn(0);
    EXPECTED_CALL(WSAGetLastError())
        .SetReturn(WSAEWOULDBLOCK);
    (void)socketio_send(ioHandle, TEST_BUFFER_VALUE, TEST_BUFFER_SIZE, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    umock_c_reset_all_calls();

    EXPECTED_CALL(closesocket(IGNORED_NUM_ARG));
    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(list_item_get_value(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(list_remove(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(list_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    socketio_destroy(ioHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(socketio_open_socket_io_NULL_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };

    umock_c_reset_all_calls();

    // act
    int result = socketio_open(NULL, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

TEST_FUNCTION(socketio_open_socket_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    umock_c_reset_all_calls();

    EXPECTED_CALL(socket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG))
        .SetReturn(INVALID_SOCKET);

    // act
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_open_getaddrinfo_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    umock_c_reset_all_calls();

    g_addrinfo_call_fail = true;
    EXPECTED_CALL(socket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(getaddrinfo(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &TEST_ADDR_INFO, IGNORED_PTR_ARG));
    EXPECTED_CALL(closesocket(IGNORED_NUM_ARG));

    // act
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_open_connect_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    umock_c_reset_all_calls();

    EXPECTED_CALL(socket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(getaddrinfo(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &TEST_ADDR_INFO, IGNORED_PTR_ARG));
    EXPECTED_CALL(connect(IGNORED_NUM_ARG, &test_sock_addr, IGNORED_NUM_ARG))
        .SetReturn(WSAECONNREFUSED);
    EXPECTED_CALL(closesocket(IGNORED_NUM_ARG));
    EXPECTED_CALL(freeaddrinfo(&TEST_ADDR_INFO));

    // act
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_open_ioctlsocket_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    umock_c_reset_all_calls();

    EXPECTED_CALL(socket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(getaddrinfo(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &TEST_ADDR_INFO, IGNORED_PTR_ARG));
    EXPECTED_CALL(connect(IGNORED_NUM_ARG, &test_sock_addr, IGNORED_NUM_ARG));
    EXPECTED_CALL(ioctlsocket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG))
        .SetReturn(WSAENETDOWN);
    EXPECTED_CALL(closesocket(IGNORED_NUM_ARG));
    EXPECTED_CALL(freeaddrinfo(&TEST_ADDR_INFO));

    // act
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_open_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    umock_c_reset_all_calls();

    EXPECTED_CALL(socket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(getaddrinfo(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &TEST_ADDR_INFO, IGNORED_PTR_ARG));
    EXPECTED_CALL(connect(IGNORED_NUM_ARG, &test_sock_addr, IGNORED_NUM_ARG));
    EXPECTED_CALL(ioctlsocket(IGNORED_NUM_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(freeaddrinfo(&TEST_ADDR_INFO));

    // act
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_close_socket_io_NULL_fails)
{
    // arrange
    // act
    int result = socketio_close(NULL, test_on_io_close_complete, (void*)0x4242);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

TEST_FUNCTION(socketio_close_Succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    EXPECTED_CALL(closesocket(IGNORED_NUM_ARG));

    // act
    result = socketio_close(ioHandle, test_on_io_close_complete, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_send_socket_io_fails)
{
    // arrange

    // act
    int result = socketio_send(NULL, (const void*)TEST_BUFFER_VALUE, TEST_BUFFER_SIZE, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

TEST_FUNCTION(socketio_send_buffer_NULL_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    // act
    result = socketio_send(ioHandle, NULL, TEST_BUFFER_SIZE, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_send_size_zero_fails)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    // act
    result = socketio_send(ioHandle, (const void*)TEST_BUFFER_VALUE, 0, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_send_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(send(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));

    // act
    result = socketio_send(ioHandle, (const void*)TEST_BUFFER_VALUE, TEST_BUFFER_SIZE, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_send_returns_1_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    static ADDRINFO addrInfo = { AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 128, NULL, (struct sockaddr*)0x11, NULL };

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(send(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG)).SetReturn(1);
    EXPECTED_CALL(WSAGetLastError()).SetReturn(WSAEWOULDBLOCK);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(list_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    // act
    result = socketio_send(ioHandle, (const void*)TEST_BUFFER_VALUE, TEST_BUFFER_SIZE, OnSendComplete, (void*)TEST_CALLBACK_CONTEXT);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    list_head_count = list_item_count;
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_dowork_socket_io_NULL_fails)
{
    // arrange

    // act
    socketio_dowork(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(socketio_dowork_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(recv(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(WSAGetLastError());
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    socketio_dowork(ioHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_dowork_recv_bytes_succeeds)
{
    // arrange
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);

    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);

    umock_c_reset_all_calls();

    EXPECTED_CALL(list_get_head_item(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(recv(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG))
        .CopyOutArgumentBuffer(2, "t", 1)
        .SetReturn(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(recv(IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(WSAGetLastError());
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    socketio_dowork(ioHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    socketio_destroy(ioHandle);
}

// socketio_setoption tests

static CONCRETE_IO_HANDLE setup_socket()
{
    SOCKETIO_CONFIG socketConfig = { HOSTNAME_ARG, PORT_NUM, NULL };
    CONCRETE_IO_HANDLE ioHandle = socketio_create(&socketConfig);
    int result = socketio_open(ioHandle, test_on_io_open_complete, &callbackContext, test_on_bytes_received, &callbackContext, test_on_io_error, &callbackContext);
    ASSERT_ARE_EQUAL(int, 0, result);
    return ioHandle;
}

static CONCRETE_IO_HANDLE setup_socket_and_expect_WSAIoctl()
{
    CONCRETE_IO_HANDLE ioHandle = setup_socket();

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(WSAIoctl(*(SOCKET*)ioHandle, SIO_KEEPALIVE_VALS, IGNORED_PTR_ARG,
        sizeof(struct tcp_keepalive), NULL, 0, IGNORED_PTR_ARG, NULL, NULL))
        .IgnoreArgument_lpvInBuffer()
        .IgnoreArgument_lpcbBytesReturned();

    memset(&persisted_tcp_keepalive, 0, sizeof(struct tcp_keepalive));

    return ioHandle;
}

static void verify_mocks_and_destroy_socket(CONCRETE_IO_HANDLE ioHandle)
{
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    socketio_destroy(ioHandle);
}

TEST_FUNCTION(socketio_setoption_fails_when_handle_is_null)
{
    // arrange
    int irrelevant = 1;

    // act
    int result = socketio_setoption(NULL, "tcp_keepalive", &irrelevant);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

TEST_FUNCTION(socketio_setoption_fails_when_option_name_is_null)
{
    // arrange
    int irrelevant = 1;

    CONCRETE_IO_HANDLE ioHandle = setup_socket();

    umock_c_reset_all_calls();

    // act
    int result = socketio_setoption(ioHandle, NULL, &irrelevant);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(socketio_setoption_fails_when_value_is_null)
{
    // arrange
    CONCRETE_IO_HANDLE ioHandle = setup_socket();

    umock_c_reset_all_calls();

    // act
    int result = socketio_setoption(ioHandle, "tcp_keepalive", NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(socketio_setoption_fails_when_it_receives_an_unsupported_option)
{
    // arrange
    int irrelevant = 1;

    CONCRETE_IO_HANDLE ioHandle = setup_socket();

    umock_c_reset_all_calls();

    // act
    int result = socketio_setoption(ioHandle, "unsupported_option_name", &irrelevant);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(calling_socketio_setoption_with_tcp_keepalive_does_not_impact_the_other_two_options)
{
    // arrange
    int irrelevant = 1;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    // act
    int result = socketio_setoption(ioHandle, "tcp_keepalive", &irrelevant);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.keepalivetime);
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.keepaliveinterval);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(calling_socketio_setoption_for_option_tcp_keepalive_time_does_not_impact_the_other_two)
{
    // arrange
    int irrelevant = 1;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    // act
    int result = socketio_setoption(ioHandle, "tcp_keepalive_time", &irrelevant);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.onoff);
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.keepaliveinterval);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(calling_socketio_setoption_for_option_tcp_keepalive_interval_does_not_impact_the_other_two)
{
    // arrange
    int irrelevant = 1;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    // act
    int result = socketio_setoption(ioHandle, "tcp_keepalive_interval", &irrelevant);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.onoff);
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.keepalivetime);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(tcp_keepalive_time_arg_to_socketio_setoption_is_converted_to_milliseconds)
{
    // arrange
    int result;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    int time = 3;

    // act
    result = socketio_setoption(ioHandle, "tcp_keepalive_time", &time);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, time * 1000, persisted_tcp_keepalive.keepalivetime);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(tcp_keepalive_interval_arg_to_socketio_setoption_is_converted_to_milliseconds)
{
    // arrange
    int result;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    int interval = 15;

    // act
    result = socketio_setoption(ioHandle, "tcp_keepalive_interval", &interval);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, interval * 1000, persisted_tcp_keepalive.keepaliveinterval);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(tcp_keepalive_arg_is_not_modified_by_socketio_setoption)
{
    // arrange
    int result;
    CONCRETE_IO_HANDLE ioHandle = setup_socket_and_expect_WSAIoctl();

    int onoff = -42;

    // act
    result = socketio_setoption(ioHandle, "tcp_keepalive", &onoff);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, onoff, persisted_tcp_keepalive.onoff);

    verify_mocks_and_destroy_socket(ioHandle);
}

TEST_FUNCTION(socketio_setoption_does_not_persist_keepalive_values_if_WSAIoctl_fails)
{
    // arrange
    int irrelevant = 1;

    CONCRETE_IO_HANDLE ioHandle = setup_socket();

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(WSAIoctl(*(SOCKET*)ioHandle, SIO_KEEPALIVE_VALS, IGNORED_PTR_ARG,
        sizeof(struct tcp_keepalive), NULL, 0, IGNORED_PTR_ARG, NULL, NULL))
        .IgnoreArgument_lpvInBuffer()
        .IgnoreArgument_lpcbBytesReturned()
        .SetReturn(1); // 1st call fails, keepalive changes should be discarded
    STRICT_EXPECTED_CALL(WSAIoctl(*(SOCKET*)ioHandle, SIO_KEEPALIVE_VALS, IGNORED_PTR_ARG,
        sizeof(struct tcp_keepalive), NULL, 0, IGNORED_PTR_ARG, NULL, NULL))
        .IgnoreArgument_lpvInBuffer()
        .IgnoreArgument_lpcbBytesReturned()
        .SetReturn(0); // purpose of 2nd call is just to see the keepalive state after the 1st call

    memset(&persisted_tcp_keepalive, 0, sizeof(struct tcp_keepalive));

    // act
    int result = socketio_setoption(ioHandle, "tcp_keepalive", &irrelevant);
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    result = socketio_setoption(ioHandle, "tcp_keepalive_time", &irrelevant); // use different option for 2nd call so we don't overwrite the value from the 1st
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, 0, persisted_tcp_keepalive.onoff);

    verify_mocks_and_destroy_socket(ioHandle);
}

END_TEST_SUITE(socketio_win32_unittests)
