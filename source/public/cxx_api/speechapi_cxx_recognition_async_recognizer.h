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
        SessionStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SessionStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SoundStarted(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        SoundStopped(GetSessionEventConnectionsChangedCallback(), GetSessionEventConnectionsChangedCallback()),
        IntermediateResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        FinalResult(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        NoMatch(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback()),
        Canceled(GetRecoEventConnectionsChangedCallback(), GetRecoEventConnectionsChangedCallback())
    {
    };

    virtual ~AsyncRecognizer() {};

    virtual std::future<std::shared_ptr<RecoResult>> RecognizeAsync() = 0;
    virtual std::future<void> StartContinuousRecognitionAsync() = 0;
    virtual std::future<void> StopContinuousRecognitionAsync() = 0;

    EventSignal<const SessionEventArgs&> SessionStarted;
    EventSignal<const SessionEventArgs&> SessionStopped;

    EventSignal<const SessionEventArgs&> SoundStarted;
    EventSignal<const SessionEventArgs&> SoundStopped;

    EventSignal<const RecoEventArgs&> IntermediateResult;
    EventSignal<const RecoEventArgs&> FinalResult;
    EventSignal<const RecoEventArgs&> NoMatch;
    EventSignal<const RecoEventArgs&> Canceled;


protected:

    AsyncRecognizer() {};

    virtual void RecoEventConnectionsChanged(const EventSignal<const RecoEventArgs&>& recoEvent) {};
    virtual void SessionEventConnectionsChanged(const EventSignal<const SessionEventArgs&>& sessionEvent) {};

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


} }; // CARBON_NAMESPACE_ROOT :: Recognition
