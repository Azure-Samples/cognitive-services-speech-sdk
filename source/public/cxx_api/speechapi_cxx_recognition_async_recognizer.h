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


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


template <class RecoResult, class RecoEventArgs>
class AsyncRecognizer : public Recognizer
{
public:

    AsyncRecognizer(RecognizerParameters& parameters) throw() :
        Recognizer(parameters),
        m_SessionStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        m_SessionStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        m_SoundStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        m_SoundStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        m_IntermediateResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        m_FinalResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        m_NoMatch(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        m_Canceled(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback())
    {
    };

    virtual ~AsyncRecognizer() {};

    virtual std::future<std::shared_ptr<RecoResult>> RecognizeAsync() = 0;
    virtual std::future<void> StartContinuousRecognitionAsync() = 0;
    virtual std::future<void> StopContinuousRecognitionAsync() = 0;

    EventSignal<const SessionEventArgs&>& SessionStarted() { return m_SessionStarted; }
    EventSignal<const SessionEventArgs&>& SessionStopped() { return m_SessionStopped; }

    EventSignal<const SessionEventArgs&>& SoundStarted() { return m_SoundStarted; }
    EventSignal<const SessionEventArgs&>& SoundStopped() { return m_SoundStopped; }

    EventSignal<const RecoEventArgs&>& IntermediateResult() { return m_IntermediateResult; }
    EventSignal<const RecoEventArgs&>& FinalResult() { return m_FinalResult; }
    EventSignal<const RecoEventArgs&>& NoMatch() { return m_NoMatch; }
    EventSignal<const RecoEventArgs&>& Canceled() { return m_Canceled; }

private:
    EventSignal<const SessionEventArgs&> m_SessionStarted;
    EventSignal<const SessionEventArgs&> m_SessionStopped;

    EventSignal<const SessionEventArgs&> m_SoundStarted;
    EventSignal<const SessionEventArgs&> m_SoundStopped;

    EventSignal<const RecoEventArgs&> m_IntermediateResult;
    EventSignal<const RecoEventArgs&> m_FinalResult;
    EventSignal<const RecoEventArgs&> m_NoMatch;
    EventSignal<const RecoEventArgs&> m_Canceled;


protected:

    AsyncRecognizer() {};

    virtual void RecoEventConnectionsChanged(const EventSignal<const RecoEventArgs&>& recoEvent) { UNUSED(recoEvent); };
    virtual void SessionEventConnectionsChanged(const EventSignal<const SessionEventArgs&>& sessionEvent) { UNUSED(sessionEvent); };

private:

    AsyncRecognizer(const AsyncRecognizer&) = delete;
    AsyncRecognizer(const AsyncRecognizer&&) = delete;

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
