//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_async_recognizer.h: Public API declarations for AsyncRecognizer<RecoResult, RecoEventArgs> C++ template class
//

#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_recognition_eventargs.h>
#include <speechapi_cxx_keyword_recognition_model.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// AsyncRecognizer abstract base class.
/// </summary>
template <class RecoResult, class RecoEventArgs>
class AsyncRecognizer : public Recognizer
{
public:

    /// <summary>
    /// Performs recognition in a non-blocking (asynchronous) mode.
    /// </summary>
    /// <returns>Future containing result value (a shared pointer to RecoResult)
    /// of the asynchronous recognition.
    /// </returns>
    virtual std::future<std::shared_ptr<RecoResult>> RecognizeAsync() = 0;

    /// <summary>
    /// Asynchronously initiates continuous recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    virtual std::future<void> StartContinuousRecognitionAsync() = 0;

    /// <summary>
    /// Asynchronously terminates ongoing continuous recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    virtual std::future<void> StopContinuousRecognitionAsync() = 0;

    /// <summary>
    /// Asynchronously initiates keyword recognition operation.
    /// </summary>
    /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK.This functionality is currently not included in the SDK itself.
    /// <param name="model">The keyword recognition model that specifies the keyword to be recognized.</param>
    /// <returns>An asynchronous operation that starts the keyword recognition.</returns>
    virtual std::future<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model) = 0;

    /// <summary>
    /// Asynchronously terminates ongoing keyword recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    virtual std::future<void> StopKeywordRecognitionAsync() = 0;

    /// <summary>
    /// Signal for events indicating the start of a recognition session (operation).
    /// </summary>
    EventSignal<const SessionEventArgs&> SessionStarted;

    /// <summary>
    /// Signal for events indicating the end of a recognition session (operation).
    /// </summary>
    EventSignal<const SessionEventArgs&> SessionStopped;

    /// <summary>
    /// Signal for events indicating the start of speech.
    /// </summary>
    EventSignal<const RecognitionEventArgs&> SpeechStartDetected;

    /// <summary>
    /// Signal for events indicating the end of speech.
    /// </summary>
    EventSignal<const RecognitionEventArgs&> SpeechEndDetected;

    /// <summary>
    /// Signal for events containing intermediate recognition results.
    /// </summary>
    EventSignal<const RecoEventArgs&> IntermediateResult;

    /// <summary>
    /// Signal for events containing final recognition results.
    /// (indicating a successful recognition attempt).
    /// </summary>
    EventSignal<const RecoEventArgs&> FinalResult;

    /// <summary>
    /// Signal for events containing no-match recognition results 
    /// (indicating an successful recognition attempt).
    /// </summary>
    EventSignal<const RecoEventArgs&> NoMatch;

    /// <summary>
    /// Signal for events containing canceled recognition results
    /// (indicating a recognition attempt that was canceled as a result or a direct cancellation request 
    /// or, alternatively, a transport or protocol failure).
    /// </summary>
    EventSignal<const RecoEventArgs&> Canceled;

