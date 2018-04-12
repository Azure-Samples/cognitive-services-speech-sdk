//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_async_recognizer.h: Public API declarations for AsyncRecognizer<RecoResult, RecoEventArgs> C++ template class
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_session_eventargs.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


template <class RecoResult, class RecoEventArgs>
class AsyncRecognizer : public Recognizer
{
public:

    AsyncRecognizer(SPXRECOHANDLE hreco) throw() :
        Recognizer(hreco),
        SessionStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SessionStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SpeechStartDetected(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SpeechEndDetected(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
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
    };

    virtual ~AsyncRecognizer()
    {
        SPX_DBG_TRACE_SCOPE("~AsyncRecognizer start", "~AsyncRecognizerEnd");
        for (auto handle : { &m_hasyncRecognize, &m_hasyncStartContinuous, &m_hasyncStopContinuous }) 
        {
            if (*handle != SPXHANDLE_INVALID && ::Recognizer_AsyncHandle_IsValid(*handle))
            {
                ::Recognizer_AsyncHandle_Close(*handle);
                *handle = SPXHANDLE_INVALID;
            }
        }
    };

    virtual std::future<std::shared_ptr<RecoResult>> RecognizeAsync() = 0;
    virtual std::future<void> StartContinuousRecognitionAsync() = 0;
    virtual std::future<void> StopContinuousRecognitionAsync() = 0;

    virtual std::future<void> StartKeywordRecognitionAsync(const wchar_t* keyword) = 0;
    virtual std::future<void> StopKeywordRecognitionAsync() = 0;

    EventSignal<const SessionEventArgs&> SessionStarted;
    EventSignal<const SessionEventArgs&> SessionStopped;

    EventSignal<const SessionEventArgs&> SpeechStartDetected;
    EventSignal<const SessionEventArgs&> SpeechEndDetected;

    EventSignal<const RecoEventArgs&> IntermediateResult;
    EventSignal<const RecoEventArgs&> FinalResult;
    EventSignal<const RecoEventArgs&> NoMatch;
    EventSignal<const RecoEventArgs&> Canceled;

protected:

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

    std::future<void> StartKeywordRecognitionAsyncInternal(const wchar_t* keyword)
    {
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_INIT_HR(hr);
            SPX_THROW_ON_FAIL(hr = Recognizer_AsyncHandle_Close(m_hasyncStartKeyword)); // close any unfinished previous attempt

            SPX_EXITFN_ON_FAIL(hr = Recognizer_StartKeywordRecognitionAsync(m_hreco, keyword, &m_hasyncStartKeyword));
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

    virtual void SessionEventConnectionsChanged(const EventSignal<const SessionEventArgs&>& sessionEvent)
    {
        if (&sessionEvent == &SessionStarted)
        {
            Recognizer_SessionStarted_SetEventCallback(m_hreco, SessionStarted.IsConnected() ? AsyncRecognizer::FireEvent_SessionStarted: nullptr, this);
        }
        else if (&sessionEvent == &SessionStopped)
        {
            Recognizer_SessionStopped_SetEventCallback(m_hreco, SessionStopped.IsConnected() ? AsyncRecognizer::FireEvent_SessionStopped : nullptr, this);
        }
        else if (&sessionEvent == &SpeechStartDetected)
        {
            Recognizer_SpeechStartDetected_SetEventCallback(m_hreco, SpeechStartDetected.IsConnected() ? AsyncRecognizer::FireEvent_SpeechStartDetected: nullptr, this);
        }
        else if (&sessionEvent == &SpeechEndDetected)
        {
            Recognizer_SpeechEndDetected_SetEventCallback(m_hreco, SpeechEndDetected.IsConnected() ? AsyncRecognizer::FireEvent_SpeechEndDetected: nullptr, this);
        }
    }

    static void FireEvent_SessionStarted(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent { new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->SessionStarted.Signal(*sessionEvent.get());
    }

    static void FireEvent_SessionStopped(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent { new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->SessionStopped.Signal(*sessionEvent.get());
    }

    static void FireEvent_SpeechStartDetected(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent{ new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->SpeechStartDetected.Signal(*sessionEvent.get());
    }

    static void FireEvent_SpeechEndDetected(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<SessionEventArgs> sessionEvent{ new SessionEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->SpeechEndDetected.Signal(*sessionEvent.get());
    }

    static void FireEvent_IntermediateResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->IntermediateResult.Signal(*recoEvent.get());
    }

    static void FireEvent_FinalResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->FinalResult.Signal(*recoEvent.get());
    }

    static void FireEvent_NoMatch(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<RecoEventArgs> recoEvent { new RecoEventArgs(hevent) };

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->NoMatch.Signal(*recoEvent.get());
    }

    static void FireEvent_Canceled(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);

        auto ptr = new RecoEventArgs(hevent);
        std::shared_ptr<RecoEventArgs> recoEvent(ptr);

        auto pThis = static_cast<AsyncRecognizer*>(pvContext);
        pThis->Canceled.Signal(*ptr);
    }

    SPXASYNCHANDLE m_hasyncRecognize;
    SPXASYNCHANDLE m_hasyncStartContinuous;
    SPXASYNCHANDLE m_hasyncStopContinuous;
    SPXASYNCHANDLE m_hasyncStartKeyword;
    SPXASYNCHANDLE m_hasyncStopKeyword;

private:

    AsyncRecognizer() = delete;
    AsyncRecognizer(AsyncRecognizer&&) = delete;
    AsyncRecognizer(const AsyncRecognizer&) = delete;
    AsyncRecognizer& operator=(AsyncRecognizer&&) = delete;
    AsyncRecognizer& operator=(const AsyncRecognizer&) = delete;

    inline std::function<void(const EventSignal<const SessionEventArgs&>&)> GetSessionEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const SessionEventArgs&>& sessionEvent) { this->SessionEventConnectionsChanged(sessionEvent); };
    }

    inline std::function<void(const EventSignal<const RecoEventArgs&>&)> GetRecoEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const RecoEventArgs&>& recoEvent) { this->RecoEventConnectionsChanged(recoEvent); };
    }
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
