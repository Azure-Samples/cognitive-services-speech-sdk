//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <speechapi_c_common.h>

typedef const char * const_char_ptr;

SPXAPI_(const_char_ptr) error_get_message(SPXERRORHANDLE errorHandle);

SPXAPI_(const_char_ptr) error_get_call_stack(SPXERRORHANDLE errorHandle);

SPXAPI error_get_error_code(SPXERRORHANDLE errorHandle);
