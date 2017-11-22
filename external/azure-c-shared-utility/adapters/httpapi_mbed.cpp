// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "mbed.h"
#include "EthernetInterface.h"
#include "azure_c_shared_utility/wolfssl_connection.h"
#include "azure_c_shared_utility/iot_logging.h"
#include <string.h>

#define MAX_HOSTNAME     64
#define TEMP_BUFFER_SIZE 4096

#define CHAR_COUNT(A)   (sizeof(A) - 1)

DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES)

class HTTP_HANDLE_DATA
{
public:
    char          host[MAX_HOSTNAME];
    char* certificate;
    WolfSSLConnection con;
};

HTTPAPI_RESULT HTTPAPI_Init(void)
{
    time_t ctTime;
    ctTime = time(NULL);
    HTTPAPI_RESULT result;
    LogInfo("HTTAPI_Init::Time is now (UTC) %s", ctime(&ctTime));

    result = HTTPAPI_OK;

    return result;
}

void HTTPAPI_Deinit(void)
{
    ;
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE_DATA* handle = NULL;

    if (hostName)
    {
        handle = new HTTP_HANDLE_DATA();
        if (strcpy_s(handle->host, MAX_HOSTNAME, hostName) != 0)
        {
            LogError("HTTPAPI_CreateConnection::Could not strcpy_s");
            delete handle;
            handle = NULL;
        }
        else
        {
            handle->certificate = NULL;
        }
    }
    else
    {
        LogInfo("HTTPAPI_CreateConnection:: null hostName parameter");
    }

    return (HTTP_HANDLE)handle;
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTP_HANDLE_DATA* h = (HTTP_HANDLE_DATA*)handle;

    if (h)
    {
        if (h->con.is_connected())
        {
            LogInfo("HTTPAPI_CloseConnection  h->con.close(); to %s", h->host);
            h->con.close();
        }
        if (h->certificate)
        {
            delete[] h->certificate;
        }
        delete h;
    }
}

static int readLine(WolfSSLConnection* con, char* buf, const size_t size)
{
    // reads until \r\n is encountered. writes in buf all the characters
    // read until \r\n and returns the number of characters in the buffer.
    char* p = buf;
    char  c;
    if (con->receive(&c, 1) < 0)
        return -1;
    while (c != '\r') {
        if ((p - buf + 1) >= (int)size)
            return -1;
        *p++ = c;
        if (con->receive(&c, 1) < 0)
            return -1;
    }
    *p = 0;
    if (con->receive(&c, 1) < 0 || c != '\n') // skip \n
        return -1;
    return p - buf;
}

static int readChunk(WolfSSLConnection* con, char* buf, size_t size)
{
    size_t cur, offset;

    // read content with specified length, even if it is received
    // only in chunks due to fragmentation in the networking layer.
    // returns -1 in case of error.
    offset = 0;
    while (size > 0)
    {
        cur = con->receive(buf + offset, size);

        // end of stream reached
        if (cur == 0)
            return offset;

        // read cur bytes (might be less than requested)
        size -= cur;
        offset += cur;
    }

    return offset;
}

static int skipN(WolfSSLConnection* con, size_t n, char* buf, size_t size)
{
    size_t org = n;
    // read and abandon response content with specified length
    // returns -1 in case of error.
    while (n > size)
    {
        if (readChunk(con, (char*)buf, size) < 0)
            return -1;

        n -= size;
    }

    if (readChunk(con, (char*)buf, n) < 0)
        return -1;

    return org;
}

