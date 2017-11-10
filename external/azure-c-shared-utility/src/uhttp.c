// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include <string.h>

#define MAX_HOSTNAME        64
#define TEMP_BUFFER_SIZE    4096
#define TIME_MAX_BUFFER     16

#define CHAR_COUNT(A)   (sizeof(A) - 1)

static const char* HTTP_REQUEST_LINE_FMT = "%s %s HTTP/1.1\r\n";
static const char* HTTP_CONTENT_LEN = "Content-Length";
static const char* HTTP_HOST = "Host";
static const char* HTTP_CHUNKED_ENCODING_HDR = "Transfer-Encoding: chunked\r\n";
static const char* HTTP_CRLF_VALUE = "\r\n";

typedef enum RESPONSE_MESSAGE_STATE_TAG
{
    state_initial,
    state_status_line,
    state_response_header,
    state_message_body,
    state_message_chunked,
    state_error
} RESPONSE_MESSAGE_STATE;

typedef struct HTTP_RECV_DATA_TAG
{
    int statusCode;
    RESPONSE_MESSAGE_STATE recvState;
    HTTP_HEADERS_HANDLE respHeader;
    size_t totalBodyLen;
    uint8_t* storedBytes;
    size_t storedLen;
    bool chunkedReply;
} HTTP_RECV_DATA;

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG
{
    XIO_HANDLE xioHandle;
    void* userCtx;
    ON_HTTP_CONNECTED_CALLBACK fnOnConnect;
    ON_HTTP_CHUNK_REPLY_CALLBACK fnChunkReplyCallback;
    ON_HTTP_ERROR_CALLBACK fnOnErroCb;
    HTTP_RECV_DATA recvMsg;
    bool chunkRequest;
    bool traceOn;
    char* hostName;
} HTTP_CLIENT_HANDLE_DATA;

static void sendCompleteCb(void* context, IO_SEND_RESULT send_result)
{
	(void)context;
	(void)send_result;
	//printf("Sending Data has been complete. Result %d\r\n", send_result);
}

static int ProcessStatusCodeLine(const uint8_t* buffer, size_t len, size_t* position, int* statusLen)
{
    int result = __LINE__;
    size_t index;
    int spaceFound = 0;
    const char* initSpace = NULL;
    char statusCode[4];

    for (index = 0; index < len; index++)
    {
        if (buffer[index] == ' ')
        {
            if (spaceFound == 1)
            {
                strncpy(statusCode, initSpace, 3);
                statusCode[3] = '\0';
            }
            else
            {
                initSpace = (const char*)buffer+index+1;
            }
            spaceFound++;
        }
        else if (buffer[index] == '\n')
        {
            *statusLen = (int)atol(statusCode);
            if (index < len)
            {
                *position = index+1;
            }
            else
            {
                *position = index;
            }
            result = 0;
            break;
        }
    }
    return result;
}

static int ProcessHeaderLine(const uint8_t* buffer, size_t len, size_t* position, HTTP_HEADERS_HANDLE respHeader, size_t* contentLen, bool* isChunked)
{
    int result = __LINE__;
    size_t index;
    const uint8_t* targetPos = buffer;
    bool crlfEncounted = false;
    bool colonEncountered = false;
    char* headerKey = NULL;
    bool continueProcessing = true;

    for (index = 0; index < len && continueProcessing; index++)
    {
        if (buffer[index] == ':' && !colonEncountered)
        {
            colonEncountered = true;
            size_t keyLen = (&buffer[index])-targetPos;
            headerKey = (char*)malloc(keyLen+1);
            memcpy(headerKey, targetPos, keyLen);
            headerKey[keyLen] = '\0';

            targetPos = buffer+index+1;
            crlfEncounted = false;
        }
        else if (buffer[index] == '\r')
        {
            if (headerKey != NULL)
            {
                // Remove leading spaces
                while (*targetPos == 32) { targetPos++; }

                size_t valueLen = (&buffer[index])-targetPos;
                char* headerValue = (char*)malloc(valueLen+1);
                memcpy(headerValue, targetPos, valueLen);
                headerValue[valueLen] = '\0';

                if (HTTPHeaders_AddHeaderNameValuePair(respHeader, headerKey, headerValue) != HTTP_HEADERS_OK)
                {
                    result = __LINE__;
                    continueProcessing = false;
                }
                else
                {
                    if (strcmp(headerKey, HTTP_CONTENT_LEN) == 0)
                    {
                        *isChunked = false;
                        *contentLen = atol(headerValue);
                    }
                    else if (strcmp(headerKey, "Transfer-Encoding") == 0)
                    {
                        *isChunked = true;
                        *contentLen = 0;
                    }
                }
                free(headerKey);
                headerKey = NULL;
                free(headerValue);
            }
        }
        else if (buffer[index] == '\n')
        {
            if (index < len)
            {
                *position = index+1;
            }
            else
            {
                *position = index;
            }
            if (crlfEncounted)
            {
                result = 0;
                break;
            }
            else
            {
                colonEncountered = false;
                crlfEncounted = true;
                targetPos = buffer+index+1;
            }
        }
        else
        {
            crlfEncounted = false;
        }
    }
    if (headerKey != NULL)
    {
        free(headerKey);
    }
    return result;
}

