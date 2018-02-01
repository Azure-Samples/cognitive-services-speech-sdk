// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"
#include <stddef.h>
#include <assert.h>
#include "windows.h"
#include "winhttp.h"
#include <string.h>
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/threadapi.h"

#define TEMP_BUFFER_SIZE 1024
#define MESSAGE_BUFFER_SIZE 260
#define USE_HTTP_WINHTTP

#define LogErrorWinHTTPWithGetLastErrorAsString(FORMAT, ...) { \
    DWORD errorMessageID = GetLastError(); \
    LogError(FORMAT, __VA_ARGS__); \
    CHAR messageBuffer[MESSAGE_BUFFER_SIZE]; \
    if (errorMessageID == 0) \
    {\
        LogError("GetLastError() returned 0. Make sure you are calling this right after the code that failed. "); \
    } \
    else\
    {\
        int size = FormatMessageA(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, \
            GetModuleHandleA("WinHttp"), errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, MESSAGE_BUFFER_SIZE, NULL); \
        if (size == 0)\
        {\
            size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, MESSAGE_BUFFER_SIZE, NULL); \
        }\
        if (size == 0)\
        {\
            LogError("GetLastError Code: %d. ", errorMessageID); \
        }\
        else\
        {\
            LogError("GetLastError Code: %u, Message: %s.", errorMessageID, messageBuffer); \
        }\
    }\
}

#if defined(USE_HTTP_WINHTTP)
DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES)
#endif

typedef enum HTTPAPI_STATE_TAG
{
    HTTPAPI_NOT_INITIALIZED,
    HTTPAPI_INITIALIZED
} HTTPAPI_STATE;

typedef struct HTTP_HANDLE_DATA_TAG
{
    HINTERNET ConnectionHandle;
    unsigned int timeout;
} HTTP_HANDLE_DATA;

static HTTPAPI_STATE g_HTTPAPIState = HTTPAPI_NOT_INITIALIZED;

/*There's a global SessionHandle for all the connections*/
static HINTERNET g_SessionHandle;
static size_t nUsersOfHTTPAPI = 0; /*used for reference counting (a weak one)*/

/*returns NULL if it failed to construct the headers*/
static char* ConstructHeadersString(HTTP_HEADERS_HANDLE httpHeadersHandle, bool addChunk)
{
    char* result;
    size_t headersCount;
    const char kChunk[] = "Transfer-Encoding:chunked\r\n";

    if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = NULL;
        LogError("HTTPHeaders_GetHeaderCount failed.");
    }
    else
    {
        size_t i;
        
        /*the total size of all the headers is given by sumof(lengthof(everyheader)+2)*/
        size_t toAlloc = 0;

        if (addChunk)
        {
            toAlloc += (sizeof(kChunk) - 1);
        }

        for (i = 0; i < headersCount; i++)
        {
            char *temp;
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) == HTTP_HEADERS_OK)
            {
                toAlloc += strlen(temp);
                toAlloc += 2;
                free(temp);
            }
            else
            {
                LogError("HTTPHeaders_GetHeader failed");
                break;
            }
        }

        if (i < headersCount)
        {
            result = NULL;
        }
        else
        {
            toAlloc++; // null terminate
            result = malloc(toAlloc);
            
            if (result == NULL)
            {
                LogError("unable to malloc");
                /*let it be returned*/
            }
            else
            {
                {
                    result[0] = '\0';
                }

                for (i = 0; i < headersCount; i++)
                {
                    char* temp;
                    if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &temp) != HTTP_HEADERS_OK)
                    {
                        LogError("unable to HTTPHeaders_GetHeader");
                        break;
                    }
                    else
                    {
                        (void)strcat_s(result, toAlloc, temp);
                        (void)strcat_s(result, toAlloc, "\r\n");
                        free(temp);
                    }
                }
                if (addChunk)
                {
                    strcat_s(result, toAlloc, kChunk);
                }

                if (i < headersCount)
                {
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*all is good*/
                }
            }
        }
    }

    return result;
}

static wchar_t* AllocWide(const char*mbString)
{
    wchar_t* pswString;
    size_t size = MultiByteToWideChar(CP_ACP, 0, mbString, -1, NULL, 0);
    if (size == 0)
    {
        LogError("MultiByteToWideChar failed");
        return NULL;
    }

    pswString = malloc(sizeof(wchar_t)*size);
    if (pswString == NULL)
    {
        LogError("malloc failed");
        return NULL;
    }

    if (MultiByteToWideChar(CP_ACP, 0, mbString, -1, pswString, (int)size) == 0)
    {
        LogError("MultiByteToWideChar failed");
        free(pswString);
        return NULL;
    }

    return pswString;
}

#if defined(USE_HTTP_WINHTTP)
HTTPAPI_RESULT HTTPAPI_Init(void)
{
    HTTPAPI_RESULT result;

    if (nUsersOfHTTPAPI == 0)
    {
        if ((g_SessionHandle = WinHttpOpen(
            NULL,
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0)) == NULL)
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpen failed.");
            result = HTTPAPI_INIT_FAILED;
        }
        else
        {
            nUsersOfHTTPAPI++;
            g_HTTPAPIState = HTTPAPI_INITIALIZED;
            result = HTTPAPI_OK;
        }
    }
    else
    {
        nUsersOfHTTPAPI++;
        result = HTTPAPI_OK;
    }

    return result;
}

void HTTPAPI_Deinit(void)
{
    if (nUsersOfHTTPAPI > 0)
    {
        nUsersOfHTTPAPI--;
        if (nUsersOfHTTPAPI == 0)
        {
            if (g_SessionHandle != NULL)
            {
                (void)WinHttpCloseHandle(g_SessionHandle);
                g_SessionHandle = NULL;
                g_HTTPAPIState = HTTPAPI_NOT_INITIALIZED;
            }
        }
    }

    
}

static HTTP_HANDLE CreateConnection(const char* hostName, INTERNET_PORT port)
{
    HTTP_HANDLE_DATA* result;
    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
        result = NULL;
    }
    else
    {
        result = (HTTP_HANDLE_DATA*)malloc(sizeof(HTTP_HANDLE_DATA));
        if (result == NULL)
        {
            LogError("malloc returned NULL.");
        }
        else
        {
            wchar_t* hostNameTemp = AllocWide(hostName);
            if (hostNameTemp == NULL)
            {
                LogError("MultiByteToWideChar failed");
                free(result);
                result = NULL;
            }
            else
            {
                result->ConnectionHandle = WinHttpConnect(
                    g_SessionHandle,
                    hostNameTemp,
                    port,
                    0);

                if (result->ConnectionHandle == NULL)
                {
                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpConnect returned NULL.");
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->timeout = 30000; /*default from MSDN (  https://msdn.microsoft.com/en-us/library/windows/desktop/aa384116(v=vs.85).aspx  )*/
                }
                free(hostNameTemp);
            }
        }
    }

    return (HTTP_HANDLE)result;
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    return CreateConnection(hostName, 443);
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
    }
    else
    {
        HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;

        if (handleData != NULL)
        {
            if (handleData->ConnectionHandle != NULL)
            {
                (void)WinHttpCloseHandle(handleData->ConnectionHandle);
                handleData->ConnectionHandle = NULL;
            }
            free(handleData);
        }
    }
}

