//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once

#include <fstream>

#ifndef _MSC_VER
#define _FILE_OFFSET_BITS 64
#include <string>
typedef int errno_t;
#endif

namespace PAL {

    int waccess(const wchar_t *path, int mode);
    int access(const char *path, int mode);

    errno_t fopen_s(FILE **file, const char *fileName, const char *mode);

    void OpenStream(std::fstream& stream, const std::wstring& filename, bool readonly);
} // PAL
