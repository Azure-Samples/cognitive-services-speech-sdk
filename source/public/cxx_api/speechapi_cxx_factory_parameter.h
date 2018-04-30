//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_factory_parameter.h: Public API declarations for FactoryParameterValue C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <speechapi_cxx_value.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Enumerates parameters that can be used to configure a recognizer factory.
/// </summary>
enum class FactoryParameter { Region = 1, SubscriptionKey = 2, AuthorizationToken = 3, Endpoint = 4};

/// <summary>
/// Represents the value of a recognizer factory parameter returned by the subscript operator
/// on the corresponding parameter value collection.
/// </summary>
class FactoryParameterValue : public Value
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a parameter name.
    /// </summary>
    FactoryParameterValue(SPXFACTORYHANDLE hfactory, const std::wstring& name) : m_hfactory(hfactory), m_name(name) { }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a FactoryParameter value.
    /// </summary>
    FactoryParameterValue(SPXFACTORYHANDLE hfactory, FactoryParameter parameter) : m_hfactory(hfactory), m_name(ParameterNameFromEnum(parameter)) { }

    /// <summary>
    /// Returns true if the encapsulated value has a string type.
    /// </summary>
    bool IsString() override { return ContainsString(m_hfactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this FactoryParameterValue as a string.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this FactoryParameterValue instance corresponds
    /// to a non-existing string parameter. </param>
    std::wstring GetString(const std::wstring& defaultValue) override { return GetString(m_hfactory, m_name.c_str(), defaultValue.c_str()); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const std::wstring& value) override { return SetString(m_hfactory, m_name, value); }

    /// <summary>
    /// Returns true if the encapsulated value has a number type.
    /// </summary>
    bool IsNumber() override { return ContainsNumber(m_hfactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this FactoryParameterValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this FactoryParameterValue instance corresponds
    /// to a non-existing number parameter. </param>
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hfactory, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified number inside the encapsulated value.
    /// </summary>
    void SetNumber(int32_t value) override { SetNumber(m_hfactory, m_name.c_str(), value); }

    /// <summary>
    /// Returns true if the encapsulated value has a Boolean type.
    /// </summary>
    bool IsBool() override { return ContainsBool(m_hfactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this FactoryParameterValue as a Boolean.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this FactoryParameterValue instance corresponds
    /// to a non-existing parameter. </param>
    bool GetBool(bool defaultValue) override { return GetBool(m_hfactory, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified Boolean inside the encapsulated value.
    /// </summary>
    void SetBool(bool value) override { SetBool(m_hfactory, m_name.c_str(), value); }

private: 

    std::wstring ParameterNameFromEnum(FactoryParameter parameter)
    {
        static_assert((int)FactoryParameter_Region == (int)FactoryParameter::Region, "FactoryParameter_* enum values == FactoryParameter::* enum values");
        static_assert((int)FactoryParameter_SubscriptionKey == (int)FactoryParameter::SubscriptionKey, "FactoryParameter_* enum values == FactoryParameter::* enum values");
        static_assert((int)FactoryParameter_AuthorizationToken == (int)FactoryParameter::AuthorizationToken, "FactoryParameter_* enum values == FactoryParameter::* enum values");
        static_assert((int)FactoryParameter_Endpoint == (int)FactoryParameter::Endpoint, "FactoryParameter_* enum values == FactoryParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(SpeechFactory_GetParameter_Name(static_cast<Factory_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXFACTORYHANDLE hfactory, const std::wstring& name, const std::wstring& defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(SpeechFactory_GetParameter_String(hfactory, name.c_str(), sz, maxCharCount, defaultValue.c_str()));
        return sz;
    }

    static int32_t GetNumber(SPXFACTORYHANDLE hfactory, const std::wstring& name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(SpeechFactory_GetParameter_Int32(hfactory, name.c_str(), &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXFACTORYHANDLE hfactory, const std::wstring& name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(SpeechFactory_GetParameter_Bool(hfactory, name.c_str(), &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXFACTORYHANDLE hfactory, const std::wstring& name, const std::wstring& value)
    {
        SPX_THROW_ON_FAIL(SpeechFactory_SetParameter_String(hfactory, name.c_str(), value.c_str()));
    }

    static void SetNumber(SPXFACTORYHANDLE hfactory, const std::wstring& name, int32_t value)
    {
        SPX_THROW_ON_FAIL(SpeechFactory_SetParameter_Int32(hfactory, name.c_str(), value));
    }

    static void SetBool(SPXFACTORYHANDLE hfactory, const std::wstring& name, bool value)
    {
        SPX_THROW_ON_FAIL(SpeechFactory_SetParameter_Bool(hfactory, name.c_str(), value));
    }

    static bool ContainsString(SPXFACTORYHANDLE hfactory, const std::wstring& name)
    {
        return SpeechFactory_ContainsParameter_String(hfactory, name.c_str());
    }

    static bool ContainsNumber(SPXFACTORYHANDLE hfactory, const std::wstring& name)
    {
        return SpeechFactory_ContainsParameter_Int32(hfactory, name.c_str());
    }

    static bool ContainsBool(SPXFACTORYHANDLE hfactory, const std::wstring& name)
    {
        return SpeechFactory_ContainsParameter_Bool(hfactory, name.c_str());
    }

private:

    SPXFACTORYHANDLE m_hfactory;
    std::wstring m_name;
};

/// <summary>
/// Represents a collection of named recognizer factory parameters.
/// </summary>
class FactoryParameterCollection : public HandleValueCollection<SPXFACTORYHANDLE, FactoryParameterValue>
{
public:
    typedef HandleValueCollection<SPXFACTORYHANDLE, FactoryParameterValue> base_type;

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit FactoryParameterCollection(SPXFACTORYHANDLE hfactory) : HandleValueCollection(hfactory) { }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="name">String name of the requested parameter value.</param>
    /// <returns>Value object mapped to the specified name.</returns>
    Value operator[](const std::wstring& name) override { return Value(new FactoryParameterValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the FactoryParameter values.</param>
    /// <returns>Value object mapped to the specified FactoryParameter enum.</returns>
    Value operator[](FactoryParameter parameter) { return Value(new FactoryParameterValue(m_handle, parameter)); }

    // public HandleValueCollection<SPXFACTORYHANDLE, FactoryParameterValue>
    bool ContainsString(const std::wstring& name) override { return base_type::ContainsString(name); }
    void SetString(const std::wstring& name, const std::wstring& value) override { base_type::SetString(name, value); }
    std::wstring GetString(const std::wstring& name, const std::wstring& defaultValue = L"") override { return base_type::GetString(name, defaultValue); }

    bool ContainsNumber(const std::wstring& name) override { return base_type::ContainsNumber(name); }
    void SetNumber(const std::wstring& name, int32_t value) override { base_type::SetNumber(name, value); }
    int32_t GetNumber(const std::wstring& name, int32_t defaultValue = 0) override { return base_type::GetNumber(name, defaultValue); }

    bool ContainsBool(const std::wstring& name) override { return base_type::ContainsNumber(name); }
    void SetBool(const std::wstring& name, bool value) override { base_type::SetBool(name, value); }
    bool GetBool(const std::wstring& name, bool defaultValue = false) override { return base_type::GetBool(name, defaultValue); }

private:
    DISABLE_DEFAULT_CTORS(FactoryParameterCollection);
};


} } } // Microsoft::CognitiveServices::Speech
