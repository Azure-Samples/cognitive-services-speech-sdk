// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/shared_util_options.h"

/*Codes_SRS_HTTPAPI_COMPACT_21_001: [ The httpapi_compact shall implement the methods defined by the `httpapi.h`. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_002: [ The httpapi_compact shall support the http requests. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_003: [ The httpapi_compact shall return error codes defined by HTTPAPI_RESULT. ]*/
#include "httpapi.h"

#define MAX_HOSTNAME     64
#define TEMP_BUFFER_SIZE 1024

/*Codes_SRS_HTTPAPI_COMPACT_21_077: [ The HTTPAPI_ExecuteRequest shall wait, at least, 10 seconds for the SSL open process. ]*/
#define MAX_OPEN_RETRY   100
/*Codes_SRS_HTTPAPI_COMPACT_21_084: [ The HTTPAPI_CloseConnection shall wait, at least, 10 seconds for the SSL close process. ]*/
#define MAX_CLOSE_RETRY   100
/*Codes_SRS_HTTPAPI_COMPACT_21_079: [ The HTTPAPI_ExecuteRequest shall wait, at least, 20 seconds to send a buffer using the SSL connection. ]*/
#define MAX_SEND_RETRY   200
/*Codes_SRS_HTTPAPI_COMPACT_21_081: [ The HTTPAPI_ExecuteRequest shall try to read the message with the response up to 20 seconds. ]*/
#define MAX_RECEIVE_RETRY   200
/*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
#define RETRY_INTERVAL_IN_MICROSECONDS  100

DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES)

typedef struct HTTP_HANDLE_DATA_TAG
{
    char*           certificate;
    char*           x509ClientCertificate;
    char*           x509ClientPrivateKey;
    char*           tlsIoVersion;

    XIO_HANDLE      xio_handle;
    size_t          received_bytes_count;
    unsigned char*  received_bytes;
    unsigned int    is_io_error : 1;
    unsigned int    is_connected : 1;
    unsigned int    send_completed : 1;
} HTTP_HANDLE_DATA;

/*the following function does the same as sscanf(pos2, "%d", &sec)*/
/*this function only exists because some of platforms do not have sscanf. */
static int ParseStringToDecimal(const char *src, int* dst)
{
    int result;
    char* next;
    long num = strtol(src, &next, 0);
    if (src == next || num < INT_MIN || num > INT_MAX)
    {
        result = EOF;
    }
    else
    {
        result = 1;
    }
    if (num < INT_MIN) num = INT_MIN;
    if (num > INT_MAX) num = INT_MAX;
    *dst = (int)num;
    return result;
}

/*the following function does the same as sscanf(pos2, "%x", &sec)*/
/*this function only exists because some of platforms do not have sscanf. This is not a full implementation; it only works with well-defined x numbers. */
#define HEXA_DIGIT_VAL(c)         (((c>='0') && (c<='9')) ? (c-'0') : ((c>='a') && (c<='f')) ? (c-'a'+10) : ((c>='A') && (c<='F')) ? (c-'A'+10) : -1)
static int ParseStringToHexadecimal(const char *src, size_t* dst)
{
    int result;
    int digitVal;
    if (src == NULL)
    {
        result = EOF;
    }
    else if (HEXA_DIGIT_VAL(*src) == -1)
    {
        result = EOF;
    }
    else
    {
        (*dst) = 0;
        while ((digitVal = HEXA_DIGIT_VAL(*src)) != -1)
        {
            (*dst) *= 0x10;
            (*dst) += (size_t)digitVal;
            src++;
        }
        result = 1;
    }
    return result;
}

/*the following function does the same as sscanf(buf, "HTTP/%*d.%*d %d %*[^\r\n]", &ret) */
/*this function only exists because some of platforms do not have sscanf. This is not a full implementation; it only works with well-defined HTTP response. */
static int  ParseHttpResponse(const char* src, int* dst)
{
    int result;
    static const char HTTPPrefix[] = "HTTP/";
    bool fail;
    const char* runPrefix;

    if ((src == NULL) || (dst == NULL))
    {
        result = EOF;
    }
    else
    {
        fail = false;
        runPrefix = HTTPPrefix;

        while((*runPrefix) != '\0')
        {
            if ((*runPrefix) != (*src))
            {
                fail = true;
                break;
            }
            src++;
            runPrefix++;
        }

        if (!fail)
        {
            while ((*src) != '.')
            {
                if ((*src) == '\0')
                {
                    fail = true;
                    break;
                }
                src++;
            }
        }

        if (!fail)
        {
            while ((*src) != ' ')
            {
                if ((*src) == '\0')
                {
                    fail = true;
                    break;
                }
                src++;
            }
        }

        if (fail)
        {
            result = EOF;
        }
        else
        {
            result = ParseStringToDecimal(src, dst);
        }
    }

    return result;
}

HTTPAPI_RESULT HTTPAPI_Init(void)
{
/*Codes_SRS_HTTPAPI_COMPACT_21_004: [ The HTTPAPI_Init shall allocate all memory to control the http protocol. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_007: [ If there is not enough memory to control the http protocol, the HTTPAPI_Init shall return HTTPAPI_ALLOC_FAILED. ]*/
/**
 * No memory is necessary.
 */

 /*Codes_SRS_HTTPAPI_COMPACT_21_006: [ If HTTPAPI_Init succeed allocating all the needed memory, it shall return HTTPAPI_OK. ]*/
return HTTPAPI_OK;
}

void HTTPAPI_Deinit(void)
{
    /*Codes_SRS_HTTPAPI_COMPACT_21_009: [ The HTTPAPI_Init shall release all memory allocated by the httpapi_compact. ]*/
    /**
    * No memory was necessary.
    */
}

