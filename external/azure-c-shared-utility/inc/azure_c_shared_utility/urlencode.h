// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef URLENCODE_H
#define URLENCODE_H

#include "azure_c_shared_utility/strings.h"

#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_EncodeString, const char*, textEncode);
    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_Encode, STRING_HANDLE, input);

#ifdef __cplusplus
}
#endif

#endif /* URLENCODE_H */

