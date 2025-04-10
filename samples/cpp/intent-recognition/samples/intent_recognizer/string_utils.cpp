//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
//

#if !defined(_MSC_VER)
#include <strings.h>
#include <cstdlib>
#include <clocale>
#else
#include <windows.h>
#undef min
#undef max
#undef pascal
#endif

#include <ctype.h>
#include <cctype>
#include <codecvt>
#include <locale>
#include <wchar.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <cstring>
#include <sstream>
#include <iterator>
#include "string_utils.h"
#include <stdint.h>
#include <memory>

namespace PAL {

using namespace std;

/// <summary>
/// Performs a case-insensitive comparison of strings.
/// </summary>
/// <param name="string1">First Null-terminated string to compare..</param>
/// <param name="string2">Second Null-terminated string to compare..</param>
/// <returns>The return value indicates the relation of string1 to string2 as follows.
/// <list type="table">
///     <item>
///         <value>&lt; 0</value>
///         <description>string1 less than string2</description>
///     </item>
///     <item>
///         <value>= 0</value>
///         <description>string1 is identical to string2</description>
///     </item>
///     <item>
///         <value>&gt; 0</value>
///         <description>string1 greater than string2</description>
///     </item>
/// </list>
///
/// </returns>
int stricmp(const char * string1, const char * string2)
{
#ifdef _MSC_VER
    return _stricmp(string1, string2);
#else
    return ::strcasecmp(string1, string2);
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
    assert(src);
    assert(dst);
    assert(dstSize != 0);

    auto toCopy = std::min(dstSize, srcSize);
    if (dstSize == toCopy && src[toCopy - 1] != 0)
    {
        if (!truncate)
        {
            throw std::invalid_argument("Destination buffer is too small.");
        }
        else // truncate; the array will be null-terminated after copy
        {
            toCopy--; // prevent runtime error from strncpy_s
        }
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
    assert(src);
    assert(dst);
    assert(dstSize != 0);

    auto toCopy = std::min(dstSize, srcSize);
    if (dstSize == toCopy && src[toCopy - 1] != 0)
    {
        if (!truncate)
        {
            throw std::invalid_argument("Destination buffer is too small.");
        }
        else // truncate; the array will be null-terminated after copy
        {
            toCopy--; // prevent runtime error from wcsncpy_s
        }
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
#ifdef _MSC_VER
    constexpr DWORD flags = WC_ERR_INVALID_CHARS;
    const auto utf16Length = static_cast<int>(wstring.length());
    const auto utf8Length = ::WideCharToMultiByte(CP_UTF8, flags, wstring.data(), utf16Length, nullptr, 0, nullptr, nullptr);
    std::string target{};
    target.resize(utf8Length);
    ::WideCharToMultiByte(CP_UTF8, flags, &wstring[0], utf16Length, &target[0], utf8Length, nullptr, nullptr);
    return target;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstring);
#endif
}

std::wstring ToWString(const std::string& string)
{
#ifdef _MSC_VER
    constexpr DWORD flags = MB_ERR_INVALID_CHARS;
    const auto utf8Length = static_cast<int>(string.length());
    const auto utf16Size = ::MultiByteToWideChar(CP_UTF8, flags, string.data(), utf8Length, nullptr, 0);
    std::wstring target{};
    target.resize(utf16Size);
    ::MultiByteToWideChar(CP_UTF8, flags, string.data(), utf8Length, &target[0], utf16Size);
    return target;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(string);
#endif
}

std::u32string StringToU32String(const std::string& string)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(string);
}

bool ToBool(const std::string& str)
{
    return !str.empty() &&
           (stricmp(str.c_str(), "false") != 0) &&
           (str != "0");
}

std::string Join(const std::vector<std::string>& elements, const char* const delimiter)
{
    std::ostringstream os;
    auto b = begin(elements), e = end(elements);

    if (b != e)
    {
        std::copy(b, prev(e), std::ostream_iterator<std::string>(os, delimiter));
        b = prev(e);
    }
    if (b != e)
    {
        os << *b;
    }

    return os.str();
}

std::string Join(const std::list<std::string>& elements, const char* const delimiter)
{
    std::ostringstream os;
    auto b = begin(elements), e = end(elements);

    if (b != e)
    {
        std::copy(b, prev(e), std::ostream_iterator<std::string>(os, delimiter));
        b = prev(e);
    }
    if (b != e)
    {
        os << *b;
    }

    return os.str();
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

    if (!str.empty())
    {
        result.push_back(str.substr(start));
    }

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

#ifdef WIN32
    #define strtok_reentrant(str, delimiters, context) strtok_s(str, delimiters, context)
#else
    #define strtok_reentrant(str, delimiters, context) strtok_r(str, delimiters, context)
#endif

    static std::string TransformString(const std::string& value, char(*transformer)(char, const std::locale&))
    {
        if (value.empty())
        {
            return value;
        }

        std::string transformed(value);
        std::transform(
            transformed.begin(),
            transformed.end(),
            transformed.begin(),
            [transformer](char c) { return transformer(c, std::locale::classic()); });
        return transformed;
    }

    std::string StringUtils::ToUpper(const std::string & value)
    {
        return TransformString(value, std::toupper);
    }

    std::string StringUtils::ToLower(const std::string& value)
    {
        return TransformString(value, std::tolower);
    }

    std::vector<std::string> StringUtils::Tokenize(const char *str, const size_t len, const char *delim)
    {
        std::vector<std::string> tokens;

        std::unique_ptr<char[]> copy(new char[len + 1]);
        if (copy == nullptr)
        {
            return tokens;
        }

        memcpy(copy.get(), str, len);
        copy[len] = '\0';

        char *ptr = nullptr;

        for (char *tkn = strtok_reentrant(copy.get(), delim, &ptr)
            ; tkn != nullptr
            ; tkn = strtok_reentrant(nullptr, delim, &ptr))
        {
            tokens.push_back(tkn);
        }

        return tokens;
    }

    std::vector<std::string> StringUtils::Tokenize(const std::string& str, const char *delim)
    {
        return Tokenize(str.c_str(), str.length(), delim);
    }

    std::string StringUtils::PascalCaseToSnakeCase(const std::string & pascal)
    {
        std::string snake_case;
        snake_case.reserve(static_cast<size_t>(pascal.length() * 1.2));

        bool first = true;
        for (size_t i = 0; i < pascal.length(); i++, first = false)
        {
            auto c = (unsigned char)(pascal[i]);
            if (isupper(c))
            {
                if (!first)
                {
                    snake_case += "_";
                }

                snake_case += (char)(tolower(c));
            }
            else
            {
                snake_case += (char)(c);
            }
        }

        return snake_case;
    }

    std::string StringUtils::SnakeCaseToPascalCase(const std::string & snake_case)
    {
        std::string pascal(snake_case);

        bool capitaliseNext = true;
        size_t o = 0;
        for (size_t i = 0; i < snake_case.length() && o < snake_case.length(); i++)
        {
            auto c = (unsigned char)(snake_case[i]);

            if (c == '_')
            {
                capitaliseNext = true;
                continue;
            }

            if (capitaliseNext)
            {
                pascal[o++] = (char)(toupper(c));
                capitaliseNext = false;
            }
            else
            {
                pascal[o++] = (char)c;
            }
        }

        // the new string may be shorter than the previous one so let's resize it
        pascal.resize(o);
        return pascal;
    }

    static void TrimDetermineStart(const std::string& str, size_t& startIndex, const size_t endIndex, bool(*predicate)(const char, bool))
    {
        startIndex = 0;

        // determine where to start the trimmed string from
        for (size_t i = 0; i < endIndex; i++)
        {
            char c = str[i];
            if (isspace(static_cast<int>(c)) || (predicate != nullptr && predicate(c, true)))
            {
                startIndex++;
            }
            else
            {
                break;
            }
        }
    }

    static void TrimDetermineEnd(const std::string& str, const size_t startIndex, size_t& endIndex, bool(*predicate)(const char, bool))
    {
        endIndex = str.length();

        // determine where to end the trimmed string
        if (str.length() > 0)
        {
            for (size_t i = str.length() - 1; i > startIndex; i--)
            {
                char c = str[i];
                if (isspace(static_cast<int>(str[i])) || (predicate != nullptr && predicate(c, false)))
                {
                    endIndex--;
                }
                else
                {
                    break;
                }
            }
        }
    }

    static void TrimDetermineStartEnd(const std::string& str, size_t& startIndex, size_t& endIndex, bool(*predicate)(const char, bool))
    {
        startIndex = 0;
        endIndex = str.length();

        TrimDetermineStart(str, startIndex, endIndex, predicate);
        TrimDetermineEnd(str, startIndex, endIndex, predicate);
    }

    const std::string StringUtils::Trim(const std::string& str)
    {
        size_t startIndex, endIndex;
        TrimDetermineStartEnd(str, startIndex, endIndex, nullptr);
        return std::string(str, startIndex, endIndex - startIndex);
    }

    void StringUtils::Trim(const std::string& str, std::ostringstream& oss, bool(*predicate)(const char, bool))
    {
        size_t startIndex, endIndex;
        TrimDetermineStartEnd(str, startIndex, endIndex, predicate);
        oss.write(str.c_str() + startIndex, endIndex - startIndex);
    }

    const std::string StringUtils::TrimStart(const std::string& str)
    {
        size_t startIndex = 0, endIndex = str.length();
        TrimDetermineStart(str, startIndex, endIndex, nullptr);
        return std::string(str, startIndex, endIndex - startIndex);
    }

    const std::string StringUtils::TrimEnd(const std::string& str)
    {
        size_t startIndex = 0, endIndex = str.length();
        TrimDetermineEnd(str, startIndex, endIndex, nullptr);
        return std::string(str, startIndex, endIndex - startIndex);
    }

    std::string StringUtils::FormatString(
        const std::string& format,
        const std::map<std::string, std::string>& values,
        bool exceptionsOnErrors)
    {
        // create strings to append to and reserve extra space to avoid re-allocation
        std::string appendTo;
        appendTo.reserve(static_cast<size_t>(format.length() * 1.2));

        AppendFormatString(appendTo, format, values, exceptionsOnErrors);

        return appendTo;
    }

    void StringUtils::AppendFormatString(
        std::string& appendTo,
        const std::string& format,
        const std::map<std::string, std::string>& values,
        bool exceptionsOnErrors)
    {
        if (format.empty())
        {
            return;
        }

        std::string value;
        std::string* active = &appendTo;

        for (size_t i = 0; i < format.size(); i++)
        {
            char current = format[i];
            char next = i < format.size() - 1 ? format[i + 1] : '\0';

            switch (current)
            {
                case '{':
                    if (next == '{')
                    {
                        // escaped '{'
                        *active += current;
                        i++;
                    }
                    else
                    {
                        active = &value;
                    }
                    break;

                case '}':
                    if (next == '}')
                    {
                        // escaped '}'
                        *active += current;
                        i++;
                    }
                    else
                    {
                        auto found = values.find(value);
                        if (found == values.end())
                        {
                            if (exceptionsOnErrors)
                            {
                                throw std::invalid_argument("Invalid replacement value at index " + std::to_string(i - value.length()) + ": {" + value + "}");
                            }
                            else
                            {
                                // add back the replacement string
                                appendTo += "{";
                                appendTo += value;
                                appendTo += "}";
                            }
                        }
                        else
                        {
                            appendTo += found->second;
                        }

                        value.clear();
                        active = &appendTo;
                    }
                    break;

                default:
                    *active += current;
                    break;
            }
        }

        // Do we have any trailing replacement keys?
        if (active == &value)
        {
            if (exceptionsOnErrors)
            {
                throw std::invalid_argument("Incorrect replacement value at end of string");
            }
            else
            {
                appendTo += '{';
                appendTo += value;
            }
        }
    }

    bool StringUtils::ReplaceWithSubString(std::string& input, const std::string& key, const std::string& replaceString)
    {
        bool success = false;
        size_t pos = input.find(key);
        if (pos != std::string::npos)
        {
            try
            {
                input.replace(pos + key.length(), replaceString.length(), replaceString, 0, replaceString.length());
                success = true;
            }
            catch (std::exception&)
            {
                success = false;
            }
        }
        return success;
    }
} // PAL

