// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file base64.h
*	@brief Prototypes for functions related to encoding/decoding
*	a @c buffer using standard base64 encoding.
*/

#ifndef BASE64_H
#define BASE64_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

/**
 * @brief	Base64 encodes a buffer and returns the resulting string.
 *
 * @param	input	The buffer that needs to be base64 encoded.
 *
 * 			Base64_Encode takes as a parameter a pointer to a BUFFER. If @p input is @c NULL then
 * 			@c Base64_Encode returns @c NULL. The size of the BUFFER pointed to by @p input may
 * 			be zero. If when allocating memory to produce the encoding a failure occurs, then @c
 * 			Base64_Encode returns @c NULL. Otherwise
 * 			@c Base64_Encode returns a pointer to a STRING. That string contains the
 * 			base 64 encoding of the @p input. This encoding of @p input will not contain embedded
 * 			line feeds.
 *
 * @return	A @c STRING_HANDLE containing the base64 encoding of @p input.
 */
MOCKABLE_FUNCTION(, STRING_HANDLE, Base64_Encode, BUFFER_HANDLE, input);

/**
 * @brief	Base64 encodes the buffer pointed to by @p source and returns the resulting string.
 *
 * @param	source	The buffer that needs to be base64 encoded.
 * @param	size  	The size.
 *
 * 			This function produces a @c STRING_HANDLE containing the base64 encoding of the
 * 			buffer pointed to by @p source, having the size as given by
 * 			@p size. If @p source is @c NULL then @c Base64_Encode_Bytes returns @c NULL
 * 			If @p source is not @c NULL and @p size is zero, then @c Base64_Encode_Bytes produces
 * 			an empty @c STRING_HANDLE. Otherwise, @c Base64_Encode_Bytes produces a
 * 			@c STRING_HANDLE containing the Base64 representation of the buffer. In case of
 * 			any errors, @c Base64_Encode_Bytes returns @c NULL.].
 *
 * @return	@c NULL in case an error occurs or a @c STRING_HANDLE containing the base64 encoding
 * 			of @p input.
 *
 */
MOCKABLE_FUNCTION(, STRING_HANDLE, Base64_Encode_Bytes, const unsigned char*, source, size_t, size);

/**
 * @brief	Base64 decodes the buffer pointed to by @p source and returns the resulting buffer.
 *
 * @param	source	A base64 encoded string buffer.
 *
 *       	This function decodes the string pointed at by @p source using base64 decoding and
 * 			returns the resulting buffer. If @p source is @c NULL then
 * 			@c Base64_Decoder returns NULL. If the string pointed to by @p source is zero
 * 			length then the handle returned refers to a zero length buffer. If there is any
 * 			memory allocation failure during the decode or if the source string has an invalid
 * 			length for a base 64 encoded string then @c Base64_Decoder returns @c NULL.
 * 
 * @return	A @c BUFFER_HANDLE pointing to a buffer containing the result of base64 decoding @p
 * 			source.
 */
MOCKABLE_FUNCTION(, BUFFER_HANDLE, Base64_Decoder, const char*, source);

#ifdef __cplusplus
}
#endif

#endif /* BASE64_H */