static HTTPAPI_RESULT QueryHeaders(
    HINTERNET requestHandle,
    HTTP_HEADERS_HANDLE responseHeadersHandle
    )
{
    wchar_t* responseHeadersTemp;
    DWORD responseHeadersTempLength = sizeof(responseHeadersTemp);
    HTTPAPI_RESULT result;

    (void)WinHttpQueryHeaders(
        requestHandle,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER,
        &responseHeadersTempLength,
        WINHTTP_NO_HEADER_INDEX);

    responseHeadersTemp = (wchar_t*)malloc(responseHeadersTempLength + 2);
    if (responseHeadersTemp == NULL)
    {
        result = HTTPAPI_ALLOC_FAILED;
        LogError("malloc failed: (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    else
    {
        result = HTTPAPI_OK;

        if (WinHttpQueryHeaders(
            requestHandle,
            WINHTTP_QUERY_RAW_HEADERS_CRLF,
            WINHTTP_HEADER_NAME_BY_INDEX,
            responseHeadersTemp,
            &responseHeadersTempLength,
            WINHTTP_NO_HEADER_INDEX))
        {
            wchar_t *next_token;
            wchar_t* token = wcstok_s(responseHeadersTemp, L"\r\n", &next_token);
            while ((token != NULL) &&
                (token[0] != L'\0'))
            {
                char* tokenTemp;
                size_t tokenTemp_size;

                tokenTemp_size = WideCharToMultiByte(CP_ACP, 0, token, -1, NULL, 0, NULL, NULL);
                if (tokenTemp_size == 0)
                {
                    LogError("WideCharToMultiByte failed");
                }
                else
                {
                    tokenTemp = malloc(sizeof(char)*tokenTemp_size);
                    if (tokenTemp == NULL)
                    {
                        LogError("malloc failed");
                    }
                    else
                    {
                        if (WideCharToMultiByte(CP_ACP, 0, token, -1, tokenTemp, (int)tokenTemp_size, NULL, NULL) > 0)
                        {
                            /*breaking the token in 2 parts: everything before the first ":" and everything after the first ":"*/
                            /* if there is no such character, then skip it*/
                            /*if there is a : then replace is by a '\0' and so it breaks the original string in name and value*/

                            char* whereIsColon = strchr(tokenTemp, ':');
                            if (whereIsColon != NULL)
                            {
                                *whereIsColon = '\0';
                                if (HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, tokenTemp, whereIsColon + 1) != HTTP_HEADERS_OK)
                                {
                                    LogError("HTTPHeaders_AddHeaderNameValuePair failed");
                                    result = HTTPAPI_HTTP_HEADERS_FAILED;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            LogError("WideCharToMultiByte failed");
                        }
                        free(tokenTemp);
                    }
                }


                token = wcstok_s(NULL, L"\r\n", &next_token);
            }
        }
        else
        {
            LogError("WinHttpQueryHeaders failed");
        }

        free(responseHeadersTemp);
    }

    return result;
}

HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content,
    size_t contentLength, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPI_RESULT result;
    if (g_HTTPAPIState != HTTPAPI_INITIALIZED)
    {
        LogError("g_HTTPAPIState not HTTPAPI_INITIALIZED");
        result = HTTPAPI_NOT_INIT;
    }
    else
    {
        HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;

        if ((handleData == NULL) ||
            (relativePath == NULL) ||
            (httpHeadersHandle == NULL))
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("NULL parameter detected (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        }
        else
        {
            wchar_t* requestTypeString = NULL;

            switch (requestType)
            {
            default:
                break;

            case HTTPAPI_REQUEST_GET:
                requestTypeString = L"GET";
                break;

            case HTTPAPI_REQUEST_POST:
                requestTypeString = L"POST";
                break;

            case HTTPAPI_REQUEST_PUT:
                requestTypeString = L"PUT";
                break;

            case HTTPAPI_REQUEST_DELETE:
                requestTypeString = L"DELETE";
                break;

            case HTTPAPI_REQUEST_PATCH:
                requestTypeString = L"PATCH";
                break;
            }

            if (requestTypeString == NULL)
            {
                result = HTTPAPI_INVALID_ARG;
                LogError("requestTypeString was NULL (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else
            {
                const char* headers2;
                headers2 = ConstructHeadersString(httpHeadersHandle, false);
                if (headers2 != NULL)
                {
                    wchar_t* relativePathTemp = AllocWide(relativePath);
                    result = HTTPAPI_OK; /*legacy code*/

                    if (relativePathTemp == NULL)
                    {
                        result = HTTPAPI_ALLOC_FAILED;
                        LogError("malloc failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    }
                    else
                    {
                        {
                            wchar_t* headersTemp = AllocWide(headers2);
                            if (headersTemp == NULL)
                            {
                                result = HTTPAPI_STRING_PROCESSING_ERROR;
                                LogError("MultiByteToWideChar was 0. (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                            }
                            else
                            {
                                HINTERNET requestHandle = WinHttpOpenRequest(
                                    handleData->ConnectionHandle,
                                    requestTypeString,
                                    relativePathTemp,
                                    NULL,
                                    WINHTTP_NO_REFERER,
                                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                                    WINHTTP_FLAG_SECURE);
                                if (requestHandle == NULL)
                                {
                                    result = HTTPAPI_OPEN_REQUEST_FAILED;
                                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = %s).", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                }
                                else
                                {
                                    if (WinHttpSetTimeouts(requestHandle,
                                        0,                      /*_In_  int dwResolveTimeout - The initial value is zero, meaning no time-out (infinite). */
                                        60000,                  /*_In_  int dwConnectTimeout, -  The initial value is 60,000 (60 seconds).*/
                                        handleData->timeout,    /*_In_  int dwSendTimeout, -  The initial value is 30,000 (30 seconds).*/
                                        handleData->timeout     /* int dwReceiveTimeout The initial value is 30,000 (30 seconds).*/
                                        ) == FALSE)
                                    {
                                        result = HTTPAPI_SET_TIMEOUTS_FAILED;
                                        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                    }
                                    else
                                    {
                                        DWORD dwSecurityFlags = 0;

                                        if (!WinHttpSetOption(
                                            requestHandle,
                                            WINHTTP_OPTION_SECURITY_FLAGS,
                                            &dwSecurityFlags,
                                            sizeof(dwSecurityFlags)))
                                        {
                                            result = HTTPAPI_SET_OPTION_FAILED;
                                            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption failed (result = %s).", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                        }
                                        else
                                        {
                                            if (!WinHttpSendRequest(
                                                requestHandle,
                                                headersTemp,
                                                (DWORD)-1L, /*An unsigned long integer value that contains the length, in characters, of the additional headers. If this parameter is -1L ... */
                                                (void*)content,
                                                (DWORD)contentLength,
                                                (DWORD)contentLength,
                                                0))
                                            {
                                                result = HTTPAPI_SEND_REQUEST_FAILED;
                                                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSendRequest: (result = %s).", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                            }
                                            else
                                            {
                                                if (!WinHttpReceiveResponse(
                                                    requestHandle,
                                                    0))
                                                {
                                                    result = HTTPAPI_RECEIVE_RESPONSE_FAILED;
                                                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReceiveResponse: (result = %s).", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                }
                                                else
                                                {
                                                    DWORD dwStatusCode = 0;
                                                    DWORD dwBufferLength = sizeof(DWORD);
                                                    DWORD responseBytesAvailable;

                                                    if (!WinHttpQueryHeaders(
                                                        requestHandle,
                                                        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                                        WINHTTP_HEADER_NAME_BY_INDEX,
                                                        &dwStatusCode,
                                                        &dwBufferLength,
                                                        WINHTTP_NO_HEADER_INDEX))
                                                    {
                                                        result = HTTPAPI_QUERY_HEADERS_FAILED;
                                                        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryHeaders failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                    }
                                                    else
                                                    {
                                                        BUFFER_HANDLE useToReadAllResponse = (responseContent != NULL) ? responseContent : BUFFER_new();

                                                        if (statusCode != NULL)
                                                        {
                                                            *statusCode = dwStatusCode;
                                                        }

                                                        if (useToReadAllResponse == NULL)
                                                        {
                                                            result = HTTPAPI_ERROR;
                                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                        }
                                                        else
                                                        {

                                                            int goOnAndReadEverything = 1;
                                                            do
                                                            {
                                                                /*from MSDN: If no data is available and the end of the file has not been reached, one of two things happens. If the session is synchronous, the request waits until data becomes available.*/
                                                                if (!WinHttpQueryDataAvailable(requestHandle, &responseBytesAvailable))
                                                                {
                                                                    result = HTTPAPI_QUERY_DATA_AVAILABLE_FAILED;
                                                                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed (result = %s).", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                    goOnAndReadEverything = 0;
                                                                }
                                                                else if (responseBytesAvailable == 0)
                                                                {
                                                                    /*end of the stream, go out*/
                                                                    result = HTTPAPI_OK;
                                                                    goOnAndReadEverything = 0;
                                                                }
                                                                else
                                                                {
                                                                    if (BUFFER_enlarge(useToReadAllResponse, responseBytesAvailable) != 0)
                                                                    {
                                                                        result = HTTPAPI_ERROR;
                                                                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                        goOnAndReadEverything = 0;
                                                                    }
                                                                    else
                                                                    {
                                                                        /*Add the read bytes to the response buffer*/
                                                                        size_t bufferSize;
                                                                        const uint8_t* bufferContent;

                                                                        if (BUFFER_content(useToReadAllResponse, &bufferContent) != 0)
                                                                        {
                                                                            result = HTTPAPI_ERROR;
                                                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                            goOnAndReadEverything = 0;
                                                                        }
                                                                        else if (BUFFER_size(useToReadAllResponse, &bufferSize) != 0)
                                                                        {
                                                                            result = HTTPAPI_ERROR;
                                                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                            goOnAndReadEverything = 0;
                                                                        }
                                                                        else
                                                                        {
                                                                            DWORD bytesReceived;
                                                                            if (!WinHttpReadData(requestHandle, (LPVOID)(bufferContent + bufferSize - responseBytesAvailable), responseBytesAvailable, &bytesReceived))
                                                                            {
                                                                                result = HTTPAPI_READ_DATA_FAILED;
                                                                                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReadData failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                                goOnAndReadEverything = 0;
                                                                            }
                                                                            else
                                                                            {
                                                                                /*if for some reason bytesReceived is zero If you are using WinHttpReadData synchronously, and the return value is TRUE and the number of bytes read is zero, the transfer has been completed and there are no more bytes to read on the handle.*/
                                                                                if (bytesReceived == 0)
                                                                                {
                                                                                    /*end of everything, but this looks like an error still, or a non-conformance between WinHttpQueryDataAvailable and WinHttpReadData*/
                                                                                    result = HTTPAPI_READ_DATA_FAILED;
                                                                                    LogError("bytesReceived was unexpectedly zero (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                                    goOnAndReadEverything = 0;
                                                                                }
                                                                                else
                                                                                {
                                                                                    /*all is fine, keep going*/
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }

                                                            } while (goOnAndReadEverything != 0);
                                                        }
                                                    }

                                                    if (result == HTTPAPI_OK && responseHeadersHandle != NULL)
                                                    {
                                                        result = QueryHeaders(requestHandle, responseHeadersHandle);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    (void)WinHttpCloseHandle(requestHandle);
                                }
                            }
                            free(headersTemp);
                        }
                        free(relativePathTemp);
                    }
                    free((void*)headers2);
                }
                else
                {
                    result = HTTPAPI_ALLOC_FAILED; /*likely*/
                    LogError("ConstructHeadersString failed");
                }
            }
        }
    }

    return result;
}

HTTPAPI_RESULT HTTPAPI_SetOption(HTTP_HANDLE handle, const char* optionName, const void* value)
{
    HTTPAPI_RESULT result;
    if (
        (handle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to HTTPAPI_SetOption");
    }
    else
    {
        HTTP_HANDLE_DATA* httpHandleData = (HTTP_HANDLE_DATA*)handle;
        if (strcmp("timeout", optionName) == 0)
        {
            long timeout = (long)(*(unsigned int*)value);
            httpHandleData->timeout = timeout;
            result = HTTPAPI_OK;
        }
        else
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("unknown option %s", optionName);
        }
    }
    return result;
}

HTTPAPI_RESULT HTTPAPI_CloneOption(const char* optionName, const void* value, const void** savedValue)
{
    HTTPAPI_RESULT result;
    if (
        (optionName == NULL) ||
        (value == NULL) ||
        (savedValue == NULL)
        )
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("invalid argument(NULL) passed to HTTPAPI_CloneOption");
    }
    else
    {
        if (strcmp("timeout", optionName) == 0)
        {
            /*by convention value is pointing to an unsigned int */
            unsigned int* temp = malloc(sizeof(unsigned int)); /*shall be freed by HTTPAPIEX*/
            if (temp == NULL)
            {
                result = HTTPAPI_ERROR;
                LogError("malloc failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else
            {
                *temp = *(const unsigned int*)value;
                *savedValue = temp;
                result = HTTPAPI_OK;
            }
        }
        else
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("unknown option %s", optionName);
        }
    }
    return result;
}

#endif // USE_HTTP_WINHTTP

///////////////////////////////////////////////////////////////////////////////

typedef struct PENDING_SOCKET_IO_TAG
{
    uint8_t* bytes;
    size_t size;
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
    LIST_HANDLE pending_io_list;
    int flags;
} PENDING_SOCKET_IO;

typedef struct _ASYNCHTTPREQUEST
{
    HINTERNET SessionHandle;
    HINTERNET ConnectionHandle;
    HINTERNET requestHandle;
    LIST_HANDLE pending_io_list;
    PENDING_SOCKET_IO* pending_socket_io;
    PENDING_SOCKET_IO* completed_socket_io;
    BUFFER_HANDLE rxbuff;
    size_t rxsize;
    DWORD dwStatusCode;
    bool done;
} ASYNCHTTPREQUEST;

static int AsyncHttpInit(
    ASYNCHTTPREQUEST *pRequest)
{
    assert(pRequest->rxbuff == NULL);
    pRequest->rxbuff = BUFFER_new();
    if (NULL == pRequest->rxbuff)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("BUFFER_new() failed.");
        return HTTPAPI_ALLOC_FAILED;
    }

    assert(pRequest->pending_io_list == NULL);
    pRequest->pending_io_list = list_create();
    if (pRequest->pending_io_list == NULL)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("list_create() failed (result = HTTPAPI_ALLOC_FAILED).");
        return HTTPAPI_ALLOC_FAILED;
    }

    pRequest->rxsize = 0;

    return 0;
}

static int AsyncHttpCloseRequest(
    ASYNCHTTPREQUEST *pRequest)
{
    if (NULL != pRequest->requestHandle)
    {
        (void)WinHttpCloseHandle(pRequest->requestHandle);
        LogInfo("Closed http request handle: 0x%x.", pRequest->requestHandle);
        pRequest->requestHandle = NULL;
    }

    return 0;
}

static int AsyncHttpCloseConnection(
    ASYNCHTTPREQUEST *pRequest)
{
    int result;

    result = AsyncHttpCloseRequest(pRequest);

    if (NULL != pRequest->ConnectionHandle)
    {
        (void)WinHttpCloseHandle(pRequest->ConnectionHandle);
        pRequest->ConnectionHandle = NULL;
    }

    if (NULL != pRequest->SessionHandle)
    {
        (void)WinHttpCloseHandle(pRequest->SessionHandle);
        pRequest->SessionHandle = NULL;
    }

    return result;
}

// process any completed IO.  Can only be called from dowork()
static void http_asyncio_completeio(
    ASYNCHTTPREQUEST* data)
{
    PENDING_SOCKET_IO *completed_socket_io;

    completed_socket_io = data->completed_socket_io;
    data->completed_socket_io = NULL;

    if (completed_socket_io)
    {
        if (completed_socket_io->on_send_complete)
        {
            completed_socket_io->on_send_complete(completed_socket_io->callback_context, IO_SEND_OK);
        }
        free(completed_socket_io->bytes);
        free(completed_socket_io);
    }
}

static void http_asyncio_clearpending(
    ASYNCHTTPREQUEST* data)
{
    LIST_ITEM_HANDLE item;
    do
    {
        item = list_get_head_item(data->pending_io_list);
        if (item)
        {
            data->completed_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(item);
            list_remove(data->pending_io_list, item);
        }

        http_asyncio_completeio(data);

    } while (item);
}

// move pending to completed. Can only be called from WinHTTP Callbacks
static void http_asyncio_completepending(
    ASYNCHTTPREQUEST* data)
{
    PENDING_SOCKET_IO *completed_socket_io = data->pending_socket_io;

    data->pending_socket_io = NULL;

    data->completed_socket_io = completed_socket_io;
}

static void AsyncHttpDestroy(
    ASYNCHTTPREQUEST *pRequest)
{
    AsyncHttpCloseConnection(pRequest);
    if (pRequest->rxbuff)
    {
        BUFFER_delete(pRequest->rxbuff);
        pRequest->rxbuff = NULL;
    }

    if (pRequest->pending_io_list)
    {
        http_asyncio_completepending(pRequest);
        http_asyncio_clearpending(pRequest);
        list_destroy(pRequest->pending_io_list);
        pRequest->pending_io_list = NULL;
    }
}

static int AsyncHttpCreateConnection(
    ASYNCHTTPREQUEST *pRequest,
    const char *host
)
{
    wchar_t* wsz;

    pRequest->dwStatusCode = 0;
    pRequest->SessionHandle = WinHttpOpen(
        NULL,
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);

    if (NULL == pRequest->SessionHandle)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpen() failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    wsz = AllocWide(host);
    if (NULL == wsz)
    {
        LogError("malloc failed (result = HTTPAPI_ALLOC_FAILED)");
        return HTTPAPI_ALLOC_FAILED;
    }

    pRequest->ConnectionHandle = WinHttpConnect(
        pRequest->SessionHandle,
        wsz,
        443,
        0);
    free(wsz);
    if (NULL == pRequest->ConnectionHandle)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpConnect() failed (result = HTTPAPI_ALLOC_FAILED).");
        return HTTPAPI_ALLOC_FAILED;
    }

    return 0;
}

static int add_pending_io(ASYNCHTTPREQUEST* ws_io_instance, const uint8_t* buffer, size_t bufferSize, int flags, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    PENDING_SOCKET_IO* pending_socket_io;
    LIST_ITEM_HANDLE item;

    if (ws_io_instance->pending_io_list == NULL)
    {
        /* Codes_SRS_WSIO_01_055: [If queueing the data fails (i.e. due to insufficient memory), wsio_send shall fail and return a non-zero value.] */
        return __LINE__;
    }

    pending_socket_io = (PENDING_SOCKET_IO*)malloc(sizeof(PENDING_SOCKET_IO));
    if (pending_socket_io == NULL)
    {
        /* Codes_SRS_WSIO_01_055: [If queueing the data fails (i.e. due to insufficient memory), wsio_send shall fail and return a non-zero value.] */
        return __LINE__;
    }

    pending_socket_io->bytes = (uint8_t*)malloc(bufferSize);
    if (pending_socket_io->bytes == NULL)
    {
        /* Codes_SRS_WSIO_01_055: [If queueing the data fails (i.e. due to insufficient memory), wsio_send shall fail and return a non-zero value.] */
        free(pending_socket_io);
        return __LINE__;
    }

    pending_socket_io->flags = flags;
    pending_socket_io->size = bufferSize;
    pending_socket_io->on_send_complete = on_send_complete;
    pending_socket_io->callback_context = callback_context;
    (void)memcpy(pending_socket_io->bytes, buffer, bufferSize);

    /* Codes_SRS_WSIO_01_105: [The data and callback shall be queued by calling list_add on the list created in wsio_create.] */
    pending_socket_io->pending_io_list = ws_io_instance->pending_io_list;
    item = list_add(ws_io_instance->pending_io_list, pending_socket_io);

    if (item == NULL)
    {
        /* Codes_SRS_WSIO_01_055: [If queueing the data fails (i.e. due to insufficient memory), wsio_send shall fail and return a non-zero value.] */
        free(pending_socket_io->bytes);
        free(pending_socket_io);
        return __LINE__;
    }

    /* Codes_SRS_WSIO_01_107: [On success, wsio_send shall return 0.] */
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

typedef enum _UHTTP_STATE
{
    UHTTP_STATE_NONE = 0,
    UHTTP_REQUEST_READY,
    UHTTP_REQUEST_SENT,
    UHTTP_RESPONSE_HEADERS,
    UHTTP_RECV,
    UHTTP_RECV_COMPLETE,
} UHTTP_STATE;

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG
{
    XIO_HANDLE xioHandle;
    void* userCtx;
    ON_HTTP_CONNECTED_CALLBACK fnOnConnect;
    ON_HTTP_CHUNK_REPLY_CALLBACK fnChunkReplyCallback;
    ON_HTTP_ERROR_CALLBACK fnOnErroCb;
    ASYNCHTTPREQUEST request;
    DWORD dwContentLength;
    UHTTP_STATE state;
    HTTP_HEADERS_HANDLE respHeader;
    DWORD bytesReceived;
    bool recvCalled;
    bool shouldCallErrorCallback;
    bool calledConnect;
} HTTP_CLIENT_HANDLE_DATA;

#define CONTENTLEN_CHUNKED 0xffffffff

static int SetCallbacks(HINTERNET handle, WINHTTP_STATUS_CALLBACK pfnCB, void* context)
{
    if (!WinHttpSetOption(handle, WINHTTP_OPTION_CONTEXT_VALUE, &context, sizeof(context)))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption(WINHTTP_OPTION_CONTEXT_VALUE) failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    if (WINHTTP_INVALID_STATUS_CALLBACK == WinHttpSetStatusCallback(
        handle,
        pfnCB,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, // It is required to handle WINHTTP_CALLBACK_FALG_HANDLES as context is used in callback.
        0))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetStatusCallback failed (result = HTTPAPI_ERROR)");
        return HTTPAPI_ERROR;
    }

    return 0;
}

void http_asyncio_queuepending(
    ASYNCHTTPREQUEST* data)
{
    LIST_ITEM_HANDLE io;
    PENDING_SOCKET_IO *pending_socket_io;

    http_asyncio_completeio(data);

    if (!data->pending_socket_io)
    {
        io = list_get_head_item(data->pending_io_list);
        if (io)
        {
            pending_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(io);
            list_remove(data->pending_io_list, io);

            data->pending_socket_io = pending_socket_io;
            if (!WinHttpWriteData(data->requestHandle, pending_socket_io->bytes, (DWORD)pending_socket_io->size, NULL))
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpWriteData() failed (result = HTTP_CLIENT_SEND_REQUEST_FAILED)");
                return;
            }

        }
        else if (data->done)
        {
            data->done = false;
        }
    }
}

#if defined(USE_HTTP_WINHTTP)
HTTP_CLIENT_HANDLE http_client_create(XIO_HANDLE xioHandle, ON_HTTP_CONNECTED_CALLBACK onConnect, ON_HTTP_ERROR_CALLBACK on_http_error, ON_HTTP_CHUNK_REPLY_CALLBACK on_chunk_reply, void* callback_ctx)
{
    HTTP_CLIENT_HANDLE result = NULL;

    (void)xioHandle;

    if (on_http_error == NULL || on_chunk_reply == NULL)
    {
        /* Codes_SRS_UHTTP_07_002: [If msgCb is NULL then http_client_create shall return NULL] */
        LogError("Invalid Message receive handle value");
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(HTTP_CLIENT_HANDLE_DATA));
        if (result == NULL)
        {
            /* Codes_SRS_UHTTP_07_003: [If http_client_create encounters any error then it shall return NULL] */
            LogError("Failure allocating http_client_handle");
        }
        else
        {
            memset(result, 0, sizeof(HTTP_CLIENT_HANDLE_DATA));
            result->xioHandle = xioHandle;
            result->fnOnConnect = onConnect;
            result->fnChunkReplyCallback = on_chunk_reply;
            result->fnOnErroCb = on_http_error;
            result->userCtx = callback_ctx;
            result->state = UHTTP_STATE_NONE;
            result->shouldCallErrorCallback = false;

            if (AsyncHttpInit(&result->request))
            {
                free(result);
                result = NULL;
            }
        }
    }
    /* Codes_SRS_UHTTP_07_001: [http_client_create shall return an initialize the http client.] */
    return (HTTP_CLIENT_HANDLE)result;
}

void http_client_destroy(HTTP_CLIENT_HANDLE handle)
{
    /* Codes_SRS_UHTTP_07_004: [if hanlde is NULL then http_client_destroy shall do nothing] */
    if (handle != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        AsyncHttpDestroy(&data->request);
        if (data->xioHandle != NULL)
        {
            (void)xio_destroy(data->xioHandle); 
        }

        if (data->respHeader != NULL)
        {
            HTTPHeaders_Free(data->respHeader);
            data->respHeader = NULL;
        }

        /* Codes_SRS_UHTTP_07_005: [http_client_destroy shall free any memory that is allocated in this translation unit] */
        free(data);
        HTTPAPI_Deinit();
    }
}

HTTP_CLIENT_RESULT http_client_open(HTTP_CLIENT_HANDLE handle, const char* host)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || host == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        result = HTTP_CLIENT_INVALID_ARG;
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        result = AsyncHttpCreateConnection(
            &data->request,
            host);
        if (!result)
        {
            result = HTTP_CLIENT_OK;
        }
    }
    return result;
}

void http_client_close(HTTP_CLIENT_HANDLE handle)
{
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
    /* Codes_SRS_UHTTP_07_009: [If handle is NULL then http_client_close shall do nothing] */
    if (data != NULL)
    {
        if (data->xioHandle != NULL)
        {
            /* Codes_SRS_UHTTP_07_010: [If the xio_handle is NOT NULL http_client_close shall call xio_close to close the handle] */
            (void)xio_close(data->xioHandle, NULL, NULL);
        }

        http_asyncio_clearpending(&data->request);
        AsyncHttpCloseConnection(&data->request);
    }
}

static HTTP_CLIENT_RESULT AllocHeaders(
    HTTP_CLIENT_HANDLE_DATA* data)
{
    DWORD dwBufferLength = sizeof(DWORD);
    if (!WinHttpQueryHeaders(
        data->request.requestHandle,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &data->request.dwStatusCode,
        &dwBufferLength,
        WINHTTP_NO_HEADER_INDEX))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryHeaders failed (result = HTTPAPI_QUERY_HEADERS_FAILED)");
        return HTTPAPI_QUERY_HEADERS_FAILED;
    }

    if (!WinHttpQueryHeaders(
        data->request.requestHandle,
        WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &data->dwContentLength,
        &dwBufferLength,
        WINHTTP_NO_HEADER_INDEX))
    {
        data->dwContentLength = CONTENTLEN_CHUNKED;
    }

    if (data->respHeader)
    {
        HTTPHeaders_Free(data->respHeader);
    }

    data->respHeader = HTTPHeaders_Alloc();

    if (QueryHeaders(data->request.requestHandle, data->respHeader))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("QueryHeaders failed (result = HTTPAPI_QUERY_HEADERS_FAILED)");
        return HTTPAPI_QUERY_HEADERS_FAILED;
    }

    return HTTPAPI_OK;
}

static VOID CALLBACK HttpAsyncCB(
    IN HINTERNET hInternet,
    IN DWORD_PTR dwContext,
    IN DWORD dwInternetStatus,
    IN LPVOID lpvStatusInformation OPTIONAL,
    IN DWORD dwStatusInformationLength
)
{
    (void)hInternet;
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)dwContext;

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
    {
        DWORD *pBytesWritten = (DWORD*)lpvStatusInformation;
        if (!data->request.pending_socket_io || data->request.pending_socket_io->size < *pBytesWritten)
        {
            LogError("Invalid size");
            return;
        }

        data->request.pending_socket_io->size -= *pBytesWritten;
        if (!data->request.pending_socket_io->size)
        {
            http_asyncio_completepending(&data->request);
        }
        break;
    }
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        http_asyncio_completepending(&data->request);
        http_asyncio_completeio(&data->request);

        if (data->recvCalled)
        {
            return;
        }

        data->recvCalled = true;
        if (!WinHttpReceiveResponse(
            data->request.requestHandle,
            0))
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReceiveResponse failed.");
            return;
        }
        break;

    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    {
        WINHTTP_ASYNC_RESULT *r = (WINHTTP_ASYNC_RESULT*)lpvStatusInformation;
        if (r->dwError != ERROR_WINHTTP_INCORRECT_HANDLE_STATE)
        {
            LogError("WINHTTP_CALLBACK_STATUS_REQUEST_ERROR: %d %d\n", r->dwError, r->dwResult);
            data->shouldCallErrorCallback = true;
        }
        break;
    }

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        if (!AllocHeaders(data))
        {
            data->state = UHTTP_RESPONSE_HEADERS;
        }
        break;
    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        data->bytesReceived += dwStatusInformationLength;
        if (data->dwContentLength == CONTENTLEN_CHUNKED)
        {
            data->state = UHTTP_RECV_COMPLETE;
        }
        else if (data->bytesReceived == data->dwContentLength)
        {
            data->state = UHTTP_RECV_COMPLETE;
        }
        else if (!WinHttpQueryDataAvailable(data->request.requestHandle, NULL))
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed.");
            return;
        }
        break;
    }
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        DWORD*pdw = (DWORD*)lpvStatusInformation;

        if ((data->bytesReceived + *pdw) > BUFFER_length(data->request.rxbuff))
        {
            if (BUFFER_enlarge(data->request.rxbuff, (data->bytesReceived + *pdw) - BUFFER_length(data->request.rxbuff)))
            {
                LogError("BUFFER_enlarge failed");
                return;
            }
        }

        if (!WinHttpReadData(data->request.requestHandle, BUFFER_u_char(data->request.rxbuff) + data->bytesReceived, *pdw, NULL))
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReadData failed (result = HTTPAPI_READ_DATA_FAILED)");
            return;
        }
        break;
    }
    }
}

static HTTP_CLIENT_RESULT SendRequest(
    HTTP_CLIENT_HANDLE handle,
    HTTP_CLIENT_REQUEST_TYPE requestType,
    const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle,
    const uint8_t* content, 
    size_t contentLength,
    bool addChunk
    )
{
    BOOL fRet;
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        return HTTP_CLIENT_INVALID_ARG;
    }

    if ((relativePath == NULL) ||
        (httpHeadersHandle == NULL))
    {
        LogError("NULL parameter detected (result = HTTP_CLIENT_INVALID_ARG)");
        return HTTP_CLIENT_INVALID_ARG;
    }

    wchar_t* requestTypeString = NULL;

    switch (requestType)
    {
    default:
        LogError("requestTypeString was NULL (result = HTTP_CLIENT_INVALID_ARG)");
        return HTTP_CLIENT_INVALID_ARG;

    case HTTP_CLIENT_REQUEST_GET:
        requestTypeString = L"GET";
        break;

    case HTTP_CLIENT_REQUEST_POST:
        requestTypeString = L"POST";
        break;

    case HTTP_CLIENT_REQUEST_PUT:
        requestTypeString = L"PUT";
        break;

    case HTTP_CLIENT_REQUEST_DELETE:
        requestTypeString = L"DELETE";
        break;

    case HTTP_CLIENT_REQUEST_PATCH:
        requestTypeString = L"PATCH";
        break;

    }

    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
    LIST_ITEM_HANDLE item;
    PENDING_SOCKET_IO* pending_socket_io = NULL;

    data->recvCalled = false;
    data->state = UHTTP_STATE_NONE;
    data->request.done = !addChunk;
    data->bytesReceived = 0;
    wchar_t* relativePathTemp = AllocWide(relativePath);

    if (NULL == relativePathTemp)
    {
        LogError("malloc failed (result = HTTPAPI_ALLOC_FAILED)");
        return HTTPAPI_ALLOC_FAILED;
    }

    data->request.requestHandle = WinHttpOpenRequest(
        data->request.ConnectionHandle,
        requestTypeString,
        relativePathTemp,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    free(relativePathTemp);
    if (data->request.requestHandle == NULL)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    if (SetCallbacks(data->request.requestHandle, HttpAsyncCB, data))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption(WINHTTP_OPTION_CONTEXT_VALUE) failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    char* headers2;
    headers2 = ConstructHeadersString(httpHeadersHandle, addChunk);
    if (headers2 == NULL)
    {
        LogError("ConstructHeadersString failed (result = HTTPAPI_ALLOC_FAILED)");
        return HTTPAPI_ALLOC_FAILED;
    }

    wchar_t* headersTemp = AllocWide(headers2);
    free(headers2);

    if (headersTemp == NULL)
    {
        LogError("MultiByteToWideChar was 0. (result = HTTPAPI_STRING_PROCESSING_ERROR)");
        return HTTPAPI_STRING_PROCESSING_ERROR;
    }

    http_asyncio_completeio(&data->request);
    http_asyncio_completepending(&data->request);
    http_asyncio_clearpending(&data->request);

    if (contentLength)
    {
        add_pending_io(&data->request, content, contentLength, 0, NULL, NULL);
        item = list_get_head_item(data->request.pending_io_list);
        pending_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(item);
        list_remove(data->request.pending_io_list, item);
        content = pending_socket_io->bytes;
    }

    data->request.pending_socket_io = pending_socket_io;
    fRet = WinHttpSendRequest(
        data->request.requestHandle,
        headersTemp,
        (DWORD)wcslen(headersTemp), /*An unsigned long integer value that contains the length, in characters, of the additional headers. If this parameter is -1L ... */
        (void*)content,
        (DWORD)contentLength,
        (DWORD)contentLength,
        0);
    free(headersTemp);
    if (!fRet)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSendRequest: (result = HTTPAPI_SEND_REQUEST_FAILED).");
        return HTTPAPI_SEND_REQUEST_FAILED;
    }

    return HTTP_CLIENT_OK;
}

HTTP_CLIENT_RESULT http_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content, size_t contentLength)
{
    return SendRequest(handle, requestType, relativePath, httpHeadersHandle, content, contentLength, false);
}

HTTP_CLIENT_RESULT http_client_start_chunk_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle)
{
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
    HTTP_CLIENT_RESULT result;
    data->dwContentLength = CONTENTLEN_CHUNKED;
    result = SendRequest(handle, requestType, relativePath, httpHeadersHandle, WINHTTP_NO_REQUEST_DATA, WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH, true);
    return result;
}

static void ChunkSent(void* context, IO_SEND_RESULT send_result)
{
    (void)send_result;
    free(context);
}

HTTP_CLIENT_RESULT http_client_send_chunk_request(HTTP_CLIENT_HANDLE handle, const uint8_t* content, size_t contentLength)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        result = HTTP_CLIENT_INVALID_ARG;
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        char chunk[16];

        int len = sprintf_s(chunk, 16, "%X\r\n", (unsigned)contentLength);
        result = add_pending_io(&data->request, (uint8_t*)chunk, len, 0, NULL, NULL);
        if (0 != contentLength)
        {
            result = add_pending_io(&data->request, content, contentLength, 0, NULL, NULL);
        }

        result = add_pending_io(&data->request, (uint8_t*)"\r\n", 2, 0, NULL, NULL);
    }
    return result;
}

HTTP_CLIENT_RESULT http_client_end_chunk_request(HTTP_CLIENT_HANDLE handle)
{
    HTTP_CLIENT_RESULT result;

    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        return HTTP_CLIENT_INVALID_ARG;
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        result = http_client_send_chunk_request(handle, NULL, 0);
        data->request.done = true;
        return result;
    }
}

