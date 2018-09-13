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

vector<string> split(string str, const string& token)
{
    vector<string> result;
    while (str.size())
    {
        size_t index = str.find(token);
        if (index != string::npos)
        {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)
                result.push_back(str);
        }
        else
        {
            result.push_back(str);
            str.clear();
        }
    }

    return result;
}

} // PAL
