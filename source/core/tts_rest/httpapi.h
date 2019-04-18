// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file httpapi.h
 *  @brief This module implements the standard HTTP API used by the C IoT client
 *         library.
 *
 *  @details For example, on the Windows platform the HTTP API code uses
 *           WinHTTP and for Linux it uses curl and so forth. HTTPAPI must support
 *           HTTPs (HTTP+SSL).
 */

#ifndef HTTPAPI_H
#define HTTPAPI_H

#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

typedef struct HTTP_HANDLE_DATA_TAG* HTTP_HANDLE;

#define AMBIGUOUS_STATUS_CODE           (300)

#define HTTPAPI_RESULT_VALUES                \
HTTPAPI_OK,                                  \
HTTPAPI_INVALID_ARG,                         \
HTTPAPI_ERROR,                               \
HTTPAPI_OPEN_REQUEST_FAILED,                 \
HTTPAPI_SET_OPTION_FAILED,                   \
HTTPAPI_SEND_REQUEST_FAILED,                 \
HTTPAPI_RECEIVE_RESPONSE_FAILED,             \
HTTPAPI_QUERY_HEADERS_FAILED,                \
HTTPAPI_QUERY_DATA_AVAILABLE_FAILED,         \
HTTPAPI_READ_DATA_FAILED,                    \
HTTPAPI_ALREADY_INIT,                        \
HTTPAPI_NOT_INIT,                            \
HTTPAPI_HTTP_HEADERS_FAILED,                 \
HTTPAPI_STRING_PROCESSING_ERROR,             \
HTTPAPI_ALLOC_FAILED,                        \
HTTPAPI_INIT_FAILED,                         \
HTTPAPI_INSUFFICIENT_RESPONSE_BUFFER,        \
HTTPAPI_SET_X509_FAILURE,                    \
HTTPAPI_SET_TIMEOUTS_FAILED                  \

/** @brief Enumeration specifying the possible return values for the APIs in  
 *         this module.
 */
DEFINE_ENUM(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);

#define HTTPAPI_REQUEST_TYPE_VALUES\
    HTTPAPI_REQUEST_GET,            \
    HTTPAPI_REQUEST_POST,           \
    HTTPAPI_REQUEST_PUT,            \
    HTTPAPI_REQUEST_DELETE,         \
    HTTPAPI_REQUEST_PATCH           \

/** @brief Enumeration specifying the HTTP request verbs accepted by
 *         the HTTPAPI module.
 */
DEFINE_ENUM(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);

#define MAX_HOSTNAME_LEN        65
#define MAX_USERNAME_LEN        65
#define MAX_PASSWORD_LEN        65

typedef void(*ON_CHUNK_RECEIVED)(void* context, const unsigned char* buffer, size_t size);

/**
 * @brief Global initialization for the HTTP API component.
 *
 *        Platform specific implementations are expected to initialize
 *        the underlying HTTP API stacks.
 * 
 * @return @c HTTPAPI_OK if initialization is successful or an error
 *         code in case it fails.
 */
MOCKABLE_FUNCTION(, HTTPAPI_RESULT, HTTPAPI_Init);

/** @brief Free resources allocated in ::HTTPAPI_Init. */
MOCKABLE_FUNCTION(, void, HTTPAPI_Deinit);

/**
 * @brief Creates an HTTPS connection to the host specified by the @p
 *        hostName parameter.
 *
 * @param hostName Name of the host.
 *
 *        This function returns a handle to the newly created connection.
 *        You can use the handle in subsequent calls to execute specific
 *        HTTP calls using ::HTTPAPI_ExecuteRequest.
 * 
 * @return A @c HTTP_HANDLE to the newly created connection or @c NULL in
 *         case an error occurs.
 */
MOCKABLE_FUNCTION(, HTTP_HANDLE, HTTPAPI_CreateConnection, const char*, hostName);

/**
 * @brief Closes a connection created with ::HTTPAPI_CreateConnection.
 *
 * @param handle The handle to the HTTP connection created via ::HTTPAPI_CreateConnection.
 * 
 *        All resources allocated by ::HTTPAPI_CreateConnection should be
 *        freed in ::HTTPAPI_CloseConnection.
 */
