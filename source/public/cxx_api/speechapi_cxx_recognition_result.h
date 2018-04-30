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


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };

/// <summary>
/// Specifies properties that can be retrieved from a RecognitionResult.
/// </summary>
enum class ResultProperty { Json = 1, LanguageUnderstandingJson = 2, ErrorDetails = 3 };

/// <summary>
/// Represents the value of a RecognitionResult property returned by the subscript operator.
/// </summary>
class ResultPropertyValue : public Value
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a property name.
    /// </summary>
    ResultPropertyValue(SPXRESULTHANDLE hresult, const std::wstring& name) : m_hresult(hresult), m_name(name) { }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle and a ResultProperty value.
    /// </summary>
    ResultPropertyValue(SPXRESULTHANDLE hresult, ResultProperty property) : m_hresult(hresult), m_name(PropertyNameFromEnum(property)) { }

    /// <summary>
    /// Returns true if the encapsulated value has a string type.
    /// </summary>
    bool IsString() override { return ContainsString(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a string.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing string property. </param>
    std::wstring GetString(const std::wstring& defaultValue) override { return GetString(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified string inside the encapsulated value.
    /// </summary>
    void SetString(const std::wstring& value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    /// <summary>
    /// Returns true if the encapsulated value has a number type.
    /// </summary>
    bool IsNumber() override { return ContainsNumber(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a number.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing number property. </param>
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified number inside the encapsulated value.
    /// </summary>
    void SetNumber(int32_t value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    /// <summary>
    /// Returns true if the encapsulated value has a Boolean type.
    /// </summary>
    bool IsBool() override { return ContainsBool(m_hresult, m_name.c_str()); }

    /// <summary>
    /// Returns the content of this ResultPropertyValue as a Boolean.
    /// </summary>
    /// <param name="defaultValue">Default value to return if this ResultPropertyValue instance corresponds
    /// to a non-existing Boolean property. </param>
    bool GetBool(bool defaultValue) override { return GetBool(m_hresult, m_name.c_str(), defaultValue); }

    /// <summary>
    /// Stores the specified Boolean inside the encapsulated value.
    /// </summary>
    void SetBool(bool value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

private:

    DISABLE_DEFAULT_CTORS(ResultPropertyValue);

    std::wstring PropertyNameFromEnum(ResultProperty property)
    {
        static_assert((int)ResultProperty_Json == (int)ResultProperty::Json, "ResultProperty_* enum values == ResultProperty::* enum values");
        static_assert((int)ResultProperty_LanguageUnderstandingJson == (int)ResultProperty::LanguageUnderstandingJson, "ResultProperty_* enum values == ResultProperty::* enum values");
        static_assert((int)ResultProperty_ErrorDetails == (int)ResultProperty::ErrorDetails, "ResultProperty_* enum values == ResultProperty::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Result_GetProperty_Name(static_cast<Result_Property>(property), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRESULTHANDLE hresult, const std::wstring& name, const std::wstring& defaultValue)
    {
        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Result_GetProperty_String(hresult, name.c_str(), sz, maxCharCount, defaultValue.c_str()));
        return sz;
    }

    static int32_t GetNumber(SPXRESULTHANDLE hresult, const std::wstring& name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Result_GetProperty_Int32(hresult, name.c_str(), &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRESULTHANDLE hresult, const std::wstring& name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Result_GetProperty_Bool(hresult, name.c_str(), &value, defaultValue));
        return !!value;
    }

    static bool ContainsString(SPXRESULTHANDLE hresult, const std::wstring& name)
    {
        return Result_ContainsProperty_String(hresult, name.c_str());
    }

    static bool ContainsNumber(SPXRESULTHANDLE hresult, const std::wstring& name)
    {
        return Result_ContainsProperty_Int32(hresult, name.c_str());
    }

    static bool ContainsBool(SPXRESULTHANDLE hresult, const std::wstring& name)
    {
        return Result_ContainsProperty_Bool(hresult, name.c_str());
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
    explicit ResultPropertyValueCollection(SPXRESULTHANDLE hresult) :
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
    Value operator[](const std::wstring& name) override { return Value(new ResultPropertyValue(m_handle, name)); }

    /// <summary>
    /// Subscript operator.
    /// </summary>
    /// <param name="index">One of the ResultProperty values.</param>
    /// <returns>Value object mapped to the specified ResultProperty enum.</returns>
    Value operator[](ResultProperty index) { return Value(new ResultPropertyValue(m_handle, index)); }
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
    const Speech::Reason& Reason;

    /// <summary>
    /// Normalized text generated by a speech recognition engine from recognized input. 
    /// This field is filled if the recognition status (<see cref="Reason"/>) is set to Recognized or IntermediateResult.
    /// </summary>
    const std::wstring& Text;

    /// <summary>
    /// Collection of additional RecognitionResult properties.
    /// </summary>
    ResultPropertyValueCollection& Properties;

    /// <summary>
    /// In case of an unsuccessful recognition, provides a brief description of an occurred error.
    /// This field is only filled-out if the recognition status (<see cref="Reason"/>) is set to Canceled.
    /// </summary>
    const std::wstring ErrorDetails;

protected:

    /*! \cond PROTECTED */

    explicit RecognitionResult(SPXRESULTHANDLE hresult) :
        m_properties(hresult),
        ResultId(m_resultId),
        Reason(m_reason),
        Text(m_text),
        Properties(m_properties),
        ErrorDetails(Properties[ResultProperty::ErrorDetails].GetString()),
        Handle(m_hresult),
        m_hresult(hresult)
    {
        PopulateResultFields(hresult, &m_resultId, &m_reason, &m_text);
    }

    const SPXRESULTHANDLE& Handle;

    /*! \endcond */

private:

    void PopulateResultFields(SPXRESULTHANDLE hresult, std::wstring *presultId, Speech::Reason* preason, std::wstring* ptext)
    {
        static_assert((int)Reason_NoMatch == (int)Microsoft::CognitiveServices::Speech::Reason::NoMatch, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Canceled == (int)Microsoft::CognitiveServices::Speech::Reason::Canceled, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Recognized == (int)Microsoft::CognitiveServices::Speech::Reason::Recognized, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_OtherRecognizer == (int)Microsoft::CognitiveServices::Speech::Reason::OtherRecognizer, 
            "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_IntermediateResult == (int)Microsoft::CognitiveServices::Speech::Reason::IntermediateResult, 
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
            *preason = (Speech::Reason)reason;
        }

        if (ptext != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetText(hresult, sz, maxCharCount));
            *ptext = sz;
        }
    };

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    Speech::Reason m_reason;
    std::wstring m_text;
};


} } } // Microsoft::CognitiveServices::Speech