void http_client_dowork(HTTP_CLIENT_HANDLE handle)
{
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        if (!data->request.requestHandle)
        {
            if (!data->request.done)
            {
                if (data->fnOnErroCb)
                {
                    data->fnOnErroCb((HTTP_CLIENT_HANDLE)data, data->userCtx);
                }
            }

            return;
        }

        if (data->shouldCallErrorCallback)
        {
            data->shouldCallErrorCallback = false;
            data->calledConnect = true;
            if (data->fnOnErroCb)
            {
                data->fnOnErroCb((HTTP_CLIENT_HANDLE)data, data->userCtx);
            }
        }

        if (data->state == UHTTP_STATE_NONE)
        {
            return;
        }

        http_asyncio_queuepending(&data->request);

        switch (data->state)
        {
        case UHTTP_RECV_COMPLETE:
            {
                bool done = false;

                if (!data->calledConnect)
                {
                    data->calledConnect = true;
                    if (data->fnOnConnect)
                    {
                        data->fnOnConnect(handle, data->userCtx);
                    }
                }
                if (data->dwContentLength != CONTENTLEN_CHUNKED)
                {
                    data->fnChunkReplyCallback(handle, data->userCtx, BUFFER_u_char(data->request.rxbuff), data->bytesReceived, data->request.dwStatusCode, data->respHeader, true);
                    done = true;
                }
                else
                {
                    data->fnChunkReplyCallback(handle, data->userCtx, BUFFER_u_char(data->request.rxbuff), data->bytesReceived, data->request.dwStatusCode, data->respHeader, data->bytesReceived == 0);
                    done = data->bytesReceived == 0;
                    data->bytesReceived = 0; // reset for next chunk
                }

                data->request.done = done;
                if (done)
                {
                    data->state = UHTTP_STATE_NONE;
                    if (data->respHeader != NULL)
                    {
                        HTTPHeaders_Free(data->respHeader);
                        data->respHeader = NULL;
                    }
                }
                else
                {
                    data->state = UHTTP_REQUEST_READY;
                    if (!WinHttpQueryDataAvailable(data->request.requestHandle, NULL))
                    {
                        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed.");
                        return;
                    }
                }
            }
            break;
        case UHTTP_RESPONSE_HEADERS:
            data->state = UHTTP_RECV;
            if (!WinHttpQueryDataAvailable(data->request.requestHandle, NULL))
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed.");
                return;
            }
            break;
        }

    }
}

