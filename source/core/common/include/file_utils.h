//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once

int PAL_waccess(const wchar_t *path, int mode);

#ifndef _MSC_VER
#include <string>

std::string wtocharpath(const wchar_t *wstr);
#endif