static void getLogTime(char* timeResult, size_t len)
{
    if (timeResult != NULL)
    {
        time_t localTime = time(NULL);
        struct tm* tmInfo = localtime(&localTime);
        if (strftime(timeResult, len, "%H:%M:%S", tmInfo) == 0)
        {
            timeResult[0] = '\0';
        }
    }
}

static int WriteTextLine(HTTP_CLIENT_HANDLE_DATA* data, const char* writeText)
{
    int result;
    if (xio_send(data->xioHandle, writeText, strlen(writeText), sendCompleteCb, NULL) != 0)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
        if (data->traceOn)
        {
            LOG(LOG_TRACE, LOG_LINE, "%s", writeText);
        }
    }
    return result;
}

static int WriteDataLine(HTTP_CLIENT_HANDLE_DATA* data, const uint8_t* writeData, size_t length)
{
    int result;
    if (xio_send(data->xioHandle, writeData, length, sendCompleteCb, NULL) != 0)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
        if (data->traceOn)
        {
            for (size_t index = 0; index < length; index++)
            {
                LOG(LOG_TRACE, 0, "0x%02x ", writeData[index]);
            }
        }
    }
    return result;
}

static int ConvertCharToHex(const uint8_t* hexText, size_t len)
{
    int result = 0;
    for (size_t index = 0; index < len; index++)
    {
        int accumulator = 0;
        if (hexText[index] >= 48 && hexText[index] <= 57)
        {
            accumulator = hexText[index] - 48;
        }
        else if (hexText[index] >= 65 && hexText[index] <= 70)
        {
            accumulator = hexText[index] - 55;
        }
        else if (hexText[index] >= 97 && hexText[index] <= 102)
        {
            accumulator = hexText[index] - 87;
        }
        if (index > 0)
        {
            result = result << 4;
        }
        result += accumulator;
    }
    return result;
}