#endif // USE_HTTP_WINHTTP
///////////////////////////////////////////////////////////////////////////////

typedef enum IO_STATE_TAG
{
    IO_STATE_NOT_OPEN,
    IO_STATE_OPENING,
    IO_STATE_OPENED,
    IO_STATE_CONNECTING,
    IO_STATE_HEADERS,
    IO_STATE_CONNECTED,
    IO_STATE_DATA,
    IO_STATE_ERROR,
    IO_STATE_CLOSING,
    IO_STATE_CLOSED
} IO_STATE;

typedef struct _WSIO_INSTANCE
{
    ASYNCHTTPREQUEST request;
    HINTERNET wsHandle;
    ON_WSIO_BYTES_RECEIVED on_bytes_received;
    void* on_bytes_received_context;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    void* on_io_open_complete_context;
    ON_IO_ERROR on_io_error;
    void* on_io_error_context;
    IO_STATE io_state;
    int port;
    char* host;
    char* relative_path;
    HTTP_HEADERS_HANDLE hHeaders;
    WSIO_MSG_TYPE rxtype;
    LOCK_HANDLE hLock;
} WSIO_INSTANCE;

#define WS_GROW_SIZE    0x1000

static WSIO_INSTANCE* WSGetAndLock(CONCRETE_IO_HANDLE ws_io)
{
    WSIO_INSTANCE *wsio_instance = (WSIO_INSTANCE *)ws_io;
    if (wsio_instance != NULL && !wsio_instance->hLock)
    {
        // not a fully formed (or already destroyed) handle
        return NULL;
    }
    Lock(wsio_instance->hLock);
    return wsio_instance;
}

