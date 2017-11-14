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
        m_hreco(SPXHANDLE_INVALID),
        m_hasyncRecognize(SPXHANDLE_INVALID),
        m_hasyncStartContinuous(SPXHANDLE_INVALID),
        m_hasyncStopContinuous(SPXHANDLE_INVALID)
    {
        throw nullptr;
    };

    SpeechRecognizer(const std::wstring& language) : 
        AsyncRecognizer(m_recoParameters),
        m_hreco(SPXHANDLE_INVALID),
        m_hasyncRecognize(SPXHANDLE_INVALID),
        m_hasyncStartContinuous(SPXHANDLE_INVALID),
        m_hasyncStopContinuous(SPXHANDLE_INVALID)
    {
        throw nullptr;
    };

    SpeechRecognizer(SPXRECOHANDLE hreco) :
        AsyncRecognizer(m_recoParameters),
        m_hreco(hreco),
        m_hasyncRecognize(SPXHANDLE_INVALID),
        m_hasyncStartContinuous(SPXHANDLE_INVALID),
        m_hasyncStopContinuous(SPXHANDLE_INVALID)
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    ~SpeechRecognizer()
    {
        SPX_DBG_TRACE_FUNCTION();

        if (m_hreco != SPXHANDLE_INVALID)
        {
            ::Recognizer_Handle_Close(m_hreco);
            m_hreco = SPXHANDLE_INVALID;
        }

        if (m_hreco != SPXHANDLE_INVALID)
        {
            ::Recognizer_AsyncHandle_Close(m_hasyncRecognize);
            m_hasyncRecognize = SPXHANDLE_INVALID;
        }

        if (m_hreco != SPXHANDLE_INVALID)
        {
            ::Recognizer_AsyncHandle_Close(m_hasyncStartContinuous);
            m_hasyncStartContinuous = SPXHANDLE_INVALID;
        }

        if (m_hreco != SPXHANDLE_INVALID)
        {
            ::Recognizer_AsyncHandle_Close(m_hasyncStopContinuous);
            m_hasyncStopContinuous = SPXHANDLE_INVALID;
        }
    };

    bool IsEnabled() override 
    {
        bool fEnabled = false;
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_IsEnabled(m_hreco, &fEnabled));
        return fEnabled;
    };

    void Enable() override
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Enable(m_hreco));
    };

    void Disable() override
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Disable(m_hreco));
    };

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

    std::future<void> StartContinuousRecognitionAsync() override 
    {
        auto future = std::async([=]() -> void {
            SPX_INIT_HR(hr);
            SPX_THROW_ON_FAIL(hr = Recognizer_AsyncHandle_Close(m_hasyncStartContinuous)); // close any unfinished previous attempt

            SPX_EXITFN_ON_FAIL(hr = Recognizer_StartContinuousRecognitionAsync(m_hreco, &m_hasyncStartContinuous));
            SPX_EXITFN_ON_FAIL(hr = Recognizer_StartContinuousRecognitionAsync_WaitFor(m_hasyncStartContinuous, UINT32_MAX));

            SPX_EXITFN_CLEANUP:
            SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(m_hasyncStartContinuous)); // don't overwrite HR on cleanup
            m_hasyncStartContinuous = SPXHANDLE_INVALID;

            SPX_THROW_ON_FAIL(hr);
        });

        return future;
    };

    std::future<void> StopContinuousRecognitionAsync() override
    {
        auto future = std::async([=]() -> void {
            SPX_INIT_HR(hr);
            SPX_THROW_ON_FAIL(hr = Recognizer_AsyncHandle_Close(m_hasyncStopContinuous)); // close any unfinished previous attempt

            SPX_EXITFN_ON_FAIL(hr = Recognizer_StopContinuousRecognitionAsync(m_hreco, &m_hasyncStopContinuous));
            SPX_EXITFN_ON_FAIL(hr = Recognizer_StopContinuousRecognitionAsync_WaitFor(m_hasyncStopContinuous, UINT32_MAX));

            SPX_EXITFN_CLEANUP:
            SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(m_hasyncStopContinuous)); // don't overwrite HR on cleanup
            m_hasyncStartContinuous = SPXHANDLE_INVALID;

            SPX_THROW_ON_FAIL(hr);
        });

        return future;
    };


private:

    SpeechRecognizer(const SpeechRecognizer&) = delete;
    SpeechRecognizer(const SpeechRecognizer&&) = delete;

    SpeechRecognizer& operator=(const SpeechRecognizer&) = delete;

    SPXRECOHANDLE m_hreco;
    SPXASYNCHANDLE m_hasyncRecognize;
    SPXASYNCHANDLE m_hasyncStartContinuous;
    SPXASYNCHANDLE m_hasyncStopContinuous;

    RecognizerParameters m_recoParameters; 
};


} } }; // CARBON_NAMESPACE_ROOT :: Recognition :: Speech
