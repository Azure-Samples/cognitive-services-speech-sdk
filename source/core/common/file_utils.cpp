//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include  <wchar.h>
#include "file_utils.h"
#include "platform.h"

namespace PAL {

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

errno_t fopen_s(FILE** file, const char* fileName, const char* mode)
{
#ifdef _MSC_VER
    return ::fopen_s(file, fileName, mode);
#else
    FILE *f = fopen(fileName, mode);
    if (f == NULL) 
    {
        return -1;
    }
    *file = f;
    return 0;
#endif
}


int waccess(const wchar_t *path, int mode)
{
#ifdef _MSC_VER
    return _waccess(path, mode);
#else
    return access(wtocharpath(path).c_str(), mode);
#endif
}

void OpenStream(std::fstream& stream, const std::wstring& filename, bool readonly)
{
    if (filename.empty())
        throw std::runtime_error("File: filename is empty");

    std::ios_base::openmode mode = std::ios_base::binary;
    mode = mode | (readonly ? std::ios_base::in : std::ios_base::out);

#ifdef _MSC_VER
    stream.open(filename.c_str(), mode);
#else
    stream.open(wtocharpath(filename.c_str()).c_str(), mode);
#endif
}

}; // PAL