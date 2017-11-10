// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file httpapiex.h
*	@brief		This is a utility module that provides HTTP requests with
*				build-in retry capabilities.
*
*	@details	HTTAPIEX is a utility module that provides HTTP requests with build-in
*				retry capability to an HTTP server. Features over "regular" HTTPAPI include:
*					- Optional parameters
*					- Implementation independent
*					- Retry mechanism
*					- Persistent options
*/

#ifndef HTTPAPIEX_H
#define HTTPAPIEX_H

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/umock_c_prod.h"
 
#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

typedef struct HTTPAPIEX_HANDLE_DATA_TAG* HTTPAPIEX_HANDLE;

#define HTTPAPIEX_RESULT_VALUES \
    HTTPAPIEX_OK, \
    HTTPAPIEX_ERROR, \
    HTTPAPIEX_INVALID_ARG, \
    HTTPAPIEX_RECOVERYFAILED
/*to be continued*/

/** @brief Enumeration specifying the status of calls to various APIs in this module.
*/
DEFINE_ENUM(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

/**
 * @brief	Creates an @c HTTPAPIEX_HANDLE that can be used in further calls.
 *
 * @param	hostName	Pointer to a null-terminated string that contains the host name
 * 						of an HTTP server.
 * 						
 *			If @p hostName is @c NULL then @c HTTPAPIEX_Create returns @c NULL. The @p
 *			hostName value is saved and associated with the returned handle. If creating
 *			the handle fails for any reason, then @c HTTAPIEX_Create returns @c NULL.
 *			Otherwise, @c HTTPAPIEX_Create returns an @c HTTAPIEX_HANDLE suitable for
 *			further calls to the module.
 *
 * @return	An @c HTTAPIEX_HANDLE suitable for further calls to the module.
 */
MOCKABLE_FUNCTION(, HTTPAPIEX_HANDLE, HTTPAPIEX_Create, const char*, hostName);

/**
 * @brief	Tries to execute an HTTP request.
 *
 * @param	handle					 	A valid @c HTTPAPIEX_HANDLE value.
 * @param	requestType				 	A value from the ::HTTPAPI_REQUEST_TYPE enum.
 * @param	relativePath			 	Relative path to send the request to on the server.
 * @param	requestHttpHeadersHandle 	Handle to the request HTTP headers.
 * @param	requestContent			 	The request content.
 * @param 	statusCode		 	        If non-null, the HTTP status code is written to this
 * 										pointer.
 * @param	responseHttpHeadersHandle	Handle to the response HTTP headers.
 * @param	responseContent			 	The response content.
 * 										
 * 			@c HTTPAPIEX_ExecuteRequest tries to execute an HTTP request of type @p
 * 			requestType, on the server's @p relativePath, pushing the request HTTP
 * 			headers @p requestHttpHeadersHandle, having the content of the request
 * 			as pointed to by @p requestContent. If successful,  @c HTTAPIEX_ExecuteRequest
 * 			writes in the out @p parameter statusCode the HTTP status, populates the @p
 * 			responseHeadersHandle with the response headers and copies the response body
 * 			to @p responseContent.
 *
 * @return	An @c HTTAPIEX_HANDLE suitable for further calls to the module.
 */
MOCKABLE_FUNCTION(, HTTPAPIEX_RESULT, HTTPAPIEX_ExecuteRequest, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent);

/**
 * @brief	Frees all resources used by the @c HTTPAPIEX_HANDLE object.
 *
 * @param	handle	The @c HTTPAPIEX_HANDLE object to be freed.
 */
MOCKABLE_FUNCTION(, void, HTTPAPIEX_Destroy, HTTPAPIEX_HANDLE, handle);

/**
 * @brief	Sets the option @p optionName to the value pointed to by @p value.
 *
 * @param	handle	  	The @c HTTPAPIEX_HANDLE representing this session.
 * @param	optionName	Name of the option.
 * @param	value	  	The value to be set for the option.
 *
 * @return	An @c HTTPAPIEX_RESULT indicating the status of the call.
 */
MOCKABLE_FUNCTION(, HTTPAPIEX_RESULT, HTTPAPIEX_SetOption, HTTPAPIEX_HANDLE, handle, const char*, optionName, const void*, value);

#ifdef __cplusplus
}
#endif

#endif /* HTTPAPIEX_H */
