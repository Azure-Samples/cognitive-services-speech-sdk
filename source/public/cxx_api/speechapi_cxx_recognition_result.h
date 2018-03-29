//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_result.h: Public API declarations for RecognitionResult C++ base class and related enum class
//

#pragma once
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };
enum class ResultProperty { LuisJson = 1 };


class ResultPropertyValue : public Value
{
public:

    ResultPropertyValue(SPXRESULTHANDLE hresult, const wchar_t* name) : m_hresult(hresult), m_name(name) { }
    ResultPropertyValue(SPXRESULTHANDLE hresult, enum ResultProperty property) : m_hresult(hresult), m_name(PropertyNameFromEnum(property)) { }

    bool IsString() override { return ContainsString(m_hresult, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hresult, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    bool IsNumber() override { return ContainsNumber(m_hresult, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hresult, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    bool IsBool() override { return ContainsBool(m_hresult, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hresult, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }


private:

    ResultPropertyValue() = delete;
    ResultPropertyValue(ResultPropertyValue&&) = delete;
    ResultPropertyValue(const ResultPropertyValue&) = delete;
    ResultPropertyValue& operator=(ResultPropertyValue&&) = delete;
    ResultPropertyValue& operator=(const ResultPropertyValue&) = delete;

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


class ResultPropertyValueCollection : public HandleValueCollection<SPXRESULTHANDLE, ResultPropertyValue>
{
public:

    ResultPropertyValueCollection(SPXRESULTHANDLE hresult) :
        HandleValueCollection(hresult)
    {
    }

    ~ResultPropertyValueCollection() { }

    Value operator[](const wchar_t* name) override { return Value(new ResultPropertyValue(m_handle, name)); }
    Value operator[](enum ResultProperty index) { return Value(new ResultPropertyValue(m_handle, index)); }
};


class RecognitionResult
{
private:

    RecognitionResult() = delete;
    RecognitionResult(RecognitionResult&&) = delete;
    RecognitionResult(const RecognitionResult&) = delete;
    RecognitionResult& operator=(RecognitionResult&&) = delete;
    RecognitionResult& operator=(const RecognitionResult&) = delete;

    ResultPropertyValueCollection m_properties;


public:

    virtual ~RecognitionResult()
    {
        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    const std::wstring& ResultId;
    const enum Reason& Reason;
    
    const std::wstring& Text;

    ResultPropertyValueCollection& Properties;


protected:

    RecognitionResult(SPXRESULTHANDLE hresult, const std::wstring& resultId, const enum Reason& reason, const std::wstring& text) :
        m_properties(hresult),
        ResultId(resultId),
        Reason(reason),
        Text(text),
        Properties(m_properties)
    {
    };

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
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
