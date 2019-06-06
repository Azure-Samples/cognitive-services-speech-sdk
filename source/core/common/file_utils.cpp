//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include  <wchar.h>
#include "file_utils.h"
#include "platform.h"
#include <cstring>
#include "string_utils.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

namespace PAL {

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
    return path ? ::access(PAL::ToString(path).c_str(), mode) : -1;
#endif
}

int access(const char *path, int mode)
{
#ifdef _MSC_VER
    auto s = ToWString(path);
    return _waccess(s.c_str(), mode);
#else
    return ::access(path, mode);
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
    stream.open(PAL::ToString(filename.c_str()).c_str(), mode);
#endif
}

std::string AppendPath(const std::string& str1, const std::string& str2)
{
    std::string appendedPath;
    char separator = '/';
    std::string tmp = str1;

#ifdef _WIN32
    separator = '\\';
#endif

    if (str1[str1.length()] != separator)
    {
        tmp += separator;
        appendedPath = tmp + str2;
    }
    else
    {
        appendedPath = str1 + str2;
    }
    return appendedPath;
}

} // PAL
