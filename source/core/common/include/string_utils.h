//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <string>
#include <vector>

namespace PAL {

// Similarly to strcpy_s, wcscpy_s functions, dstSize and srcSize are sizes of corresponding
// arrays in terms of number of elements (in wide characters).
void wcscpy(wchar_t *dst, size_t dstSize, const wchar_t *src, size_t srcSize, bool truncate);
void strcpy(char *dst, size_t dstSize, const char *src, size_t srcSize, bool truncate);

// Performs a case-insensitive comparison of two null-terminated strings given by a and b.
int wcsicmp(const wchar_t *a, const wchar_t *b);
int stricmp(const char *a, const char *b);

// Same as above, but only compares up to n elements (wide characters).
int wcsnicmp(const wchar_t *a, const wchar_t *b, size_t n);
int strnicmp(const char *a, const char *b, size_t n);

std::string ToString(const std::wstring& wstring);
std::wstring ToWString(const std::string& string);

std::string BoolToString(bool b);
bool ToBool(const std::string& str);

std::vector<std::string> split(std::string str, const std::string& token);

} // PAL