/*Codes_SRS_HTTPAPI_COMPACT_21_011: [ The HTTPAPI_CreateConnection shall create an http connection to the host specified by the hostName parameter. ]*/
HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    HTTP_HANDLE_DATA* http_instance;
    TLSIO_CONFIG tlsio_config;

    if (hostName == NULL)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_014: [ If the hostName is NULL, the HTTPAPI_CreateConnection shall return NULL as the handle. ]*/
        LogError("Invalid host name. Null hostName parameter.");
        http_instance = NULL;
    }
    else if (*hostName == '\0')
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_015: [ If the hostName is empty, the HTTPAPI_CreateConnection shall return NULL as the handle. ]*/
        LogError("Invalid host name. Empty string.");
        http_instance = NULL;
    }
    else
    {
        http_instance = (HTTP_HANDLE_DATA*)malloc(sizeof(HTTP_HANDLE_DATA));
        /*Codes_SRS_HTTPAPI_COMPACT_21_013: [ If there is not enough memory to control the http connection, the HTTPAPI_CreateConnection shall return NULL as the handle. ]*/
        if (http_instance == NULL)
        {
            LogError("There is no memory to control the http connection");
        }
        else
        {
            tlsio_config.hostname = hostName;
            tlsio_config.port = 443;
            tlsio_config.underlying_io_interface = NULL;
            tlsio_config.underlying_io_parameters = NULL;

            http_instance->xio_handle = xio_create(platform_get_default_tlsio(), (void*)&tlsio_config);

            /*Codes_SRS_HTTPAPI_COMPACT_21_016: [ If the HTTPAPI_CreateConnection failed to create the connection, it shall return NULL as the handle. ]*/
            if (http_instance->xio_handle == NULL)
            {
                LogError("Create connection failed");
                free(http_instance);
                http_instance = NULL;
            }
            else
            {
                http_instance->is_connected = 0;
                http_instance->is_io_error = 0;
                http_instance->received_bytes_count = 0;
                http_instance->received_bytes = NULL;
                http_instance->certificate = NULL;
                http_instance->x509ClientCertificate = NULL;
                http_instance->x509ClientPrivateKey = NULL;
                http_instance->tlsIoVersion = NULL;
            }
        }
    }

    /*Codes_SRS_HTTPAPI_COMPACT_21_012: [ The HTTPAPI_CreateConnection shall return a non-NULL handle on success. ]*/
    return (HTTP_HANDLE)http_instance;
}

static void on_io_close_complete(void* context)
{
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)context;

    if (http_instance != NULL)
    {
        http_instance->is_connected = 0;
    }
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)handle;

    /*Codes_SRS_HTTPAPI_COMPACT_21_020: [ If the connection handle is NULL, the HTTPAPI_CloseConnection shall not do anything. ]*/
    if (http_instance != NULL)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_019: [ If there is no previous connection, the HTTPAPI_CloseConnection shall not do anything. ]*/
        if (http_instance->xio_handle != NULL)
        {
            http_instance->is_io_error = 0;
            /*Codes_SRS_HTTPAPI_COMPACT_21_017: [ The HTTPAPI_CloseConnection shall close the connection previously created in HTTPAPI_ExecuteRequest. ]*/
            if (xio_close(http_instance->xio_handle, on_io_close_complete, http_instance) != 0)
            {
                LogError("The SSL got error closing the connection");
                /*Codes_SRS_HTTPAPI_COMPACT_21_087: [ If the xio return anything different than 0, the HTTPAPI_CloseConnection shall destroy the connection anyway. ]*/
                http_instance->is_connected = 0;
            }
            else
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_084: [ The HTTPAPI_CloseConnection shall wait, at least, 10 seconds for the SSL close process. ]*/
                int countRetry = MAX_CLOSE_RETRY;
                while (http_instance->is_connected == 1)
                {
                    xio_dowork(http_instance->xio_handle);
                    if ((countRetry--) < 0)
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_085: [ If the HTTPAPI_CloseConnection retries 10 seconds to close the connection without success, it shall destroy the connection anyway. ]*/
                        LogError("Close timeout. The SSL didn't close the connection");
                        http_instance->is_connected = 0;
                    }
                    else if (http_instance->is_io_error == 1)
                    {
                        LogError("The SSL got error closing the connection");
                        http_instance->is_connected = 0;
                    }
                    else if (http_instance->is_connected == 1)
                    {
                        LogInfo("Waiting for TLS close connection");
                        /*Codes_SRS_HTTPAPI_COMPACT_21_086: [ The HTTPAPI_CloseConnection shall wait, at least, 100 milliseconds between retries. ]*/
                        ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
                    }
                }
            }
            /*Codes_SRS_HTTPAPI_COMPACT_21_076: [ After close the connection, The HTTPAPI_CloseConnection shall destroy the connection previously created in HTTPAPI_CreateConnection. ]*/
            xio_destroy(http_instance->xio_handle);
        }

        /*Codes_SRS_HTTPAPI_COMPACT_21_018: [ If there is a certificate associated to this connection, the HTTPAPI_CloseConnection shall free all allocated memory for the certificate. ]*/
        if (http_instance->certificate)
        {
            free(http_instance->certificate);
        }

        /*Codes_SRS_HTTPAPI_COMPACT_06_001: [ If there is a x509 client certificate associated to this connection, the HTTAPI_CloseConnection shall free all allocated memory for the certificate. ]*/
        if (http_instance->x509ClientCertificate)
        {
            free(http_instance->x509ClientCertificate);
        }

        /*Codes_SRS_HTTPAPI_COMPACT_06_002: [ If there is a x509 client private key associated to this connection, then HTTP_CloseConnection shall free all the allocated memory for the private key. ]*/
        if (http_instance->x509ClientPrivateKey)
        {
            free(http_instance->x509ClientPrivateKey);
        }
        if (http_instance->tlsIoVersion)
        {
            free(http_instance->tlsIoVersion);
        }
        free(http_instance);
    }
}

static void on_io_open_complete(void* context, IO_OPEN_RESULT_DETAILED open_result_detailed)
{
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)context;
    IO_OPEN_RESULT open_result = open_result_detailed.result;

    if (http_instance != NULL)
    {
        if (open_result == IO_OPEN_OK)
        {
            http_instance->is_connected = 1;
            http_instance->is_io_error = 0;
        }
        else
        {
            http_instance->is_io_error = 1;
        }
    }
}

static void on_send_complete(void* context, IO_SEND_RESULT send_result)
{
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)context;

    if (http_instance != NULL)
    {
        if (send_result == IO_SEND_OK)
        {
            http_instance->send_completed = 1;
            http_instance->is_io_error = 0;
        }
        else
        {
            http_instance->is_io_error = 1;
        }
    }
}

#define TOLOWER(c) (((c>='A') && (c<='Z'))?c-'A'+'a':c)
static int InternStrnicmp(const char* s1, const char* s2, size_t n)
{
    int result;

    if (s1 == NULL) result = -1;
    else if (s2 == NULL) result = 1;
    else
    {
        result = 0;

        while(n-- && result == 0)
        {
            if (*s1 == 0) result = -1;
            else if (*s2 == 0) result = 1;
            else
            {

                result = TOLOWER(*s1) - TOLOWER(*s2);
                ++s1;
                ++s2;
            }
        }
    }

    return result;
}

