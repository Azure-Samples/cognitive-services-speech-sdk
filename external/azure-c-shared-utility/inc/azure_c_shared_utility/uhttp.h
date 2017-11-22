// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/umock_c_prod.h"
#include "azure_c_shared_utility/xio.h"

typedef struct HTTP_CLIENT_HANDLE_DATA_TAG* HTTP_CLIENT_HANDLE;

#define HTTP_CLIENT_RESULT_VALUES \
HTTP_CLIENT_OK,                   \
HTTP_CLIENT_INVALID_ARG,          \
HTTP_CLIENT_ERROR,                \
HTTP_CLIENT_OPEN_REQUEST_FAILED,  \
HTTP_CLIENT_SET_OPTION_FAILED,    \
HTTP_CLIENT_SEND_REQUEST_FAILED,  \
HTTP_CLIENT_ALREADY_INIT,         \
HTTP_CLIENT_HTTP_HEADERS_FAILED,  \
HTTP_CLIENT_INVALID_CHUNK_REQUEST \

/** @brief Enumeration specifying the possible return values for the APIs in
*		   this module.
*/
DEFINE_ENUM(HTTP_CLIENT_RESULT, HTTP_CLIENT_RESULT_VALUES);

#define HTTP_CLIENT_REQUEST_TYPE_VALUES \
    HTTP_CLIENT_REQUEST_OPTIONS,         \
    HTTP_CLIENT_REQUEST_GET,            \
    HTTP_CLIENT_REQUEST_POST,           \
    HTTP_CLIENT_REQUEST_PUT,            \
    HTTP_CLIENT_REQUEST_DELETE,         \
    HTTP_CLIENT_REQUEST_PATCH           \

DEFINE_ENUM(HTTP_CLIENT_REQUEST_TYPE, HTTP_CLIENT_REQUEST_TYPE_VALUES);

typedef void(*ON_HTTP_CONNECTED_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx);
typedef void(*ON_HTTP_ERROR_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx);
typedef void(*ON_HTTP_CHUNK_REPLY_CALLBACK)(
    HTTP_CLIENT_HANDLE handle, 
    void* callback_ctx, 
    const uint8_t* content,
    size_t contentLen, 
    unsigned int statusCode,
    HTTP_HEADERS_HANDLE responseHeadersHandle, 
    int bReplyComplete);

MOCKABLE_FUNCTION(, HTTP_CLIENT_HANDLE, http_client_create, XIO_HANDLE, xioHandle, ON_HTTP_CONNECTED_CALLBACK, onConnected, ON_HTTP_ERROR_CALLBACK, on_http_error, ON_HTTP_CHUNK_REPLY_CALLBACK, on_chunk_reply, void*, callback_ctx);

MOCKABLE_FUNCTION(, void, http_client_destroy, HTTP_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, http_client_open, HTTP_CLIENT_HANDLE, handle, const char*, host);

MOCKABLE_FUNCTION(, void, http_client_close, HTTP_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, http_client_execute_request, HTTP_CLIENT_HANDLE, handle, HTTP_CLIENT_REQUEST_TYPE, requestType, const char*, relativePath,
    HTTP_HEADERS_HANDLE, httpHeadersHandle, const unsigned char*, content, size_t, contentLength);

MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, http_client_start_chunk_request, HTTP_CLIENT_HANDLE, handle, HTTP_CLIENT_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, httpHeadersHandle);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, http_client_send_chunk_request, HTTP_CLIENT_HANDLE, handle, const uint8_t*, content, size_t, chunkSize);
MOCKABLE_FUNCTION(, HTTP_CLIENT_RESULT, http_client_end_chunk_request, HTTP_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, void, http_client_dowork, HTTP_CLIENT_HANDLE, handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HTTP_CLIENT_H */