//Note: This function assumes that "Host:" and "Content-Length:" headers are setup
//      by the caller of HTTPAPI_ExecuteRequest() (which is true for httptransport.c).
HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content,
    size_t contentLength, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{

    HTTPAPI_RESULT result;
    size_t  headersCount;
    char    buf[TEMP_BUFFER_SIZE];
    int     ret;
    WolfSSLConnection* con = NULL;
    size_t  bodyLength = 0;
    bool    chunked = false;
    const unsigned char* receivedContent;

    const char* method = (requestType == HTTPAPI_REQUEST_GET) ? "GET"
        : (requestType == HTTPAPI_REQUEST_POST) ? "POST"
        : (requestType == HTTPAPI_REQUEST_PUT) ? "PUT"
        : (requestType == HTTPAPI_REQUEST_DELETE) ? "DELETE"
        : (requestType == HTTPAPI_REQUEST_PATCH) ? "PATCH"
        : NULL;

    if (handle == NULL ||
        relativePath == NULL ||
        httpHeadersHandle == NULL ||
        method == NULL ||
        HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    HTTP_HANDLE_DATA* httpHandle = (HTTP_HANDLE_DATA*)handle;
    con = &(httpHandle->con);

    if (!con->is_connected())
    {
        // Load the certificate
        if ((httpHandle->certificate != NULL) &&
			(!con->load_certificate((const unsigned char*)httpHandle->certificate, strlen(httpHandle->certificate) + 1)))
        {
            result = HTTPAPI_ERROR;
            LogError("Could not load certificate (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }

        // Make the connection
        if (con->connect(httpHandle->host, 443) != 0)
        {
            result = HTTPAPI_ERROR;
            LogError("Could not connect (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
    }

    //Send request
    if ((ret = snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\n", method, relativePath)) < 0
        || ret >= sizeof(buf))
    {
        result = HTTPAPI_STRING_PROCESSING_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }
    if (con->send_all(buf, strlen(buf)) < 0)
    {
        result = HTTPAPI_SEND_REQUEST_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    //Send default headers
    for (size_t i = 0; i < headersCount; i++)
    {
        char* header;
        if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &header) != HTTP_HEADERS_OK)
        {
            result = HTTPAPI_HTTP_HEADERS_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
        if (con->send_all(header, strlen(header)) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            free(header);
            goto exit;
        }
        if (con->send_all("\r\n", 2) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            free(header);
            goto exit;
        }
        free(header);
    }

    //Close headers
    if (con->send_all("\r\n", 2) < 0)
    {
        result = HTTPAPI_SEND_REQUEST_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    //Send data (if available)
    if (content && contentLength > 0)
    {
        if (con->send_all((char*)content, contentLength) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
    }

    //Receive response
    if (readLine(con, buf, sizeof(buf)) < 0)
    {
        result = HTTPAPI_READ_DATA_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    //Parse HTTP response
    if (sscanf(buf, "HTTP/%*d.%*d %d %*[^\r\n]", &ret) != 1)
    {
        //Cannot match string, error
        LogInfo("HTTPAPI_ExecuteRequest::Not a correct HTTP answer=%s", buf);
        result = HTTPAPI_READ_DATA_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }
    if (statusCode)
        *statusCode = ret;

    //Read HTTP response headers
    if (readLine(con, buf, sizeof(buf)) < 0)
    {
        result = HTTPAPI_READ_DATA_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    while (buf[0])
    {
        const char ContentLength[] = "content-length:";
        const char TransferEncoding[] = "transfer-encoding:";

        if (strncasecmp(buf, ContentLength, CHAR_COUNT(ContentLength)) == 0)
        {
            if (sscanf(buf + CHAR_COUNT(ContentLength), " %d", &bodyLength) != 1)
            {
                result = HTTPAPI_READ_DATA_FAILED;
                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                goto exit;
            }
        }
        else if (strncasecmp(buf, TransferEncoding, CHAR_COUNT(TransferEncoding)) == 0)
        {
            const char* p = buf + CHAR_COUNT(TransferEncoding);
            while (isspace(*p)) p++;
            if (strcasecmp(p, "chunked") == 0)
                chunked = true;
        }

        char* whereIsColon = strchr((char*)buf, ':');
        if (whereIsColon && responseHeadersHandle != NULL)
        {
            *whereIsColon = '\0';
            HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, buf, whereIsColon + 1);
        }

        if (readLine(con, buf, sizeof(buf)) < 0)
        {
            result = HTTPAPI_READ_DATA_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
    }

    //Read HTTP response body
    if (!chunked)
    {
        if (bodyLength)
        {
            if (responseContent != NULL)
            {
                if (BUFFER_pre_build(responseContent, bodyLength) != 0)
                {
                    result = HTTPAPI_ALLOC_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                }
                else if (BUFFER_content(responseContent, &receivedContent) != 0)
                {
                    (void)BUFFER_unbuild(responseContent);

                    result = HTTPAPI_ALLOC_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                }

                if (readChunk(con, (char*)receivedContent, bodyLength) < 0)
                {
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    goto exit;
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
            else
            {
                (void)skipN(con, bodyLength, buf, sizeof(buf));
                result = HTTPAPI_OK;
            }
        }
        else
        {
            result = HTTPAPI_OK;
        }
    }
    else
    {
        size_t size = 0;
        result = HTTPAPI_OK;
        for (;;)
        {
            int chunkSize;
            if (readLine(con, buf, sizeof(buf)) < 0)    // read [length in hex]/r/n
            {
                result = HTTPAPI_READ_DATA_FAILED;
                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                goto exit;
            }
            if (sscanf(buf, "%x", &chunkSize) != 1)     // chunkSize is length of next line (/r/n is not counted)
            {
                //Cannot match string, error
                result = HTTPAPI_RECEIVE_RESPONSE_FAILED;
                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                goto exit;
            }

            if (chunkSize == 0)
            {
                // 0 length means next line is just '\r\n' and end of chunks
                if (readChunk(con, (char*)buf, 2) < 0
                    || buf[0] != '\r' || buf[1] != '\n') // skip /r/n
                {
                    (void)BUFFER_unbuild(responseContent);

                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    goto exit;
                }
                break;
            }
            else
            {
                if (responseContent != NULL)
                {
                    if (BUFFER_enlarge(responseContent, chunkSize) != 0)
                    {
                        (void)BUFFER_unbuild(responseContent);

                        result = HTTPAPI_ALLOC_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    }
                    else if (BUFFER_content(responseContent, &receivedContent) != 0)
                    {
                        (void)BUFFER_unbuild(responseContent);

                        result = HTTPAPI_ALLOC_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    }

                    if (readChunk(con, (char*)receivedContent + size, chunkSize) < 0)
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                        goto exit;
                    }
                }
                else
                {
                    if (skipN(con, chunkSize, buf, sizeof(buf)) < 0)
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                        goto exit;
                    }
                }

                if (readChunk(con, (char*)buf, 2) < 0
                    || buf[0] != '\r' || buf[1] != '\n') // skip /r/n
                {
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    goto exit;
                }
                size += chunkSize;
            }
        }

    }

exit:
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
    else if (strcmp("TrustedCerts", optionName) == 0)
    {
        HTTP_HANDLE_DATA* h = (HTTP_HANDLE_DATA*)handle;
        if (h->certificate)
        {
            delete[] h->certificate;
        }

        int len = strlen((char*)value);
        h->certificate = new char[len + 1];
        if (h->certificate == NULL)
        {
            result = HTTPAPI_ERROR;
            LogError("unable to allocate certificate memory in HTTPAPI_SetOption");
        }
        else
        {
            (void)strcpy(h->certificate, (const char*)value);
            result = HTTPAPI_OK;
        }
    }
    else
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("unknown option %s", optionName);
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
    else if (strcmp("TrustedCerts", optionName) == 0)
    {
        size_t certLen = strlen((const char*)value);
        char* tempCert = (char*)malloc(certLen+1);
        if (tempCert == NULL)
        {
            result = HTTPAPI_INVALID_ARG;
            LogError("unable to allocate certificate memory in HTTPAPI_CloneOption");
        }
        else
        {
            (void)strcpy(tempCert, (const char*)value);
            *savedValue = tempCert;
            result = HTTPAPI_OK;
        }
    }
    else
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("unknown option %s", optionName);
    }
    return result;
}