static void on_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    unsigned char* new_received_bytes;
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)context;

    if (http_instance != NULL)
    {

        if (buffer == NULL)
        {
            http_instance->is_io_error = 1;
            LogError("NULL pointer error");
        }
        else
        {
            /* Here we got some bytes so we'll buffer them so the receive functions can consumer it */
            new_received_bytes = (unsigned char*)realloc(http_instance->received_bytes, http_instance->received_bytes_count + size);
            if (new_received_bytes == NULL)
            {
                http_instance->is_io_error = 1;
                LogError("Error allocating memory for received data");
            }
            else
            {
                http_instance->received_bytes = new_received_bytes;
                if (memcpy(http_instance->received_bytes + http_instance->received_bytes_count, buffer, size) == NULL)
                {
                    http_instance->is_io_error = 1;
                    LogError("Error copping received data to the HTTP bufffer");
                }
                else
                {
                    http_instance->received_bytes_count += size;
                }
            }
        }
    }
}

static void on_io_error(void* context)
{
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)context;
    if (http_instance != NULL)
    {
        http_instance->is_io_error = 1;
        LogError("Error signalled by underlying IO");
    }
}

static int conn_receive(HTTP_HANDLE_DATA* http_instance, char* buffer, int count)
{
    int result;

    if ((http_instance == NULL) || (buffer == NULL) || (count < 0))
    {
        LogError("conn_receive: %s", ((http_instance == NULL) ? "Invalid HTTP instance" : "Invalid HTTP buffer"));
        result = -1;
    }
    else
    {
        result = 0;
        while (result < count)
        {
            xio_dowork(http_instance->xio_handle);

            /* if any error was detected while receiving then simply break and report it */
            if (http_instance->is_io_error != 0)
            {
                LogError("xio reported error on dowork");
                result = -1;
                break;
            }

            if (http_instance->received_bytes_count >= (size_t)count)
            {
                /* Consuming bytes from the receive buffer */
                (void)memcpy(buffer, http_instance->received_bytes, count);
                (void)memmove(http_instance->received_bytes, http_instance->received_bytes + count, http_instance->received_bytes_count - count);
                http_instance->received_bytes_count -= count;

                /* we're not reallocating at each consumption so that we don't trash due to byte by byte consumption */
                if (http_instance->received_bytes_count == 0)
                {
                    free(http_instance->received_bytes);
                    http_instance->received_bytes = NULL;
                }

                result = count;
                break;
            }

            /*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
            ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
        }
    }

    return result;
}

static void conn_receive_discard_buffer(HTTP_HANDLE_DATA* http_instance)
{
    if (http_instance != NULL)
    {
        if (http_instance->received_bytes != NULL)
        {
            free(http_instance->received_bytes);
            http_instance->received_bytes = NULL;
        }
        http_instance->received_bytes_count = 0;
    }
}

static int readLine(HTTP_HANDLE_DATA* http_instance, char* buf, const size_t maxBufSize)
{
    int resultLineSize;

    if ((http_instance == NULL) || (buf == NULL) || (maxBufSize == 0))
    {
        LogError("%s", ((http_instance == NULL) ? "Invalid HTTP instance" : "Invalid HTTP buffer"));
        resultLineSize = -1;
    }
    else
    {
        char* destByte = buf;
        /*Codes_SRS_HTTPAPI_COMPACT_21_081: [ The HTTPAPI_ExecuteRequest shall try to read the message with the response up to 20 seconds. ]*/
        int countRetry = MAX_RECEIVE_RETRY;
        bool endOfSearch = false;
        resultLineSize = -1;
        while (!endOfSearch)
        {
            xio_dowork(http_instance->xio_handle);

            /* if any error was detected while receiving then simply break and report it */
            if (http_instance->is_io_error != 0)
            {
                LogError("xio reported error on dowork");
                endOfSearch = true;
            }
            else
            {
                unsigned char* receivedByte = http_instance->received_bytes;
                while (receivedByte < (http_instance->received_bytes + http_instance->received_bytes_count))
                {
                    if ((*receivedByte) != '\r')
                    {
                        (*destByte) = (*receivedByte);
                        destByte++;
                        receivedByte++;

                        if (destByte >= (buf + maxBufSize - 1))
                        {
                            LogError("Received message is bigger than the http buffer");
                            receivedByte = http_instance->received_bytes + http_instance->received_bytes_count;
                            endOfSearch = true;
                            break;
                        }
                    }
                    else
                    {
                        receivedByte++;
                        if ((receivedByte < (http_instance->received_bytes + http_instance->received_bytes_count)) && ((*receivedByte) == '\n'))
                        {
                            receivedByte++;
                        }
                        (*destByte) = '\0';
                        resultLineSize = (int)(destByte - buf);
                        endOfSearch = true;
                        break;
                    }
                }

                http_instance->received_bytes_count -= (receivedByte - http_instance->received_bytes);
                if (http_instance->received_bytes_count != 0)
                {
                    (void)memmove(http_instance->received_bytes, receivedByte, http_instance->received_bytes_count);
                }
                else
                {
                    conn_receive_discard_buffer(http_instance);
                }
            }

            if (!endOfSearch)
            {
                if ((countRetry--) > 0)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
                    ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
                }
                else
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                    LogError("Receive timeout. The HTTP request is incomplete");
                    endOfSearch = true;
                }
            }
        }
    }

    return resultLineSize;
}

static int readChunk(HTTP_HANDLE_DATA* http_instance, char* buf, size_t size)
{
    int cur, offset;

    // read content with specified length, even if it is received
    // only in chunks due to fragmentation in the networking layer.
    // returns -1 in case of error.
    offset = 0;
    while (size > (size_t)0)
    {
        cur = conn_receive(http_instance, buf + offset, (int)size);

        // end of stream reached
        if (cur == 0)
        {
            break;
        }

        // read cur bytes (might be less than requested)
        size -= (size_t)cur;
        offset += cur;
    }

    return offset;
}