protected:

    /*! \cond PROTECTED */

    explicit AsyncRecognizer(SPXRECOHANDLE hreco) throw() :
        Recognizer(hreco),
        SessionStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SessionStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SpeechStartDetected(GetRecognitionEventConnectionsChangedCallback(), GetRecognitionEventConnectionsChangedCallback()),
        SpeechEndDetected(GetRecognitionEventConnectionsChangedCallback(), GetRecognitionEventConnectionsChangedCallback()),
        IntermediateResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        FinalResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        NoMatch(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        Canceled(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        m_hasyncRecognize(SPXHANDLE_INVALID),
        m_hasyncStartContinuous(SPXHANDLE_INVALID),
        m_hasyncStopContinuous(SPXHANDLE_INVALID),
        m_hasyncStartKeyword(SPXHANDLE_INVALID),
        m_hasyncStopKeyword(SPXHANDLE_INVALID)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    };

    virtual ~AsyncRecognizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    };

    virtual void TermRecognizer() override
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        // Ask the base to term first, thus avoiding deadlocks in up/down vs down/up lock order
        Recognizer::TermRecognizer();

        // Disconnect the event signals in reverse construction order
        Canceled.DisconnectAll();
        NoMatch.DisconnectAll();
        FinalResult.DisconnectAll();
        IntermediateResult.DisconnectAll();
        SpeechEndDetected.DisconnectAll();
        SpeechStartDetected.DisconnectAll();
        SessionStopped.DisconnectAll();
        SessionStarted.DisconnectAll();

        // Close the async handles we have open for Recognize, StartContinuous, and StopContinuous
        for (auto handle : { &m_hasyncRecognize, &m_hasyncStartContinuous, &m_hasyncStopContinuous })
        {
            if (*handle != SPXHANDLE_INVALID && ::Recognizer_AsyncHandle_IsValid(*handle))
            {
                ::Recognizer_AsyncHandle_Close(*handle);
                *handle = SPXHANDLE_INVALID;
            }
        }
    }

    std::future<std::shared_ptr<RecoResult>> RecognizeAsyncInternal()
    {
        auto future = std::async(std::launch::async, [=]() -> std::shared_ptr<RecoResult> {
            SPX_INIT_HR(hr);

            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(hr = Recognizer_Recognize(m_hreco, &hresult));

            return std::make_shared<RecoResult>(hresult);
        });

        return future;
    };

    std::future<void> StartContinuousRecognitionAsyncInternal()
    {
        auto future = std::async(std::launch::async, [=]() -> void {
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

    std::future<void> StopContinuousRecognitionAsyncInternal()
    {
        auto future = std::async(std::launch::async, [=]() -> void {
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

    std::future<void> StartKeywordRecognitionAsyncInternal(std::shared_ptr<KeywordRecognitionModel> model)
    {
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_INIT_HR(hr);
            SPX_THROW_ON_FAIL(hr = Recognizer_AsyncHandle_Close(m_hasyncStartKeyword)); // close any unfinished previous attempt

            auto hkeyword = (SPXKEYWORDHANDLE)(*model.get());
            SPX_EXITFN_ON_FAIL(hr = Recognizer_StartKeywordRecognitionAsync(m_hreco, hkeyword, &m_hasyncStartKeyword));
            SPX_EXITFN_ON_FAIL(hr = Recognizer_StartKeywordRecognitionAsync_WaitFor(m_hasyncStartKeyword, UINT32_MAX));

            SPX_EXITFN_CLEANUP:
            SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(m_hasyncStartKeyword)); // don't overwrite HR on cleanup
            m_hasyncStartKeyword = SPXHANDLE_INVALID;

            SPX_THROW_ON_FAIL(hr);
        });

        return future;
    };

    std::future<void> StopKeywordRecognitionAsyncInternal()
    {
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_INIT_HR(hr);
            SPX_THROW_ON_FAIL(hr = Recognizer_AsyncHandle_Close(m_hasyncStopKeyword)); // close any unfinished previous attempt

            SPX_EXITFN_ON_FAIL(hr = Recognizer_StopKeywordRecognitionAsync(m_hreco, &m_hasyncStopKeyword));
            SPX_EXITFN_ON_FAIL(hr = Recognizer_StopKeywordRecognitionAsync_WaitFor(m_hasyncStopKeyword, UINT32_MAX));

            SPX_EXITFN_CLEANUP:
            SPX_REPORT_ON_FAIL(/* hr = */ Recognizer_AsyncHandle_Close(m_hasyncStopKeyword)); // don't overwrite HR on cleanup
            m_hasyncStartKeyword = SPXHANDLE_INVALID;

            SPX_THROW_ON_FAIL(hr);
        });

        return future;
    };

    virtual void RecoEventConnectionsChanged(const EventSignal<const RecoEventArgs&>& recoEvent)
    {
        if (m_hreco != SPXHANDLE_INVALID)
        {
            SPX_DBG_TRACE_VERBOSE("%s: m_hreco=0x%8x", __FUNCTION__, m_hreco);
            SPX_DBG_TRACE_VERBOSE_IF(!::Recognizer_Handle_IsValid(m_hreco), "%s: m_hreco is INVALID!!!", __FUNCTION__);

            if (&recoEvent == &IntermediateResult)
            {
                Recognizer_IntermediateResult_SetEventCallback(m_hreco, IntermediateResult.IsConnected() ? AsyncRecognizer::FireEvent_IntermediateResult: nullptr, this);
            }
            else if (&recoEvent == &FinalResult)
            {
                Recognizer_FinalResult_SetEventCallback(m_hreco, FinalResult.IsConnected() ? AsyncRecognizer::FireEvent_FinalResult: nullptr, this);
            }
            else if (&recoEvent == &NoMatch)
            {
                Recognizer_NoMatch_SetEventCallback(m_hreco, NoMatch.IsConnected() ? AsyncRecognizer::FireEvent_NoMatch : nullptr, this);
            }
            else if (&recoEvent == &Canceled)
            {
                Recognizer_Canceled_SetEventCallback(m_hreco, Canceled.IsConnected() ? AsyncRecognizer::FireEvent_Canceled : nullptr, this);
            }
        }
    }

    virtual void RecogntionEventConnectionsChanged(const EventSignal<const RecognitionEventArgs&>& recognitionEvent)
    {
        if (m_hreco != SPXHANDLE_INVALID)
        {
            SPX_DBG_TRACE_VERBOSE("%s: m_hreco=0x%8x", __FUNCTION__, m_hreco);
            SPX_DBG_TRACE_VERBOSE_IF(!::Recognizer_Handle_IsValid(m_hreco), "%s: m_hreco is INVALID!!!", __FUNCTION__);

            if (&recognitionEvent == &SpeechStartDetected)
            {
                Recognizer_SpeechStartDetected_SetEventCallback(m_hreco, SpeechStartDetected.IsConnected() ? AsyncRecognizer::FireEvent_SpeechStartDetected : nullptr, this);
            }
            else if (&recognitionEvent == &SpeechEndDetected)
            {
                Recognizer_SpeechEndDetected_SetEventCallback(m_hreco, SpeechEndDetected.IsConnected() ? AsyncRecognizer::FireEvent_SpeechEndDetected : nullptr, this);
            }
        }
    }

    virtual void SessionEventConnectionsChanged(const EventSignal<const SessionEventArgs&>& sessionEvent)
    {
        if (m_hreco != SPXHANDLE_INVALID)
        {
            SPX_DBG_TRACE_VERBOSE("%s: m_hreco=0x%8x", __FUNCTION__, m_hreco);
            SPX_DBG_TRACE_VERBOSE_IF(!::Recognizer_Handle_IsValid(m_hreco), "%s: m_hreco is INVALID!!!", __FUNCTION__);

            if (&sessionEvent == &SessionStarted)
            {
                Recognizer_SessionStarted_SetEventCallback(m_hreco, SessionStarted.IsConnected() ? AsyncRecognizer::FireEvent_SessionStarted: nullptr, this);
            }
            else if (&sessionEvent == &SessionStopped)
            {
                Recognizer_SessionStopped_SetEventCallback(m_hreco, SessionStopped.IsConnected() ? AsyncRecognizer::FireEvent_SessionStopped : nullptr, this);
            }
        }
    }

    static void FireEvent_SessionStarted(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent { new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SessionStarted.Signal(*sessionEvent.get());
    }

    static void FireEvent_SessionStopped(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent { new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SessionStopped.Signal(*sessionEvent.get());
    }

    static void FireEvent_SpeechStartDetected(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecognitionEventArgs> recoEvent{ new RecognitionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SpeechStartDetected.Signal(*recoEvent.get());
    }

    static void FireEvent_SpeechEndDetected(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecognitionEventArgs> recoEvent{ new RecognitionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SpeechEndDetected.Signal(*recoEvent.get());
    }

    static void FireEvent_IntermediateResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->IntermediateResult.Signal(*recoEvent.get());
    }

    static void FireEvent_FinalResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->FinalResult.Signal(*recoEvent.get());
    }

    static void FireEvent_NoMatch(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->NoMatch.Signal(*recoEvent.get());
    }

    static void FireEvent_Canceled(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);

        auto ptr = new RecoEventArgs(hevent);
        std::shared_ptr<RecoEventArgs> recoEvent(ptr);

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->Canceled.Signal(*ptr);
    }

    SPXASYNCHANDLE m_hasyncRecognize;
    SPXASYNCHANDLE m_hasyncStartContinuous;
    SPXASYNCHANDLE m_hasyncStopContinuous;
    SPXASYNCHANDLE m_hasyncStartKeyword;
    SPXASYNCHANDLE m_hasyncStopKeyword;

    /*! \endcond */

private:

    DISABLE_DEFAULT_CTORS(AsyncRecognizer);

    inline std::function<void(const EventSignal<const SessionEventArgs&>&)> GetSessionEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const SessionEventArgs&>& sessionEvent) { this->SessionEventConnectionsChanged(sessionEvent); };
    }

    inline std::function<void(const EventSignal<const RecoEventArgs&>&)> GetRecoEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const RecoEventArgs&>& recoEvent) { this->RecoEventConnectionsChanged(recoEvent); };
    }

    inline std::function<void(const EventSignal<const RecognitionEventArgs&>&)> GetRecognitionEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const RecognitionEventArgs&>& recoEvent) { this->RecogntionEventConnectionsChanged(recoEvent); };
    }
};


} } } // Microsoft::CognitiveServices::Speech
