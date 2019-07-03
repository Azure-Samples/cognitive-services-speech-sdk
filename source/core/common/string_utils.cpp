//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#if !defined(_MSC_VER)
#include <strings.h>
#include <cstdlib>
#include <clocale>
#endif

#include <codecvt>
#include <locale>
#include <wchar.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include "string_utils.h"

namespace PAL {

using namespace std;

int stricmp(const char *a, const char *b)
{
#ifdef _MSC_VER
    return _stricmp(a, b);
#else
    return ::strcasecmp(a, b);
#endif
}

int wcsicmp(const wchar_t *a, const wchar_t *b)
{
#ifdef _MSC_VER
    return _wcsicmp(a, b);
#else
    return ::wcscasecmp(a, b);
#endif
}

int wcsnicmp(const wchar_t *a, const wchar_t *b, size_t n)
{
#ifdef _MSC_VER
        return _wcsnicmp(a, b, n);
#else
        return ::wcsncasecmp(a, b, n);
#endif
}

int strnicmp(const char *a, const char *b, size_t n)
{
#ifdef _MSC_VER
    return _strnicmp(a, b, n);
#else
    return ::strncasecmp(a, b, n);
#endif
}

int xcsicmp(const WCHAR* a, const WCHAR* b)
{
#if defined(SPX_WCHAR_IS_8)
    return stricmp(a, b);
#elif defined(SPX_WCHAR_IS_16)
    std::u16string cmp1 = std::u16string(a);
    std::u16string cmp2 = std::u16string(b);
    return cmp1 == cmp2 ? 0 : 1;
#else
    return wcsicmp(a, b);
#endif
}

// Similarly to strcpy_s, wcscpy_s functions, dstSize and srcSize are sizes of corresponding
// arrays in terms of number of elements (in wide characters).
void strcpy(char *dst, size_t dstSize, const char *src, size_t srcSize, bool truncate)
{
    // TODO (alrezni): throw instead of asserting,
    // see https://msdn.microsoft.com/en-us/library/5dae5d43.aspx
    // and https://msdn.microsoft.com/en-us/library/td1esda9.aspx
    // for more details on error conditions, add unit tests.
    assert(src);
    assert(dst);
    assert(dstSize != 0);

    auto toCopy = std::min(dstSize, srcSize);

    if (!truncate)
    {
        assert(dstSize > toCopy);
    }

#ifdef _MSC_VER
    strncpy_s(dst, dstSize, src, toCopy);
#else
    std::strncpy(dst, src, toCopy);
#endif

    dst[std::min(toCopy, dstSize-1)] = 0; // make sure the string is null-terminated
}

void wcscpy(wchar_t *dst, size_t dstSize, const wchar_t *src, size_t srcSize, bool truncate)
{
    // TODO (alrezni): throw instead of asserting, 
    // see https://msdn.microsoft.com/en-us/library/5dae5d43.aspx
    // and https://msdn.microsoft.com/en-us/library/td1esda9.aspx
    // for more details on error conditions, add unit tests.
    assert(src);
    assert(dst);
    assert(dstSize != 0);

    auto toCopy = std::min(dstSize, srcSize);

    if (!truncate)
    {
        assert(dstSize > toCopy);
    }

#ifdef _MSC_VER
    wcsncpy_s(dst, dstSize, src, toCopy);
#else
    wcsncpy(dst, src, toCopy);
#endif

    dst[std::min(toCopy, dstSize-1)] = 0; // make sure the string is null-terminated
}

std::string ToString(const std::wstring& wstring)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstring);
}

std::wstring ToWString(const std::string& string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(string);
}

std::string BoolToString(bool b)
{
    return b ? "true" : "false";
}

bool ToBool(const std::string& str)
{
    return stricmp(str.c_str(), "true") == 0;
}

vector<string> split(const string& str, const char delim)
{
    vector<string> result;

    size_t start = 0;
    size_t end = str.find(delim);
    while (end != string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start));

    return result;
}

std::wstring WCHARToWString(const WCHAR* string)
{
    std::wstring result;
#ifdef _MSC_VER
    result = std::wstring(string);
#else
    const WCHAR* pch = string;
    while (*pch)
    {
        result += (wchar_t)((*pch) & 0xffff);
        pch++;
    }
#endif
    return result;
}

wchar_string ToWCHARString(const std::wstring& string)
{
#ifdef _MSC_VER
    return string;
#else
#if defined(SPX_WCHAR_IS_16)
    std::u16string result = ToU16String(string);
    return result;
#endif
#endif
}

std::u16string ToU16String(const std::wstring& string)
{
    std::u16string dest;
    auto *pch = string.c_str();
    while (*pch)
    {
        dest += (uint16_t)((*pch) & 0xffff);
        pch++;
    }
    return dest;
}

} // PAL
