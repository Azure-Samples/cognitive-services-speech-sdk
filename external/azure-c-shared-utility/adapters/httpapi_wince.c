// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include <string.h>
#include "certs.h"
#include "winsock2.h"
#include "sslsock.h"
#include "schnlsp.h"

#define MAX_HOSTNAME     64
#define TEMP_BUFFER_SIZE 4096

#define CHAR_COUNT(A)   (sizeof(A) - 1)

DEFINE_ENUM_STRINGS(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES)

// data structure that contains all the information required to manage a connection
class HTTP_HANDLE_DATA
{
public:
    char   host[MAX_HOSTNAME];
    SOCKET _sock_fd;
    struct sockaddr_in _remoteHost;
};

// Thanks to Luca Calligaris for his article:
// https://lcalligaris.wordpress.com/2011/04/07/implementing-a-secure-socket/

#define SSL_OPT_NO_CHECK  0
#define SSL_OPT_ISSUER_CHECK 1
#define SSL_OPT_STRICT_CHECK 2

#define SSL_OPT_DEFAULT   SSL_OPT_STRICT_CHECK

static int g_SslOptions = SSL_OPT_DEFAULT;

// callback called every time a certificate needs to be validated, depending on g_SslOptions global var it can perform
// a strict validation (default) or just validate any certificate (may be needed for testing and debugging, not in production)
int CALLBACK SSLValidateCertHook(DWORD  dwType, LPVOID pvArg,
    DWORD  dwChainLen, LPBLOB pCertChain, DWORD dwFlags)
{
    HCERTSTORE hCertStore = NULL;
    PCCERT_CONTEXT pCertInStore = NULL, pCertPassed = NULL, pPrevCertContext = NULL;
    const LPCTSTR SystemStore[] = { _T("CA"), _T("ROOT"), _T("MY") };
    unsigned int i;
    int retcode = SSL_ERR_NO_CERT;

    //The only thing we can handle
    if (dwType != SSL_CERT_X509)
        return SSL_ERR_CERT_UNKNOWN;

    //Do not check the certificate at all
    if (g_SslOptions == SSL_OPT_NO_CHECK)
        return SSL_ERR_OKAY;

    //Get a context struct from the certificate blob
    pCertPassed = CertCreateCertificateContext(X509_ASN_ENCODING, pCertChain->pBlobData, pCertChain->cbSize);

    if (!pCertPassed)
        return SSL_ERR_FAILED;

    //Check for a matching certificate in the stores
    for (i = 0; i<(sizeof(SystemStore) / sizeof(SystemStore[0])); i++)
    {
        //open the store
        hCertStore = CertOpenSystemStore((HCRYPTPROV)NULL, SystemStore[i]);

        if (!hCertStore)
            continue;

        pPrevCertContext = NULL;

        do
        {
            //Find a certificate from the same issuer
            pCertInStore = CertFindCertificateInStore(hCertStore, X509_ASN_ENCODING,
                0, CERT_FIND_ISSUER_NAME, &pCertPassed->pCertInfo->Issuer, pPrevCertContext);

            if (pCertInStore)
            {
                //It's enough if we have a certificate from the same issuer
                if (g_SslOptions == SSL_OPT_ISSUER_CHECK)
                {
                    retcode = SSL_ERR_OKAY;
                    break;
                }
                else if (g_SslOptions == SSL_OPT_STRICT_CHECK)
                {
                    //Compare (exact match) the certificate with the one we have found
                    if (CertCompareCertificate(X509_ASN_ENCODING, pCertInStore->pCertInfo, pCertPassed->pCertInfo))
                    {
                        retcode = SSL_ERR_OKAY;
                        break;
                    }
                }
            }

            pPrevCertContext = pCertInStore;
        } while (pCertInStore);

        CertCloseStore(hCertStore, 0);

        if (retcode != SSL_ERR_NO_CERT)
            break;
    }

    if (pCertInStore)
        CertFreeCertificateContext(pCertInStore);

    CertFreeCertificateContext(pCertPassed);

    return retcode;
}

