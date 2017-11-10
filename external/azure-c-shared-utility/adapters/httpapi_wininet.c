// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "malloc.h"
#include <stddef.h>
#include "windows.h"
#include "wininet.h"
#include <string.h>
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/strings.h"

#define TEMP_BUFFER_SIZE 1024

DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);

typedef enum HTTPAPI_STATE_TAG
{
    HTTPAPI_NOT_INITIALIZED,
    HTTPAPI_INITIALIZED
} HTTPAPI_STATE;

typedef struct HTTP_HANDLE_DATA_TAG
{
    HINTERNET SessionHandle;
    HINTERNET ConnectionHandle;
} HTTP_HANDLE_DATA;

static HTTPAPI_STATE g_HTTPAPIState;

static HTTPAPI_RESULT ConstructHeadersString(HTTP_HEADERS_HANDLE httpHeadersHandle, char* headers, size_t headersBufferSize)
{
    HTTPAPI_RESULT result;
    size_t headersCount;
    headers[0] = '\0';

    if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_HTTP_HEADERS_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    else
    {
        size_t i;

        for (i = 0; i < headersCount; i++)
        {
            char tempBuffer[TEMP_BUFFER_SIZE];
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, tempBuffer, TEMP_BUFFER_SIZE) == HTTP_HEADERS_OK)
            {
                strcat_s(headers, headersBufferSize, tempBuffer);
                strcat_s(headers, headersBufferSize, "\r\n");
            }
        }

        result = HTTPAPI_OK;
    }

    return result;
}
static size_t nUsersOfHTTPAPI = 0; /*used for reference counting (a weak one)*/

HTTPAPI_RESULT HTTPAPI_Init(void)
{
    HTTPAPI_RESULT result;

    if (nUsersOfHTTPAPI == 0)
    {
        LogInfo("HTTP_API first init.");
        /*do internetOpen here... TFS202146*/
        nUsersOfHTTPAPI++;
        g_HTTPAPIState = HTTPAPI_INITIALIZED;
        result = HTTPAPI_OK;
        LogInfo("HTTP_API has now %d users", (int)nUsersOfHTTPAPI);
    }
    else
    {
        nUsersOfHTTPAPI++;
        result = HTTPAPI_OK;
        LogInfo("HTTP_API has now %d users", (int)nUsersOfHTTPAPI);
    }

    return result;
}

void HTTPAPI_Deinit(void)
{
    if (nUsersOfHTTPAPI > 0)
    {
        nUsersOfHTTPAPI--;
        LogInfo("HTTP_API has now %d users", (int)nUsersOfHTTPAPI);
        if (nUsersOfHTTPAPI == 0)
        {
            LogInfo("Deinitializing HTTP_API");
            g_HTTPAPIState = HTTPAPI_NOT_INITIALIZED;

        }
    }
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE_DATA* result = (HTTP_HANDLE_DATA*)malloc(sizeof(HTTP_HANDLE_DATA));
    if (result == NULL)
    {
        /* error */
    }
    else
    {
        wchar_t hostNameTemp[1024];

        if (MultiByteToWideChar(CP_ACP, 0, hostName, -1, hostNameTemp, sizeof(hostNameTemp) / sizeof(hostNameTemp[0])) == 0)
        {
            /* error */
        }
        else
        {
            result->SessionHandle = InternetOpen(
                NULL,
                INTERNET_OPEN_TYPE_DIRECT,
                NULL,
                NULL,
                0);
            if (result->SessionHandle != NULL)
            {
                result->ConnectionHandle = InternetConnectW(
                    result->SessionHandle,
                    hostNameTemp,
                    INTERNET_DEFAULT_HTTPS_PORT,
                    NULL,
                    NULL,
                    INTERNET_SERVICE_HTTP,
                    0,
                    0);
            }

            if ((result->SessionHandle == NULL) ||
                (result->ConnectionHandle == NULL))
            {
                HTTPAPI_CloseConnection(result);
                result = NULL;
            }
        }
    }

    return (HTTP_HANDLE)result;
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;

    if (handleData != NULL)
    {
        if (handleData->ConnectionHandle != NULL)
        {
            (void)InternetCloseHandle(handleData->ConnectionHandle);
            handleData->ConnectionHandle = NULL;
        }
        if (handleData->SessionHandle != NULL)
        {
            (void)InternetCloseHandle(handleData->SessionHandle);
            handleData->SessionHandle = NULL;
        }
        free(handleData);
    }
}

HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content,
    size_t contentLength, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPI_RESULT result;
    HTTP_HANDLE_DATA* handleData = (HTTP_HANDLE_DATA*)handle;

    if ((handleData == NULL) ||
        (relativePath == NULL) ||
        (httpHeadersHandle == NULL))
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
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
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        }
        else
        {
            wchar_t relativePathTemp[1024];
            char headers[1024];
            wchar_t headersTemp[1024];

            result = ConstructHeadersString(httpHeadersHandle, headers, sizeof(headers));
            if (result == HTTPAPI_OK)
            {
                if (MultiByteToWideChar(CP_ACP, 0, relativePath, -1, relativePathTemp, sizeof(relativePathTemp) / sizeof(relativePathTemp[0])) == 0)
                {
                    result = HTTPAPI_STRING_PROCESSING_ERROR;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                }
                else
                {
                    if (MultiByteToWideChar(CP_ACP, 0, headers, -1, headersTemp, sizeof(headersTemp) / sizeof(headersTemp[0])) == 0)
                    {
                        result = HTTPAPI_STRING_PROCESSING_ERROR;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    }
                    else
                    {
                        PCWSTR rgpszAcceptTypes[] = { L"text/*", NULL };
                        HINTERNET requestHandle = HttpOpenRequestW(
                            handleData->ConnectionHandle,
                            requestTypeString,
                            relativePathTemp,
                            NULL,
                            NULL,
                            rgpszAcceptTypes,
                            INTERNET_FLAG_SECURE,
                            0);
                        if (requestHandle == NULL)
                        {
                            result = HTTPAPI_OPEN_REQUEST_FAILED;
                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                        }
                        else
                        {
                            unsigned long int timeout = 55000;
                            if (!InternetSetOption(
                                requestHandle,
                                INTERNET_OPTION_RECEIVE_TIMEOUT, /*Sets or retrieves an unsigned long integer value that contains the time-out value, in milliseconds, to receive a response to a request.*/
                                &timeout,
                                sizeof(timeout)))
                            {
                                result = HTTPAPI_SET_TIMEOUTS_FAILED;
                                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                            }
                            else
                            {
                                DWORD dwSecurityFlags = 0;
                                if (!InternetSetOption(
                                    requestHandle,
                                    INTERNET_OPTION_SECURITY_FLAGS,
                                    &dwSecurityFlags,
                                    sizeof(dwSecurityFlags)))
                                {
                                    result = HTTPAPI_SET_OPTION_FAILED;
                                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                }
                                else
                                {
                                    if (!HttpSendRequestW(
                                        requestHandle,
                                        headersTemp,
                                        -1,
                                        (void*)content,
                                        contentLength))
                                    {
                                        result = HTTPAPI_SEND_REQUEST_FAILED;
                                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                    }
                                    else
                                    {
                                        DWORD dwStatusCode = 0;
                                        DWORD dwBufferLength = sizeof(DWORD);
                                        DWORD responseBytesAvailable;

                                        if (responseHeadersHandle != NULL)
                                        {
                                            wchar_t responseHeadersTemp[16384];
                                            DWORD responseHeadersTempLength = sizeof(responseHeadersTemp);

                                            if (HttpQueryInfo(
                                                requestHandle,
                                                HTTP_QUERY_RAW_HEADERS_CRLF,
                                                responseHeadersTemp,
                                                &responseHeadersTempLength,
                                                0))
                                            {
                                                wchar_t *next_token;
                                                wchar_t* token = wcstok_s(responseHeadersTemp, L"\r\n", &next_token);
                                                while ((token != NULL) &&
                                                    (token[0] != L'\0'))
                                                {
                                                    char tokenTemp[1024];

                                                    if (WideCharToMultiByte(CP_ACP, 0, token, -1, tokenTemp, sizeof(tokenTemp), NULL, NULL) > 0)
                                                    {
                                                        /*breaking the token in 2 parts: everything before the first ":" and everything after the first ":"*/
                                                        /* if there is no such character, then skip it*/
                                                        /*if there is a : then replace is by a '\0' and so it breaks the original string in name and value*/

                                                        char* whereIsColon = strchr(tokenTemp, ':');
                                                        if (whereIsColon != NULL)
                                                        {
                                                            *whereIsColon = '\0';
                                                            HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, tokenTemp, whereIsColon + 1);
                                                        }
                                                    }
                                                    token = wcstok_s(NULL, L"\r\n", &next_token);
                                                }
                                            }
                                        }

                                        if (!HttpQueryInfo(
                                            requestHandle,
                                            HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                                            &dwStatusCode,
                                            &dwBufferLength,
                                            0))
                                        {
                                            result = HTTPAPI_QUERY_HEADERS_FAILED;
                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                        }
                                        else
                                        {
                                            BUFFER_HANDLE useToReadAllResponse = (responseContent != NULL) ? responseContent : BUFFER_new();
                                            /*HTTP status code (dwStatusCode) can be either ok (<HTTP_STATUS_AMBIGUOUS) or "not ok (>=HTTP_STATUS_AMBIGUOUS)*/
                                            if (useToReadAllResponse == NULL)
                                            {
                                                result = HTTPAPI_ERROR;
                                                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                            }
                                            else
                                            {
                                                int goOnAndReadEverything = 1;
                                                /*set the response code*/
                                                if (statusCode != NULL)
                                                {
                                                    *statusCode = dwStatusCode;
                                                }

                                                do
                                                {
                                                    /*from MSDN:  If there is currently no data available and the end of the file has not been reached, the request waits until data becomes available*/
                                                    if (!InternetQueryDataAvailable(requestHandle, &responseBytesAvailable, 0, 0))
                                                    {
                                                        result = HTTPAPI_QUERY_DATA_AVAILABLE_FAILED;
                                                        LogError("InternetQueryDataAvailable failed (result = %s) GetLastError = %d", ENUM_TO_STRING(HTTPAPI_RESULT, result), GetLastError());
                                                        goOnAndReadEverything = 0;
                                                    }
                                                    else if (responseBytesAvailable == 0)
                                                    {
                                                        /*end of the stream, go out*/
                                                        if (dwStatusCode >= HTTP_STATUS_AMBIGUOUS)
                                                        {
                                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                        }

                                                        result = HTTPAPI_OK;
                                                        goOnAndReadEverything = 0;
                                                    }
                                                    else
                                                    {
                                                        /*add the needed space to the buffer*/
                                                        if (BUFFER_enlarge(useToReadAllResponse, responseBytesAvailable) != 0)
                                                        {
                                                            result = HTTPAPI_ERROR;
                                                            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                            goOnAndReadEverything = 0;
                                                        }
                                                        else
                                                        {
                                                            unsigned char* bufferContent;
                                                            size_t bufferSize;
                                                            /*add the data to the buffer*/
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
                                                                if (!InternetReadFile(requestHandle, bufferContent + bufferSize - responseBytesAvailable, responseBytesAvailable, &bytesReceived))
                                                                {
                                                                    result = HTTPAPI_READ_DATA_FAILED;
                                                                    LogError("InternetReadFile failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                                                                    goOnAndReadEverything = 0;
                                                                }
                                                                else
                                                                {
                                                                    /*if for some reason bytesReceived is zero, MSDN says To ensure all data is retrieved, an application must continue to call the InternetReadFile function until the function returns TRUE and the lpdwNumberOfBytesRead parameter equals zero*/
                                                                    if (bytesReceived == 0)
                                                                    {
                                                                        /*end of everything, but this looks like an error still, or a non-conformance between InternetQueryDataAvailable and InternetReadFile*/
                                                                        result = HTTPAPI_READ_DATA_FAILED;
                                                                        LogError("InternetReadFile failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
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

                                                if (responseContent == NULL)
                                                {
                                                    BUFFER_delete(useToReadAllResponse);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            (void)InternetCloseHandle(requestHandle);
                        }
                    }
                }
            }
        }
    }

    return result;
}