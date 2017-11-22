// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "curl/curl.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/uhttp.h"

#define TEMP_BUFFER_SIZE 1024

DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);

typedef struct HTTP_HANDLE_DATA_TAG
{
    CURL* curl;
    char* hostURL;
    long timeout;
    long lowSpeedLimit;
    long lowSpeedTime;
    long forbidReuse;
    long freshConnect;
    long verbose;
    long httpVersion;
    struct curl_slist* headers;
    uint8_t* postdata;
} HTTP_HANDLE_DATA;

typedef struct HTTP_RESPONSE_CONTENT_BUFFER_TAG
{
    uint8_t* buffer;
    size_t bufferSize;
    uint8_t error;
} HTTP_RESPONSE_CONTENT_BUFFER;

static size_t nUsersOfHTTPAPI = 0; /*used for reference counting (a weak one)*/

HTTPAPI_RESULT HTTPAPI_Init(void)
{
    HTTPAPI_RESULT result;
    if (nUsersOfHTTPAPI == 0)
    {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
        {
            result = HTTPAPI_INIT_FAILED;
            LogErrorResult(HTTPAPI_RESULT, result);
        }
        else
        {
            nUsersOfHTTPAPI++;
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
            curl_global_cleanup();
        }
    }
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE_DATA* httpHandleData = NULL;

    if (hostName != NULL)
    {
        httpHandleData = (HTTP_HANDLE_DATA*)malloc(sizeof(HTTP_HANDLE_DATA));
        if (httpHandleData != NULL)
        {
            size_t hostURL_size = strlen("https://") + strlen(hostName) + 1;
            httpHandleData->hostURL = malloc(hostURL_size);
            if (httpHandleData->hostURL == NULL)
            {
                LogError("unable to malloc");
            }
            else
            {
                if ((strcpy_s(httpHandleData->hostURL, hostURL_size, "https://") == 0) &&
                    (strcat_s(httpHandleData->hostURL, hostURL_size, hostName) == 0))
                {
                    httpHandleData->curl = curl_easy_init();
                    if (httpHandleData->curl == NULL)
                    {
                        free(httpHandleData->hostURL);
                        free(httpHandleData);
                        httpHandleData = NULL;
                    }
                    else
                    {
                        httpHandleData->timeout = 242 * 1000; /*242 seconds seems like a nice enough time. Reasone for 242:
                                                                1. http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT.html says Normally, name lookups can take a considerable time and limiting operations to less than a few minutes risk aborting perfectly normal operations.
                                                                2. 256KB of data... at 9600 bps transfers in about 218 seconds. Add to that a buffer of 10%... round it up to 242 :)*/
                        httpHandleData->lowSpeedTime = 0;
                        httpHandleData->lowSpeedLimit = 0;
                        httpHandleData->forbidReuse = 0;
                        httpHandleData->freshConnect = 0;
                        httpHandleData->verbose = 0;
                        httpHandleData->postdata = NULL;
                        httpHandleData->httpVersion = CURL_HTTP_VERSION_1_1;
                        httpHandleData->headers = NULL;

                        // The standard resolver implements its timeouts with
                        // signals and isn't thread safe.  Assume that our
                        // callers are multithreaded and disable the use of
                        // signals.  For more information see
                        // https://curl.haxx.se/libcurl/c/threadsafe.html
                        //
                        // We could instead use a different resolver, like the
                        // threaded resolver, which supports timeouts using
                        // separate threads, or c-ares, which is asynchronous.
                        // We chose this way for simplicity since the resolves
                        // are done synchronously.  The caller should have its
                        // own watchdog to notice hangs.
                        curl_easy_setopt(httpHandleData->curl, CURLOPT_NOSIGNAL, 1);
                    }
                }
                else
                {
                    free(httpHandleData->hostURL);
                    free(httpHandleData);
                    httpHandleData = NULL;
                }
            }
        }
    }

    return (HTTP_HANDLE)httpHandleData;
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTP_HANDLE_DATA* httpHandleData = (HTTP_HANDLE_DATA*)handle;
    if (httpHandleData != NULL)
    {
        if (httpHandleData->postdata)
        {
            free(httpHandleData->postdata);
        }
        free(httpHandleData->hostURL);
        curl_slist_free_all(httpHandleData->headers);
        curl_easy_cleanup(httpHandleData->curl);
        free(httpHandleData);
    }
}