static void onBytesReceived(void* context, const uint8_t* buffer, size_t len)
{
    HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)context;
    size_t index = 0;

    if (data != NULL && buffer != NULL && len > 0 && data->recvMsg.recvState != state_error)
    {
        if (data->recvMsg.recvState == state_initial)
        {
            if (data->recvMsg.respHeader == NULL)
            {
                data->recvMsg.respHeader = HTTPHeaders_Alloc();
            }
            data->recvMsg.chunkedReply = false;
        }

        if (data->recvMsg.storedLen == 0)
        {
            data->recvMsg.storedBytes = (uint8_t*)malloc(len);
            memcpy(data->recvMsg.storedBytes, buffer, len);
            data->recvMsg.storedLen = len;
        }
        else
        {
            size_t newSize = data->recvMsg.storedLen+len;
            uint8_t* tmpBuff = (uint8_t*)malloc(newSize);
            if (tmpBuff == NULL)
            {
                LogError("Failure reallocating buffer.");
                data->recvMsg.recvState = state_error;
                free(data->recvMsg.storedBytes);
                data->recvMsg.storedBytes = NULL;
            }
            else
            {
                memcpy(tmpBuff, data->recvMsg.storedBytes, data->recvMsg.storedLen);
                free(data->recvMsg.storedBytes);
                data->recvMsg.storedBytes = tmpBuff;
                memcpy(data->recvMsg.storedBytes+data->recvMsg.storedLen, buffer, len);
                data->recvMsg.storedLen = newSize;
            }
        }

        if (data->recvMsg.recvState == state_initial)
        {
            index = 0;
            int lineComplete = ProcessStatusCodeLine(data->recvMsg.storedBytes, data->recvMsg.storedLen, &index, &data->recvMsg.statusCode);
            if (lineComplete == 0 && data->recvMsg.statusCode > 0)
            {
                data->recvMsg.recvState = state_status_line;

                // Let's remove the unneccessary bytes
                size_t allocLen = data->recvMsg.storedLen-index;
                uint8_t* tmpBuff = (uint8_t*)malloc(allocLen);
                memcpy(tmpBuff, data->recvMsg.storedBytes+index, allocLen);
                free(data->recvMsg.storedBytes);
                data->recvMsg.storedBytes = tmpBuff;
                data->recvMsg.storedLen = allocLen;
            }
        }
        if (data->recvMsg.recvState == state_status_line)
        {
            // Gather the Header
            index = 0;
            int headerComplete = ProcessHeaderLine(data->recvMsg.storedBytes, data->recvMsg.storedLen, &index, data->recvMsg.respHeader, &data->recvMsg.totalBodyLen, &data->recvMsg.chunkedReply);
            if (headerComplete == 0)
            {
                if (data->recvMsg.totalBodyLen == 0)
                {
                    if (data->recvMsg.chunkedReply)
                    {
                        data->recvMsg.recvState = state_message_chunked;
                    }
                    else
                    {
                        // Content len is 0 so we are finished with the body
                        data->fnReplyCallback((HTTP_CLIENT_HANDLE)data, data->userCtx, NULL, 0, data->recvMsg.statusCode, data->recvMsg.respHeader);
                        data->recvMsg.recvState = state_message_body;
                    }
                }
                else
                {
                    data->recvMsg.recvState = state_response_header;
                }
            }
            if (index > 0)
            {
                // Let's remove the unneccessary bytes
                size_t allocLen = data->recvMsg.storedLen-index;
                uint8_t* tmpBuff = (uint8_t*)malloc(allocLen);
                memcpy(tmpBuff, data->recvMsg.storedBytes+index, allocLen);
                free(data->recvMsg.storedBytes);
                data->recvMsg.storedBytes = tmpBuff;
                data->recvMsg.storedLen = allocLen;
            }
        }
        if (data->recvMsg.recvState == state_response_header)
        {
            if (data->recvMsg.totalBodyLen != 0)
            {
                if (data->recvMsg.storedLen == data->recvMsg.totalBodyLen)
                {
                    data->fnReplyCallback((HTTP_CLIENT_HANDLE)data, data->userCtx, data->recvMsg.storedBytes, data->recvMsg.storedLen, data->recvMsg.statusCode, data->recvMsg.respHeader);
                    data->recvMsg.recvState = state_message_body;
                }
                else if (data->recvMsg.storedLen > data->recvMsg.totalBodyLen)
                {
                    data->recvMsg.recvState = state_error;
                }
            }
            else
            {
                // chunked
                if (data->fnChunkReplyCallback != NULL)
                {
                    data->fnChunkReplyCallback((HTTP_CLIENT_HANDLE)data, data->userCtx, NULL, 0, data->recvMsg.statusCode, data->recvMsg.respHeader, false);
                }
            }
        }
        if (data->recvMsg.recvState == state_message_chunked)
        {
            // Chunked reply
            size_t chunkLen = 0;
            size_t bytesLen = data->recvMsg.storedLen;

            for (index = 0; index < bytesLen; index++)
            {
                if (data->recvMsg.storedBytes[index] == '\r')
                {
                    if ((index + chunkLen + 2) > bytesLen)
                    {
                        // not enough space
                        break;
                    }

                    if (chunkLen == 0)
                    {
                        if (data->fnChunkReplyCallback != NULL)
                        {
                            data->fnChunkReplyCallback((HTTP_CLIENT_HANDLE)data, data->userCtx, NULL, 0, data->recvMsg.statusCode, data->recvMsg.respHeader, true);
                            data->recvMsg.recvState = state_message_body;
                        }
                    }
                    else if (chunkLen <= data->recvMsg.storedLen-index)
                    {
                        // Send the user the chunk
                        if (data->fnChunkReplyCallback != NULL)
                        {
                            data->fnChunkReplyCallback(
                                (HTTP_CLIENT_HANDLE)data, 
                                data->userCtx, 
                                data->recvMsg.storedBytes + index + 2, 
                                chunkLen, 
                                data->recvMsg.statusCode, 
                                data->recvMsg.respHeader, 
                                false);
                        }
                    }

                    index += (chunkLen + 3);
                    chunkLen = 0;
                }
                else
                {
                    chunkLen <<= 4;
                    chunkLen |= ConvertCharToHex(data->recvMsg.storedBytes + index, 1);
                }
            }

            if (index == bytesLen && !chunkLen)
            {
                // we can safely dump this payload.
                free(data->recvMsg.storedBytes);
                data->recvMsg.storedBytes = NULL;
                data->recvMsg.storedLen = 0;
            }
        }
        if (data->recvMsg.recvState == state_message_body || data->recvMsg.recvState == state_error)
        {
            HTTPHeaders_Free(data->recvMsg.respHeader);
            data->recvMsg.respHeader = NULL;
            free(data->recvMsg.storedBytes);
            data->recvMsg.storedBytes = NULL;
            data->recvMsg.storedLen = 0;
        }
    }
}

