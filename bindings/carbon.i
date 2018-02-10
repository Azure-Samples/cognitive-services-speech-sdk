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
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_todo.h>

#include <speechapi_cxx_eventargs.h>
#include <speechapi_cxx_eventsignal.h>

#include <speechapi_cxx_session_eventargs.h>

#include <speechapi_cxx_todo_recognition.h>
#include <speechapi_cxx_recognition_result.h>
#include <speechapi_cxx_recognition_eventargs.h>

#include <speechapi_cxx_recognizer.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_recognition_base_async_recognizer.h>

#include <speechapi_cxx_speech_recognition_result.h>
#include <speechapi_cxx_speech_recognition_eventargs.h>
#include <speechapi_cxx_speech_recognizer.h>

#include <speechapi_cxx_todo_intent.h>

#include <speechapi_cxx_recognizer_factory.h>

#include <speechapi_cxx_todo_session.h>
#include <speechapi_cxx_todo_recognition.h>
%}

%include <std_except.i>
%include <std_shared_ptr.i>
%include <std_wstring.i>

%include <wrappers.h>

%include <speechapi_cxx_common.h>
%include <speechapi_cxx_todo.h>

// TODO: swig tries to invoke a deleted function
// future(const future&) = delete; 
// See how we can get around that.

%ignore Carbon::Recognition::AsyncRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StopContinuousRecognitionAsync();

%ignore Carbon::Recognition::Speech::SpeechRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StopContinuousRecognitionAsync();

%ignore Carbon::Recognition::Intent::IntentRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StopContinuousRecognitionAsync();

%shared_ptr(Carbon::Recognition::BaseAsyncRecognizer)
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognizer)
%shared_ptr(Carbon::Recognition::Intent::IntentRecognizer)

%shared_ptr(Carbon::Recognition::RecognitionResult)
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognitionResult)

// Process symbols in header
%include <speechapi_cxx_eventargs.h>
%include <speechapi_cxx_eventsignal.h>

%include <speechapi_cxx_session_eventargs.h>

%include <speechapi_cxx_todo_recognition.h>
%include <speechapi_cxx_recognition_result.h>
%include <speechapi_cxx_recognition_eventargs.h>

%include <speechapi_cxx_recognizer.h>
%include <speechapi_cxx_recognition_async_recognizer.h>

%inline %{
    typedef std::shared_ptr<Carbon::Recognition::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
%}

%extend Carbon::Recognition::AsyncRecognizer {
    std::shared_ptr<RecoResult> Recognize() {
        return ($self)->RecognizeAsync().get();
    }
}

%template(FutureSpeechRecognitionResult) FutureWrapper<SpeechRecognitionResultPtr>;

%extend Carbon::Recognition::Speech::SpeechRecognizer {
    std::shared_ptr<SpeechRecognitionResult> Recognize() {
        return ($self)->RecognizeAsync().get();
    }

    // using snake case here, so it won't be ignored.
    FutureWrapper<SpeechRecognitionResultPtr> recognize_async()
    {
        auto future = ($self)->RecognizeAsync();
        return FutureWrapper<SpeechRecognitionResultPtr>(std::move(future));
    }
}

%template(AsyncRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::RecognitionResult, Carbon::Recognition::RecognitionEventArgs>;
%include <speechapi_cxx_recognition_base_async_recognizer.h>

%include <speechapi_cxx_speech_recognition_result.h>
%include <speechapi_cxx_speech_recognition_eventargs.h>

%template(SpeechRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Speech::SpeechRecognitionResult, Carbon::Recognition::Speech::SpeechRecognitionEventArgs>;
%include <speechapi_cxx_speech_recognizer.h>


%template(IntentRecognizerBase) Carbon::Recognition::AsyncRecognizer<int, int>;
%include <speechapi_cxx_todo_intent.h>


%include <speechapi_cxx_recognizer_factory.h>

%include <speechapi_cxx_todo_session.h>
%include <speechapi_cxx_todo_recognition.h>