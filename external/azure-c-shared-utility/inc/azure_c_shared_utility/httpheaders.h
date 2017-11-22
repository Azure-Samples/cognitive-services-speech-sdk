// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file httpheaders.h    
*	@brief This is a utility module that handles HTTP message-headers.
*
*	@details An application would use ::HTTPHeaders_Alloc to create a new set of HTTP headers.
*			 After getting the handle, the application would build in several headers by
*			 consecutive calls to ::HTTPHeaders_AddHeaderNameValuePair. When the headers are
*			 constructed, the application can retrieve the stored data by calling one of the
*			 following functions:
*				- ::HTTPHeaders_FindHeaderValue - when the name of the header is known and it  
*				  wants to know the value of that header  
*				- ::HTTPHeaders_GetHeaderCount - when the application needs to know the count  
*				  of all the headers  
*				- ::HTTPHeaders_GetHeader - when the application needs to retrieve the
*				  <code>name + ": " + value</code> string based on an index.
*/

#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

/*Codes_SRS_HTTP_HEADERS_99_001:[ HttpHeaders shall have the following interface]*/

#define HTTP_HEADERS_RESULT_VALUES \
HTTP_HEADERS_OK,                  \
HTTP_HEADERS_INVALID_ARG,         \
HTTP_HEADERS_ALLOC_FAILED,        \
HTTP_HEADERS_INSUFFICIENT_BUFFER, \
HTTP_HEADERS_ERROR                \

/** @brief Enumeration specifying the status of calls to various APIs in this module.
*/
DEFINE_ENUM(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
typedef struct HTTP_HEADERS_HANDLE_DATA_TAG* HTTP_HEADERS_HANDLE;

/**
 * @brief	Produces a @c HTTP_HANDLE that can later be used in subsequent calls to the module.
 * 			
 *			This function returns @c NULL in case an error occurs. After successful execution
 *			::HTTPHeaders_GetHeaderCount will report @c 0 existing headers.
 *
 * @return	A HTTP_HEADERS_HANDLE representing the newly created collection of HTTP headers.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_HANDLE, HTTPHeaders_Alloc);

/**
 * @brief	De-allocates the data structures allocated by previous API calls to the same handle.
 *
 * @param	httpHeadersHandle	A valid @c HTTP_HEADERS_HANDLE value.
 */
MOCKABLE_FUNCTION(, void, HTTPHeaders_Free, HTTP_HEADERS_HANDLE, httpHeadersHandle);

/**
 * @brief	Adds a header record from the @p name and @p value parameters.
 *
 * @param	httpHeadersHandle	A valid @c HTTP_HEADERS_HANDLE value.
 * @param	name			 	The name of the HTTP header to add. It is invalid for
 * 								the name to include the ':' character or character codes
 * 								outside the range 33-126.
 * @param	value			 	The value to be assigned to the header.
 *
 *			The function stores the @c name:value pair in such a way that when later
 *			retrieved by a call to ::HTTPHeaders_GetHeader it will return a string
 *			that is @c strcmp equal to @c name+": "+value. If the name already exists
 *			in the collection of headers, the function concatenates the new value
 *			after the existing value, separated by a comma and a space as in:
 *			<code>old-value+", "+new-value</code>.
 * 
 * @return	Returns @c HTTP_HEADERS_OK when execution is successful or an error code from
 * 			the ::HTTPAPIEX_RESULT enum.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_RESULT, HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);

/**
 * @brief	This API performs exactly the same as ::HTTPHeaders_AddHeaderNameValuePair
 * 			except that if the header name already exists then the already existing value
 * 			will be replaced as opposed to being concatenated to.
 *
 * @param	httpHeadersHandle	A valid @c HTTP_HEADERS_HANDLE value.
 * @param	name			 	The name of the HTTP header to add/replace. It is invalid for
 * 								the name to include the ':' character or character codes
 * 								outside the range 33-126.
 * @param	value			 	The value to be assigned to the header.
 *
 * @return	Returns @c HTTP_HEADERS_OK when execution is successful or an error code from
 * 			the ::HTTPAPIEX_RESULT enum.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);

/**
 * @brief	Retrieves the value for a previously stored name.
 *
 * @param	httpHeadersHandle	A valid @c HTTP_HEADERS_HANDLE value.
 * @param	name			 	The name of the HTTP header to find.
 *
 * @return	The return value points to a string that shall be @c strcmp equal
 * 			to the original stored string.
 */
MOCKABLE_FUNCTION(, const char*, HTTPHeaders_FindHeaderValue, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name);

/**
 * @brief	This API retrieves the number of stored headers.
 *
 * @param	httpHeadersHandle	A valid @c HTTP_HEADERS_HANDLE value.
 * @param	headersCount		If non-null, the API writes the number of
 * 								into the memory pointed at by this parameter.
 *
 * @return	Returns @c HTTP_HEADERS_OK when execution is successful or
 * 			@c HTTP_HEADERS_ERROR when an error occurs.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_RESULT, HTTPHeaders_GetHeaderCount, HTTP_HEADERS_HANDLE, httpHeadersHandle, size_t*, headersCount);

/**
 * @brief	This API retrieves the string name+": "+value for the header
 * 			element at the given @p index.
 *
 * @param	handle			A valid @c HTTP_HEADERS_HANDLE value.
 * @param	index			Zero-based index of the item in the
 * 							headers collection.
 * @param   destination		If non-null, the header value is written into a
 * 							new string a pointer to which is written into this
 * 							parameters. It is the caller's responsibility to free
 * 							this memory.
 *
 * @return	Returns @c HTTP_HEADERS_OK when execution is successful or
 * 			@c HTTP_HEADERS_ERROR when an error occurs.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_RESULT, HTTPHeaders_GetHeader, HTTP_HEADERS_HANDLE, handle, size_t, index, char**, destination);

/**
 * @brief	This API produces a clone of the @p handle parameter.
 *
 * @param   handle  A valid @c HTTP_HEADERS_HANDLE value.
 *
 *			If @p handle is not @c NULL this function clones the content
 *			of the handle to a new handle and returns it.
 *			
 * @return	A @c HTTP_HEADERS_HANDLE containing a cloned copy of the
 * 			contents of @p handle.
 */
MOCKABLE_FUNCTION(, HTTP_HEADERS_HANDLE, HTTPHeaders_Clone, HTTP_HEADERS_HANDLE, handle);

#ifdef __cplusplus
}
#endif 

#endif /* HTTPHEADERS_H */
