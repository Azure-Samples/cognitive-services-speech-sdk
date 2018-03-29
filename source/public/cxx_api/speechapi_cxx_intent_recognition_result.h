//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_recognition_result.h: Public API declarations for IntentRecognitionResult C++ class
//

#pragma once
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {


class IntentRecognitionResult final : public RecognitionResult
{
public:

    IntentRecognitionResult(SPXRESULTHANDLE hresult) :
        RecognitionResult(hresult, m_resultId, m_reason, m_text),
        IntentId(m_intentId),
        m_hresult(hresult)
    {
        PopulateResultFields(hresult, &m_resultId, &m_reason, &m_text);
        PopulateIntentFields(hresult, &m_intentId);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%S; reason=0x%x; text=%S", __FUNCTION__, this, m_hresult, m_resultId.c_str(), m_reason, m_text.c_str());
    };

    ~IntentRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hresult);

        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    const std::wstring& IntentId;


protected:

    IntentRecognitionResult() = delete;
    IntentRecognitionResult(IntentRecognitionResult&&) = delete;
    IntentRecognitionResult(const IntentRecognitionResult&) = delete;
    IntentRecognitionResult& operator=(IntentRecognitionResult&&) = delete;
    IntentRecognitionResult& operator=(const IntentRecognitionResult&) = delete;

    void PopulateIntentFields(SPXRESULTHANDLE hresult, std::wstring* pintentId)
    {
        SPX_INIT_HR(hr);
        
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];

        if (pintentId != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = IntentResult_GetIntentId(hresult, sz, maxCharCount));
            *pintentId = sz;
        }
    }

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    enum Reason m_reason;
    std::wstring m_text;

    std::wstring m_intentId;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent
