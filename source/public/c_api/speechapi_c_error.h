//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
//

#pragma once
#include <speechapi_c_common.h>

typedef const char * const_char_ptr;

SPXAPI_(const_char_ptr) error_get_message(SPXERRORHANDLE errorHandle);

SPXAPI_(const_char_ptr) error_get_call_stack(SPXERRORHANDLE errorHandle);

SPXAPI error_get_error_code(SPXERRORHANDLE errorHandle);

SPXAPI error_release(SPXERRORHANDLE errorHandle);
