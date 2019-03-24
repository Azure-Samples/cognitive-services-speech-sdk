// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "string_token.h"

typedef struct STRING_TOKEN_TAG
{
    const char* source;
    size_t length;

    const char* token_start;
    const char* delimiter_start;
    const char* delimiter;
} STRING_TOKEN;

static size_t* get_delimiters_lengths(const char** delimiters, size_t n_delims)
{
    size_t* result;

    if ((result = malloc(sizeof(size_t) * n_delims)) == NULL)
    {
        LogError("Failed to allocate array for delimiters lengths");
    }
    else
    {
        size_t i;
        for (i = 0; i < n_delims; i++)
        {
            if (delimiters[i] == NULL)
            {
                // Codes_SRS_STRING_TOKENIZER_09_002: [ If any of the strings in delimiters are NULL, the function shall return NULL ]
                LogError("Invalid argument (delimiter %lu is NULL)", (unsigned long)i);
                free(result);
                result = NULL;
                break;
            }
            else
            {
                result[i] = strlen(delimiters[i]);
            }
        }
    }

    return result;
}

static int get_next_token(STRING_TOKEN* token, const char** delimiters, size_t n_delims)
{
    int result;

    if (token->token_start != NULL && token->delimiter_start == NULL)
    {
        // The parser reached the end of the input string.
        result = __FAILURE__;
    }
    else
    {
        size_t* delimiters_lengths;

        if ((delimiters_lengths = get_delimiters_lengths(delimiters, n_delims)) == NULL)
        {
            LogError("Failed to get delimiters lengths");
            result = __FAILURE__;
        }
        else
        {
            const char* new_token_start;
            const char* current_pos;
            const char* stop_pos = (char*)token->source + token->length;
            size_t j; // iterator for the delimiters.

            if (token->delimiter_start == NULL)
            {
                // Codes_SRS_STRING_TOKENIZER_09_005: [ The source string shall be split in a token starting from the beginning of source up to occurrence of any one of the demiliters, whichever occurs first in the order provided ]
                new_token_start = (char*)token->source;
            }
            else
            {
                // Codes_SRS_STRING_TOKENIZER_09_010: [ The next token shall be selected starting from the position in source right after the previous delimiter up to occurrence of any one of demiliters, whichever occurs first in the order provided ]
                new_token_start = token->delimiter_start + strlen(token->delimiter);
            }

            current_pos = new_token_start;
            result = 0;

            while (current_pos < stop_pos)
            {
                for (j = 0; j < n_delims; j++)
                {
                    if (*current_pos == *delimiters[j])
                    {
                        size_t k;
                        for (k = 1; k < delimiters_lengths[j] && (current_pos + k) < stop_pos; k++)
                        {
                            if (*(current_pos + k) != *(delimiters[j] + k))
                            {
                                break;
                            }
                        }

                        if (k == delimiters_lengths[j])
                        {
                            token->delimiter_start = current_pos;
                            token->delimiter = delimiters[j];

                            if (token->delimiter_start == token->source)
                            {
                                // Delimiter occurs in the beginning of the source string.
                                token->token_start = NULL;
                            }
                            else
                            {
                                token->token_start = new_token_start;
                            }
                            goto SCAN_COMPLETED;
                        }
                    }
                }

                current_pos++;
            }

            // Codes_SRS_STRING_TOKENIZER_09_006: [ If the source string does not have any of the demiliters, the resulting token shall be the entire source string ]
            // Codes_SRS_STRING_TOKENIZER_09_011: [ If the source string, starting right after the position of the last delimiter found, does not have any of the demiliters, the resulting token shall be the entire remaining of the source string ]
            if (current_pos == stop_pos)
            {
                token->token_start = new_token_start;
                token->delimiter_start = NULL;
                // Codes_SRS_STRING_TOKENIZER_09_019: [ If the current token extends to the end of source, the function shall return NULL ]
                token->delimiter = NULL;
            }

SCAN_COMPLETED:
            free(delimiters_lengths);
        }
    }

    return result;
}

