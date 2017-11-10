// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRING_TOKENIZER_H
#define STRING_TOKENIZER_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif

typedef struct STRING_TOKEN_TAG* STRING_TOKENIZER_HANDLE;

MOCKABLE_FUNCTION(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create_from_char, const char*, input);
MOCKABLE_FUNCTION(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
MOCKABLE_FUNCTION(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, t);

#ifdef __cplusplus
}
#else
#endif

#endif  /*STRING_TOKENIZER_H*/