static void onOpenComplete(void* context, IO_OPEN_RESULT open_result)
{
	(void)open_result;

    if (context != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)context;
        if (data->fnOnConnect != NULL)
        {
            data->fnOnConnect((HTTP_CLIENT_HANDLE)data, data->userCtx);
        }
    }
}

static void onIoError(void* context)
{
    if (context != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)context;
        data->fnOnErroCb((HTTP_CLIENT_HANDLE)data, data->userCtx);
    }
}

static int ConstructHttpHeaders(HTTP_HEADERS_HANDLE httpHeaderHandle, size_t contentLen, STRING_HANDLE buffData, bool chunkData, const char* pszhost)
{
    int result = 0;
    size_t headerCnt;
    if (!pszhost)
    {
        result = __LINE__;
    }
    else if (STRING_concat(buffData, "Host:") ||
             STRING_concat(buffData, pszhost) ||
             STRING_concat(buffData, HTTP_CRLF_VALUE))
    {
        result = __LINE__;
        LogError("Failed building Host header data");
    }
    else if (HTTPHeaders_GetHeaderCount(httpHeaderHandle, &headerCnt) != HTTP_HEADERS_OK)
    {
        result = __LINE__;
    }
    else
    {
        for (size_t index = 0; index < headerCnt && result == 0; index++)
        {
            char* header;
            if (HTTPHeaders_GetHeader(httpHeaderHandle, index, &header) != HTTP_HEADERS_OK)
            {
                result = __LINE__;
                LogError("Failed in HTTPHeaders_GetHeader");
            }
            else if (strcmp(header, HTTP_HOST) == 0)
            {
                free(header);
                continue;
            }
            else
            {
                size_t dataLen = strlen(header)+2;
                char* sendData = malloc(dataLen+1);
                if (sendData == NULL)
                {
                    result = __LINE__;
                    LogError("Failed in allocating header data");
                }
                else
                {
                    if (snprintf(sendData, dataLen+1, "%s\r\n", header) <= 0)
                    {
                        result = __LINE__;
                        LogError("Failed in constructing header data");
                    }
                    else
                    {
                        if (STRING_concat(buffData, sendData) != 0)
                        {
                            result = __LINE__;
                            LogError("Failed in building header data");
                        }
                    }
                    free(sendData);
                }
                free(header);
            }
        }

        if (result == 0)
        {
            /* Codes_SRS_UHTTP_07_015: [http_client_execute_request shall add the Content-Length http header item to the request if the contentLength is > 0] */
            if (chunkData)
            {
                if (STRING_concat(buffData, HTTP_CHUNKED_ENCODING_HDR) != 0)
                {
                    result = __LINE__;
                    LogError("Failed building content len header data");
                }
            }
            else
            {
                size_t fmtLen = strlen(HTTP_CONTENT_LEN)+strlen(HTTP_CRLF_VALUE)+8;
                char* content = malloc(fmtLen+1);
                if (sprintf(content, "%s: %d%s", HTTP_CONTENT_LEN, (int)contentLen, HTTP_CRLF_VALUE) <= 0)
                {
                    result = __LINE__;
                    LogError("Failed allocating content len header data");
                }
                else
                {
                    if (STRING_concat(buffData, content) != 0)
                    {
                        result = __LINE__;
                        LogError("Failed building content len header data");
                    }
                }
                free(content);
            }

            if (STRING_concat(buffData, "\r\n") != 0)
            {
                result = __LINE__;
                LogError("Failed sending header finalization data");
            }
        }
    }
    return result;
}

