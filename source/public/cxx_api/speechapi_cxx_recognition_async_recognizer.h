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

    AsyncRecognizer(RecognizerParameters& parameters) :
        Recognizer(parameters)
    {
    };

    virtual ~AsyncRecognizer() {};

    virtual std::future<std::shared_ptr<RecoResult>> RecognizeAsync() = 0;
    virtual std::future<void> StartContinuousRecognitionAsync() = 0;
    virtual std::future<void> StopContinuousRecognitionAsync() = 0;

    EventSignal<SessionEventArgs> SessionStarted;
    EventSignal<SessionEventArgs> SessionStopped;

    EventSignal<SessionEventArgs> SoundStarted;
    EventSignal<SessionEventArgs> SoundStopped;

    EventSignal<RecoEventArgs> IntermediateResult;
    EventSignal<RecoEventArgs> FinalResult;
    EventSignal<RecoEventArgs> NoMatch;
    EventSignal<RecoEventArgs> Canceled;


protected:

    AsyncRecognizer() {};


private:

    AsyncRecognizer(const AsyncRecognizer&) = delete;
    AsyncRecognizer(const AsyncRecognizer&&) = delete;

    AsyncRecognizer& operator=(const AsyncRecognizer&) = delete;
};


} }; // CARBON_NAMESPACE_ROOT :: Recognition