static void WSUnlock(WSIO_INSTANCE *wsio_instance)
{
    Unlock(wsio_instance->hLock);
}

// All calls to set the io_state to IO_STATE_NOT_OPEN must pass through this method.  Otherwise, the WSIO instance may
// not be correctly prepared for a later wsio_open.
//
// The lock should be held while calling this function.
static void WSClose(WSIO_INSTANCE* wsio_instance)
{
    FUNC_ENTER("Close wsio_instance=0x%x, requestHandle:0x%x", wsio_instance, wsio_instance->request.requestHandle);
    wsio_instance->io_state = IO_STATE_CLOSING;

    // must check before AsyncHttpDestroy() which will set requestHandle to NULL.
    if (wsio_instance->request.requestHandle == NULL)
    {
        LogInfo("The requestHandle is already closed, no need to wait for CLOSING callback.");
        wsio_instance->io_state = IO_STATE_CLOSED;
    }

    AsyncHttpDestroy(&wsio_instance->request);

    // Wait for the CLOSING callback from WinHTTP.
    LogInfo("Checking io_state, until it reaches IO_STATE_CLOSED. Current state: %d, wsio_instance=0x%x, requestHandle:0x%x", wsio_instance->io_state, wsio_instance, wsio_instance->request.requestHandle);
    while (wsio_instance->io_state != IO_STATE_CLOSED)
    {
        Unlock(wsio_instance->hLock);
        ThreadAPI_Sleep(20);
        Lock(wsio_instance->hLock);
    }
    LogInfo("The http request handle is closed. Current state: %d, wsio_instance=0x%x, requestHandle:0x%x", wsio_instance->io_state, wsio_instance, wsio_instance->request.requestHandle);

    wsio_instance->io_state = IO_STATE_NOT_OPEN;

    LogInfo("Set state to IO_STATE_NOT_OPEN. wsio_instance=0x%x, request_handle: 0x%x, state: %d", wsio_instance, wsio_instance->request.requestHandle, wsio_instance->io_state);

    FUNC_RETURN("");
}

