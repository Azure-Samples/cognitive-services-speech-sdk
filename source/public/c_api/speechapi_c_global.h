//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI Global_SetParameter_String(const wchar_t* name, const wchar_t* value);
SPXAPI Global_GetParameter_String(const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) Global_HasParameter_String(const wchar_t* name);

SPXAPI Global_SetParameter_Int32(const wchar_t* name, int32_t value);
SPXAPI Global_GetParameter_Int32(const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) Global_HasParameter_Int32(const wchar_t* name);

SPXAPI Global_SetParameter_Bool(const wchar_t* name, bool value);
SPXAPI Global_GetParameter_Bool(const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) Global_HasParameter_Bool(const wchar_t* name);