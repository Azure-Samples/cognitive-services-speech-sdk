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
        SPX_DBG_TRACE_FUNCTION();

        //TODO:ROBCH: check to make sure the hresult is valid
        //throw nullptr;
    };

    virtual ~SpeechRecognitionResult()
    {
        SPX_DBG_TRACE_FUNCTION();

        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    
private:

    SpeechRecognitionResult(const SpeechRecognitionResult&) = delete;
    SpeechRecognitionResult(const SpeechRecognitionResult&&) = delete;

    SpeechRecognitionResult& operator=(const SpeechRecognitionResult&) = delete;

    SPXRESULTHANDLE m_hresult;

    std::wstring m_resultId;
    RecognitionReason m_reason;
    std::wstring m_text;
    PayloadItems m_payload;    
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