MOCKABLE_FUNCTION(, void, HTTPAPI_CloseConnection, HTTP_HANDLE, handle);

/**
 * @brief Sends the HTTP request to the host and handles the response for
 *        the HTTP call.
 *
 * @param handle                 The handle to the HTTP connection created
 *                               via ::HTTPAPI_CreateConnection.
 * @param requestType            Specifies which HTTP method is used (GET,
 *                               POST, DELETE, PUT, PATCH).
 * @param relativePath           Specifies the relative path of the URL
 *                               excluding the host name.
 * @param httpHeadersHandle      Specifies a set of HTTP headers (name-value
 *                               pairs) to be added to the
 *                               HTTP request. The @p httpHeadersHandle
 *                               handle can be created and setup with
 *                               the proper name-value pairs by using the
 *                               HTTPHeaders APIs available in @c
 *                               HTTPHeaders.h.
 * @param content                Specifies a pointer to the request body.
 *                               This value is optional and can be @c NULL.
 * @param contentLength          Specifies the request body size (this is
 *                               typically added into the HTTP headers as
 *                               the Content-Length header). This value is
 *                               optional and can be 0.
 * @param statusCode             This is an out parameter, where
 *                               ::HTTPAPI_ExecuteRequest returns the status
 *                               code from the HTTP response (200, 201, 400,
 *                               401, etc.)
 * @param responseHeadersHandle  This is an HTTP headers handle to which
 *                               ::HTTPAPI_ExecuteRequest must add all the
 *                               HTTP response headers so that the caller of
 *                               ::HTTPAPI_ExecuteRequest can inspect them.
 *                               You can manipulate @p responseHeadersHandle
 *                               by using the HTTPHeaders APIs available in
 *                               @c HTTPHeaders.h
 * @param responseContent        This is a buffer that must be filled by
 *                               ::HTTPAPI_ExecuteRequest with the contents
 *                               of the HTTP response body. The buffer size
 *                               must be increased by the
 *                               ::HTTPAPI_ExecuteRequest implementation in
 *                               order to fit the response body.
 *                               ::HTTPAPI_ExecuteRequest must also handle
 *                               chunked transfer encoding for HTTP responses.
 *                               To manipulate the @p responseContent buffer,
 *                               use the APIs available in @c Strings.h.
 *
 * @return                       @c HTTPAPI_OK if the API call is successful or an error
 *                               code in case it fails.
 */
MOCKABLE_FUNCTION(, HTTPAPI_RESULT, HTTPAPI_ExecuteRequest, HTTP_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath,
                                             HTTP_HEADERS_HANDLE, httpHeadersHandle, const unsigned char*, content,
                                             size_t, contentLength, unsigned int*, statusCode,
                                             HTTP_HEADERS_HANDLE, responseHeadersHandle, BUFFER_HANDLE, responseContent,
                                             ON_CHUNK_RECEIVED, on_chunk_received, void*, on_chunk_received_context);

/**
 * @brief Sets the option named @p optionName bearing the value
 *        @p value for the HTTP_HANDLE @p handle.
 *
 * @param handle        The handle to the HTTP connection created via
 *                      ::HTTPAPI_CreateConnection.
 * @param optionName    A @c NULL terminated string representing the name
 *                      of the option.
 * @param value         A pointer to the value for the option.
 *
 * @return              @c HTTPAPI_OK if initialization is successful or an error
 *                      code in case it fails.
 */
MOCKABLE_FUNCTION(, HTTPAPI_RESULT, HTTPAPI_SetOption, HTTP_HANDLE, handle, const char*, optionName, const void*, value);

/**
 * @brief Clones the option named @p optionName bearing the value @p value
 *        into the pointer @p savedValue.
 *
 * @param optionName    A @c NULL terminated string representing the name of
 *                      the option
 * @param value         A pointer to the value of the option.
 * @param savedValue    This pointer receives the copy of the value of the
 *                      option. The copy needs to be free-able.
 *
 * @return              @c HTTPAPI_OK if initialization is successful or an error
 *                      code in case it fails.
 */
MOCKABLE_FUNCTION(, HTTPAPI_RESULT, HTTPAPI_CloneOption, const char*, optionName, const void*, value, const void**, savedValue);

#ifdef __cplusplus
}
#endif

#endif /* HTTPAPI_H */
