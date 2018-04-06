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
%include <stdint.i>

%shared_ptr(Carbon::Recognition::IDefaultRecognizerFactory)
%shared_ptr(Carbon::Recognition::Recognizer)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::RecognitionResult, Carbon::Recognition::RecognitionEventArgs>)
%shared_ptr(Carbon::Recognition::BaseAsyncRecognizer)
%shared_ptr(Carbon::Recognition::RecognitionResult)
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognitionResult)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Speech::SpeechRecognitionResult, Carbon::Recognition::Speech::SpeechRecognitionEventArgs>);
%shared_ptr(Carbon::Recognition::Speech::SpeechRecognizer)
%shared_ptr(Carbon::Recognition::Intent::IntentRecognitionResult)
%shared_ptr(Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Intent::IntentRecognitionResult, Carbon::Recognition::Intent::IntentRecognitionEventArgs>)
%shared_ptr(Carbon::Recognition::Intent::IntentRecognizer)
%shared_ptr(Carbon::Recognition::Intent::IntentTrigger)
%shared_ptr(Carbon::Recognition::Intent::LuisModel)
%shared_ptr(Carbon::Recognition::IRecognizerFactory)
%shared_ptr(Carbon::Recognition::IDefaultRecognizerFactory)

%ignore CallbackWrapper::GetFunction();
%ignore FutureWrapper::FutureWrapper;
%include <wrappers.h>

%include <speechapi_cxx_common.h>

%ignore Carbon::NotYetImplementedException;

%ignore Carbon::ValueCollection::ValueCollection();
%ignore Carbon::ValueCollection::ValueCollection(Handle handle);
%include <speechapi_cxx_todo.h>
%include <speechapi_cxx_value.h>

%template(ValueCollectionRecognizerParameter) Carbon::HandleValueCollection<SPXRECOHANDLE, Carbon::Recognition::RecognizerParameterValue>;
#%template(ValueCollectionResultProperty) Carbon::HandleValueCollection<SPXRESULTHANDLE, Carbon::Recognition::ResultPropertyValue>;
%ignore Carbon::HandleValueCollection<SPXRECOFACTORYHANDLE, Carbon::Recognition::RecognizerFactoryParameterValue>;
%ignore Carbon::HandleValueCollection<SPXSESSIONHANDLE, Carbon::SessionParameterValue>;

%ignore Carbon::Recognition::AsyncRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::AsyncRecognizer::StopContinuousRecognitionAsync();

%inline %{
    typedef std::shared_ptr<Carbon::Recognition::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
    typedef std::shared_ptr<Carbon::Recognition::Intent::IntentRecognitionResult> IntentRecognitionResultPtr;
%}