static int skipN(HTTP_HANDLE_DATA* http_instance, size_t n)
{
    // read and abandon response content with specified length
    // returns -1 in case of error.

    int result;

    if (http_instance == NULL)
    {
        LogError("Invalid HTTP instance");
        result = -1;
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_081: [ The HTTPAPI_ExecuteRequest shall try to read the message with the response up to 20 seconds. ]*/
        int countRetry = MAX_RECEIVE_RETRY;
        result = (int)n;
        while (n > 0)
        {
            xio_dowork(http_instance->xio_handle);

            /* if any error was detected while receiving then simply break and report it */
            if (http_instance->is_io_error != 0)
            {
                LogError("xio reported error on dowork");
                result = -1;
                n = 0;
            }
            else
            {
                if (http_instance->received_bytes_count <= n)
                {
                    n -= http_instance->received_bytes_count;
                    http_instance->received_bytes_count = 0;
                }
                else
                {
                    http_instance->received_bytes_count -= n;
                    (void)memmove(http_instance->received_bytes, http_instance->received_bytes + n, http_instance->received_bytes_count);
                    n = 0;
                }

                if (n > 0)
                {
                    if ((countRetry--) > 0)
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
                        ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
                    }
                    else
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                        LogError("Receive timeout. The HTTP request is incomplete");
                        n = 0;
                        result = -1;
                    }
                }
            }
        }
    }

    return result;
}


/*Codes_SRS_HTTPAPI_COMPACT_21_021: [ The HTTPAPI_ExecuteRequest shall execute the http communtication with the provided host, sending a request and reciving the response. ]*/
static HTTPAPI_RESULT OpenXIOConnection(HTTP_HANDLE_DATA* http_instance)
{
    HTTPAPI_RESULT result;

    if (http_instance->is_connected != 0)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;
    }
    else
    {
        http_instance->is_io_error = 0;

        if ((http_instance->tlsIoVersion != NULL) &&
            (xio_setoption(http_instance->xio_handle, OPTION_TLS_VERSION, http_instance->tlsIoVersion) != 0))
        {
            result = HTTPAPI_SET_OPTION_FAILED;
            LogInfo("Could not set TLS IO version");
        }

        /*Codes_SRS_HTTPAPI_COMPACT_21_022: [ If a Certificate was provided, the HTTPAPI_ExecuteRequest shall set this option on the transport layer. ]*/
        if ((http_instance->certificate != NULL) &&
            (xio_setoption(http_instance->xio_handle, OPTION_TRUSTED_CERT, http_instance->certificate) != 0))
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_023: [ If the transport failed setting the Certificate, the HTTPAPI_ExecuteRequest shall not send any request and return HTTPAPI_SET_OPTION_FAILED. ]*/
            result = HTTPAPI_SET_OPTION_FAILED;
            LogInfo("Could not load certificate");
        }
        /*Codes_SRS_HTTPAPI_COMPACT_06_003: [ If the x509 client certificate is provided, the HTTPAPI_ExecuteRequest shall set this option on the transport layer. ]*/
        else if ((http_instance->x509ClientCertificate != NULL) &&
            (xio_setoption(http_instance->xio_handle, SU_OPTION_X509_CERT, http_instance->x509ClientCertificate) != 0))
        {
            /*Codes_SRS_HTTPAPI_COMPACT_06_005: [ If the transport failed setting the client certificate, the HTTPAPI_ExecuteRequest shall not send any request and return HTTPAPI_SET_OPTION_FAILED. ]*/
            result = HTTPAPI_SET_OPTION_FAILED;
            LogInfo("Could not load the client certificate");
        }
        else if ((http_instance->x509ClientPrivateKey != NULL) &&
            (xio_setoption(http_instance->xio_handle, SU_OPTION_X509_PRIVATE_KEY, http_instance->x509ClientPrivateKey) != 0))
        {

            /*Codes_SRS_HTTPAPI_COMPACT_06_006: [ If the transport failed setting the client certificate private key, the HTTPAPI_ExecuteRequest shall not send any request and return HTTPAPI_SET_OPTION_FAILED. ] */
            result = HTTPAPI_SET_OPTION_FAILED;
            LogInfo("Could not load the client certificate private key");
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_024: [ The HTTPAPI_ExecuteRequest shall open the transport connection with the host to send the request. ]*/
            if (xio_open(http_instance->xio_handle, on_io_open_complete, http_instance, on_bytes_received, http_instance, on_io_error, http_instance) != 0)
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_025: [ If the open process failed, the HTTPAPI_ExecuteRequest shall not send any request and return HTTPAPI_OPEN_REQUEST_FAILED. ]*/
                result = HTTPAPI_OPEN_REQUEST_FAILED;
            }
            else
            {
                int countRetry;
                /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
                result = HTTPAPI_OK;
                /*Codes_SRS_HTTPAPI_COMPACT_21_077: [ The HTTPAPI_ExecuteRequest shall wait, at least, 10 seconds for the SSL open process. ]*/
                countRetry = MAX_OPEN_RETRY;
                while ((http_instance->is_connected == 0) &&
                    (http_instance->is_io_error == 0))
                {
                    xio_dowork(http_instance->xio_handle);
                    LogInfo("Waiting for TLS connection");
                    if ((countRetry--) < 0)
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_078: [ If the HTTPAPI_ExecuteRequest cannot open the connection in 10 seconds, it shall fail and return HTTPAPI_OPEN_REQUEST_FAILED. ]*/
                        LogError("Open timeout. The HTTP request is incomplete");
                        result = HTTPAPI_OPEN_REQUEST_FAILED;
                        break;
                    }
                    else
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
                        ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
                    }
                }
            }
        }
    }

    if ((http_instance->is_io_error != 0) && (result == HTTPAPI_OK))
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_025: [ If the open process failed, the HTTPAPI_ExecuteRequest shall not send any request and return HTTPAPI_OPEN_REQUEST_FAILED. ]*/
        result = HTTPAPI_OPEN_REQUEST_FAILED;
    }

    return result;
}