static int WSQueueRead(
    WSIO_INSTANCE* wsio_instance)
{
    DWORD ret;

    wsio_instance->io_state = IO_STATE_CONNECTED;
    ret = WinHttpWebSocketReceive(wsio_instance->wsHandle,
        BUFFER_u_char(wsio_instance->request.rxbuff) + wsio_instance->request.rxsize,
        (DWORD)(BUFFER_length(wsio_instance->request.rxbuff) - wsio_instance->request.rxsize),
        NULL,
        NULL);
    if (ret)
    {
        LogError("WinHttpWebSocketReceive() failed with error %d.", ret);
        return -1;
    }

    return 0;
}

static void WSError(
    WSIO_INSTANCE* wsio_instance)
{
    wsio_instance->io_state = IO_STATE_ERROR;
}

static VOID CALLBACK WSAsyncCB(
    IN HINTERNET hInternet,
    IN DWORD_PTR dwContext,
    IN DWORD dwInternetStatus,
    IN LPVOID lpvStatusInformation OPTIONAL,
    IN DWORD dwStatusInformationLength
    )
{
    if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_HANDLE_CREATED)
    {
        // no need to handle. and there is no context returned.
        return;
    }

    WSIO_INSTANCE* wsio_instance = WSGetAndLock((CONCRETE_IO_HANDLE)dwContext);

    (void)hInternet;
    (void)dwStatusInformationLength;

    if (!wsio_instance)
    {
        return;
    }

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        if (!WinHttpReceiveResponse(
            wsio_instance->request.requestHandle,
            0))
        {
            LogErrorWinHTTPWithGetLastErrorAsString("WinHttpReceiveResponse: (result = HTTPAPI_RECEIVE_RESPONSE_FAILED).");
            WSError(wsio_instance);
        }
        break;

    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    {
        WINHTTP_ASYNC_RESULT* p = (WINHTTP_ASYNC_RESULT*)lpvStatusInformation;
        LogError("WSCB: WINHTTP_CALLBACK_STATUS_REQUEST_ERROR dwError=%d dwResult=%d\n", p->dwError, p->dwResult);
        WSError(wsio_instance);
        break;
    }

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        wsio_instance->io_state = IO_STATE_HEADERS;
        break;

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        WINHTTP_WEB_SOCKET_STATUS* pStatus = (WINHTTP_WEB_SOCKET_STATUS*)lpvStatusInformation;
        wsio_instance->request.rxsize += (size_t)pStatus->dwBytesTransferred;
        switch (pStatus->eBufferType)
        {
        default:
            LogError("Unhandled eBufferType");
            WSError(wsio_instance);
            break;
        case WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE:
            LogInfo("Socket closed by server");
            wsio_close(wsio_instance, NULL, NULL);
            WSError(wsio_instance);
            break;
        case WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE:
        case WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE:
            if (BUFFER_enlarge(wsio_instance->request.rxbuff, WS_GROW_SIZE))
            {
                LogError("BUFFER_enlarge failed");
                WSError(wsio_instance);
                return;
            }
            (void)WSQueueRead(wsio_instance);
            break;
        case WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE:
        case WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE:
            wsio_instance->rxtype = (pStatus->eBufferType == WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE) ? WSIO_MSG_TYPE_TEXT : WSIO_MSG_TYPE_BINARY;
            wsio_instance->io_state = IO_STATE_DATA;
            break;
        }
        break;
    }
    case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
    {
        DWORD *pBytesWritten = (DWORD*)lpvStatusInformation;
        if (!wsio_instance->request.pending_socket_io ||
            wsio_instance->request.pending_socket_io->size != *pBytesWritten)
        {
            LogError("Invalid size");
            WSError(wsio_instance);
            break;
        }

        http_asyncio_completepending(&wsio_instance->request);
        http_asyncio_completeio(&wsio_instance->request);
        break;
    }
    case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
    {
        // According to https://msdn.microsoft.com/en-us/library/windows/desktop/aa384090(v=vs.85).aspx, the callback needs to deal with
        // WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING notification to gracefully close the connection.
        LogInfo("Recevie WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING for instance 0x%x. Set io_state to CLOSED (was %d before)", wsio_instance, wsio_instance->io_state);
        wsio_instance->io_state = IO_STATE_CLOSED;
        break;
    }
    }

    WSUnlock(wsio_instance);
}