static size_t HeadersWriteFunction(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HTTP_HEADERS_HANDLE responseHeadersHandle = (HTTP_HEADERS_HANDLE)userdata;
    char* headerLine = (char*)ptr;

    if (headerLine != NULL)
    {
        char* token = strtok(headerLine, "\r\n");
        while ((token != NULL) &&
               (token[0] != '\0'))
        {
            char* whereIsColon = strchr(token, ':');
            if(whereIsColon!=NULL)
            {
                *whereIsColon='\0';
                HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, token, whereIsColon+1);
                *whereIsColon=':';
            }
            else
            {
                /*not a header, maybe a status-line*/
            }
            token = strtok(NULL, "\r\n");
        }
    }

    return size * nmemb;
}

static size_t ContentWriteFunction(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HTTP_RESPONSE_CONTENT_BUFFER* responseContentBuffer = (HTTP_RESPONSE_CONTENT_BUFFER*)userdata;
    if ((userdata != NULL) &&
        (ptr != NULL) &&
        (size * nmemb > 0))
    {
        void* newBuffer = realloc(responseContentBuffer->buffer, responseContentBuffer->bufferSize + (size * nmemb));
        if (newBuffer != NULL)
        {
            responseContentBuffer->buffer = newBuffer;
            memcpy(responseContentBuffer->buffer + responseContentBuffer->bufferSize, ptr, size * nmemb);
            responseContentBuffer->bufferSize += size * nmemb;
        }
        else
        {
            LogError("Could not allocate buffer of size %zu", (size_t)(responseContentBuffer->bufferSize + (size * nmemb)));
            responseContentBuffer->error = 1;
            if (responseContentBuffer->buffer != NULL)
            {
                free(responseContentBuffer->buffer);
            }
        }
    }

    return size * nmemb;
}

