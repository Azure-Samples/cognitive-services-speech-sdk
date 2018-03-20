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


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };


class RecognitionResultValue : public Value
{
public:

    RecognitionResultValue(SPXRESULTHANDLE hresult, const wchar_t* name) :
        m_hresult(hresult),
        m_name(name)
    {
    }

    // --- Value virtual overrides ---

    bool IsString() override { return HasString(m_hresult, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hresult, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    bool IsNumber() override { return HasNumber(m_hresult, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hresult, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    bool IsBool() override { return HasBool(m_hresult, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hresult, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { UNUSED(value); SPX_THROW_HR(SPXERR_NOT_IMPL); }

    // --- Static helpers ---
    
    static std::wstring GetString(SPXRESULTHANDLE hresult, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t cch = 4096;
        wchar_t sz[cch+1];
        SPX_THROW_ON_FAIL(Result_GetValue_String(hresult, name, sz, cch, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRESULTHANDLE hresult, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Result_GetValue_Int32(hresult, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRESULTHANDLE hresult, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Result_GetValue_Bool(hresult, name, &value, defaultValue));
        return !!value;
    }

    static bool HasString(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_HasValue_String(hresult, name);
    }

    static bool HasNumber(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_HasValue_Int32(hresult, name);
    }

    static bool HasBool(SPXRESULTHANDLE hresult, const wchar_t* name)
    {
        return Result_HasValue_Bool(hresult, name);
    }

private:

    SPXRESULTHANDLE m_hresult;
    std::wstring m_name;
};


class RecognitionResultValueCollection : public ValueCollection<SPXRESULTHANDLE, RecognitionResultValue>
{
public:

    RecognitionResultValueCollection(SPXRESULTHANDLE hresult) :
        ValueCollection(hresult)
    {
    }
};


class RecognitionResult
{
private:

    RecognitionResult() = delete;
    RecognitionResult(RecognitionResult&&) = delete;
    RecognitionResult(const RecognitionResult&) = delete;
    RecognitionResult& operator=(RecognitionResult&&) = delete;
    RecognitionResult& operator=(const RecognitionResult&) = delete;

    RecognitionResultValueCollection m_values;


public:

    virtual ~RecognitionResult()
    {
        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    const std::wstring& ResultId;
    const enum Reason& Reason;
    
    const std::wstring& Text;

    RecognitionResultValueCollection& Values;


protected:

    RecognitionResult(SPXRESULTHANDLE hresult, const std::wstring& resultId, const enum Reason& reason, const std::wstring& text) :
        m_values(hresult),
        ResultId(resultId),
        Reason(reason),
        Text(text),
        Values(m_values)
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

        const size_t cch = 1024;
        wchar_t sz[cch+1];

        if (presultId != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetResultId(hresult, sz, cch));
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
            SPX_THROW_ON_FAIL(hr = Result_GetText(hresult, sz, cch));
            *ptext = sz;
        }
    };

    SPXRESULTHANDLE m_hresult;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
