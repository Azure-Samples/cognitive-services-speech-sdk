//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_parameter_collection.h: Public API declarations for RecognizerParameterValueCollection C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Enumerates parameters that can be used to configure a recognizer.
/// </summary>
enum class RecognizerParameter { DeploymentId = 1 };

/// <summary>
/// Represents the value of a recognizer parameter returned by the subscript operator
/// on the corresponding parameter value collection.
/// </summary>
class RecognizerParameterValue : public Value
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a parameter name.
    /// </summary>
    RecognizerParameterValue(SPXRECOHANDLE hreco, const std::wstring& name) : m_hreco(hreco), m_name(name) { }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a RecognizerParameter value.
    /// </summary>
    RecognizerParameterValue(SPXRECOHANDLE hreco, RecognizerParameter parameter) : m_hreco(hreco), m_name(ParameterNameFromEnum(parameter)) { }

    // --- Value virtual overrides ---
    
    /// <summary>
    /// Returns true if the encapsulated value has a string type.
    /// </summary>
    bool IsString() override { return ContainsString(m_hreco, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerParameterValue as a string.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerParameterValue instance corresponds
    /// to a non-existing string parameter. </param>
    std::wstring GetString(const std::wstring& defaultValue) override { return GetString(m_hreco, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const std::wstring& value) override { return SetString(m_hreco, m_name.c_str(), value); }

    /// <summary>
    /// Returns true if the encapsulated value has a number type.
    /// </summary>
    bool IsNumber() override { return ContainsNumber(m_hreco, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerParameterValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerParameterValue instance corresponds
    /// to a non-existing number parameter. </param>
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hreco, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified number inside the encapsulated value.
    /// </summary>
    void SetNumber(int32_t value) override { SetNumber(m_hreco, m_name.c_str(), value); }

    /// <summary>
    /// Returns true if the encapsulated value has a Boolean type.
    /// </summary>
    bool IsBool() override { return ContainsBool(m_hreco, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this RecognizerParameterValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this RecognizerParameterValue instance corresponds
    /// to a non-existing Boolean parameter. </param>
    bool GetBool(bool defaultValue) override { return GetBool(m_hreco, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified Boolean inside the encapsulated value.
    /// </summary>
    void SetBool(bool value) override { SetBool(m_hreco, m_name.c_str(), value); }


private:

    std::wstring ParameterNameFromEnum(RecognizerParameter parameter)
    {
        static_assert((int)RecognizerParameter_DeploymentId == (int)RecognizerParameter::DeploymentId, "RecognizerParameter_* enum values == RecognizerParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Name(static_cast<Recognizer_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRECOHANDLE hreco, const std::wstring& name, const std::wstring& defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_String(hreco, name.c_str(), sz, maxCharCount, defaultValue.c_str()));
        return sz;
    }

    static int32_t GetNumber(SPXRECOHANDLE hreco, const std::wstring& name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Int32(hreco, name.c_str(), &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRECOHANDLE hreco, const std::wstring& name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Bool(hreco, name.c_str(), &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXRECOHANDLE hreco, const std::wstring& name, const std::wstring& value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_String(hreco, name.c_str(), value.c_str()));
    }

    static void SetNumber(SPXRECOHANDLE hreco, const std::wstring& name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Int32(hreco, name.c_str(), value));
    }

    static void SetBool(SPXRECOHANDLE hreco, const std::wstring& name, bool value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Bool(hreco, name.c_str(), value));
    }

    static bool ContainsString(SPXRECOHANDLE hreco, const std::wstring& name)
    {
        return Recognizer_ContainsParameter_String(hreco, name.c_str());
    }

    static bool ContainsNumber(SPXRECOHANDLE hreco, const std::wstring& name)
    {
        return Recognizer_ContainsParameter_Int32(hreco, name.c_str());
    }

    static bool ContainsBool(SPXRECOHANDLE hreco, const std::wstring& name)
    {
        return Recognizer_ContainsParameter_Bool(hreco, name.c_str());
    }

private:

    SPXRECOHANDLE m_hreco;
    std::wstring m_name;
};



/// <summary>
/// Represents a collection of named recognizer parameters.
/// </summary>
class RecognizerParameterValueCollection : public HandleValueCollection<SPXRECOHANDLE, RecognizerParameterValue>
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit RecognizerParameterValueCollection(SPXRECOHANDLE hreco) :
        HandleValueCollection(hreco)
    {
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~RecognizerParameterValueCollection() { SPX_DBG_TRACE_FUNCTION(); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="name">String name of the requested parameter value.</param>
    /// <returns>Value object mapped to the specified name.</returns>
    Value operator[](const std::wstring& name) override { return Value(new RecognizerParameterValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the RecognizerParameter values.</param>
    /// <returns>Value object mapped to the specified RecognizerParameter enum.</returns>
    Value operator[](RecognizerParameter parameter) { return Value(new RecognizerParameterValue(m_handle, parameter)); }

private:

    DISABLE_DEFAULT_CTORS(RecognizerParameterValueCollection);
};


} } } // Microsoft::CognitiveServices::Speech