STRING_TOKEN_HANDLE StringToken_GetFirst(const char* source, size_t length, const char** delimiters, size_t n_delims)
{
    STRING_TOKEN* result;

    // Codes_SRS_STRING_TOKENIZER_09_001: [ If source or delimiters are NULL, or n_delims is zero, the function shall return NULL ]
    if (source == NULL || delimiters == NULL || n_delims == 0)
    {
        LogError("Invalid argument (source=%p, delimiters=%p, n_delims=%lu)", source, delimiters, (unsigned long)n_delims);
        result = NULL;
    }
    else
    {
        // Codes_SRS_STRING_TOKENIZER_09_003: [ A STRING_TOKEN structure shall be allocated to hold the token parameters ]
        if ((result = (STRING_TOKEN*)malloc(sizeof(STRING_TOKEN))) == NULL)
        {
            // Codes_SRS_STRING_TOKENIZER_09_004: [ If the STRING_TOKEN structure fails to be allocated, the function shall return NULL ]
            LogError("Failed allocating STRING_TOKEN");
        }
        else
        {
            (void)memset(result, 0, sizeof(STRING_TOKEN));
            result->source = source;
            result->length = length;

            if (get_next_token(result, delimiters, n_delims) != 0)
            {
                LogError("Failed to get first token");
                // Codes_SRS_STRING_TOKENIZER_09_007: [ If any failure occurs, all memory allocated by this function shall be released ]
                free(result);
                result = NULL;
            }
        }
    }

    return result;
}

bool StringToken_GetNext(STRING_TOKEN_HANDLE token, const char** delimiters, size_t n_delims)
{
    bool result;

    // Codes_SRS_STRING_TOKENIZER_09_008: [ If token or delimiters are NULL, or n_delims is zero, the function shall return false ]
    if (token == NULL || delimiters == NULL || n_delims == 0)
    {
        LogError("Invalid argument (token=%p, delimiters=%p, n_delims=%lu)", token, delimiters, (unsigned long)n_delims);
        result = false;
    }
    else if (get_next_token(token, delimiters, n_delims) != 0)
    {
        // Codes_SRS_STRING_TOKENIZER_09_009: [ If the previous token already extended to the end of source, the function shall return false ]
        result = false;
    }
    else
    {
        // Codes_SRS_STRING_TOKENIZER_09_012: [ If a token was identified, the function shall return true ]
        result = true;
    }

    return result;
}

const char* StringToken_GetValue(STRING_TOKEN_HANDLE token)
{
    const char* result;

    // Codes_SRS_STRING_TOKENIZER_09_013: [ If token is NULL the function shall return NULL ]
    if (token == NULL)
    {
        LogError("Invalig argument (token is NULL)");
        result = NULL;
    }
    // Beyond the length of the source string.
    else if (token->token_start == (token->source + token->length))
    {
        result = NULL;
    }
    // Two delimiters side by side (empty token).
    else if (token->token_start == token->delimiter_start)
    {
        result = NULL;
    }
    else
    {
        // Codes_SRS_STRING_TOKENIZER_09_014: [ The function shall return the pointer to the position in source where the current token starts. ]
        result = token->token_start;
    }

    return result;
}

size_t StringToken_GetLength(STRING_TOKEN_HANDLE token)
{
    size_t result;

    // Codes_SRS_STRING_TOKENIZER_09_015: [ If token is NULL the function shall return zero ]
    if (token == NULL)
    {
        LogError("Invalig argument (token is NULL)");
        result = 0;
    }
    // Codes_SRS_STRING_TOKENIZER_09_016: [ The function shall return the length of the current token ]
    else if (token->token_start == NULL)
    {
        result = 0;
    }
    else if (token->delimiter_start == NULL)
    {
        result = token->source + token->length - token->token_start;
    }
    else
    {
        result = token->delimiter_start - token->token_start;
    }

    return result;
}

const char* StringToken_GetDelimiter(STRING_TOKEN_HANDLE token)
{
    const char* result;

    if (token == NULL)
    {
        // Codes_SRS_STRING_TOKENIZER_09_017: [ If token is NULL the function shall return NULL ]
        LogError("Invalig argument (token is NULL)");
        result = NULL;
    }
    else
    {
        // Codes_SRS_STRING_TOKENIZER_09_018: [ The function shall return a pointer to the delimiter that defined the current token, as passed to the previous call to StringToken_GetNext() or StringToken_GetFirst() ]
        result = token->delimiter;
    }

    return result;
}

