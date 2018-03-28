//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory_parameter.h: Public API declarations for RecognizerFactoryParameter C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_c_common.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class RecognizerFactoryParameter : public Value
{
public:

    RecognizerFactoryParameter(const wchar_t* name) : m_name(name) { }

    bool IsString() override { return IsString(m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_name.c_str(), value); }

    bool IsNumber() override { return IsNumber(m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_name.c_str(), value); }

    bool IsBool() override { return IsBool(m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_name.c_str(), value); }

    RecognizerFactoryParameter(RecognizerFactoryParameter&&) = delete;
    RecognizerFactoryParameter(const RecognizerFactoryParameter&) = delete;
    RecognizerFactoryParameter& operator=(RecognizerFactoryParameter&&) = delete;
    RecognizerFactoryParameter& operator=(const RecognizerFactoryParameter&) = delete;

private: 

    static std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t cch = 1024;
        wchar_t sz[cch+1];
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_String(name, sz, cch, defaultValue));
        return sz;
    }

    static int32_t GetNumber(const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_Int32(name, &value, defaultValue));
        return value;
    }

    static bool GetBool(const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_Bool(name, &value, defaultValue));
        return !!value;
    }

    static void SetString(const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_String(name, value));
    }

    static void SetNumber(const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_Int32(name, value));
    }

    static void SetBool(const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_Bool(name, value));
    }

    static bool IsString(const wchar_t* name)
    {
        return RecognizerFactory_HasParameter_String(name);
    }

    static bool IsNumber(const wchar_t* name)
    {
        return RecognizerFactory_HasParameter_Int32(name);
    }

    static bool IsBool(const wchar_t* name)
    {
        return RecognizerFactory_HasParameter_Bool(name);
    }

private:

    std::wstring m_name;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
