//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_result.h: Public API declarations for RecognitionResult C++ base class and related enum class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };

/// <summary>
/// Specifies properties that can be retrieved from a RecognitionResult.
/// </summary>
enum class ResultProperty { LuisJson = 1 };

/// <summary>
/// Represents the value of a RecognitionResult property returned by the subscript operator.
/// </summary>
class ResultPropertyValue : public Value
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a property name.
    /// </summary>
    ResultPropertyValue(SPXRESULTHANDLE hresult, const wchar_t* name) : m_hresult(hresult), m_name(name) { }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a ResultProperty value.
    /// </summary>
    ResultPropertyValue(SPXRESULTHANDLE hresult, enum ResultProperty property) : m_hresult(hresult), m_name(PropertyNameFromEnum(property)) { }

    /// <summary>
    /// Returns true if the encapsulated value has a string type.
    /// </summary>
    bool IsString() override { return ContainsString(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a string.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing property. </param>
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const wchar_t* value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    /// <summary>
    /// Returns true if the encapsulated value has a number type.
    /// </summary>
    bool IsNumber() override { return ContainsNumber(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing property. </param>
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified number inside the encapsulated value.
    /// </summary>
    void SetNumber(int32_t value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    /// <summary>
    /// Returns true if the encapsulated value has a boolean type.
    /// </summary>
    bool IsBool() override { return ContainsBool(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a boolean.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing property. </param>
    bool GetBool(bool defaultValue) override { return GetBool(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified boolean inside the encapsulated value.
    /// </summary>
    void SetBool(bool value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

private:

    DISABLE_DEFAULT_CTORS(ResultPropertyValue);

    std::wstring PropertyNameFromEnum(ResultProperty property)
    {
        static_assert((int)ResultProperty_LuisJson == (int)ResultProperty::LuisJson, "ResultProperty_* enum values == ResultProperty::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Result_GetProperty_Name(static_cast<Result_Property>(property), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRESULTHANDLE hresult, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Result_GetProperty_String(hresult, name, sz, maxCharCount, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRESULTHANDLE hresult, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Result_GetProperty_Int32(hresult, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRESULTHANDLE hresult, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Result_GetProperty_Bool(hresult, name, &value, defaultValue));
        return !!value;
    }

    static bool ContainsString(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_ContainsProperty_String(hresult, name);
    }

    static bool ContainsNumber(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_ContainsProperty_Int32(hresult, name);
    }

    static bool ContainsBool(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_ContainsProperty_Bool(hresult, name);
    }

private:

    SPXRESULTHANDLE m_hresult;
    std::wstring m_name;
};

/// <summary>
/// Represents a collection of named RecognitionResult properties.
/// </summary>
class ResultPropertyValueCollection : public HandleValueCollection<SPXRESULTHANDLE, ResultPropertyValue>
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    ResultPropertyValueCollection(SPXRESULTHANDLE hresult) :
        HandleValueCollection(hresult)
    {
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~ResultPropertyValueCollection() { }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="name">String name of the requested value.</param>
    /// <returns>Value object mapped to the specified name.</returns>
    Value operator[](const wchar_t* name) override { return Value(new ResultPropertyValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the ResultProperty values.</param>
    /// <returns>Value object mapped to the specified ResultProperty enum.</returns>
    Value operator[](enum ResultProperty index) { return Value(new ResultPropertyValue(m_handle, index)); }
};

/// <summary>
/// Contains detailed information about result of a recognition operation.
/// </summary>
class RecognitionResult
{
private:
    DISABLE_DEFAULT_CTORS(RecognitionResult);

    ResultPropertyValueCollection m_properties;

public:

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~RecognitionResult()
    {
        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    /// <summary>
    /// Unique result id.
    /// </summary>
    const std::wstring& ResultId;

    /// <summary>
    /// Recognition status.
    /// </summary>
    const enum Reason& Reason;

    /// <summary>
    /// Normalized text generated by a speech recognition engine from recognized input. 
    /// If recognition was canceled as a result of a transport or a protocol failure, 
    /// it contains the detail failure information.
    /// </summary>
    const std::wstring& Text;

    /// <summary>
    /// Collection of additional RecognitionResult properties.
    /// </summary>
    ResultPropertyValueCollection& Properties;


protected:

    RecognitionResult(SPXRESULTHANDLE hresult) :
        m_properties(hresult),
        ResultId(m_resultId),
        Reason(m_reason),
        Text(m_text),
        Properties(m_properties),
        Handle(m_hresult),
        m_hresult(hresult)
    {
        PopulateResultFields(hresult, &m_resultId, &m_reason, &m_text);
    }

    const SPXRESULTHANDLE& Handle;

private:

    void PopulateResultFields(SPXRESULTHANDLE hresult, std::wstring *presultId, enum Reason* preason, std::wstring* ptext)
    {
        static_assert((int)Reason_NoMatch == (int)CARBON_NAMESPACE_ROOT::Recognition::Reason::NoMatch, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Canceled == (int)CARBON_NAMESPACE_ROOT::Recognition::Reason::Canceled, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Recognized == (int)CARBON_NAMESPACE_ROOT::Recognition::Reason::Recognized, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_OtherRecognizer == (int)CARBON_NAMESPACE_ROOT::Recognition::Reason::OtherRecognizer, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_IntermediateResult == (int)CARBON_NAMESPACE_ROOT::Recognition::Reason::IntermediateResult, 
            "Reason_* enum values == Reason::* enum values");

        SPX_INIT_HR(hr);

        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];

        if (presultId != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetResultId(hresult, sz, maxCharCount));
            *presultId = sz;
        }

        if (preason != nullptr)
        {
            Result_RecognitionReason reason;
            SPX_THROW_ON_FAIL(hr = Result_GetRecognitionReason(hresult, &reason));
            *preason = (enum Reason)reason;
        }

        if (ptext != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetText(hresult, sz, maxCharCount));
            *ptext = sz;
        }
    };

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    enum Reason m_reason;
    std::wstring m_text;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
