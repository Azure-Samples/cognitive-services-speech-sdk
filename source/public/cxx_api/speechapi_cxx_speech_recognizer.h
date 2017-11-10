#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognition_result.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Speech {


class SpeechRecognizer final : virtual public AsyncRecognizer<SpeechRecognitionResult, SpeeechRecognitionEventArgs>
{
public:

    SpeechRecognizer() : 
        AsyncRecognizer(m_recoParameters),
        m_hreco(SPXHANDLE_INVALID)
    {
        throw nullptr;
    };

    SpeechRecognizer(const std::wstring& language) : 
        AsyncRecognizer(m_recoParameters),
        m_hreco(SPXHANDLE_INVALID)
    {
        throw nullptr;
    };

    SpeechRecognizer(SPXRECOHANDLE hreco) :
        AsyncRecognizer(m_recoParameters),
        m_hreco(hreco)
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    ~SpeechRecognizer()
    {
        SPX_DBG_TRACE_FUNCTION();

        ::Recognizer_Handle_Close(m_hreco);
        m_hreco = SPXHANDLE_INVALID;
    };

    bool IsEnabled() override { return false; };
    void Enable() override { throw nullptr; };
    void Disable() override { throw nullptr; };
   
    std::future<std::shared_ptr<SpeechRecognitionResult>> RecognizeAsync() override
    {
        auto future = std::async([=]() -> std::shared_ptr<SpeechRecognitionResult> {
            SPX_INIT_HR(hr);

            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(hr = Recognizer_Recognize(m_hreco, &hresult));

            return std::make_shared<SpeechRecognitionResult>(hresult);
        });

        return future;
    };

    std::future<void> StartContinuousRecognitionAsync() override { throw nullptr; };
    std::future<void> StopContinuousRecognitionAsync() override { throw nullptr; };


private:

    SpeechRecognizer(const SpeechRecognizer&) = delete;
    SpeechRecognizer(const SpeechRecognizer&&) = delete;

    SpeechRecognizer& operator=(const SpeechRecognizer&) = delete;

    SPXRECOHANDLE m_hreco;
    RecognizerParameters m_recoParameters; 
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
