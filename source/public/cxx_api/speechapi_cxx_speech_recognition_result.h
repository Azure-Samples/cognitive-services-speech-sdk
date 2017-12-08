//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_speech_recognition_result.h: Public API declarations for SpeechRecognitionResult C++ class
//

#pragma once
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Speech {


class SpeechRecognitionResult final : public RecognitionResult
{
public:

    SpeechRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(m_resultId, m_reason, m_text, m_payload),
        m_hresult(hresult)
    {
        PopulateResultFields(hresult);

        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%S; reason=0x%x; text=%S", __FUNCTION__, this, m_hresult, m_resultId.c_str(), m_reason, m_text.c_str());
    };

    virtual ~SpeechRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hresult);

        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };


protected:

    void PopulateResultFields(SPXRESULTHANDLE hresult)
    {
        static_assert((int)Reason_NoMatch == (int)Reason::NoMatch, "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Canceled == (int)Reason::Canceled, "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_Recognized == (int)Reason::Recognized, "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_OtherRecognizer == (int)Reason::OtherRecognizer, "Reason_* enum values == Reason::* enum values");
        static_assert((int)Reason_IntermediateResult == (int)Reason::IntermediateResult, "Reason_* enum values == Reason::* enum values");

        SPX_INIT_HR(hr);

        const size_t cch = 1024;
        wchar_t sz[cch+1];

        SPX_THROW_ON_FAIL(hr = Result_GetResultId(hresult, sz, cch));
        m_resultId = sz;

        Result_RecognitionReason reason;
        SPX_THROW_ON_FAIL(hr = Result_GetRecognitionReason(hresult, &reason));
        m_reason = (enum class Reason)reason;

        SPX_THROW_ON_FAIL(hr = Result_GetText(hresult, sz, cch));
        m_text = sz;
    };


private:

    SpeechRecognitionResult(const SpeechRecognitionResult&) = delete;
    SpeechRecognitionResult(const SpeechRecognitionResult&&) = delete;

    SpeechRecognitionResult& operator=(const SpeechRecognitionResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    enum class Reason m_reason;
    std::wstring m_text;
    PayloadItems m_payload;    
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
