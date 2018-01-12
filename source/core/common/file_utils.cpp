//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include  <wchar.h>
#include "file_utils.h"
#include "platform.h"

#ifndef _MSC_VER
#include <unistd.h>

std::string wtocharpath(const wchar_t *wstr)
{
    size_t length = wcslen(wstr);
    std::string str;
    str.resize(2 * length + 1); // max: 1 wchar => 2 mb chars
    std::wcstombs(&str[0], wstr, str.size()); // note: technically it is forbidden to stomp over std::strings 0 terminator, but it is known to work in all implementations
    str.resize(std::strlen(&str[0])); // set size correctly for shorter strings
    return str;
}

#endif

int PAL_waccess(const wchar_t *path, int mode)
{
#ifdef _MSC_VER
    return _waccess(path, mode);
#else
    return access(wtocharpath(path).c_str(), mode);
#endif
}
