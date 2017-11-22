// Copyright (c) Texas Instruments. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ti/net/http/httpcli.h>

#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"

#define CONTENT_BUF_LEN     128

static const char* getHttpMethod(HTTPAPI_REQUEST_TYPE requestType)
{
    switch (requestType) {
        case HTTPAPI_REQUEST_GET:
            return (HTTPStd_GET);
        case HTTPAPI_REQUEST_POST:
            return (HTTPStd_POST);
        case HTTPAPI_REQUEST_PUT:
            return (HTTPStd_PUT);
        case HTTPAPI_REQUEST_DELETE:
            return (HTTPStd_DELETE);
        case HTTPAPI_REQUEST_PATCH:
            return (HTTPStd_PATCH);
        default:
            return (NULL);
    }
}

static int splitHeader(char *headerName, char **headerValue)
{
    *headerValue = strchr(headerName, ':');
    if (*headerValue == NULL) {
         return (-1);
    }

    **headerValue = '\0';
    (*headerValue)++;
    while (**headerValue == ' ') {
        (*headerValue)++;
    }

    return (0);
}

HTTPAPI_RESULT HTTPAPI_Init(void)
{
    return (HTTPAPI_OK);
}

void HTTPAPI_Deinit(void)
{
}

HTTP_HANDLE HTTPAPI_CreateConnection(const char* hostName)
{
    int ret;
    struct sockaddr addr;
    HTTPCli_Handle cli;

    ret = HTTPCli_initSockAddr(&addr, hostName, 0);
    if (ret < 0) {
		LogError("HTTPCli_initSockAddr failed, ret=%d", ret);
        return (NULL);
    }
    ((struct sockaddr_in *) (&addr))->sin_port = htons(HTTPStd_SECURE_PORT);

    cli = HTTPCli_create();
    if (cli == NULL) {
		LogError("HTTPCli_create failed");
        return (NULL);
    }

    ret = HTTPCli_connect(cli, &addr, HTTPCli_TYPE_TLS, NULL);
    if (ret < 0) {
		LogError("HTTPCli_connect failed, ret=%d", ret);
        HTTPCli_delete(&cli);
        return (NULL);
    }
     
    return ((HTTP_HANDLE) cli);
}

void HTTPAPI_CloseConnection(HTTP_HANDLE handle)
{
    HTTPCli_Handle cli = (HTTPCli_Handle) handle;

    if (cli) {
        HTTPCli_disconnect(cli);
        HTTPCli_delete(&cli);
    }
}

