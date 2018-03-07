//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_global_parameter_collection.h: Public API declarations for GlobalParameterCollection C++ class
//

#pragma once
#include <speechapi_c_common.h>
#include <speechapi_c_global.h>
#include <speechapi_cxx_parameter.h>


namespace CARBON_NAMESPACE_ROOT {


class GlobalParameter : public Parameter
{
public:

    GlobalParameter(SPXHANDLE, const wchar_t* name) :
        m_name(name)
    {
    }

    // --- Parameter virtual overrides ---

    bool HasString() override { return HasString(m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_name.c_str(), value); }

    bool HasNumber() override { return HasNumber(m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_name.c_str(), value); }

    bool HasBool() override { return HasBool(m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_name.c_str(), value); }

    // --- Static helpers ---
    
    static std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t cch = 1024;
        wchar_t sz[cch+1];
        SPX_THROW_ON_FAIL(Global_GetParameter_String(name, sz, cch, defaultValue));
        return sz;
    }

    static int32_t GetNumber(const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Global_GetParameter_Int32(name, &value, defaultValue));
        return value;
    }

    static bool GetBool(const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Global_GetParameter_Bool(name, &value, defaultValue));
        return !!value;
    }

    static void SetString(const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(Global_SetParameter_String(name, value));
    }

    static void SetNumber(const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Global_SetParameter_Int32(name, value));
    }

    static void SetBool(const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(Global_SetParameter_Bool(name, value));
    }

    static bool HasString(const wchar_t* name)
    {
        return Global_HasParameter_String(name);
    }

    static bool HasNumber(const wchar_t* name)
    {
        return Global_HasParameter_Int32(name);
    }

    static bool HasBool(const wchar_t* name)
    {
        return Global_HasParameter_Bool(name);
    }

private:

    std::wstring m_name;
};


class GlobalParameterCollection : public ParameterCollection<SPXRECOHANDLE, GlobalParameter>
{
public:

    GlobalParameterCollection() : ParameterCollection(nullptr)
    {
    }


private:

    GlobalParameterCollection(GlobalParameterCollection&&) = delete;
    GlobalParameterCollection(const GlobalParameterCollection&) = delete;
    GlobalParameterCollection& operator=(GlobalParameterCollection&&) = delete;
    const GlobalParameterCollection& operator=(const GlobalParameterCollection&) = delete;
};


} // CARBON_NAMESPACE_ROOT :: Recognition
