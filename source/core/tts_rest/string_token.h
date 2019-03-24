// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRING_TOKEN_H
#define STRING_TOKEN_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#include <stdbool.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

typedef struct STRING_TOKEN_TAG* STRING_TOKEN_HANDLE;

/*
*    @brief     Tries to identify the first token defined in source up to its length using the delimiters provided. 
*    @Remark    If no delimiter is found, the entire source string becomes the resulting token. Empty tokens (when delimiters occur side-by-side) can also detected.
*    @param     source        The initial string to be tokenized.
*    @param     length        The length of the source string, not including the null-terminator.
*    @param     delimiters    Array with null-terminated strings to be used as token delimiters.
*    @param     n_delims      Number of elements in delimiters array.
*    @return    A STRING_TOKEN_HANDLE if no failures occur (arguments, memory allocation), or NULL otherwise.
*/
MOCKABLE_FUNCTION(, STRING_TOKEN_HANDLE, StringToken_GetFirst, const char*, source, size_t, length, const char**, delimiters, size_t, n_delims)

/*
*    @brief     Tries to identify the next token defined in source up to its length using the delimiters provided.
*    @Remark    After StringToken_GetFirst is initially called, StringToken_GetNext shall be called for obtaining the next tokens.
*               If no delimiter is found, the entire source string becomes the resulting token. 
*               Empty tokens (when delimiters occur side-by-side) can also detected.
*    @param     token         The handle returned by StringToken_GetFirst.
*    @param     delimiters    Array with null-terminated strings to be used as token delimiters.
*    @param     n_delims      Number of elements in delimiters array.
*    @return    True if a token could be identified, or false if the tokenizer already reached the end of the source string an no more tokens are available.
*/
MOCKABLE_FUNCTION(, bool, StringToken_GetNext, STRING_TOKEN_HANDLE, token, const char**, delimiters, size_t, n_delims)

/*
*    @brief     Gets the pointer to the beginning of the current token identified in the source string.
*    @param     token         The handle returned by StringToken_GetFirst.
*    @return    The pointer to the token identified in the last successfull call to StringToken_GetFirst or StringToken_GetNext.
*               If the token string is empty (e.g., between two delimiters side-by-side or if a delimiter occurs at the end of the source string),
*               NULL is returned instead of an empty string.
*/
MOCKABLE_FUNCTION(, const char*, StringToken_GetValue, STRING_TOKEN_HANDLE, token)

/*
*    @brief     Gets the length of the current token identified in the source string.
*    @param     token         The handle returned by StringToken_GetFirst.
*    @return    A non-zero value if the current token is not empty, zero otherwise.
*/
MOCKABLE_FUNCTION(, size_t, StringToken_GetLength, STRING_TOKEN_HANDLE, token)

/*
*    @brief     Gets a pointer to last delimiter identified (which defined the current token).
*    @param     token         The handle returned by StringToken_GetFirst.
*    @return    If a delimiter was identified in the previous search,
*               the pointer returned is to the element in the delimiters array provided in the previous call to StringToken_GetFirst or StringToken_GetNext;
*               it returns NULL if no delimiter was found in the previous call to those functions.
*/
MOCKABLE_FUNCTION(, const char*, StringToken_GetDelimiter, STRING_TOKEN_HANDLE, token)

/*
*    @brief     Splits a string into an array with all the tokens identified using the delimiters provided.
*    @param     source           The string to be tokenized.
*    @param     length           The length of the source string, not including the null-terminator.
*    @param     delimiters       Array with null-terminated strings to be used as token delimiters.
*    @param     n_delims         Number of elements in delimiters array.
*    @param     include_empty    Indicates if empty strings shall be included (as NULL values) in the resulting array.
*    @param     tokens           If no failures occur, the resulting array with the split tokens (dynamically allocated).
*    @param     token_count      The number of elements in the tokens array.
*    @return    Zero if no failures occur, or a non-zero value otherwise.
*/
MOCKABLE_FUNCTION(, int, StringToken_Split, const char*, source, size_t, length, const char**, delimiters, size_t, n_delims, bool, include_empty, char***, tokens, size_t*, token_count)

/*
*    @brief     Destroys the handle created when calling StringToken_GetFirst.
*    @param     token         The handle returned by StringToken_GetFirst.
*    @return    Nothing
*/
MOCKABLE_FUNCTION(, void, StringToken_Destroy, STRING_TOKEN_HANDLE, token)

#ifdef __cplusplus
}
#endif

#endif  /*STRING_TOKEN_H*/
