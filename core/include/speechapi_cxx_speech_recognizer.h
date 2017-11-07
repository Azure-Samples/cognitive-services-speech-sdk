#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognition_result.h>


namespace CARBON_NAMESPACE_ROOT :: Recognition :: Speech {


class SpeechRecognizer final : virtual public AsyncRecognizer<SpeechRecognitionResult, SpeeechRecognitionEventArgs>
{
public:

    SpeechRecognizer() : 
        AsyncRecognizer(m_recoParameters) 
    {
        SPX_DBG_TRACE_FUNCTION();
    };

    SpeechRecognizer(const std::wstring& language) : 
        AsyncRecognizer(m_recoParameters) 
    {
        throw nullptr;
    };

    ~SpeechRecognizer()
    {
        SPX_DBG_TRACE_FUNCTION();
    };

    bool IsEnabled() override { return false; };
    void Enable() override { throw nullptr; };
    void Disable() override { throw nullptr; };
   
    std::future<std::shared_ptr<SpeechRecognitionResult>> RecognizeAsync() override { throw nullptr; };
    std::future<void> StartContinuousRecognitionAsync() override { throw nullptr; };
    std::future<void> StopContinuousRecognitionAsync() override { throw nullptr; };


private:

    SpeechRecognizer(const SpeechRecognizer&) = delete;
    SpeechRecognizer(const SpeechRecognizer&&) = delete;

    SpeechRecognizer& operator=(const SpeechRecognizer&) = delete;

    RecognizerParameters m_recoParameters; 
};


}; // CARBON_NAMESPACE_ROOT :: Recognition
