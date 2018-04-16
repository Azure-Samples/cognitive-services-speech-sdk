//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_factory_parameter.h: Public API declarations for RecognizerFactoryParameterValue C++ static class methods
//

#pragma once
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {

/// <summary>
/// Enumerates parameters that can be used to configure a recognizer factory.
/// </summary>
enum class FactoryParameter { SpeechSubscriptionKey = 1, SpeechEndpoint = 2 };

/// <summary>
/// Represents the value of a recognizer factory parameter returned by the subscript operator
/// on the corresponding parameter value collection.
/// </summary>
class RecognizerFactoryParameterValue : public Value
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a parameter name.
    /// </summary>
    RecognizerFactoryParameterValue(SPXRECOFACTORYHANDLE hrecofactory, const wchar_t* name) : m_hrecofactory(hrecofactory), m_name(name) { }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a FactoryParameter value.
    /// </summary>
    RecognizerFactoryParameterValue(SPXRECOFACTORYHANDLE hrecofactory, enum FactoryParameter parameter) : m_hrecofactory(hrecofactory), m_name(ParameterNameFromEnum(parameter)) { }

    /// <summary>
    /// Returns true if the encapsulated value has a string type.
    /// </summary>
    bool IsString() override { return ContainsString(m_hrecofactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerFactoryParameterValue as a string.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerFactoryParameterValue instance corresponds
    /// to a non-existing string parameter. </param>
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hrecofactory, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const wchar_t* value) override { return SetString(m_hrecofactory, m_name.c_str(), value); }

    /// <summary>
    /// Returns true if the encapsulated value has a number type.
    /// </summary>
    bool IsNumber() override { return ContainsNumber(m_hrecofactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerFactoryParameterValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerFactoryParameterValue instance corresponds
    /// to a non-existing number parameter. </param>
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hrecofactory, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified number inside the encapsulated value.
    /// </summary>
    void SetNumber(int32_t value) override { SetNumber(m_hrecofactory, m_name.c_str(), value); }

    /// <summary>
    /// Returns true if the encapsulated value has a Boolean type.
    /// </summary>
    bool IsBool() override { return ContainsBool(m_hrecofactory, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerFactoryParameterValue as a Boolean.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerFactoryParameterValue instance corresponds
    /// to a non-existing parameter. </param>
    bool GetBool(bool defaultValue) override { return GetBool(m_hrecofactory, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified Boolean inside the encapsulated value.
    /// </summary>
    void SetBool(bool value) override { SetBool(m_hrecofactory, m_name.c_str(), value); }

    DISABLE_COPY_AND_MOVE(RecognizerFactoryParameterValue);

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

/// <summary>
/// Represents a collection of named recognizer factory parameters.
/// </summary>
class RecognizerFactoryParameterCollection : public HandleValueCollection<SPXRECOFACTORYHANDLE, RecognizerFactoryParameterValue>
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    RecognizerFactoryParameterCollection(SPXRECOFACTORYHANDLE hrecofactory = SPXHANDLE_DEFAULT) : HandleValueCollection(hrecofactory) { }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="name">String name of the requested parameter value.</param>
    /// <returns>Value object mapped to the specified name.</returns>
    Value operator[](const wchar_t* name) override { return Value(new RecognizerFactoryParameterValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the FactoryParameter values.</param>
    /// <returns>Value object mapped to the specified FactoryParameter enum.</returns>
    Value operator[](enum FactoryParameter parameter) { return Value(new RecognizerFactoryParameterValue(m_handle, parameter)); }
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
