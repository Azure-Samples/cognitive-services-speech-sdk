#pragma once
#include <string>
#include <speechapi_cxx_common.h>

namespace CARBON_NAMESPACE_ROOT :: Recognition :: Speech {

class SpeechRecognitionResult final : public RecognitionResult
{
public:

    SpeechRecognitionResult() :
        RecognitionResult(m_resultId, m_reason, m_text, m_payload)
    {
        throw nullptr;
    };

    virtual ~SpeechRecognitionResult(){};

    
private:

    SpeechRecognitionResult(const SpeechRecognitionResult&) = delete;
    SpeechRecognitionResult(const SpeechRecognitionResult&&) = delete;

    SpeechRecognitionResult& operator=(const SpeechRecognitionResult&) = delete;

    std::wstring m_resultId;
    RecognitionReason m_reason;
    std::wstring m_text;
    PayloadItems m_payload;    
};

}; // CARBON_NAMESPACE_ROOT :: Recognition