static int WSOpen(
    WSIO_INSTANCE* wsio_instance)
{
    wchar_t* relativePathTemp;
    int result = HTTPAPI_OK;

    // The connection should already have been created by wsio_open().
    // Not need to call AsyncHttpCreateConnection() again. Just check the sessionsHandle and connectionHandle.
    if ((wsio_instance->request.ConnectionHandle == NULL) || (wsio_instance->request.SessionHandle == NULL))
    {
        LogError("The http session or connection handle is not available.");
        return HTTPAPI_ERROR;
    }

    relativePathTemp = AllocWide(wsio_instance->relative_path);

    if (NULL == relativePathTemp)
    {
        LogError("malloc failed (result = HTTPAPI_ALLOC_FAILED)");
        return HTTPAPI_ALLOC_FAILED;
    }

    wsio_instance->request.requestHandle = WinHttpOpenRequest(
        wsio_instance->request.ConnectionHandle,
        L"GET",
        relativePathTemp,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    free(relativePathTemp);
    if (wsio_instance->request.requestHandle == NULL)
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpOpenRequest failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    if (SetCallbacks(wsio_instance->request.requestHandle, WSAsyncCB, wsio_instance))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption(WINHTTP_OPTION_CONTEXT_VALUE) failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    if (!WinHttpSetOption(wsio_instance->request.requestHandle, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSetOption(WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET) failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        return HTTPAPI_OPEN_REQUEST_FAILED;
    }

    wsio_instance->io_state = IO_STATE_CONNECTING;

    wchar_t* headersTemp = NULL;
    if (wsio_instance->hHeaders)
    {
        char* headers2;
        headers2 = ConstructHeadersString(wsio_instance->hHeaders, false);
        if (headers2 == NULL)
        {
            LogError("ConstructHeadersString failed (result = HTTPAPI_ALLOC_FAILED)");
            return HTTPAPI_ALLOC_FAILED;
        }
        headersTemp = AllocWide(headers2);
        free(headers2);
    }

    if (!WinHttpSendRequest(
        wsio_instance->request.requestHandle,
        headersTemp,
        (DWORD)(headersTemp ? wcslen(headersTemp) : 0),
        NULL,
        0,
        0,
        (DWORD_PTR)NULL))
    {
        LogErrorWinHTTPWithGetLastErrorAsString("WinHttpSendRequest failed (result = HTTPAPI_OPEN_REQUEST_FAILED).");
        result = HTTPAPI_OPEN_REQUEST_FAILED;
    }

    if (headersTemp)
    {
        free(headersTemp);
    }

    return result;
}

CONCRETE_IO_HANDLE wsio_create(void* io_create_parameters)
{
    /* Codes_SRS_WSIO_01_003: [io_create_parameters shall be used as a WSIO_CONFIG*.] */
    WSIO_CONFIG* ws_io_config = io_create_parameters;

    if ((ws_io_config == NULL) ||
        /* Codes_SRS_WSIO_01_004: [If any of the WSIO_CONFIG fields host, protocol_name or relative_path is NULL then wsio_create shall return NULL.] */
        (ws_io_config->host == NULL) ||
        (ws_io_config->relative_path == NULL))
    {
        return NULL;
    }

    /* Codes_SRS_WSIO_01_001: [wsio_create shall create an instance of a wsio and return a non-NULL handle to it.] */
    WSIO_INSTANCE* result;
    size_t hostlen;
    size_t pathlen;

    result = malloc(sizeof(WSIO_INSTANCE));
    if (result == NULL)
    {
        return NULL;
    }

    memset(result, 0, sizeof(WSIO_INSTANCE));

    do
    {
        result->hLock = Lock_Init();
        if (!result->hLock)
        {
            LogError("Could not allocate wsio lock");   
            break;
        }

        hostlen = strlen(ws_io_config->host) + 1;
        /* Codes_SRS_WSIO_01_006: [The members host, protocol_name, relative_path and trusted_ca shall be copied for later use (they are needed when the IO is opened).] */
        result->host = (char*)malloc(hostlen);
        if (result->host == NULL)
        {
            /* Codes_SRS_WSIO_01_005: [If allocating memory for the new wsio instance fails then wsio_create shall return NULL.] */
            break;
        }

        pathlen = strlen(ws_io_config->relative_path) + 1;
        result->relative_path = (char*)malloc(pathlen);
        if (result->relative_path == NULL)
        {
            /* Codes_SRS_WSIO_01_005: [If allocating memory for the new wsio instance fails then wsio_create shall return NULL.] */
            break;
        }

        (void)strcpy_s(result->host, hostlen, ws_io_config->host);
        (void)strcpy_s(result->relative_path, pathlen, ws_io_config->relative_path);
        
        return result;
    } while (0);

    // we reach here if there is an initialization error
    wsio_destroy(result);
    return NULL;
}

void wsio_destroy(CONCRETE_IO_HANDLE ws_io)
{
    FUNC_ENTER("Destroying wsio handle: 0x%x", ws_io);

    /* Codes_SRS_WSIO_01_008: [If ws_io is NULL, wsio_destroy shall do nothing.] */
    if (ws_io != NULL)
    {
        // n.b. don't use WSGetAndLock because we need to support partially 
        // constructed objects for the benefit of wsio_create()
        WSIO_INSTANCE* wsio_instance = (WSIO_INSTANCE *)ws_io;

        if (wsio_instance->hLock)
        {
            Lock(wsio_instance->hLock);
        }

        /* Codes_SRS_WSIO_01_009: [wsio_destroy shall execute a close action if the IO has already been open or an open action is already pending.] */
        (void)wsio_close(wsio_instance, NULL, NULL);

        AsyncHttpDestroy(&wsio_instance->request);

        /* Codes_SRS_WSIO_01_007: [wsio_destroy shall free all resources associated with the wsio instance.] */
        if (wsio_instance->host) 
        { 
            free(wsio_instance->host); 
        }
        if (wsio_instance->relative_path) 
        { 
            free(wsio_instance->relative_path); 
        }
        if (wsio_instance->hLock)
        {
            Unlock(wsio_instance->hLock);
            Lock_Deinit(wsio_instance->hLock);
            LogInfo("wiso->hLock released. wsio_instance: 0x%x", wsio_instance);
        }

        free(ws_io);
    }
    FUNC_RETURN("");
}

int wsio_open(CONCRETE_IO_HANDLE ws_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_WSIO_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result = 0;

    FUNC_ENTER("open the wsio handle: 0x%x", ws_io);

    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if (wsio_instance == NULL)
    {
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_WSIO_01_034: [If another open is in progress or has completed successfully (the IO is open), wsio_open shall fail and return a non-zero value without performing any connection related activities.] */
        if (wsio_instance->io_state != IO_STATE_NOT_OPEN)
        {
            result = __LINE__;
        }
        else if (AsyncHttpInit(&wsio_instance->request))
        {
            result = __LINE__;
        }
        else if (BUFFER_enlarge(wsio_instance->request.rxbuff, WS_GROW_SIZE))
        {
            result = __LINE__;
        }
        else
        {
            // rxbuff was created at 0 size in AsyncHttpInit, so its size is WS_GROW_SIZE now.
            assert(BUFFER_length(wsio_instance->request.rxbuff) == WS_GROW_SIZE);

            wsio_instance->on_bytes_received = on_bytes_received;
            wsio_instance->on_bytes_received_context = on_bytes_received_context;
            wsio_instance->on_io_open_complete = on_io_open_complete;
            wsio_instance->on_io_open_complete_context = on_io_open_complete_context;
            wsio_instance->on_io_error = on_io_error;
            wsio_instance->on_io_error_context = on_io_error_context;

            result = AsyncHttpCreateConnection(
                &wsio_instance->request,
                wsio_instance->host);
            if (!result)
            {
                wsio_instance->io_state = IO_STATE_OPENING;
            }
        }

        WSUnlock(wsio_instance);
    }

    FUNC_RETURN("result=%d", result);
    return result;
}

int wsio_close(CONCRETE_IO_HANDLE ws_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result;

    FUNC_ENTER("Closing the wsio handle: 0x%x", ws_io);

    /* Codes_SRS_WSIO_01_052: [If any of the arguments ws_io or buffer are NULL, wsio_send shall fail and return a non-zero value.] */
    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if (wsio_instance == NULL)
    {
        result = __LINE__;
    }
    else
    {
        WSClose(wsio_instance);
        WSUnlock(wsio_instance);

        if (on_io_close_complete)
        {
            on_io_close_complete(on_io_close_complete_context);
        }

        result = 0;
    }

    FUNC_RETURN("result=%d", result);
    return result;
}

/* Codes_SRS_WSIO_01_050: [wsio_send shall send the buffer bytes through the websockets connection.] */
int wsio_send(CONCRETE_IO_HANDLE ws_io, const void* buffer, size_t size, WSIO_MSG_TYPE message_type, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    /* Codes_SRS_WSIO_01_052: [If any of the arguments ws_io or buffer are NULL, wsio_send shall fail and return a non-zero value.] */
    if ((buffer == NULL) ||
        /* Codes_SRS_WSIO_01_053: [If size is zero then wsio_send shall fail and return a non-zero value.] */
        (size == 0))
    {
        return __LINE__;
    }

    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if (wsio_instance == NULL)
    {
        return __LINE__;
    }

    int result = add_pending_io(&wsio_instance->request,
        buffer,
        size,
        (int)message_type,
        on_send_complete,
        callback_context);

    WSUnlock(wsio_instance);

    return result;
}

static void OpenComplete(WSIO_INSTANCE* wsio_instance, IO_OPEN_RESULT result)
{
    if (result == IO_OPEN_OK)
    {
        wsio_instance->io_state = IO_STATE_OPENED;
    }
    else
    {
        WSClose(wsio_instance);
    }

    wsio_instance->on_io_open_complete(wsio_instance->on_io_open_complete_context, result);
}

void wsio_dowork(CONCRETE_IO_HANDLE ws_io)
{
    LIST_ITEM_HANDLE io;
    DWORD dwBufferLength;
    WINHTTP_WEB_SOCKET_BUFFER_TYPE bufferType;

    /* Codes_SRS_WSIO_01_063: [If the ws_io argument is NULL, wsio_dowork shall do nothing.] */
    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if (wsio_instance == NULL)
    {
        return;
    }

    switch (wsio_instance->io_state)
    {
        case IO_STATE_OPENING:
            if (WSOpen(wsio_instance))
            {
                LogInfo("WSOpen() failed. wsio_instance: 0x%x", wsio_instance);
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }
            wsio_instance->io_state = IO_STATE_OPENED;
            break;
        case IO_STATE_HEADERS:
            dwBufferLength = sizeof(DWORD);
            if (!WinHttpQueryHeaders(
                wsio_instance->request.requestHandle,
                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX,
                &wsio_instance->request.dwStatusCode,
                &dwBufferLength,
                WINHTTP_NO_HEADER_INDEX))
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryHeaders failed (result = HTTPAPI_QUERY_HEADERS_FAILED)");
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }
            else if (wsio_instance->request.dwStatusCode != 101)
            {
                LogErrorWinHTTPWithGetLastErrorAsString("Upgrade failed %d (result = HTTPAPI_ERROR)", wsio_instance->request.dwStatusCode);
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }
            else if (!WinHttpQueryDataAvailable(wsio_instance->request.requestHandle, &dwBufferLength))
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpQueryDataAvailable failed.");
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }

            wsio_instance->wsHandle = WinHttpWebSocketCompleteUpgrade(wsio_instance->request.requestHandle, (DWORD_PTR)NULL);
            if (NULL == wsio_instance->wsHandle)
            {
                LogErrorWinHTTPWithGetLastErrorAsString("WinHttpWebSocketCompleteUpgrade failed (result = HTTPAPI_ERROR)");
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }
            else if (SetCallbacks(wsio_instance->wsHandle, WSAsyncCB, wsio_instance))
            {
                OpenComplete(wsio_instance, IO_OPEN_ERROR);
                break;
            }

            OpenComplete(wsio_instance, IO_OPEN_OK);

            (void)WSQueueRead(wsio_instance);
            break;
        case IO_STATE_DATA:
            wsio_instance->on_bytes_received(
                wsio_instance->on_bytes_received_context,
                BUFFER_u_char(wsio_instance->request.rxbuff),
                (size_t)wsio_instance->request.rxsize,
                wsio_instance->rxtype);
            wsio_instance->request.rxsize = 0;
            (void)WSQueueRead(wsio_instance);
            break;
        case IO_STATE_ERROR:
            if (wsio_instance->on_io_error != NULL)
            {
                wsio_instance->on_io_error(wsio_instance->on_io_error_context);
            }
            WSClose(wsio_instance);
            break;
        case IO_STATE_CLOSING:
            break;
        default:
            if (wsio_instance->io_state == IO_STATE_CONNECTED && !wsio_instance->request.pending_socket_io)
            {
                io = list_get_head_item(wsio_instance->request.pending_io_list);
                if (NULL == io)
                {
                    break;
                }

                wsio_instance->request.pending_socket_io = (PENDING_SOCKET_IO*)list_item_get_value(io);
                list_remove(wsio_instance->request.pending_io_list, io);

                if (wsio_instance->request.pending_socket_io->flags == (int)WSIO_MSG_TYPE_TEXT)
                {
                    bufferType = WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE;
                }
                else
                {
                    bufferType = WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE;
                }

                if (WinHttpWebSocketSend(
                    wsio_instance->wsHandle,
                    bufferType,
                    wsio_instance->request.pending_socket_io->bytes,
                    (DWORD)wsio_instance->request.pending_socket_io->size))
                {
                    LogErrorWinHTTPWithGetLastErrorAsString("WinHttpWriteData() failed (result = HTTP_CLIENT_SEND_REQUEST_FAILED)");
                    WSError(wsio_instance);
                    break;
                }
            }
            break;
    }
    WSUnlock(wsio_instance);
}

int wsio_setoption(CONCRETE_IO_HANDLE ws_io, const char* optionName, const void* value)
{
    /* Codes_SRS_WSIO_01_052: [If any of the arguments ws_io or buffer are NULL, wsio_send shall fail and return a non-zero value.] */
    if ((optionName == NULL))
    {
        return __LINE__;
    }

    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if ((ws_io == NULL))
    {
        return __LINE__;
    }

    int result;
    if (!strcmp(optionName, "connectionheaders"))
    {
        wsio_instance->hHeaders = (HTTP_HEADERS_HANDLE)value;
        result = 0;
    }
    else
    {
        result = __LINE__;
    }

    WSUnlock(wsio_instance);
    
    return result;
}

int wsio_gethttpstatus(CONCRETE_IO_HANDLE ws_io)
{
    WSIO_INSTANCE* wsio_instance = WSGetAndLock(ws_io);
    if (!wsio_instance)
    {
        return -1;
    }

    int res = (int)wsio_instance->request.dwStatusCode;
    WSUnlock(wsio_instance);

    return res;
}
///////////////////////////////////////////////////////////////////////////////