// Initializes SSL layer, called only once on initialization
int SSLInit(SOCKET s, int SslOptions)
{
    DWORD optval = SO_SEC_SSL;
    SSLVALIDATECERTHOOK hook;
    SSLPROTOCOLS protocolsToUse;
    int ret;

    g_SslOptions = SslOptions;//WSAIoctl cannot handle the pointer copy, use a global variable

    if (setsockopt(s, SOL_SOCKET, SO_SECURE, (LPSTR)&optval, sizeof(optval)) ==
        SOCKET_ERROR)
    {
        goto SSLInit_Error;
    }

    hook.HookFunc = SSLValidateCertHook;
    hook.pvArg = NULL; //WSAIoctl cannot handle the pointer copy, use a global variable

    if (WSAIoctl(s, SO_SSL_SET_VALIDATE_CERT_HOOK,
        &hook, sizeof(SSLVALIDATECERTHOOK),
        NULL, 0, NULL, NULL, NULL) == SOCKET_ERROR)
    {
        goto SSLInit_Error;
    }

    //specify TLS1 protocol

    protocolsToUse.dwCount = 1;
    protocolsToUse.ProtocolList[0].dwFlags = 0;
    protocolsToUse.ProtocolList[0].dwProtocol = SSL_PROTOCOL_TLS1;
    protocolsToUse.ProtocolList[0].dwVersion = 0;

    if (WSAIoctl(s, SO_SSL_SET_PROTOCOLS,
        (LPVOID)&protocolsToUse, sizeof(protocolsToUse),
        NULL, 0, NULL, NULL, NULL) == SOCKET_ERROR)
    {
        goto SSLInit_Error;
    }

    return 0;

SSLInit_Error:

    ret = WSAGetLastError();
    return ret;
}

HTTPAPI_RESULT HTTPAPI_Init(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    LogInfo("HTTPAPI_Init::Start");
    time_t ctTime;
    ctTime = time(NULL);

    LogInfo("HTTAPI_Init::Time is now (UTC) %s", ctime(&ctTime));

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return HTTPAPI_INIT_FAILED;
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return HTTPAPI_INIT_FAILED;
    }


    LogInfo("HTTPAPI_Init::End");
    return HTTPAPI_OK;
}

void HTTPAPI_Deinit(void)
{
    WSACleanup();
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    LogInfo("HTTPAPI_CreateConnection::Start");
    HTTP_HANDLE_DATA* handle = NULL;

    if (hostName)
    {
        LogInfo("HTTPAPI_CreateConnection::Connecting to %s", hostName);
        handle = new HTTP_HANDLE_DATA();
        if (strcpy_s(handle->host, MAX_HOSTNAME, hostName) != 0)
        {
            LogError("HTTPAPI_CreateConnection::Could not strcpy_s");
            delete handle;
            handle = NULL;
        }
        else
        {
            handle->_sock_fd = -1;
        }
    }
    else
    {
        LogInfo("HTTPAPI_CreateConnection:: null hostName parameter");
    }
    LogInfo("HTTPAPI_CreateConnection::End");

    return (HTTP_HANDLE)handle;
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTP_HANDLE_DATA* h = (HTTP_HANDLE_DATA*)handle;

    if (h)
    {
        LogInfo("HTTPAPI_CloseConnection to %s", h->host);
        if (h->_sock_fd != -1)
        {
            LogInfo("HTTPAPI_CloseConnection to %s", h->host);
            closesocket(h->_sock_fd);
            h->_sock_fd = -1;
        }
        LogInfo("HTTPAPI_CloseConnection (delete h) to %s", h->host);
        delete h;
    }
}

static int readLine(SOCKET _sock_fd, char* buf, const size_t size)
{
    // reads until \r\n is encountered. writes in buf all the characters
    // read until \r\n and returns the number of characters in the buffer.
    char* p = buf;
    char  c;
    if (recv(_sock_fd, &c, 1, 0) < 0)
        return -1;
    while (c != '\r') {
        if ((p - buf + 1) >= (int)size)
            return -1;
        *p++ = c;
        if (recv(_sock_fd, &c, 1, 0) < 0)
            return -1;
    }
    *p = 0;
    if (recv(_sock_fd, &c, 1, 0) < 0 || c != '\n') // skip \n
        return -1;
    return p - buf;
}