%template(SpeechRecognitionResultPtrFuture) FutureWrapper<SpeechRecognitionResultPtr>;
%template(IntentRecognitionResultPtrFuture) FutureWrapper<IntentRecognitionResultPtr>;
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

    IntentRecognitionResultPtr Recognize() {
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

    FutureWrapper<IntentRecognitionResultPtr> RecognizeAsync() {
        auto future = ($self)->RecognizeAsync();
        return FutureWrapper<IntentRecognitionResultPtr>(std::move(future));
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

#ifdef SWIGPYTHON
    void _Connect(CallbackWrapper<T>& callback)
#elif defined(SWIGJAVA)
    void AddEventListener(CallbackWrapper<T>& callback)
#else
    void Connect(CallbackWrapper<T>& callback)
#endif
    {
        ($self)->Connect(callback.GetFunction());
    };

#ifdef SWIGPYTHON
    void _Disconnect(CallbackWrapper<T>& callback)
#elif defined(SWIGJAVA)
    void RemoveEventListener(CallbackWrapper<T>& callback)
#else
    void Disconnect(CallbackWrapper<T>& callback)
#endif
    {
        ($self)->Disconnect(callback.GetFunction());
    };
}

%ignore Carbon::EventSignal::EventSignal;
%ignore Carbon::EventSignal::CallbackFunction;
%ignore Carbon::EventSignal::Signal;
%ignore Carbon::EventSignal::Connect(CallbackFunction callback);
%ignore Carbon::EventSignal::Disconnect(CallbackFunction callback);
%ignore Carbon::EventSignal::operator+=;
%ignore Carbon::EventSignal::operator-=;

%ignore Carbon::Recognition::ResultPropertyValue::ResultPropertyValue(SPXRESULTHANDLE, enum ResultProperty);

%ignore Carbon::Recognition::Speech::SpeechRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Speech::SpeechRecognizer::StopContinuousRecognitionAsync();

%ignore Carbon::Recognition::Intent::IntentRecognizer::RecognizeAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StartContinuousRecognitionAsync();
%ignore Carbon::Recognition::Intent::IntentRecognizer::StopContinuousRecognitionAsync();

// Process symbols in header
%include <speechapi_cxx_audioinputstream.h>
%include <speechapi_cxx_eventargs.h>
%include <speechapi_cxx_eventsignal.h>
%include <speechapi_cxx_value.h>

%include <speechapi_cxx_session_eventargs.h>

%include <speechapi_cxx_recognition_result.h>
%include <speechapi_cxx_recognition_eventargs.h>

#ifdef SWIGPYTHON
%template(_SessionEventCallback) CallbackWrapper<const Carbon::SessionEventArgs&>;
%template(_RecognitionEventCallback) CallbackWrapper<const Carbon::Recognition::RecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(SessionEventListener) CallbackWrapper<const Carbon::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Carbon::Recognition::RecognitionEventArgs&>;
#else
%template(SessionEventListener) CallbackWrapper<const Carbon::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Carbon::Recognition::RecognitionEventArgs&>;
#endif

%template(SessionEventSignal) Carbon::EventSignal<const Carbon::SessionEventArgs&>;
%template(RecognitionEventSignal) Carbon::EventSignal<const Carbon::Recognition::RecognitionEventArgs&>;

%include <speechapi_cxx_recognizer_parameter_collection.h>
%include <speechapi_cxx_recognizer.h>
%include <speechapi_cxx_recognition_async_recognizer.h>

%template(BaseRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::RecognitionResult, Carbon::Recognition::RecognitionEventArgs>;
%include <speechapi_cxx_recognition_base_async_recognizer.h>

%include <speechapi_cxx_speech_recognition_result.h>
%include <speechapi_cxx_speech_recognition_eventargs.h>

#ifdef SWIGPYTHON
%template(_SpeechRecognitionEventCallback) CallbackWrapper<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(SpeechRecognitionEventListener) CallbackWrapper<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
#else
%template(SpeechRecognitionEventListener) CallbackWrapper<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
#endif

%template(SpeechRecognitionEventSignal) Carbon::EventSignal<const Carbon::Recognition::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Speech::SpeechRecognitionResult, Carbon::Recognition::Speech::SpeechRecognitionEventArgs>;
%include <speechapi_cxx_speech_recognizer.h>

%include <speechapi_cxx_intent_recognition_result.h>
%include <speechapi_cxx_intent_recognition_eventargs.h>
%include <speechapi_cxx_luis_model.h>
%include <speechapi_cxx_intent_trigger.h>

#ifdef SWIGPYTHON
%template(_IntentEventCallback) CallbackWrapper<const Carbon::Recognition::Intent::IntentRecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(IntentEventListener) CallbackWrapper<const Carbon::Recognition::Intent::IntentRecognitionEventArgs&>;
#else
%template(IntentEventListener) CallbackWrapper<const Carbon::Recognition::Intent::IntentRecognitionEventArgs&>;
#endif

%template(IntentEventSignal) Carbon::EventSignal<const Carbon::Recognition::Intent::IntentRecognitionEventArgs&>;
%template(IntentRecognizerBase) Carbon::Recognition::AsyncRecognizer<Carbon::Recognition::Intent::IntentRecognitionResult, Carbon::Recognition::Intent::IntentRecognitionEventArgs>;
%include <speechapi_cxx_intent_recognizer.h>

%ignore Carbon::Recognition::RecognizerFactoryParameterValue::RecognizerFactoryParameterValue(SPXRECOFACTORYHANDLE, enum FactoryParameter);
%ignore Carbon::Recognition::IDefaultRecognizerFactory::CreateTranslationRecognizer;
%ignore Carbon::Recognition::IDefaultRecognizerFactory::CreateTranslationRecognizerWithFileInput;
%ignore Carbon::Recognition::RecognizerFactory::InternalDefaultRecognizerFactory::CreateTranslationRecognizer;
%ignore Carbon::Recognition::RecognizerFactory::InternalDefaultRecognizerFactory::CreateTranslationRecognizerWithFileInput;
%ignore Carbon::Recognition::DefaultRecognizerFactory::CreateTranslationRecognizer;
%ignore Carbon::Recognition::DefaultRecognizerFactory::CreateTranslationRecognizerWithFileInput;

%include <speechapi_cxx_recognizer_factory_parameter.h>
%include <speechapi_cxx_recognizer_factory.h>

%include <speechapi_cxx_session_parameter_collection.h>
%include <speechapi_cxx_session.h>

//%include <speechapi_cxx_translation_eventargs.h>
//%include <speechapi_cxx_translation_result.h>
//%include <speechapi_cxx_translation_recognizer.h>
