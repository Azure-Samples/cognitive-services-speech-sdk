//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <string>
#include <codecvt>
#include <locale>
#include <wchar.h>
#include <speechapi_cxx_common.h>
#include <speechapi_c_error.h>
#include <speechapi_c_property_bag.h>

#if defined(SWIG) && defined(SPX_UWP)
#define SPXSTRING std::wstring
#define SPXSTRING_EMPTY std::wstring()
#else
#define SPXSTRING std::string
#define SPXSTRING_EMPTY std::string()
#endif

namespace Microsoft{
namespace CognitiveServices {
namespace Speech {
namespace Utils {

#if defined(SWIG) && defined(SPX_UWP)
inline std::wstring ToSPXString(const std::string& value)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(value);
}

inline std::wstring ToSPXString(const char* value)
{
    if (!value)
        return L"";
    return ToSPXString(std::string(value));
}
#else
inline std::string ToSPXString(const char* value)
{
    return value == nullptr ? "" : value;
}

inline std::string ToSPXString(const std::string& value)
{
    return value;
}
#endif

inline std::string ToUTF8(const std::wstring& value)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(value);
}

inline std::string ToUTF8(const wchar_t* value)
{
    if (!value)
        return "";
    return ToUTF8(std::wstring(value));
}

inline std::string ToUTF8(const std::string& value)
{
    return value;
}

inline const char* ToUTF8(const char* value)
{
    return value;
}

inline static std::string CopyAndFreePropertyString(const char* value)
{
    std::string copy = (value == nullptr) ? "" : value;
    property_bag_free_string(value);
    return copy;
}

}}}}