static HTTPAPI_RESULT PrepareRequest(
    HTTP_HANDLE_DATA* httpHandleData, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content,
    size_t contentLength,
    bool addChunk)
{
    HTTPAPI_RESULT result;
    size_t headersCount;

    if (httpHandleData == NULL)
    {
        result = HTTPAPI_INVALID_ARG;
        LogErrorResult(HTTPAPI_RESULT, result);
    }
    else if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_INVALID_ARG;
        LogErrorResult(HTTPAPI_RESULT, result);
    }
    else
    {
        char* tempHostURL;
        size_t tempHostURL_size = strlen(httpHandleData->hostURL) + strlen(relativePath) + 1;
        tempHostURL = malloc(tempHostURL_size);
        if (tempHostURL == NULL)
        {
            result = HTTPAPI_ERROR;
            LogErrorResult(HTTPAPI_RESULT, result);
        }
        else
        {
            if (contentLength != 0)
            {
                if (httpHandleData->postdata)
                {
                    free(httpHandleData->postdata);
                }

                httpHandleData->postdata = malloc(contentLength);

                if (NULL == httpHandleData->postdata)
                {
                    free(tempHostURL);
                    result = HTTPAPI_ERROR;
                    LogErrorResult(HTTPAPI_RESULT, result);
                    return result;
                }

                memcpy(httpHandleData->postdata, content, contentLength);
                
                // stomp on the original.
                content = httpHandleData->postdata;
            }
            if (curl_easy_setopt(httpHandleData->curl, CURLOPT_VERBOSE, httpHandleData->verbose) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_VERBOSE (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if ((strcpy_s(tempHostURL, tempHostURL_size, httpHandleData->hostURL) != 0) ||
                (strcat_s(tempHostURL, tempHostURL_size, relativePath) != 0))
            {
                result = HTTPAPI_STRING_PROCESSING_ERROR;
                LogErrorResult(HTTPAPI_RESULT, result);
            }
            /* set the URL */
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_URL, tempHostURL) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_URL (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_TIMEOUT_MS, httpHandleData->timeout) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_TIMEOUT_MS (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_LOW_SPEED_LIMIT, httpHandleData->lowSpeedLimit) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_LOW_SPEED_LIMIT (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_LOW_SPEED_TIME, httpHandleData->lowSpeedTime) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_LOW_SPEED_TIME (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_FRESH_CONNECT, httpHandleData->freshConnect) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_FRESH_CONNECT (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_FORBID_REUSE, httpHandleData->forbidReuse) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_FORBID_REUSE (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if (curl_easy_setopt(httpHandleData->curl, CURLOPT_HTTP_VERSION, httpHandleData->httpVersion) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_HTTP_VERSION (result = %s)",
                         ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else if(getenv("CURL_CA_BUNDLE") && curl_easy_setopt(httpHandleData->curl, CURLOPT_CAINFO, getenv("CURL_CA_BUNDLE")) != CURLE_OK)
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogError("failed to set CURLOPT_CAINFO (result = %s)",
                         ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else
            {
                result = HTTPAPI_OK;

                switch (requestType)
                {
                default:
                    result = HTTPAPI_INVALID_ARG;
                    LogErrorResult(HTTPAPI_RESULT, result);
                    break;

                case HTTPAPI_REQUEST_GET:
                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_HTTPGET, 1L) != CURLE_OK)
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                    else
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_CUSTOMREQUEST, NULL) != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                    }

                    break;

                case HTTPAPI_REQUEST_POST:
                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_POST, 1L) != CURLE_OK)
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                    else
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_CUSTOMREQUEST, NULL) != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                    }

                    break;

                case HTTPAPI_REQUEST_PUT:
                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_POST, 1L))
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                    else
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_CUSTOMREQUEST, "PUT") != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                    }
                    break;

                case HTTPAPI_REQUEST_DELETE:
                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_POST, 1L) != CURLE_OK)
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                    else
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_CUSTOMREQUEST, "DELETE") != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                    }
                    break;

                case HTTPAPI_REQUEST_PATCH:
                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_POST, 1L) != CURLE_OK)
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                    else
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_CUSTOMREQUEST, "PATCH") != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                    }

                    break;
                }

                if (result == HTTPAPI_OK)
                {
                    /* add headers */
                    if (httpHandleData->headers)
                    {
                        curl_slist_free_all(httpHandleData->headers);
                    }
                    httpHandleData->headers = NULL;
                    size_t i;

                    if (addChunk)
                    {
                        httpHandleData->headers = curl_slist_append(httpHandleData->headers, "Transfer-Encoding: chunked");
                    }

                    for (i = 0; i < headersCount; i++)
                    {
                        char *tempBuffer;
                        if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &tempBuffer) != HTTP_HEADERS_OK)
                        {
                            /* error */
                            result = HTTPAPI_HTTP_HEADERS_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                            break;
                        }
                        else
                        {
                            struct curl_slist* newHeaders = curl_slist_append(httpHandleData->headers, tempBuffer);
                            free(tempBuffer);
                            if (newHeaders == NULL)
                            {
                                result = HTTPAPI_ALLOC_FAILED;
                                LogErrorResult(HTTPAPI_RESULT, result);
                                break;
                            }
                            else
                            {
                                httpHandleData->headers = newHeaders;
                            }
                        }
                    }

                    if (result == HTTPAPI_OK)
                    {
                        if (curl_easy_setopt(httpHandleData->curl, CURLOPT_HTTPHEADER, httpHandleData->headers) != CURLE_OK)
                        {
                            result = HTTPAPI_SET_OPTION_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                        else
                        {
                            /* add content */
                            if ((content != NULL) &&
                                (contentLength > 0))
                            {
                                if ((curl_easy_setopt(httpHandleData->curl, CURLOPT_POSTFIELDS, (void*)content) != CURLE_OK) ||
                                    (curl_easy_setopt(httpHandleData->curl, CURLOPT_POSTFIELDSIZE, contentLength) != CURLE_OK))
                                {
                                    result = HTTPAPI_SET_OPTION_FAILED;
                                    LogErrorResult(HTTPAPI_RESULT, result);
                                }
                            }
                            else
                            {
                                if (requestType != HTTPAPI_REQUEST_GET)
                                {
                                    if ((curl_easy_setopt(httpHandleData->curl, CURLOPT_POSTFIELDS, (void*)NULL) != CURLE_OK) ||
                                        (curl_easy_setopt(httpHandleData->curl, CURLOPT_POSTFIELDSIZE, 0) != CURLE_OK))
                                    {
                                        result = HTTPAPI_SET_OPTION_FAILED;
                                        LogErrorResult(HTTPAPI_RESULT, result);
                                    }
                                }
                                else
                                {
                                    /*GET request cannot POST, so "do nothing*/
                                }
                                if (result == HTTPAPI_OK)
                                {
                                    if ((curl_easy_setopt(httpHandleData->curl, CURLOPT_WRITEHEADER, NULL) != CURLE_OK) ||
                                        (curl_easy_setopt(httpHandleData->curl, CURLOPT_HEADERFUNCTION, NULL) != CURLE_OK))
                                    {
                                        result = HTTPAPI_SET_OPTION_FAILED;
                                        LogErrorResult(HTTPAPI_RESULT, result);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            free(tempHostURL);
        }
    }

    return result;
}

HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
                                      HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content,
                                      size_t contentLength, unsigned int* statusCode,
                                      HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPI_RESULT result;
    HTTP_HANDLE_DATA* httpHandleData = (HTTP_HANDLE_DATA*)handle;
    size_t headersCount;
    HTTP_RESPONSE_CONTENT_BUFFER responseContentBuffer;

    if ((httpHandleData == NULL) ||
        (relativePath == NULL) ||
        (httpHeadersHandle == NULL) ||
        ((content == NULL) && (contentLength > 0))
    )
    {
        result = HTTPAPI_INVALID_ARG;
        LogErrorResult(HTTPAPI_RESULT, result);
    }
    else if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_INVALID_ARG;
        LogErrorResult(HTTPAPI_RESULT, result);
    }
    else
    {
        result = PrepareRequest(httpHandleData, requestType, relativePath, httpHeadersHandle, content, contentLength, false);
        if (result == HTTPAPI_OK)
        {
            if ((curl_easy_setopt(httpHandleData->curl, CURLOPT_WRITEFUNCTION, ContentWriteFunction) != CURLE_OK))
            {
                result = HTTPAPI_SET_OPTION_FAILED;
                LogErrorResult(HTTPAPI_RESULT, result);
            }
            else
            {
                if (responseHeadersHandle != NULL)
                {
                    /* setup the code to get the response headers */
                    if ((curl_easy_setopt(httpHandleData->curl, CURLOPT_WRITEHEADER, responseHeadersHandle) != CURLE_OK) ||
                        (curl_easy_setopt(httpHandleData->curl, CURLOPT_HEADERFUNCTION, HeadersWriteFunction) != CURLE_OK))
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }
                }

                if (result == HTTPAPI_OK)
                {
                    responseContentBuffer.buffer = NULL;
                    responseContentBuffer.bufferSize = 0;
                    responseContentBuffer.error = 0;

                    if (curl_easy_setopt(httpHandleData->curl, CURLOPT_WRITEDATA, &responseContentBuffer) != CURLE_OK)
                    {
                        result = HTTPAPI_SET_OPTION_FAILED;
                        LogErrorResult(HTTPAPI_RESULT, result);
                    }

                    if (result == HTTPAPI_OK)
                    {
                        /* Execute request */
                        CURLcode curlRes = curl_easy_perform(httpHandleData->curl);
                        if (curlRes != CURLE_OK)
                        {
                            LogError("curl_easy_perform() failed: %s\n", curl_easy_strerror(curlRes));
                            result = HTTPAPI_OPEN_REQUEST_FAILED;
                            LogErrorResult(HTTPAPI_RESULT, result);
                        }
                        else
                        {
                            long httpCode;

                            /* get the status code */
                            if (curl_easy_getinfo(httpHandleData->curl, CURLINFO_RESPONSE_CODE, &httpCode) != CURLE_OK)
                            {
                                result = HTTPAPI_QUERY_HEADERS_FAILED;
                                LogErrorResult(HTTPAPI_RESULT, result);
                            }
                            else if (responseContentBuffer.error)
                            {
                                result = HTTPAPI_READ_DATA_FAILED;
                                LogErrorResult(HTTPAPI_RESULT, result);
                            }
                            else
                            {
                                if (statusCode != NULL)
                                {
                                    *statusCode = httpCode;
                                }

                                /* fill response content length */
                                if (responseContent != NULL)
                                {
                                    if ((responseContentBuffer.bufferSize > 0) && (BUFFER_build(responseContent, responseContentBuffer.buffer, responseContentBuffer.bufferSize) != 0))
                                    {
                                        result = HTTPAPI_INSUFFICIENT_RESPONSE_BUFFER;
                                        LogErrorResult(HTTPAPI_RESULT, result);
                                    }
                                    else
                                    {
                                        /*all nice*/
                                    }
                                }

                                if (httpCode >= 300)
                                {
                                    LogError("Failure in HTTP communication: server reply code is %ld", httpCode);
                                    LogInfo("HTTP Response:%*.*s", (int)responseContentBuffer.bufferSize,
                                        (int)responseContentBuffer.bufferSize, responseContentBuffer.buffer);
                                }
                                else
                                {
                                    result = HTTPAPI_OK;
                                }
                            }
                        }
                    }

                    if (responseContentBuffer.buffer != NULL)
                    {
                        free(responseContentBuffer.buffer);
                    }
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
        else if (strcmp("CURLOPT_LOW_SPEED_LIMIT", optionName) == 0)
        {
            httpHandleData->lowSpeedLimit = *(const long*)value;
            result = HTTPAPI_OK;
        }
        else if (strcmp("CURLOPT_LOW_SPEED_TIME", optionName) == 0)
        {
            httpHandleData->lowSpeedTime = *(const long*)value; 
            result = HTTPAPI_OK;
        }
        else if (strcmp("CURLOPT_FRESH_CONNECT", optionName) == 0)
        {
            httpHandleData->freshConnect = *(const long*)value;
            result = HTTPAPI_OK;
        }
        else if (strcmp("CURLOPT_FORBID_REUSE", optionName) == 0)
        {
            httpHandleData->forbidReuse = *(const long*)value;
            result = HTTPAPI_OK;
        }
        else if (strcmp("CURLOPT_VERBOSE", optionName) == 0)
        {
            httpHandleData->verbose = *(const long*)value;
            result = HTTPAPI_OK;
        }
        else if (strcmp("CURLOPT_HTTP_VERSION", optionName) == 0)
        {
            httpHandleData->httpVersion = *(const long*)value;
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
        /*all "long" options are cloned in the same way*/
        else if (
            (strcmp("CURLOPT_LOW_SPEED_LIMIT", optionName) == 0) ||
            (strcmp("CURLOPT_LOW_SPEED_TIME", optionName) == 0) ||
            (strcmp("CURLOPT_FRESH_CONNECT", optionName) == 0) ||
            (strcmp("CURLOPT_FORBID_REUSE", optionName) == 0) ||
            (strcmp("CURLOPT_VERBOSE", optionName) == 0) 
            )
        {
            /*by convention value is pointing to an long */
            long* temp = malloc(sizeof(long)); /*shall be freed by HTTPAPIEX*/
            if (temp == NULL)
            {
                result = HTTPAPI_ERROR;
                LogError("malloc failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            }
            else
            {
                *temp = *(const long*)value;
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

///////////////////////////////////////////////////////////////////////////////

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG
{
    XIO_HANDLE xioHandle;
    void* userCtx;
    ON_HTTP_CONNECTED_CALLBACK fnOnConnect;
    ON_HTTP_CHUNK_REPLY_CALLBACK fnChunkReplyCallback;
    ON_HTTP_ERROR_CALLBACK fnOnErroCb;
    const char* host;
    bool connectCalled;
    HTTP_HANDLE_DATA* hHttp;
    CURLM* multi;
    long httpCode;
    HTTP_HEADERS_HANDLE respHeader;
    bool chunked;
    const uint8_t*chunkBuf;
    size_t chunkSize;
    size_t chunkOffset;
} HTTP_CLIENT_HANDLE_DATA;

static size_t UHTTP_PostChunk(void *ptr, size_t size, size_t nmemb, void *ctx)
{
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)ctx;

    if (data && data->chunkOffset < data->chunkSize)
    {
        nmemb *= size;
        if (nmemb > (data->chunkSize - data->chunkOffset))
        {
            nmemb = (data->chunkSize - data->chunkOffset);
        }

        memcpy(ptr, data->chunkBuf + data->chunkOffset, nmemb);
        data->chunkOffset += nmemb;
        return nmemb;
    }

    return 0;
}

static size_t UHTTP_ResponseHeaders(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)userdata;
    long httpCode;

    if (!data->httpCode)
    {
        if (curl_easy_getinfo(data->hHttp->curl, CURLINFO_RESPONSE_CODE, &httpCode) != CURLE_OK)
        {
            LogError("curl_easy_getinfo failed");
        }
        // let curl handle continuation.
        else if (httpCode != 100)
        {
            data->httpCode = httpCode;
        }
    }

    size_t ret = HeadersWriteFunction(ptr, size, nmemb, data->respHeader);
    if (ret)
    {
        const char* transferEncoding = HTTPHeaders_FindHeaderValue(data->respHeader, "Transfer-Encoding");
        if (transferEncoding && strcmp(transferEncoding, "chunked") == 0)
        {
            data->chunked = true;
        }
    }
    return ret;
}

static void ChunkReply(HTTP_CLIENT_HANDLE_DATA* data, uint8_t* pBuffer, size_t size, int complete)
{
    // For CURL we need to artificially send up a connected event.
    if (!data->connectCalled)
    {
        data->connectCalled = true;
        data->fnOnConnect(data, data->userCtx);
    }

    data->fnChunkReplyCallback(
        data, 
        data->userCtx, 
        pBuffer,
        size,
        data->httpCode, 
        data->respHeader, 
        complete);
}

static size_t UHTTP_ResponseHandler(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HTTP_CLIENT_RESULT result;
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)userdata;
    /* get the status code */
    if (!data->httpCode)
    {
        if (curl_easy_getinfo(data->hHttp->curl, CURLINFO_RESPONSE_CODE, &data->httpCode) != CURLE_OK)
        {
            result = (HTTP_CLIENT_RESULT)HTTPAPI_QUERY_HEADERS_FAILED;
            LogErrorResult(HTTPAPI_RESULT, result);
        }
    }
    ChunkReply(data, ptr, size * nmemb, 0);

    return size * nmemb;
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
    HTTP_CLIENT_RESULT result;
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

    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
    HTTPAPI_REQUEST_TYPE httpAPIRequestType;

    switch (requestType)
    {
    case HTTP_CLIENT_REQUEST_GET:
        httpAPIRequestType = HTTPAPI_REQUEST_GET;
        break;
    case HTTP_CLIENT_REQUEST_POST:
        httpAPIRequestType = HTTPAPI_REQUEST_POST;
        break;
    case HTTP_CLIENT_REQUEST_PUT:
        httpAPIRequestType = HTTPAPI_REQUEST_PUT;
        break;
    case HTTP_CLIENT_REQUEST_DELETE:
        httpAPIRequestType = HTTPAPI_REQUEST_DELETE;
        break;
    case HTTP_CLIENT_REQUEST_PATCH:
        httpAPIRequestType = HTTPAPI_REQUEST_PATCH;
        break;
    default:
        return HTTP_CLIENT_INVALID_ARG;
    }

    data->httpCode = 0;
    data->chunked = false;
    data->chunkSize = data->chunkOffset = 0;

    if (data->respHeader)
    {
        HTTPHeaders_Free(data->respHeader);
    }

    data->respHeader = HTTPHeaders_Alloc();

    if (data->hHttp)
    {
        HTTPAPI_CloseConnection(data->hHttp);
        data->hHttp = NULL;
    }

    if (!data->hHttp)
    {
        data->hHttp = (HTTP_HANDLE_DATA*)HTTPAPI_CreateConnection(data->host);
        if (data->hHttp == NULL)
        {
            /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
            LogError("HTTPAPI_CreateConnection failed");
            return HTTP_CLIENT_INVALID_ARG;
        }

        if (CURLM_OK != curl_multi_add_handle(data->multi, data->hHttp->curl))
        {
            LogError("curl_multi_init failed");
            return HTTP_CLIENT_ERROR;
        }
    }

    result = (HTTP_CLIENT_RESULT)PrepareRequest(data->hHttp, httpAPIRequestType, relativePath, httpHeadersHandle, content, contentLength, addChunk);
    if (result == HTTP_CLIENT_OK && addChunk)
    {
        curl_easy_setopt(data->hHttp->curl, CURLOPT_READDATA, data);
        curl_easy_setopt(data->hHttp->curl, CURLOPT_READFUNCTION, UHTTP_PostChunk);
        curl_easy_setopt(data->hHttp->curl, CURLOPT_POSTFIELDSIZE, 2048);
    }

    if ((curl_easy_setopt(data->hHttp->curl, CURLOPT_WRITEFUNCTION, UHTTP_ResponseHandler) != CURLE_OK) ||
        (curl_easy_setopt(data->hHttp->curl, CURLOPT_WRITEDATA, data) != CURLE_OK) ||
        (curl_easy_setopt(data->hHttp->curl, CURLOPT_WRITEHEADER, data) != CURLE_OK) ||
        (curl_easy_setopt(data->hHttp->curl, CURLOPT_HEADERFUNCTION, UHTTP_ResponseHeaders) != CURLE_OK))
    {
        result = (HTTP_CLIENT_RESULT)HTTPAPI_SET_OPTION_FAILED;
        LogErrorResult(HTTPAPI_RESULT, result);
    }

    return result;
}

HTTP_CLIENT_HANDLE http_client_create(
    XIO_HANDLE xioHandle, 
    ON_HTTP_CONNECTED_CALLBACK onConnect, 
    ON_HTTP_ERROR_CALLBACK on_http_error, 
    ON_HTTP_CHUNK_REPLY_CALLBACK on_chunk_reply, 
    void* callback_ctx)
{
    HTTP_CLIENT_HANDLE result = NULL;

    (void)xioHandle;

    if (on_chunk_reply == NULL || on_http_error == NULL)
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
            result->connectCalled = false;
            result->hHttp = NULL;
            result->respHeader = NULL;
            result->multi = curl_multi_init();
            result->chunked = false;
            if (result->multi == NULL)
            {
                free(result);
                /* Codes_SRS_UHTTP_07_003: [If http_client_create encounters any error then it shall return NULL] */
                return NULL;
            }

            (void)HTTPAPI_Init();
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
        /* Codes_SRS_UHTTP_07_005: [http_client_destroy shall free any memory that is allocated in this translation unit] */
        if (data->multi)
        {
            curl_multi_cleanup(data->multi);
        }

        if (data->respHeader)
        {
            HTTPHeaders_Free(data->respHeader);
        }

        if (data->hHttp)
        {
            HTTPAPI_CloseConnection(data->hHttp);
            data->hHttp = NULL;
        }

        free(data);
        HTTPAPI_Deinit();
    }
}

HTTP_CLIENT_RESULT http_client_open(HTTP_CLIENT_HANDLE handle, const char* host)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || 
        host   == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        result = HTTP_CLIENT_INVALID_ARG;
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        data->host = host;
        result = HTTP_CLIENT_OK;
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
            data->xioHandle = NULL;
        }
    }
}

HTTP_CLIENT_RESULT http_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content, size_t contentLength)
{
    HTTP_CLIENT_RESULT result;
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

    result = SendRequest(handle, requestType, relativePath, httpHeadersHandle, content, contentLength, false);
    if (result == HTTP_CLIENT_OK)
    {
        // signal the end
        data->chunkSize = data->chunkOffset = 0;
    }

    return result;
}

HTTP_CLIENT_RESULT http_client_start_chunk_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle)
{
    return SendRequest(handle, requestType, relativePath, httpHeadersHandle, NULL, 0, true);
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

        data->chunkBuf = content;
        data->chunkOffset = 0;
        data->chunkSize = contentLength;

        int still_running;
        while ((contentLength == 0 || (data->chunkOffset != data->chunkSize)) && 
               (CURLM_OK == curl_multi_perform(data->multi, &still_running)))
        {
            if (!contentLength && !still_running)
            {
                break;
            }
        }

        result = HTTP_CLIENT_OK;
    }
    return result;
}

HTTP_CLIENT_RESULT http_client_end_chunk_request(HTTP_CLIENT_HANDLE handle)
{
    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
        return HTTP_CLIENT_INVALID_ARG;
    }

    return http_client_send_chunk_request(handle, NULL, 0);
}

void http_client_dowork(HTTP_CLIENT_HANDLE handle)
{
    int still_running;

    if (handle == NULL)
    {
        /* Codes_SRS_UHTTP_07_006: [If any parameter is NULL then http_client_connect shall return HTTP_CLIENT_INVALID_ARG] */
        LogError("Invalid handle value");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        if (data->multi &&
            (curl_multi_perform(data->multi, &still_running) == CURLM_OK) &&
            !still_running)
        {
            // The client is no longer running.  Use the HTTP code to determine 
            // if we've completed successfully.
            if (data->httpCode)
            {
                ChunkReply(data, NULL, 0, 1);
            }
            else
            {
                // No HTTP error, signal a transport error.
                data->fnOnErroCb(data, data->userCtx);
            }
        }
    }
}