static HTTPAPI_RESULT conn_send_all(HTTP_HANDLE_DATA* http_instance, const unsigned char* buf, size_t bufLen)
{
    HTTPAPI_RESULT result;

    http_instance->send_completed = 0;
    http_instance->is_io_error = 0;
    if (xio_send(http_instance->xio_handle, buf, bufLen, on_send_complete, http_instance) != 0)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_028: [ If the HTTPAPI_ExecuteRequest cannot send the request header, it shall return HTTPAPI_HTTP_HEADERS_FAILED. ]*/
        result = HTTPAPI_SEND_REQUEST_FAILED;
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_079: [ The HTTPAPI_ExecuteRequest shall wait, at least, 20 seconds to send a buffer using the SSL connection. ]*/
        int countRetry = MAX_SEND_RETRY;
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;
        while ((http_instance->send_completed == 0) && (result == HTTPAPI_OK))
        {
            xio_dowork(http_instance->xio_handle);
            if (http_instance->is_io_error != 0)
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_028: [ If the HTTPAPI_ExecuteRequest cannot send the request header, it shall return HTTPAPI_HTTP_HEADERS_FAILED. ]*/
                result = HTTPAPI_SEND_REQUEST_FAILED;
            }
            else if ((countRetry--) <= 0)
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_080: [ If the HTTPAPI_ExecuteRequest retries to send the message for 20 seconds without success, it shall fail and return HTTPAPI_SEND_REQUEST_FAILED. ]*/
                LogError("Send timeout. The HTTP request is incomplete");
                /*Codes_SRS_HTTPAPI_COMPACT_21_028: [ If the HTTPAPI_ExecuteRequest cannot send the request header, it shall return HTTPAPI_HTTP_HEADERS_FAILED. ]*/
                result = HTTPAPI_SEND_REQUEST_FAILED;
            }
            else
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_083: [ The HTTPAPI_ExecuteRequest shall wait, at least, 100 milliseconds between retries. ]*/
                ThreadAPI_Sleep(RETRY_INTERVAL_IN_MICROSECONDS);
            }
        }
    }

    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_035: [ The HTTPAPI_ExecuteRequest shall execute resquest for types `GET`, `POST`, `PUT`, `DELETE`, `PATCH`. ]*/
const char httpapiRequestString[5][7] = { "GET", "POST", "PUT", "DELETE", "PATCH" };
const char* get_request_type(HTTPAPI_REQUEST_TYPE requestType)
{
    return (const char*)httpapiRequestString[requestType];
}

/*Codes_SRS_HTTPAPI_COMPACT_21_026: [ If the open process succeed, the HTTPAPI_ExecuteRequest shall send the request message to the host. ]*/
static HTTPAPI_RESULT SendHeadsToXIO(HTTP_HANDLE_DATA* http_instance, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle, size_t headersCount)
{
    HTTPAPI_RESULT result;
    char    buf[TEMP_BUFFER_SIZE];
    int     ret;

    //Send request
    /*Codes_SRS_HTTPAPI_COMPACT_21_038: [ The HTTPAPI_ExecuteRequest shall execute the resquest for the path in relativePath parameter. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_036: [ The request type shall be provided in the parameter requestType. ]*/
    if (((ret = snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\n", get_request_type(requestType), relativePath)) < 0) ||
        (ret >= sizeof(buf)))
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_027: [ If the HTTPAPI_ExecuteRequest cannot create a buffer to send the request, it shall not send any request and return HTTPAPI_STRING_PROCESSING_ERROR. ]*/
        result = HTTPAPI_STRING_PROCESSING_ERROR;
    }
        /*Codes_SRS_HTTPAPI_COMPACT_21_028: [ If the HTTPAPI_ExecuteRequest cannot send the request header, it shall return HTTPAPI_HTTP_HEADERS_FAILED. ]*/
    else if ((result = conn_send_all(http_instance, (const unsigned char*)buf, strlen(buf))) == HTTPAPI_OK)
    {
        size_t i;
        //Send default headers
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        for (i = 0; ((i < headersCount) && (result == HTTPAPI_OK)); i++)
        {
            char* header;
            if (HTTPHeaders_GetHeader(httpHeadersHandle, i, &header) != HTTP_HEADERS_OK)
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_027: [ If the HTTPAPI_ExecuteRequest cannot create a buffer to send the request, it shall not send any request and return HTTPAPI_STRING_PROCESSING_ERROR. ]*/
                result = HTTPAPI_STRING_PROCESSING_ERROR;
            }
            else
            {
                if ((result = conn_send_all(http_instance, (const unsigned char*)header, strlen(header))) == HTTPAPI_OK)
                {
                    result = conn_send_all(http_instance, (const unsigned char*)"\r\n", (size_t)2);
                }
                free(header);
            }
        }

        //Close headers
        if (result == HTTPAPI_OK)
        {
            result = conn_send_all(http_instance, (const unsigned char*)"\r\n", (size_t)2);
        }
    }
    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_042: [ The request can contain the a content message, provided in content parameter. ]*/
static HTTPAPI_RESULT SendContentToXIO(HTTP_HANDLE_DATA* http_instance, const unsigned char* content, size_t contentLength)
{
    HTTPAPI_RESULT result;

    //Send data (if available)
    /*Codes_SRS_HTTPAPI_COMPACT_21_045: [ If the contentLength is lower than one, the HTTPAPI_ExecuteRequest shall send the request without content. ]*/
    if (content && contentLength > 0)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_044: [ If the content is not NULL, the number of bytes in the content shall be provided in contentLength parameter. ]*/
        result = conn_send_all(http_instance, content, contentLength);
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_043: [ If the content is NULL, the HTTPAPI_ExecuteRequest shall send the request without content. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;
    }
    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_030: [ At the end of the transmission, the HTTPAPI_ExecuteRequest shall receive the response from the host. ]*/
static HTTPAPI_RESULT ReceiveHeaderFromXIO(HTTP_HANDLE_DATA* http_instance, unsigned int* statusCode)
{
    HTTPAPI_RESULT result;
    char    buf[TEMP_BUFFER_SIZE];
    int     ret;

    http_instance->is_io_error = 0;

    //Receive response
    if (readLine(http_instance, buf, TEMP_BUFFER_SIZE) < 0)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
        result = HTTPAPI_READ_DATA_FAILED;
    }
    //Parse HTTP response
    else if (ParseHttpResponse(buf, &ret) != 1)
    {
        //Cannot match string, error
        /*Codes_SRS_HTTPAPI_COMPACT_21_055: [ If the HTTPAPI_ExecuteRequest cannot parser the received message, it shall return HTTPAPI_RECEIVE_RESPONSE_FAILED. ]*/
        LogInfo("Not a correct HTTP answer");
        result = HTTPAPI_RECEIVE_RESPONSE_FAILED;
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_046: [ The HTTPAPI_ExecuteRequest shall return the http status reported by the host in the received response. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_048: [ If the statusCode is NULL, the HTTPAPI_ExecuteRequest shall report not report any status. ]*/
        if (statusCode)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_047: [ The HTTPAPI_ExecuteRequest shall report the status in the statusCode parameter. ]*/
            *statusCode = ret;
        }
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;
    }

    return result;
}

static HTTPAPI_RESULT ReceiveContentInfoFromXIO(HTTP_HANDLE_DATA* http_instance, HTTP_HEADERS_HANDLE responseHeadersHandle, size_t* bodyLength, bool* chunked)
{
    HTTPAPI_RESULT result;
    char    buf[TEMP_BUFFER_SIZE];
    const char* substr;
    char* whereIsColon;
    int lengthInMsg;
    const char ContentLength[] = "content-length:";
    const size_t ContentLengthSize = sizeof(ContentLength) - 1;
    const char TransferEncoding[] = "transfer-encoding:";
    const size_t TransferEncodingSize = sizeof(TransferEncoding) - 1;
    const char Chunked[] = "chunked";
    const size_t ChunkedSize = sizeof(Chunked) - 1;

    http_instance->is_io_error = 0;

    //Read HTTP response headers
    if (readLine(http_instance, buf, sizeof(buf)) < 0)
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
        result = HTTPAPI_READ_DATA_FAILED;
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;

        while (*buf && (result == HTTPAPI_OK))
        {
            if (InternStrnicmp(buf, ContentLength, ContentLengthSize) == 0)
            {
                substr = buf + ContentLengthSize;
                if (ParseStringToDecimal(substr, &lengthInMsg) != 1)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
                    result = HTTPAPI_READ_DATA_FAILED;
                }
                else
                {
                    (*bodyLength) = (size_t)lengthInMsg;
                }
            }
            else if (InternStrnicmp(buf, TransferEncoding, TransferEncodingSize) == 0)
            {
                substr = buf + TransferEncodingSize;
#ifdef _MSC_VER
#pragma warning( push )
// disable: error 6330:  : 'const char' passed as _Param_(1) when 'unsigned char' is required in call to 'isspace'.
#pragma warning( disable : 6330 )
#endif
                while (isspace(*substr)) substr++;
#ifdef _MSC_VER
#pragma warning( pop )
#endif

                if (InternStrnicmp(substr, Chunked, ChunkedSize) == 0)
                {
                    (*chunked) = true;
                }
            }

            if (result == HTTPAPI_OK)
            {
                whereIsColon = strchr((char*)buf, ':');
                /*Codes_SRS_HTTPAPI_COMPACT_21_049: [ If responseHeadersHandle is provide, the HTTPAPI_ExecuteRequest shall prepare a Response Header usign the HTTPHeaders_AddHeaderNameValuePair. ]*/
                if (whereIsColon && (responseHeadersHandle != NULL))
                {
                    *whereIsColon = '\0';
                    HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, buf, whereIsColon + 1);
                }

                if (readLine(http_instance, buf, sizeof(buf)) < 0)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
                    /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                    result = HTTPAPI_READ_DATA_FAILED;
                }
            }
        }
    }

    return result;
}

