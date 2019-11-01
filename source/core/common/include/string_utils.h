//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <string>
#include <vector>

#ifndef _MSC_VER
#define SPX_WCHAR_IS_16 1
#endif

#if defined(SPX_WCHAR_IS_16)
#define WCHAR char16_t
#define _W(x) u ## x
#define _WR(x) uR ## x
#define wchar_string std::u16string
#else
typedef wchar_t WCHAR;
#define _W(x) L ## x
#define _WR(x) LR ## x
#define wchar_string std::wstring
#endif

namespace PAL {

// Similarly to strcpy_s, wcscpy_s functions, dstSize and srcSize are sizes of corresponding
// arrays in terms of number of elements (in wide characters).
void wcscpy(wchar_t *dst, size_t dstSize, const wchar_t *src, size_t srcSize, bool truncate);
void strcpy(char *dst, size_t dstSize, const char *src, size_t srcSize, bool truncate);

// Performs a case-insensitive comparison of two null-terminated strings given by a and b.
int wcsicmp(const wchar_t *a, const wchar_t *b);
int stricmp(const char *a, const char *b);
int xcsicmp(const WCHAR *a, const WCHAR *b);

// Same as above, but only compares up to n elements (wide characters).
int wcsnicmp(const wchar_t *a, const wchar_t *b, size_t n);
int strnicmp(const char *a, const char *b, size_t n);

std::string ToString(const std::wstring& wstring);
std::wstring ToWString(const std::string& string);

std::string BoolToString(bool b);
bool ToBool(const std::string& str);

std::vector<std::string> split(const std::string& str, const char delim);
std::u16string ToU16String(const std::wstring& wstring);
std::wstring WCHARToWString(const WCHAR* string);
wchar_string ToWCHARString(const std::wstring& string);

/// <summary>
/// String helper functions
/// </summary>
class StringUtils
{
public:
    /// <summary>
    /// Converts an ANSI string to upper case.
    /// CAUTION: This only works for ANSI strings. UTF-8 string with multi byte characters
    /// will break
    /// </summary>
    /// <param name="value">The value to convert to upper case</param>
    /// <returns>An upper case copy of the string</returns>
    static std::string ToUpper(const std::string& value);

    /// <summary>
    /// Converts an ANSI string to lower case.
    /// CAUTION: This only works for ANSI strings. UTF-8 string with multi byte characters
    /// will break
    /// </summary>
    /// <param name="value">The value to convert to lower case</param>
    /// <returns>An lower case copy of the string</returns>
    static std::string ToLower(const std::string& value);

    /// <summary>
    /// Tokenizes a string based on the passed in delimiters. Please note that empty tokens
    /// will be skipped over
    /// </summary>
    /// <param name="str">The string to tokenize</param>
    /// <param name="len">The length of the string to tokenize</param>
    /// <param name="delim">The delimiters to use</param>
    /// <returns>The string tokens</returns>
    static std::vector<std::string> Tokenize(const char *str, const size_t len, const char *delim);

    /// <summary>
    /// Tokenizes a string based on the passed in delimiters. Please note that empty tokens
    /// will be skipped over
    /// </summary>
    /// <param name="str">The string to tokenize</param>
    /// <param name="delim">The delimiters to use</param>
    /// <returns>The string tokens</returns>
    static std::vector<std::string> Tokenize(const std::string& str, const char *delim);

    /// <summary>
    /// Converts a string from PascalCase to snake_case. For example ThisIsAShortTest
    /// would become this_is_a_short_test.
    /// CAUTION: This only works for ANSI strings. UTF-8 string with multi byte characters
    /// will break
    /// </summary>
    /// <param name="str">The Pascal case string to convert</param>
    /// <returns>The equivalent snake case string</returns>
    static std::string PascalCaseToSnakeCase(const std::string& str);

    /// <summary>
    /// Converts a string from snake_case to PascalCase. For example this_is_a_short_test
    /// would become ThisIsAShortTest
    /// CAUTION: This only works for ANSI strings. UTF-8 string with multi byte characters
    /// will break
    /// </summary>
    /// <param name="str">The snake case string to convert</param>
    /// <returns>The equivalent Pascal case string</returns>
    static std::string SnakeCaseToPascalCase(const std::string& str);

    /// <summary>
    /// Gets an STL string from a C style null terminated string.
    /// </summary>
    /// <param name="str">A null terminated C style string.</param>
    /// <returns>An empty string if str is null, or the STL string equivalent.</returns>
    static inline std::string AsStringOrEmpty(const char* str)
    {
        if (str == nullptr)
        {
            return std::string{};
        }
        else
        {
            return std::string{ str };
        }
    }
};

} // PAL