int StringToken_Split(const char* source, size_t length, const char** delimiters, size_t n_delims, bool include_empty, char*** tokens, size_t* token_count)
{
    int result;

    // Codes_SRS_STRING_TOKENIZER_09_022: [ If source, delimiters, token or token_count are NULL, or n_delims is zero the function shall return a non-zero value ]
    if (source == NULL || delimiters == NULL || n_delims == 0 || tokens == NULL || token_count == NULL)
    {
        LogError("Invalid argument (source=%p, delimiters=%p, n_delims=%lu, tokens=%p, token_count=%p)", source, delimiters, (unsigned long)n_delims, tokens, token_count);
        result = __FAILURE__;
    }
    else
    {
        STRING_TOKEN_HANDLE tokenizer;

        // Codes_SRS_STRING_TOKENIZER_09_023: [ source (up to length) shall be split into individual tokens separated by any of delimiters ]
        tokenizer = StringToken_GetFirst(source, length, delimiters, n_delims);

        *token_count = 0;
        *tokens = NULL;
        // Codes_SRS_STRING_TOKENIZER_09_027: [ If no failures occur the function shall return zero ]
        result = 0;

        if (tokenizer != NULL)
        {
            do
            {
                const char* tokenValue;
                size_t tokenLength;

                tokenValue = StringToken_GetValue(tokenizer);
                tokenLength = StringToken_GetLength(tokenizer);

                if ((tokenValue != NULL && tokenLength == 0) || (tokenValue == NULL && tokenLength > 0))
                {
                    LogError("Unexpected token value (%p) or length (%lu)", tokenValue, (unsigned long)tokenLength);
                    result = __FAILURE__;
                    break;
                }
                // Codes_SRS_STRING_TOKENIZER_09_024: [ All NULL tokens shall be ommited if include_empty is not TRUE ]
                else if (tokenValue != NULL || include_empty)
                {
                    char** temp_token;
                    // Codes_SRS_STRING_TOKENIZER_09_025: [ The tokens shall be stored in tokens, and their count stored in token_count ]
                    *token_count = (*token_count) + 1;

                    if ((temp_token = (char**)realloc(*tokens, sizeof(char*) * (*token_count))) == NULL)
                    {
                        // Codes_SRS_STRING_TOKENIZER_09_026: [ If any failures splitting or storing the tokens occur the function shall return a non-zero value ]
                        LogError("Failed re-allocating the token array");
                        (*token_count)--;
                        result = __FAILURE__;
                        break;
                    }
                    else
                    {
                        *tokens = temp_token;
                        if (tokenLength == 0)
                        {
                            (*tokens)[(*token_count) - 1] = NULL;
                        }
                        else if (((*tokens)[(*token_count) - 1] = (char*)malloc(sizeof(char) * (tokenLength + 1))) == NULL)
                        {
                            // Codes_SRS_STRING_TOKENIZER_09_026: [ If any failures splitting or storing the tokens occur the function shall return a non-zero value ]
                            LogError("Failed copying token into array");
                            *token_count = (*token_count) - 1;
                            result = __FAILURE__;
                            break;
                        }
                        else
                        {
                            (void)memcpy((*tokens)[(*token_count) - 1], tokenValue, tokenLength);
                            (*tokens)[(*token_count) - 1][tokenLength] = '\0';
                        }
                    }
                }
            } while (StringToken_GetNext(tokenizer, delimiters, n_delims));

            StringToken_Destroy(tokenizer);

            if (result != 0)
            {
                while ((*token_count) > 0 && *tokens != NULL)
                {
                    *token_count = (*token_count) - 1;
                    free((*tokens)[*token_count]);
                }
                free(*tokens);
            }
        }
    }

    return result;
}

void StringToken_Destroy(STRING_TOKEN_HANDLE token)
{
    if (token == NULL)
    {
        // Codes_SRS_STRING_TOKENIZER_09_020: [ If token is NULL the function shall return ]
        LogError("Invalig argument (token is NULL)");
    }
    else
    {
        // Codes_SRS_STRING_TOKENIZER_09_021: [ Otherwise the memory allocated for STRING_TOKEN shall be released ]
        free(token);
    }
}