static HTTPAPI_RESULT ReadHTTPResponseBodyFromXIO(HTTP_HANDLE_DATA* http_instance, size_t bodyLength, bool chunked, BUFFER_HANDLE responseContent, ON_CHUNK_RECEIVED on_chunk_received, void* on_chunk_received_context)
{
    HTTPAPI_RESULT result;
    char    buf[TEMP_BUFFER_SIZE];
    const unsigned char* receivedContent;

    http_instance->is_io_error = 0;

    //Read HTTP response body
    if (!chunked)
    {
        if (bodyLength)
        {
            if (responseContent != NULL)
            {
                if (BUFFER_pre_build(responseContent, bodyLength) != 0)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_052: [ If any memory allocation get fail, the HTTPAPI_ExecuteRequest shall return HTTPAPI_ALLOC_FAILED. ]*/
                    result = HTTPAPI_ALLOC_FAILED;
                }
                else if (BUFFER_content(responseContent, &receivedContent) != 0)
                {
                    (void)BUFFER_unbuild(responseContent);

                    /*Codes_SRS_HTTPAPI_COMPACT_21_052: [ If any memory allocation get fail, the HTTPAPI_ExecuteRequest shall return HTTPAPI_ALLOC_FAILED. ]*/
                    result = HTTPAPI_ALLOC_FAILED;
                }
                else if (readChunk(http_instance, (char*)receivedContent, bodyLength) < 0)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
                    result = HTTPAPI_READ_DATA_FAILED;
                }
                else
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
                    result = HTTPAPI_OK;

                    if (on_chunk_received != NULL)
                    {
                        on_chunk_received(on_chunk_received_context, receivedContent, bodyLength);
                    }
                }
            }
            else
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_051: [ If the responseContent is NULL, the HTTPAPI_ExecuteRequest shall ignore any content in the response. ]*/
                if (skipN(http_instance, bodyLength) < 0)
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                    result = HTTPAPI_READ_DATA_FAILED;
                }
                else
                {
                    result = HTTPAPI_OK;
                }
            }
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
            result = HTTPAPI_OK;
        }
    }
    else
    {
        size_t size = 0;
        /*Codes_SRS_HTTPAPI_COMPACT_21_033: [ If the whole process succeed, the HTTPAPI_ExecuteRequest shall retur HTTPAPI_OK. ]*/
        result = HTTPAPI_OK;
        while (result == HTTPAPI_OK)
        {
            size_t chunkSize;
            if (readLine(http_instance, buf, sizeof(buf)) < 0)    // read [length in hex]/r/n
            {
                /*Codes_SRS_HTTPAPI_COMPACT_21_032: [ If the HTTPAPI_ExecuteRequest cannot read the message with the request result, it shall return HTTPAPI_READ_DATA_FAILED. ]*/
                /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                result = HTTPAPI_READ_DATA_FAILED;
            }
            else if (ParseStringToHexadecimal(buf, &chunkSize) != 1)     // chunkSize is length of next line (/r/n is not counted)
            {
                //Cannot match string, error
                /*Codes_SRS_HTTPAPI_COMPACT_21_055: [ If the HTTPAPI_ExecuteRequest cannot parser the received message, it shall return HTTPAPI_RECEIVE_RESPONSE_FAILED. ]*/
                result = HTTPAPI_RECEIVE_RESPONSE_FAILED;
            }
            else if (chunkSize == 0)
            {
                // 0 length means next line is just '\r\n' and end of chunks
                if (readChunk(http_instance, (char*)buf, (size_t)2) < 0
                    || buf[0] != '\r' || buf[1] != '\n') // skip /r/n
                {
                    (void)BUFFER_unbuild(responseContent);

                    result = HTTPAPI_READ_DATA_FAILED;
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

                        /*Codes_SRS_HTTPAPI_COMPACT_21_052: [ If any memory allocation get fail, the HTTPAPI_ExecuteRequest shall return HTTPAPI_ALLOC_FAILED. ]*/
                        result = HTTPAPI_ALLOC_FAILED;
                    }
                    else if (BUFFER_content(responseContent, &receivedContent) != 0)
                    {
                        (void)BUFFER_unbuild(responseContent);

                        /*Codes_SRS_HTTPAPI_COMPACT_21_052: [ If any memory allocation get fail, the HTTPAPI_ExecuteRequest shall return HTTPAPI_ALLOC_FAILED. ]*/
                        result = HTTPAPI_ALLOC_FAILED;
                    }
                    else if (readChunk(http_instance, (char*)receivedContent + size, chunkSize) < 0)
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                    }
                    else if (on_chunk_received != NULL)
                    {
                        on_chunk_received(on_chunk_received_context, receivedContent + size, chunkSize);
                    }
                }
                else
                {
                    /*Codes_SRS_HTTPAPI_COMPACT_21_051: [ If the responseContent is NULL, the HTTPAPI_ExecuteRequest shall ignore any content in the response. ]*/
                    if (skipN(http_instance, chunkSize) < 0)
                    {
                        /*Codes_SRS_HTTPAPI_COMPACT_21_082: [ If the HTTPAPI_ExecuteRequest retries 20 seconds to receive the message without success, it shall fail and return HTTPAPI_READ_DATA_FAILED. ]*/
                        result = HTTPAPI_READ_DATA_FAILED;
                    }
                }

                if (result == HTTPAPI_OK)
                {
                    if (readChunk(http_instance, (char*)buf, (size_t)2) < 0
                        || buf[0] != '\r' || buf[1] != '\n') // skip /r/n
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                    }

                    size += chunkSize;
                }
            }
        }

    }
    return result;
}


