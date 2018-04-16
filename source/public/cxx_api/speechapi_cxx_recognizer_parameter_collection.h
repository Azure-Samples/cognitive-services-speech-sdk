//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_parameter_collection.h: Public API declarations for RecognizerParameterValueCollection C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


/// <summary>
/// Enumerates parameters that can be used to configure a recognizer.
/// </summary>
enum class RecognizerParameter { CustomSpeechModelId = 1 };

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
    RecognizerParameterValue(SPXRECOHANDLE hreco, const wchar_t* name) : m_hreco(hreco), m_name(name) { }

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
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hreco, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const wchar_t* value) override { return SetString(m_hreco, m_name.c_str(), value); }

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
        static_assert((int)RecognizerParameter_CustomSpeechModelId == (int)RecognizerParameter::CustomSpeechModelId, "RecognizerParameter_* enum values == RecognizerParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Name(static_cast<Recognizer_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_String(hreco, name, sz, maxCharCount, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Int32(hreco, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Bool(hreco, name, &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_String(hreco, name, value));
    }

    static void SetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Int32(hreco, name, value));
    }

    static void SetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Bool(hreco, name, value));
    }

    static bool ContainsString(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_String(hreco, name);
    }

    static bool ContainsNumber(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_Int32(hreco, name);
    }

    static bool ContainsBool(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_Bool(hreco, name);
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
    RecognizerParameterValueCollection(SPXRECOHANDLE hreco) :
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
    Value operator[](const wchar_t* name) override { return Value(new RecognizerParameterValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the RecognizerParameter values.</param>
    /// <returns>Value object mapped to the specified RecognizerParameter enum.</returns>
    Value operator[](enum RecognizerParameter parameter) { return Value(new RecognizerParameterValue(m_handle, parameter)); }

private:

    DISABLE_DEFAULT_CTORS(RecognizerParameterValueCollection);
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
