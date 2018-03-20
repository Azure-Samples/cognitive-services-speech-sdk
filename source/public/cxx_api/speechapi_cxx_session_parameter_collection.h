//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session_parameter_collection.h: Public API declarations for SessionParameterCollection C++ class
//

#pragma once
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {


class SessionParameter : public Value
{
public:

    SessionParameter(SPXSESSIONHANDLE hsession, const wchar_t* name) :
        m_hsession(hsession),
        m_name(name)
    {
    }

    // --- Value virtual overrides ---

    bool IsString() override { return HasString(m_hsession, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hsession, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_hsession, m_name.c_str(), value); }

    bool IsNumber() override { return HasNumber(m_hsession, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hsession, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_hsession, m_name.c_str(), value); }

    bool IsBool() override { return HasBool(m_hsession, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hsession, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_hsession, m_name.c_str(), value); }

    // --- Static helpers ---
    
    static std::wstring GetString(SPXSESSIONHANDLE hsession, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t cch = 1024;
        wchar_t sz[cch+1];
        SPX_THROW_ON_FAIL(Session_GetParameter_String(hsession, name, sz, cch, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXSESSIONHANDLE hsession, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Session_GetParameter_Int32(hsession, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXSESSIONHANDLE hsession, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Session_GetParameter_Bool(hsession, name, &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXSESSIONHANDLE hsession, const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(Session_SetParameter_String(hsession, name, value));
    }

    static void SetNumber(SPXSESSIONHANDLE hsession, const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Session_SetParameter_Int32(hsession, name, value));
    }

    static void SetBool(SPXSESSIONHANDLE hsession, const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(Session_SetParameter_Bool(hsession, name, value));
    }

    static bool HasString(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_HasParameter_String(hsession, name);
    }

    static bool HasNumber(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_HasParameter_Int32(hsession, name);
    }

    static bool HasBool(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_HasParameter_Bool(hsession, name);
    }

private:

    SPXSESSIONHANDLE m_hsession;
    std::wstring m_name;
};


class SessionParameterCollection : public ValueCollection<SPXSESSIONHANDLE, SessionParameter>
{
public:

    SessionParameterCollection(SPXSESSIONHANDLE hsession) :
        ValueCollection(hsession)
    {
    }
};


} // CARBON_NAMESPACE_ROOT