static STRING_HANDLE ConstructHttpData(HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle, size_t contentLength, bool chunkData, const char*pszHost)
{
    STRING_HANDLE result;

    const char* method = (requestType == HTTP_CLIENT_REQUEST_GET) ? "GET"
        : (requestType == HTTP_CLIENT_REQUEST_OPTIONS) ? "OPTIONS"
        : (requestType == HTTP_CLIENT_REQUEST_POST) ? "POST"
        : (requestType == HTTP_CLIENT_REQUEST_PUT) ? "PUT"
        : (requestType == HTTP_CLIENT_REQUEST_DELETE) ? "DELETE"
        : (requestType == HTTP_CLIENT_REQUEST_PATCH) ? "PATCH"
        : NULL;
    /* Codes_SRS_UHTTP_07_014: [If the requestType is not a valid request http_client_execute_request shall return HTTP_CLIENT_ERROR] */
    if (method == NULL)
    {
        LogError("Invalid request method specified");
        result = NULL;
    }
    else
    {
        size_t buffLen = strlen(HTTP_REQUEST_LINE_FMT)+strlen(method)+strlen(relativePath);
        char* request = malloc(buffLen+1);
        if (request == NULL)
        {
            result = NULL;
            LogError("Failure allocating Request data");
        }
        else
        {
            if (snprintf(request, buffLen+1, HTTP_REQUEST_LINE_FMT, method, relativePath) <= 0)
            {
                result = NULL;
                LogError("Failure writing request buffer");
            }
            else
            {
                result = STRING_construct(request);
                if (result == NULL)
                {
                    LogError("Failure creating buffer object");
                }
                else if (ConstructHttpHeaders(httpHeadersHandle, contentLength, result, chunkData, pszHost) != 0)
                {
                    STRING_delete(result);
                    result = NULL;
                    LogError("Failure writing request buffer");
                }
            }
            free(request);
        }
    }
    return result;
}

static int SendHttpData(HTTP_CLIENT_HANDLE_DATA* data, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, size_t contentLength, bool sendChunked)
{
    int result;
    STRING_HANDLE httpData = ConstructHttpData(requestType, relativePath, httpHeadersHandle, contentLength, sendChunked, data->hostName);
    if (httpData == NULL)
    {
        result = __LINE__;
    }
    else
    {
        /* Tests_SRS_UHTTP_07_016: [http_client_execute_request shall transmit the http headers data through a call to xio_send;]*/
        char timeResult[TIME_MAX_BUFFER];
        getLogTime(timeResult, TIME_MAX_BUFFER);
        LOG(LOG_INFO, LOG_LINE, "%s", timeResult);
        if (WriteTextLine(data, STRING_c_str(httpData) ) != 0)
        {
            result = __LINE__;
            LogError("Failure writing request buffer");
        }
        else
        {
            result = 0;
        }
        STRING_delete(httpData);
    }
    return result;
}