static int readChunk(SOCKET _sock_fd, char* buf, size_t size)
{
    size_t cur, offset;

    // read content with specified length, even if it is received
    // only in chunks due to fragmentation in the networking layer.
    // returns -1 in case of error.
    offset = 0;
    while (size > 0)
    {
        cur = recv(_sock_fd, buf + offset, size, 0);

        // end of stream reached
        if (cur == 0)
            return offset;

        // read cur bytes (might be less than requested)
        size -= cur;
        offset += cur;
    }

    return offset;
}

static int skipN(SOCKET _sock_fd, size_t n, char* buf, size_t size)
{
    size_t org = n;
    // read and abandon response content with specified length
    // returns -1 in case of error.
    while (n > size)
    {
        if (readChunk(_sock_fd, (char*)buf, size) < 0)
            return -1;

        n -= size;
    }

    if (readChunk(_sock_fd, (char*)buf, n) < 0)
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
    LogInfo("HTTPAPI_ExecuteRequest::Start");

    HTTPAPI_RESULT result;
    size_t  headersCount;
    char    buf[TEMP_BUFFER_SIZE];
    int     ret;
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

    if (httpHandle->_sock_fd == -1)
    {
        // Make the connection
        if ((httpHandle->_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            LogError("init_socket failed");
            result = HTTPAPI_INIT_FAILED;
            goto exit;
        }
        else
        {
            // IP Address
            char address[5];
            char *p_address = address;

            memset(&httpHandle->_remoteHost, 0, sizeof(struct sockaddr_in));

            // Dot-decimal notation
            int result = sscanf(httpHandle->host, "%3u.%3u.%3u.%3u",
                (unsigned int*)&address[0], (unsigned int*)&address[1],
                (unsigned int*)&address[2], (unsigned int*)&address[3]);

            if (result != 4) {
                // Resolve address with DNS
                struct hostent *host_address = gethostbyname(httpHandle->host);
                if (host_address == NULL)
                {
                    LogError("set_address failed");
                    result = HTTPAPI_INVALID_ARG;
                    goto exit;
                }
                p_address = (char*)host_address->h_addr_list[0];
            }

            memcpy((char*)&httpHandle->_remoteHost.sin_addr.s_addr, p_address, 4);

            // Address family
            httpHandle->_remoteHost.sin_family = AF_INET;

            // Set port
            httpHandle->_remoteHost.sin_port = htons(443);

            if (SSLInit(httpHandle->_sock_fd, SSL_OPT_DEFAULT))
            {
                LogError("SSLInit failed");
                result = HTTPAPI_ERROR;
                goto exit;
            }

            if (connect(httpHandle->_sock_fd, (const struct sockaddr *) &httpHandle->_remoteHost, sizeof(httpHandle->_remoteHost)) < 0)
            {
                closesocket(httpHandle->_sock_fd);
                httpHandle->_sock_fd = -1;
                LogError("connect failed");
                result = HTTPAPI_ERROR;
                goto exit;
            }
        }
    }

    //Send request
    if ((ret = _snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\n", method, relativePath)) < 0
        || ret >= sizeof(buf))
    {
        result = HTTPAPI_STRING_PROCESSING_ERROR;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }
    LogInfo("HTTPAPI_ExecuteRequest::Sending=%*.*s", strlen(buf), strlen(buf), buf);
    if (send(httpHandle->_sock_fd, buf, strlen(buf), 0) < 0)
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
        LogInfo("HTTPAPI_ExecuteRequest::Sending=%*.*s", strlen(header), strlen(header), header);
        if (send(httpHandle->_sock_fd, header, strlen(header), 0) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            free(header);
            goto exit;
        }
        if (send(httpHandle->_sock_fd, "\r\n", 2, 0) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            free(header);
            goto exit;
        }
        free(header);
    }

    //Close headers
    if (send(httpHandle->_sock_fd, "\r\n", 2, 0) < 0)
    {
        result = HTTPAPI_SEND_REQUEST_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    //Send data (if available)
    if (content && contentLength > 0)
    {
        LogInfo("HTTPAPI_ExecuteRequest::Sending data=%*.*s", contentLength, contentLength, content);
        if (send(httpHandle->_sock_fd, (char*)content, contentLength, 0) < 0)
        {
            result = HTTPAPI_SEND_REQUEST_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
    }

    //Receive response
    if (readLine(httpHandle->_sock_fd, buf, sizeof(buf)) < 0)
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
    LogInfo("HTTPAPI_ExecuteRequest::Received response=%*.*s", strlen(buf), strlen(buf), buf);

    //Read HTTP response headers
    if (readLine(httpHandle->_sock_fd, buf, sizeof(buf)) < 0)
    {
        result = HTTPAPI_READ_DATA_FAILED;
        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
        goto exit;
    }

    while (buf[0])
    {
        const char ContentLength[] = "content-length:";
        const char TransferEncoding[] = "transfer-encoding:";

        LogInfo("Receiving header=%*.*s", strlen(buf), strlen(buf), buf);

        if (_strnicmp(buf, ContentLength, CHAR_COUNT(ContentLength)) == 0)
        {
            if (sscanf(buf + CHAR_COUNT(ContentLength), " %d", &bodyLength) != 1)
            {
                result = HTTPAPI_READ_DATA_FAILED;
                LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                goto exit;
            }
        }
        else if (_strnicmp(buf, TransferEncoding, CHAR_COUNT(TransferEncoding)) == 0)
        {
            const char* p = buf + CHAR_COUNT(TransferEncoding);
            while (isspace(*p)) p++;
            if (_stricmp(p, "chunked") == 0)
                chunked = true;
        }

        char* whereIsColon = strchr((char*)buf, ':');
        if (whereIsColon && responseHeadersHandle != NULL)
        {
            *whereIsColon = '\0';
            HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle, buf, whereIsColon + 1);
        }

        if (readLine(httpHandle->_sock_fd, buf, sizeof(buf)) < 0)
        {
            result = HTTPAPI_READ_DATA_FAILED;
            LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
            goto exit;
        }
    }

    //Read HTTP response body
    LogInfo("HTTPAPI_ExecuteRequest::Receiving body=%d,%x", bodyLength, responseContent);
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

                if (readChunk(httpHandle->_sock_fd, (char*)receivedContent, bodyLength) < 0)
                {
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    goto exit;
                }
                else
                {
                    LogInfo("HTTPAPI_ExecuteRequest::Received response body=%*.*s", bodyLength, bodyLength, receivedContent);
                    result = HTTPAPI_OK;
                }
            }
            else
            {
                (void)skipN(httpHandle->_sock_fd, bodyLength, buf, sizeof(buf));
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
            if (readLine(httpHandle->_sock_fd, buf, sizeof(buf)) < 0)    // read [length in hex]/r/n
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
                if (readChunk(httpHandle->_sock_fd, (char*)buf, 2) < 0
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

                    if (readChunk(httpHandle->_sock_fd, (char*)receivedContent + size, chunkSize) < 0)
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                        goto exit;
                    }
                }
                else
                {
                    if (skipN(httpHandle->_sock_fd, chunkSize, buf, sizeof(buf)) < 0)
                    {
                        result = HTTPAPI_READ_DATA_FAILED;
                        LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                        goto exit;
                    }
                }

                if (readChunk(httpHandle->_sock_fd, (char*)buf, 2) < 0
                    || buf[0] != '\r' || buf[1] != '\n') // skip /r/n
                {
                    result = HTTPAPI_READ_DATA_FAILED;
                    LogError("(result = %s)", ENUM_TO_STRING(HTTPAPI_RESULT, result));
                    goto exit;
                }
                size += chunkSize;
            }
        }

        if (size > 0)
        {
            LogInfo("HTTPAPI_ExecuteRequest::Received chunk body=%*.*s", (int)size, (int)size, (const char*)responseContent);
        }
    }

exit:
    LogInfo("HTTPAPI_ExecuteRequest::End=%d", result);
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
    else
    {
        *savedValue = NULL;
        result = HTTPAPI_INVALID_ARG;
        LogError("unknown option %s", optionName);
    }
    return result;
}