HTTPAPI_RESULT HTTPAPI_ExecuteRequest(HTTP_HANDLE handle, 
        HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
        HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content,
        size_t contentLength, unsigned int* statusCode,
        HTTP_HEADERS_HANDLE responseHeadersHandle,
        BUFFER_HANDLE responseContent)
{
    HTTPCli_Handle cli = (HTTPCli_Handle) handle;
    int ret;
    int offset;
    size_t cnt;
    char contentBuf[CONTENT_BUF_LEN] = {0};
    char *hname;
    char *hvalue;
    const char *method;
    bool moreFlag;

    method = getHttpMethod(requestType);

    if ((cli == NULL) || (method == NULL) || (relativePath == NULL)
            || (statusCode == NULL) || (responseHeadersHandle == NULL)) {
		LogError("Invalid arguments: handle=%p, requestType=%d, relativePath=%p, statusCode=%p, responseHeadersHandle=%p",
			handle, (int)requestType, relativePath, statusCode, responseHeadersHandle);
        return (HTTPAPI_INVALID_ARG);
    }
    else if (HTTPHeaders_GetHeaderCount(httpHeadersHandle, &cnt) 
            != HTTP_HEADERS_OK) {
		LogError("Cannot get header count");
		return (HTTPAPI_QUERY_HEADERS_FAILED);
    }

    /* Send the request line */
    ret = HTTPCli_sendRequest(cli, method, relativePath, true);
    if (ret < 0) {
		LogError("HTTPCli_sendRequest failed, ret=%d", ret);
        return (HTTPAPI_SEND_REQUEST_FAILED);
    }

    /* Send the request headers */
    while (cnt--) {
        ret = HTTPHeaders_GetHeader(httpHeadersHandle, cnt, &hname);
        if (ret != HTTP_HEADERS_OK) {
			LogError("Cannot get request header %d", cnt);
            return (HTTPAPI_QUERY_HEADERS_FAILED);
        }

        ret = splitHeader(hname, &hvalue);

        if (ret == 0) {
            ret = HTTPCli_sendField(cli, hname, hvalue, false);
        }

        free(hname);
        hname = NULL;

        if (ret < 0) {
			LogError("HTTP send field failed, ret=%d", ret);
			return (HTTPAPI_SEND_REQUEST_FAILED);
        }
    }

    /* Send the last header and request body */
    ret = HTTPCli_sendField(cli, NULL, NULL, true);
    if (ret < 0) {
		LogError("HTTP send empty field failed, ret=%d", ret);
        return (HTTPAPI_SEND_REQUEST_FAILED);
    }

    if (content && contentLength != 0) {
        ret = HTTPCli_sendRequestBody(cli, (const char *)content,
                contentLength);
        if (ret < 0) {
			LogError("HTTP send request body failed, ret=%d", ret);
            return (HTTPAPI_SEND_REQUEST_FAILED);
        }
    }

    /* Get the response status code */
    ret = HTTPCli_getResponseStatus(cli);
    if (ret < 0) {
		LogError("HTTP receive response failed, ret=%d", ret);
        return (HTTPAPI_RECEIVE_RESPONSE_FAILED);
    }
    *statusCode = (unsigned int)ret;

    /* Get the response headers */
    hname = NULL;
    cnt = 0;
    offset = 0;
    do {
        ret = HTTPCli_readResponseHeader(cli, contentBuf, CONTENT_BUF_LEN,
            &moreFlag);
        if (ret < 0) {
			LogError("HTTP read response header failed, ret=%d", ret);
            ret = HTTPAPI_RECEIVE_RESPONSE_FAILED;
            goto headersDone;
        }
        else if (ret == 0) {
            /* All headers read */
            goto headersDone;
        }

        if (cnt < offset + ret) {
            hname = (char *)realloc(hname, offset + ret);
            if (hname == NULL) {
				LogError("Failed reallocating memory");
                ret = HTTPAPI_ALLOC_FAILED;
                goto headersDone;
            }
            cnt = offset + ret;
        }
      
        memcpy(hname + offset, contentBuf, ret);
        offset += ret;

        if (moreFlag) {
            continue;
        }

        ret = splitHeader(hname, &hvalue);
        if (ret < 0) {
			LogError("HTTP split header failed, ret=%d", ret);
            ret = HTTPAPI_HTTP_HEADERS_FAILED;
            goto headersDone;
        }

        ret = HTTPHeaders_AddHeaderNameValuePair(responseHeadersHandle,
                hname, hvalue);
        if (ret != HTTP_HEADERS_OK) {
			LogError("Adding the response header failed");
            ret = HTTPAPI_HTTP_HEADERS_FAILED;
            goto headersDone;
        }
        offset = 0;
    } while (1);

headersDone:
    free(hname);
    hname = NULL;
    if (ret != 0) {
        return ((HTTPAPI_RESULT)ret);
    }

    /* Get response body */
    if (responseContent != NULL) {
        offset = 0;
        cnt = 0;

        do {
            ret = HTTPCli_readResponseBody(cli, contentBuf, CONTENT_BUF_LEN,
                    &moreFlag);

            if (ret < 0) {
				LogError("HTTP read response body failed, ret=%d", ret);
                ret = HTTPAPI_RECEIVE_RESPONSE_FAILED;
                goto contentDone;
            }
             
            if (ret != 0) {
                cnt = ret;
                ret = BUFFER_enlarge(responseContent, cnt); 
                if (ret != 0) {
					LogError("Failed enlarging response buffer");
                    ret = HTTPAPI_ALLOC_FAILED;
                    goto contentDone;
                }

                ret = BUFFER_content(responseContent,
                        (const unsigned char **)&hname);
                if (ret != 0) {
					LogError("Failed getting the response buffer content");
                    ret = HTTPAPI_ALLOC_FAILED;
                    goto contentDone;
                }

                memcpy(hname + offset, contentBuf, cnt);
                offset += cnt;
            }
        } while (moreFlag);

    contentDone:
        if (ret < 0) {
            BUFFER_unbuild(responseContent);
            return ((HTTPAPI_RESULT)ret);
        }
    }

    return (HTTPAPI_OK);
}

HTTPAPI_RESULT HTTPAPI_SetOption(HTTP_HANDLE handle, const char* optionName,
        const void* value)
{
    return (HTTPAPI_INVALID_ARG);
}

HTTPAPI_RESULT HTTPAPI_CloneOption(const char* optionName, const void* value,
        const void** savedValue)
{
    return (HTTPAPI_INVALID_ARG);
}
