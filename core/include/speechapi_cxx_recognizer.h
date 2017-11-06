#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_session_eventargs.h>


namespace CARBON_NAMESPACE_ROOT :: Recognition {


class Recognizer
{
public:

    virtual ~Recognizer() {};

    RecognizerParameters& Parameters;

    virtual bool IsEnabled() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;


protected:

    Recognizer(RecognizerParameters &parameters) : Parameters(parameters) {};


private:

    Recognizer(const Recognizer&) = delete;
    Recognizer(const Recognizer&&) = delete;

    Recognizer& operator=(const Recognizer&) = delete;
};


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
    EventSignal<SessionEventArgs> SoundEnded;

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

class BaseAsyncRecognizer : public AsyncRecognizer<RecognitionResult, RecognitionEventArgs>
{
public:

    template <class T>
    static std::shared_ptr<BaseAsyncRecognizer> From(const std::shared_ptr<T> &recognizer) 
    {
        static_assert(std::is_base_of<Recognizer, T>::value);
        return nullptr;
    }

};


}; // CARBON_NAMESPACE_ROOT :: Recognition