/*Codes_SRS_HTTPAPI_COMPACT_21_037: [ If the request type is unknown, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
static bool validRequestType(HTTPAPI_REQUEST_TYPE requestType)
{
    bool result;

    if ((requestType == HTTPAPI_REQUEST_GET) ||
        (requestType == HTTPAPI_REQUEST_POST) ||
        (requestType == HTTPAPI_REQUEST_PUT) ||
        (requestType == HTTPAPI_REQUEST_DELETE) ||
        (requestType == HTTPAPI_REQUEST_PATCH))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_021: [ The HTTPAPI_ExecuteRequest shall execute the http communtication with the provided host, sending a request and reciving the response. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_050: [ If there is a content in the response, the HTTPAPI_ExecuteRequest shall copy it in the responseContent buffer. ]*/
//Note: This function assumes that "Host:" and "Content-Length:" headers are setup
//      by the caller of HTTPAPI_ExecuteRequest() (which is true for httptransport.c).
HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content,
    size_t contentLength, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent,
    ON_CHUNK_RECEIVED on_chunk_received, void* on_chunk_received_context)
{
    HTTPAPI_RESULT result = HTTPAPI_ERROR;
    size_t  headersCount;
    size_t  bodyLength = 0;
    bool    chunked = false;
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)handle;

    /*Codes_SRS_HTTPAPI_COMPACT_21_034: [ If there is no previous connection, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_037: [ If the request type is unknown, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_039: [ If the relativePath is NULL or invalid, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_041: [ If the httpHeadersHandle is NULL or invalid, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_053: [ The HTTPAPI_ExecuteRequest shall produce a set of http header to send to the host. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_040: [ The request shall contain the http header provided in httpHeadersHandle parameter. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_054: [ If Http header maker cannot provide the number of headers, the HTTPAPI_ExecuteRequest shall return HTTPAPI_INVALID_ARG. ]*/
    if (http_instance == NULL ||
        relativePath == NULL ||
        httpHeadersHandle == NULL ||
        !validRequestType(requestType) ||
        HTTPHeaders_GetHeaderCount(httpHeadersHandle, &headersCount) != HTTP_HEADERS_OK)
    {
        result = HTTPAPI_INVALID_ARG;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_024: [ The HTTPAPI_ExecuteRequest shall open the transport connection with the host to send the request. ]*/
    else if ((result = OpenXIOConnection(http_instance)) != HTTPAPI_OK)
    {
        LogError("Open HTTP connection failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_026: [ If the open process succeed, the HTTPAPI_ExecuteRequest shall send the request message to the host. ]*/
    else if ((result = SendHeadsToXIO(http_instance, requestType, relativePath, httpHeadersHandle, headersCount)) != HTTPAPI_OK)
    {
        LogError("Send heads to HTTP failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_042: [ The request can contain the a content message, provided in content parameter. ]*/
    else if ((result = SendContentToXIO(http_instance, content, contentLength)) != HTTPAPI_OK)
    {
        LogError("Send content to HTTP failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_030: [ At the end of the transmission, the HTTPAPI_ExecuteRequest shall receive the response from the host. ]*/
    /*Codes_SRS_HTTPAPI_COMPACT_21_073: [ The message received by the HTTPAPI_ExecuteRequest shall starts with a valid header. ]*/
    else if ((result = ReceiveHeaderFromXIO(http_instance, statusCode)) != HTTPAPI_OK)
    {
        LogError("Receive header from HTTP failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_074: [ After the header, the message received by the HTTPAPI_ExecuteRequest can contain addition information about the content. ]*/
    else if ((result = ReceiveContentInfoFromXIO(http_instance, responseHeadersHandle, &bodyLength, &chunked)) != HTTPAPI_OK)
    {
        LogError("Receive content information from HTTP failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }
    /*Codes_SRS_HTTPAPI_COMPACT_21_075: [ The message received by the HTTPAPI_ExecuteRequest can contain a body with the message content. ]*/
    else if ((result = ReadHTTPResponseBodyFromXIO(http_instance, bodyLength, chunked, responseContent, on_chunk_received, on_chunk_received_context)) != HTTPAPI_OK)
    {
        LogError("Read HTTP response body from HTTP failed (result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
    }

    conn_receive_discard_buffer(http_instance);


    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_056: [ The HTTPAPI_SetOption shall change the HTTP options. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_057: [ The HTTPAPI_SetOption shall receive a handle that identiry the HTTP connection. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_058: [ The HTTPAPI_SetOption shall receive the option as a pair optionName/value. ]*/
HTTPAPI_RESULT HTTPAPI_SetOption(HTTP_HANDLE handle, const char* optionName, const void* value)
{
    HTTPAPI_RESULT result;
    HTTP_HANDLE_DATA* http_instance = (HTTP_HANDLE_DATA*)handle;

    if (
        (http_instance == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_059: [ If the handle is NULL, the HTTPAPI_SetOption shall return HTTPAPI_INVALID_ARG. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_060: [ If the optionName is NULL, the HTTPAPI_SetOption shall return HTTPAPI_INVALID_ARG. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_061: [ If the value is NULL, the HTTPAPI_SetOption shall return HTTPAPI_INVALID_ARG. ]*/
        result = HTTPAPI_INVALID_ARG;
    }
    else if (strcmp("TrustedCerts", optionName) == 0)
    {
        int len;

        if (http_instance->certificate)
        {
            free(http_instance->certificate);
        }

        len = (int)strlen((char*)value);
        http_instance->certificate = (char*)malloc((len + 1) * sizeof(char));
        if (http_instance->certificate == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_062: [ If any memory allocation get fail, the HTTPAPI_SetOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
            LogInfo("unable to allocate memory for the certificate in HTTPAPI_SetOption");
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_064: [ If the HTTPAPI_SetOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(http_instance->certificate, len + 1, (const char*)value);
            result = HTTPAPI_OK;
        }
    }
    else if (strcmp(OPTION_TLS_VERSION, optionName) == 0)
    {
        if (http_instance->tlsIoVersion)
        {
            free(http_instance->tlsIoVersion);
        }
        int len;
        len = (int)strlen((char*)value);
        http_instance->tlsIoVersion = (char*)malloc((len + 1) * sizeof(char));
        if (http_instance->tlsIoVersion == NULL)
        {
            result = HTTPAPI_ALLOC_FAILED;
            LogInfo("unable to allocate memory for the TLS IO version in HTTPAPI_SetOption");
        }
        else
        {
            (void)strcpy_s(http_instance->tlsIoVersion, len + 1, (const char*)value);
            result = HTTPAPI_OK;
        }
    }
    else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0)
    {
        int len;
        if (http_instance->x509ClientCertificate)
        {
            free(http_instance->x509ClientCertificate);
        }

        len = (int)strlen((char*)value);
        http_instance->x509ClientCertificate = (char*)malloc((len + 1) * sizeof(char));
        if (http_instance->x509ClientCertificate == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_062: [ If any memory allocation get fail, the HTTPAPI_SetOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
            LogInfo("unable to allocate memory for the client certificate in HTTPAPI_SetOption");
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_064: [ If the HTTPAPI_SetOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(http_instance->x509ClientCertificate, len + 1, (const char*)value);
            result = HTTPAPI_OK;
        }
    }
    else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0)
    {
        int len;
        if (http_instance->x509ClientPrivateKey)
        {
            free(http_instance->x509ClientPrivateKey);
        }

        len = (int)strlen((char*)value);
        http_instance->x509ClientPrivateKey = (char*)malloc((len + 1) * sizeof(char));
        if (http_instance->x509ClientPrivateKey == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_062: [ If any memory allocation get fail, the HTTPAPI_SetOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
            LogInfo("unable to allocate memory for the client private key in HTTPAPI_SetOption");
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_064: [ If the HTTPAPI_SetOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(http_instance->x509ClientPrivateKey, len + 1, (const char*)value);
            result = HTTPAPI_OK;
        }
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_063: [ If the HTTP do not support the optionName, the HTTPAPI_SetOption shall return HTTPAPI_INVALID_ARG. ]*/
        result = HTTPAPI_INVALID_ARG;
        LogInfo("unknown option %s", optionName);
    }
    return result;
}

/*Codes_SRS_HTTPAPI_COMPACT_21_065: [ The HTTPAPI_CloneOption shall provide the means to clone the HTTP option. ]*/
/*Codes_SRS_HTTPAPI_COMPACT_21_066: [ The HTTPAPI_CloneOption shall return a clone of the value identified by the optionName. ]*/
HTTPAPI_RESULT HTTPAPI_CloneOption(const char* optionName, const void* value, const void** savedValue)
{
    HTTPAPI_RESULT result;
    size_t certLen;
    char* tempCert;

    if (
        (optionName == NULL) ||
        (value == NULL) ||
        (savedValue == NULL)
        )
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_067: [ If the optionName is NULL, the HTTPAPI_CloneOption shall return HTTPAPI_INVALID_ARG. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_068: [ If the value is NULL, the HTTPAPI_CloneOption shall return HTTPAPI_INVALID_ARG. ]*/
        /*Codes_SRS_HTTPAPI_COMPACT_21_069: [ If the savedValue is NULL, the HTTPAPI_CloneOption shall return HTTPAPI_INVALID_ARG. ]*/
        result = HTTPAPI_INVALID_ARG;
    }
    else if (strcmp("TrustedCerts", optionName) == 0)
    {
        certLen = strlen((const char*)value);
        tempCert = (char*)malloc((certLen + 1) * sizeof(char));
        if (tempCert == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_070: [ If any memory allocation get fail, the HTTPAPI_CloneOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_072: [ If the HTTPAPI_CloneOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(tempCert, certLen + 1, (const char*)value);
            *savedValue = tempCert;
            result = HTTPAPI_OK;
        }
    }
    else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0)
    {
        certLen = strlen((const char*)value);
        tempCert = (char*)malloc((certLen + 1) * sizeof(char));
        if (tempCert == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_070: [ If any memory allocation get fail, the HTTPAPI_CloneOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_072: [ If the HTTPAPI_CloneOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(tempCert, certLen + 1, (const char*)value);
            *savedValue = tempCert;
            result = HTTPAPI_OK;
        }
    }
    else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0)
    {
        certLen = strlen((const char*)value);
        tempCert = (char*)malloc((certLen + 1) * sizeof(char));
        if (tempCert == NULL)
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_070: [ If any memory allocation get fail, the HTTPAPI_CloneOption shall return HTTPAPI_ALLOC_FAILED. ]*/
            result = HTTPAPI_ALLOC_FAILED;
        }
        else
        {
            /*Codes_SRS_HTTPAPI_COMPACT_21_072: [ If the HTTPAPI_CloneOption get success setting the option, it shall return HTTPAPI_OK. ]*/
            (void)strcpy_s(tempCert, certLen + 1, (const char*)value);
            *savedValue = tempCert;
            result = HTTPAPI_OK;
        }
    }
    else
    {
        /*Codes_SRS_HTTPAPI_COMPACT_21_071: [ If the HTTP do not support the optionName, the HTTPAPI_CloneOption shall return HTTPAPI_INVALID_ARG. ]*/
        result = HTTPAPI_INVALID_ARG;
        LogInfo("unknown option %s", optionName);
    }
    return result;
}
