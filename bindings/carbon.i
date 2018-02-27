%{
#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif
%}

// Add necessary symbols to generated header
%{
#include <wrappers.h>
#include <speechapi_cxx.h>
%}

%include <std_except.i>
%include <std_shared_ptr.i>
%include <std_wstring.i>

%shared_ptr(Carbon::Recognition::Recognizer)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::RecognitionResult, Carbon::Recognition::RecognitionEventArgs>)
%shared_ptr(Carbon::Recognition::BaseAsyncRecognizer)
%shared_ptr(Carbon::Recognition::RecognitionResult)
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognitionResult)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Speech::SpeechRecognitionResult, Carbon::Recognition::Speech::SpeechRecognitionEventArgs>);
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognizer)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<int, int>)
%shared_ptr(Carbon::Recognition::Intent::IntentRecognizer)

%ignore CallbackWrapper::GetFunction();
%ignore FutureWrapper::FutureWrapper;
%include <wrappers.h>

%include <speechapi_cxx_common.h>

%ignore Carbon::NotYetImplementedException;
%include <speechapi_cxx_todo.h>

%ignore Carbon::Recognition::AsyncRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StopContinuousRecognitionAsync();

%inline %{
    typedef std::shared_ptr<Carbon::Recognition::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
%}

%template(SpeechRecognitionResultPtrFuture) FutureWrapper<SpeechRecognitionResultPtr>;
%template(IntPtrFuture) FutureWrapper<std::shared_ptr<int>>;
%template(VoidFuture) FutureWrapper<void>;

// %extend need to come first, before the %ignore for the same method (RecognizeAsync, etc.)
%extend Carbon::Recognition::Speech::SpeechRecognizer {

    SpeechRecognitionResultPtr Recognize() {
        return ($self)->RecognizeAsync().get();
    }

    void StartContinuousRecognition()
    {
        ($self)->StartContinuousRecognitionAsync().get();
    }

    void StopContinuousRecognition()
    {
        ($self)->StopContinuousRecognitionAsync().get();
    }

    FutureWrapper<SpeechRecognitionResultPtr> RecognizeAsync() {
        auto future = ($self)->RecognizeAsync();
        return FutureWrapper<SpeechRecognitionResultPtr>(std::move(future));
    }

    FutureWrapper<void> StartContinuousRecognitionAsync()
    {
        auto future = ($self)->StartContinuousRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopContinuousRecognitionAsync()
    {
        auto future = ($self)->StopContinuousRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }
}

%extend Carbon::Recognition::Intent::IntentRecognizer {

    std::shared_ptr<int> Recognize() {
        return ($self)->RecognizeAsync().get();
    }

    void StartContinuousRecognition()
    {
        ($self)->StartContinuousRecognitionAsync().get();
    }

    void StopContinuousRecognition()
    {
        ($self)->StopContinuousRecognitionAsync().get();
    }

    FutureWrapper<std::shared_ptr<int>> RecognizeAsync() {
        auto future = ($self)->RecognizeAsync();
        return FutureWrapper<std::shared_ptr<int>>(std::move(future));
    }

    FutureWrapper<void> StartContinuousRecognitionAsync()
    {
        auto future = ($self)->StartContinuousRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopContinuousRecognitionAsync()
    {
        auto future = ($self)->StopContinuousRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }
}

%feature("director") CallbackWrapper;

%extend Carbon::EventSignal {

    void _Connect(CallbackWrapper<T>& callback)
    {
        ($self)->Connect(callback.GetFunction());
    };

    void _Disconnect(CallbackWrapper<T>& callback)
    {
        ($self)->Disconnect(callback.GetFunction());
    };
}

%ignore Carbon::EventSignal::EventSignal;
%ignore Carbon::EventSignal::CallbackFunction;
%ignore Carbon::EventSignal::Signal;
%ignore Carbon::EventSignal::Connect;
%ignore Carbon::EventSignal::Disconnect;
%ignore Carbon::EventSignal::operator+=;
%ignore Carbon::EventSignal::operator-=;

%ignore Carbon::Recognition::Speech::SpeechRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StopContinuousRecognitionAsync();

%ignore Carbon::Recognition::Intent::IntentRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StopContinuousRecognitionAsync();

// Process symbols in header
%include <speechapi_cxx_eventargs.h>
%include <speechapi_cxx_eventsignal.h>
%include <speechapi_cxx_parameter.h>

%include <speechapi_cxx_session_eventargs.h>

%include <speechapi_cxx_todo_recognition.h>
%include <speechapi_cxx_recognition_result.h>
%include <speechapi_cxx_recognition_eventargs.h>

%template(_SessionEventCallback) CallbackWrapper<const Carbon::SessionEventArgs&>;
%template(_RecognitionEventCallback) CallbackWrapper<const Carbon::Recognition::RecognitionEventArgs&>;
%template(SessionEventSignal) Carbon::EventSignal<const Carbon::SessionEventArgs&>;
%template(RecognitionEventSignal) Carbon::EventSignal<const Carbon::Recognition::RecognitionEventArgs&>;

%include <speechapi_cxx_recognizer_parameter_collection.h>
%include <speechapi_cxx_recognizer.h>
%include <speechapi_cxx_recognition_async_recognizer.h>


%template(BaseRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::RecognitionResult, Carbon::Recognition::RecognitionEventArgs>;
%include <speechapi_cxx_recognition_base_async_recognizer.h>

%include <speechapi_cxx_speech_recognition_result.h>
%include <speechapi_cxx_speech_recognition_eventargs.h>

%template(_SpeechRecognitionEventCallback) CallbackWrapper<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognitionEventSignal) Carbon::EventSignal<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Speech::SpeechRecognitionResult, Carbon::Recognition::Speech::SpeechRecognitionEventArgs>;
%include <speechapi_cxx_speech_recognizer.h>

%template(_IntentEventCallback) CallbackWrapper<const int&>;
%template(IntentEventSignal) Carbon::EventSignal<const int&>;
%template(IntentRecognizerBase) Carbon::Recognition::AsyncRecognizer<int, int>;
%include <speechapi_cxx_todo_intent.h>

%ignore Carbon::Recognition::RecognizerFactory::CreateTranslationRecognizer;
%ignore Carbon::Recognition::RecognizerFactory::CreateTranslationRecognizerWithFileInput;
%include <speechapi_cxx_recognizer_factory.h>

%include <speechapi_cxx_session_parameter_collection.h>
%include <speechapi_cxx_session.h>

%include <speechapi_cxx_todo_recognition.h>

//%include <speechapi_cxx_translation_eventargs.h>
//%include <speechapi_cxx_translation_result.h>
//%include <speechapi_cxx_translation_recognizer.h>
