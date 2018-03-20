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
        RecognitionResult(hresult, m_resultId, m_reason, m_text)
    {
        PopulateResultFields(hresult, &m_resultId, &m_reason, &m_text);
        SPX_DBG_TRACE_VERBOSE("%s (this=0x%x, handle=0x%x) -- resultid=%S; reason=0x%x; text=%S", __FUNCTION__, this, m_hresult, m_resultId.c_str(), m_reason, m_text.c_str());
    };

    virtual ~SpeechRecognitionResult()
    {
        SPX_DBG_TRACE_VERBOSE("%s (this-0x%x, handle=0x%x)", __FUNCTION__, this, m_hresult);
    };


private:

    SpeechRecognitionResult() = delete;
    SpeechRecognitionResult(SpeechRecognitionResult&&) = delete;
    SpeechRecognitionResult(const SpeechRecognitionResult&) = delete;
    SpeechRecognitionResult& operator=( SpeechRecognitionResult&&) = delete;
    SpeechRecognitionResult& operator=(const SpeechRecognitionResult&) = delete;

    std::wstring m_resultId;
    enum Reason m_reason;
    std::wstring m_text;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