HTTP_CLIENT_HANDLE http_client_create(XIO_HANDLE xioHandle, ON_HTTP_CONNECTED_CALLBACK onConnect, ON_HTTP_ERROR_CALLBACK on_http_error, ON_HTTP_CHUNK_REPLY_CALLBACK on_chunk_reply, void* callback_ctx)
{
    HTTP_CLIENT_HANDLE_DATA* result;
    if (on_chunk_reply == NULL || xioHandle == NULL)
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
            result->xioHandle = xioHandle;
            result->fnOnConnect = onConnect;
            result->fnChunkReplyCallback = on_chunk_reply;
            result->fnOnErroCb = on_http_error;
            result->userCtx = callback_ctx;
            result->recvMsg.respHeader = NULL;
            result->recvMsg.statusCode = 0;
            result->recvMsg.recvState = state_initial;
            result->recvMsg.storedBytes = NULL;
            result->recvMsg.storedLen = 0;
            result->recvMsg.totalBodyLen = 0;
            result->chunkRequest = false;
            result->traceOn = false;
            result->hostName = NULL;
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
        if (data != NULL)
        {
            if (data->xioHandle != NULL)
            {
                (void)xio_destroy(data->xioHandle);
            }

            /* Codes_SRS_UHTTP_07_005: [http_client_destroy shall free any memory that is allocated in this translation unit] */
            free(data);
        }
    }
}

HTTP_CLIENT_RESULT http_client_open(HTTP_CLIENT_HANDLE handle, const char* host)
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
        mallocAndStrcpy_s(&data->hostName, host);
        if (xio_open(data->xioHandle, onOpenComplete, data, onBytesReceived, data, onIoError, data) != 0)
        {
            /* Codes_SRS_UHTTP_07_007: [http_client_connect shall attempt to open the xioHandle and shall return HTTP_CLIENT_OPEN_REQUEST_FAILED if a failure is encountered] */
            result = HTTP_CLIENT_OPEN_REQUEST_FAILED;
        }
        else
        {
            /* Codes_SRS_UHTTP_07_008: [If http_client_connect succeeds then it shall return HTTP_CLIENT_OK] */
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

        data->recvMsg.statusCode = 0;
        data->recvMsg.recvState = state_initial;
        data->recvMsg.storedBytes = NULL;
        data->recvMsg.storedLen = 0;
        data->recvMsg.totalBodyLen = 0;
        free(data->hostName);

        /* Codes_SRS_UHTTP_07_011: [http_client_close shall free any HTTPHeader object that has not been freed] */
        if (data->recvMsg.respHeader != NULL)
        {
            HTTPHeaders_Free(data->recvMsg.respHeader);
            data->recvMsg.respHeader = NULL;
        }
    }
}

HTTP_CLIENT_RESULT http_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const uint8_t* content, size_t contentLength)
{
    HTTP_CLIENT_RESULT result;

    /* Codes_SRS_UHTTP_07_012: [If handle, relativePath, or httpHeadersHandle is NULL then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG] */
    if (handle == NULL || relativePath == NULL || 
        (content != NULL && contentLength == 0) || (content == NULL && contentLength != 0) )
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("Invalid parameter sent to execute_request");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        data->recvMsg.statusCode = 0;
        data->recvMsg.recvState = state_initial;
        data->recvMsg.storedBytes = NULL;
        data->recvMsg.storedLen = 0;
        data->recvMsg.totalBodyLen = 0;
        data->recvMsg.respHeader = HTTPHeaders_Alloc();
        if (data->recvMsg.respHeader == NULL)
        {
            /* Codes_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
            LogError("Failure allocating http data items");
            HTTPHeaders_Free(data->recvMsg.respHeader);
            data->recvMsg.respHeader = NULL;
            result = HTTP_CLIENT_ERROR;
        }
        else
        {
            if (SendHttpData(data, requestType, relativePath, httpHeadersHandle, contentLength, false) != 0)
            {
                HTTPHeaders_Free(data->recvMsg.respHeader);
                data->recvMsg.respHeader = NULL;
                result = HTTP_CLIENT_ERROR;
            }
            else
            {
                if (content != NULL && contentLength != 0)
                {
                    /* Tests_SRS_UHTTP_07_018: [upon success http_client_execute_request shall then transmit the content data, if supplied, through a call to xio_send.] */
                    if (WriteDataLine(data, content, contentLength) != 0)
                    {
                        HTTPHeaders_Free(data->recvMsg.respHeader);
                        data->recvMsg.respHeader = NULL;
                        result = HTTP_CLIENT_ERROR;
                        LogError("Failure writing content buffer");
                    }
                    else
                    {
                        result = HTTP_CLIENT_OK;
                    }
                }
                else
                {
                    result = HTTP_CLIENT_OK;
                }
            }
        }
    }
    return result;
}

HTTP_CLIENT_RESULT http_client_start_chunk_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || relativePath == NULL || httpHeadersHandle == NULL)
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("Invalid parameter sent to http_client_execute_chunk");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        data->recvMsg.statusCode = 0;
        data->recvMsg.recvState = state_initial;
        data->recvMsg.storedBytes = NULL;
        data->recvMsg.storedLen = 0;
        data->recvMsg.totalBodyLen = 0;
        data->chunkRequest = true;
        data->recvMsg.respHeader = HTTPHeaders_Alloc();
        if (data->recvMsg.respHeader == NULL)
        {
            /* Codes_SRS_UHTTP_07_017: [If any failure encountered http_client_execute_request shall return HTTP_CLIENT_ERROR] */
            LogError("Failure allocating http data items");
            HTTPHeaders_Free(data->recvMsg.respHeader);
            data->recvMsg.respHeader = NULL;
            result = HTTP_CLIENT_HTTP_HEADERS_FAILED;
        }
        else
        {
            if (SendHttpData(data, requestType, relativePath, httpHeadersHandle, 0, true) != 0)
            {
                result = HTTP_CLIENT_ERROR;
            }
            else
            {
                result = HTTP_CLIENT_OK;
            }
        }
    }
    return result;
}

HTTP_CLIENT_RESULT http_client_send_chunk_request(HTTP_CLIENT_HANDLE handle, const uint8_t* content, size_t contentLength)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL || content == NULL || contentLength == 0)
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("Invalid parameter sent to http_client_send_data_chunk");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        if (data->chunkRequest != true)
        {
            result = HTTP_CLIENT_INVALID_CHUNK_REQUEST;
            LogError("Invalid parameter sent to http_client_send_data_chunk");
        }
        else
        {
            char contentSize[32];
            sprintf(contentSize, "%x\r\n", (unsigned int)contentLength);
            if (WriteTextLine(data, contentSize) != 0)
            {
                result = HTTP_CLIENT_SEND_REQUEST_FAILED;
                LogError("Failure writing content buffer");
            }
            else if (WriteDataLine(data, content, contentLength) != 0)
            {
                result = HTTP_CLIENT_SEND_REQUEST_FAILED;
                LogError("Failure writing content buffer");
            }
            else if (WriteTextLine(data, HTTP_CRLF_VALUE) != 0)
            {
                result = HTTP_CLIENT_SEND_REQUEST_FAILED;
                LogError("Failure writing content buffer");
            }
            else
            {
                result = HTTP_CLIENT_OK;
            }
        }
    }
    return result;
}

HTTP_CLIENT_RESULT http_client_end_chunk_request(HTTP_CLIENT_HANDLE handle)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL)
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("Invalid parameter sent to execute_request");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        if (data->chunkRequest != true)
        {
            result = HTTP_CLIENT_INVALID_CHUNK_REQUEST;
            LogError("Invalid parameter sent to execute_request");
        }
        else
        {
            char contentSize[32];
            strcpy(contentSize, "0\r\n\r\n");
            if (WriteTextLine(data, contentSize) != 0)
            {
                result = HTTP_CLIENT_SEND_REQUEST_FAILED;
                LogError("Failure writing content buffer");
            }
            else
            {
                data->chunkRequest = false;
                result = HTTP_CLIENT_OK;
            }
        }
    }
    return result;
}

void http_client_dowork(HTTP_CLIENT_HANDLE handle)
{
    if (handle != NULL)
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;

        xio_dowork(data->xioHandle);
    }
}

HTTP_CLIENT_RESULT http_client_set_trace(HTTP_CLIENT_HANDLE handle, bool traceOn)
{
    HTTP_CLIENT_RESULT result;
    if (handle == NULL)
    {
        result = HTTP_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to http_client_set_trace");
    }
    else
    {
        HTTP_CLIENT_HANDLE_DATA* data = (HTTP_CLIENT_HANDLE_DATA*)handle;
        data->traceOn = traceOn;
        result = HTTP_CLIENT_OK;
    }
    return result;
}
