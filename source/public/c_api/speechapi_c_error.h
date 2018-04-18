//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include "speechapi_c_common.h"

typedef const char * const_char_ptr;

SPXAPI_(const_char_ptr) Error_GetMessage(SPXERRORHANDLE errorHandle);

SPXAPI_(const_char_ptr) Error_GetCallStack(SPXERRORHANDLE errorHandle);

SPXAPI Error_GetCode(SPXERRORHANDLE errorHandle);
