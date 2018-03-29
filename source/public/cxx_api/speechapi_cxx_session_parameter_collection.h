//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session_parameter_collection.h: Public API declarations for SessionParameterValueCollection C++ class
//

#pragma once
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {


enum class SessionParameter { SpeechSubscriptionKey = 1, SpeechEndpoint = 2 };


class SessionParameterValue : public Value
{
public:

    SessionParameterValue(SPXSESSIONHANDLE hsession, const wchar_t* name) : m_hsession(hsession), m_name(name) { }
    SessionParameterValue(SPXSESSIONHANDLE hsession, SessionParameter parameter) : m_hsession(hsession), m_name(ParameterNameFromEnum(parameter)) { }

    // --- Value virtual overrides ---

    bool IsString() override { return ContainsString(m_hsession, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hsession, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_hsession, m_name.c_str(), value); }

    bool IsNumber() override { return ContainsNumber(m_hsession, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hsession, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_hsession, m_name.c_str(), value); }

    bool IsBool() override { return ContainsBool(m_hsession, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hsession, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_hsession, m_name.c_str(), value); }


private:

    std::wstring ParameterNameFromEnum(SessionParameter parameter)
    {
        static_assert((int)SessionParameter_SpeechSubscriptionKey == (int)SessionParameter::SpeechSubscriptionKey, "SessionParameter_* enum values == SessionParameter::* enum values");
        static_assert((int)SessionParameter_SpeechEndpoint == (int)SessionParameter::SpeechEndpoint, "SessionParameter_* enum values == SessionParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Session_GetParameter_Name(static_cast<Session_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXSESSIONHANDLE hsession, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Session_GetParameter_String(hsession, name, sz, maxCharCount, defaultValue));
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

    static bool ContainsString(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_ContainsParameter_String(hsession, name);
    }

    static bool ContainsNumber(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_ContainsParameter_Int32(hsession, name);
    }

    static bool ContainsBool(SPXSESSIONHANDLE hsession, const wchar_t* name)
    {
        return Session_ContainsParameter_Bool(hsession, name);
    }

private:

    SPXSESSIONHANDLE m_hsession;
    std::wstring m_name;
};


class SessionParameterValueCollection : public HandleValueCollection<SPXSESSIONHANDLE, SessionParameterValue>
{
public:

    SessionParameterValueCollection(SPXSESSIONHANDLE hsession) :
        HandleValueCollection(hsession)
    {
    }

    Value operator[](const wchar_t* name) override { return Value(new SessionParameterValue(m_handle, name)); }
    Value operator[](enum SessionParameter parameter) { return Value(new SessionParameterValue(m_handle, parameter)); }
};


} // CARBON_NAMESPACE_ROOT
