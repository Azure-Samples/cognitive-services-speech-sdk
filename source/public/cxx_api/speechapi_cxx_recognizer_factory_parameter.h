//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory_parameter.h: Public API declarations for RecognizerFactoryParameterValue C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_c_common.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


enum class FactoryParameter { SpeechSubscriptionKey = 1, SpeechEndpoint = 2 };


class RecognizerFactoryParameterValue : public Value
{
public:

    RecognizerFactoryParameterValue(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name) : m_hrecofactory(hrecofactory), m_name(name) { }
    RecognizerFactoryParameterValue(SPXRECOFACTORYHANDLE hrecofactory, enum FactoryParameter parameter) : m_hrecofactory(hrecofactory), m_name(ParameterNameFromEnum(parameter)) { }

    bool IsString() override { return ContainsString(m_hrecofactory, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hrecofactory, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_hrecofactory, m_name.c_str(), value); }

    bool IsNumber() override { return ContainsNumber(m_hrecofactory, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hrecofactory, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_hrecofactory, m_name.c_str(), value); }

    bool IsBool() override { return ContainsBool(m_hrecofactory, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hrecofactory, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_hrecofactory, m_name.c_str(), value); }

    RecognizerFactoryParameterValue(RecognizerFactoryParameterValue&&) = delete;
    RecognizerFactoryParameterValue(const RecognizerFactoryParameterValue&) = delete;
    RecognizerFactoryParameterValue& operator=(RecognizerFactoryParameterValue&&) = delete;
    RecognizerFactoryParameterValue& operator=(const RecognizerFactoryParameterValue&) = delete;

private: 

    std::wstring ParameterNameFromEnum(FactoryParameter parameter)
    {
        static_assert((int)FactoryParameter_SpeechSubscriptionKey == (int)FactoryParameter::SpeechSubscriptionKey, "FactoryParameter_* enum values == FactoryParameter::* enum values");
        static_assert((int)FactoryParameter_SpeechEndpoint == (int)FactoryParameter::SpeechEndpoint, "FactoryParameter_* enum values == FactoryParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_Name(static_cast<Factory_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_String(hrecofactory, name, sz, maxCharCount, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_Int32(hrecofactory, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(RecognizerFactory_GetParameter_Bool(hrecofactory, name, &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_String(hrecofactory, name, value));
    }

    static void SetNumber(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_Int32(hrecofactory, name, value));
    }

    static void SetBool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(RecognizerFactory_SetParameter_Bool(hrecofactory, name, value));
    }

    static bool ContainsString(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
    {
        return RecognizerFactory_ContainsParameter_String(hrecofactory, name);
    }

    static bool ContainsNumber(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
    {
        return RecognizerFactory_ContainsParameter_Int32(hrecofactory, name);
    }

    static bool ContainsBool(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name)
    {
        return RecognizerFactory_ContainsParameter_Bool(hrecofactory, name);
    }

private:

    SPXRECOFACTORYHANDLE m_hrecofactory;
    std::wstring m_name;
};


class RecognizerFactoryParameterCollection : public HandleValueCollection<SPXRECOFACTORYHANDLE, RecognizerFactoryParameterValue>
{
public:

    RecognizerFactoryParameterCollection(SPXRECOFACTORYHANDLE hrecofactory = SPXHANDLE_DEFAULT) : HandleValueCollection(hrecofactory) { }

    Value operator[](const wchar_t* name) override { return Value(new RecognizerFactoryParameterValue(m_handle, name)); }
    Value operator[](enum FactoryParameter parameter) { return Value(new RecognizerFactoryParameterValue(m_handle, parameter)); }
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
