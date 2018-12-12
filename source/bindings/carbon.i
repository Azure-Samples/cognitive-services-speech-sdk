%{
#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif
%}

%ignore Microsoft::CognitiveServices::Speech::Utils::ToUTF8;
%ignore Microsoft::CognitiveServices::Speech::Utils::ToSPXString;

// Add necessary symbols to generated header
%{
#include <wrappers.h>
#include <speechapi_cxx.h>
%}

%include <exception.i>
%include <std_except.i>
%include <std_shared_ptr.i>
%include <std_string.i>
%include <std_wstring.i>
%include <stdint.i>
%include <std_vector.i>
%include <std_map.i>

%ignore __spx_rethrow(SPXHR);

%exception {
    try { $action }
    catch (const Swig::DirectorException &e1) { SWIG_exception(SWIG_RuntimeError, e1.what()); }
    catch (const std::exception &e2) { SWIG_exception(SWIG_RuntimeError, e2.what()); }
    catch (...) { SWIG_exception(SWIG_UnknownError,"Runtime exception"); }
}

%shared_ptr(Microsoft::CognitiveServices::Speech::Connection)
%shared_ptr(Microsoft::CognitiveServices::Speech::Recognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult, Microsoft::CognitiveServices::Speech::RecognitionEventArgs, Microsoft::CognitiveServices::Speech::RecognitionEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult, Microsoft::CognitiveServices::Speech::RecognitionEventArgs, Microsoft::CognitiveServices::Speech::RecognitionEventArgs>::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::BaseAsyncRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::RecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::NoMatchDetails)
%shared_ptr(Microsoft::CognitiveServices::Speech::CancellationDetails)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechRecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult, Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs, Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult, Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs, Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs>::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs>::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentTrigger)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::LanguageUnderstandingModel)
%shared_ptr(Microsoft::CognitiveServices::Speech::KeywordRecognitionModel)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs>::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::PropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult,Microsoft::CognitiveServices::Speech::RecognitionEventArgs,Microsoft::CognitiveServices::Speech::RecognitionEventArgs >::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult,Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs,Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs >::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult,Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs,Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs >::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult,Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs,Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs >::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig)

%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioStreamFormat)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PushAudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStream::FunctionCallbackWrapper)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStreamCallback)

#ifdef SPX_UWP
%template(StringVector) std::vector<std::wstring>;
%template(StdMapStringString) std::map<std::wstring, std::wstring>;
#else
%template(StringVector) std::vector<std::string>;
%template(StdMapStringString) std::map<std::string, std::string>;
#endif

#ifdef SWIGPYTHON
%template(Uint8Vector) std::vector<uint8_t>;
#endif

%ignore CallbackWrapper::GetFunction();
%ignore FutureWrapper::FutureWrapper;
%include <wrappers.h>

%include <speechapi_cxx_common.h>
%include <speechapi_cxx_string_helpers.h>
%include <speechapi_cxx_enums.h>

%ignore Microsoft::CognitiveServices::Speech::NotYetImplementedException;

%ignore operator=;
%ignore operator[];
%ignore operator const std::string;
%ignore operator const std::wstring;
%ignore operator int32_t;
%ignore operator SPXLUISHANDLE;
%ignore operator SPXTRIGGERHANDLE;
%ignore operator SPXAUDIOSTREAMFORMATHANDLE;
%ignore operator SPXAUDIOSTREAMHANDLE;
%ignore operator SPXAUDIOCONFIGHANDLE;
%ignore operator SPXKEYWORDHANDLE;
%ignore operator SPXRESULTHANDLE;
%ignore operator SPXLUMODELHANDLE;
%ignore operator SPXSPEECHCONFIGHANDLE;

%ignore *::PropertyId;

%inline %{
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult> IntentRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult> TranslationRecognitionResultPtr;
%}

%template(SpeechRecognitionResultPtrFuture) FutureWrapper<SpeechRecognitionResultPtr>;
%template(IntentRecognitionResultPtrFuture) FutureWrapper<IntentRecognitionResultPtr>;
%template(TranslationRecognitionResultPtrFuture) FutureWrapper<TranslationRecognitionResultPtr>;
%template(VoidFuture) FutureWrapper<void>;


%include <speechapi_cxx_speech_config.h>
%include <speechapi_cxx_speech_translation_config.h>

// %extend need to come first, before the %ignore for the same method (RecognizeOnceAsync, etc.)
%extend Microsoft::CognitiveServices::Speech::SpeechRecognizer {

    SpeechRecognitionResultPtr Recognize() {
        return ($self)->RecognizeOnceAsync().get();
    }

    void StartContinuousRecognition()
    {
        ($self)->StartContinuousRecognitionAsync().get();
    }

    void StopContinuousRecognition()
    {
        ($self)->StopContinuousRecognitionAsync().get();
    }

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
    }

    FutureWrapper<SpeechRecognitionResultPtr> RecognizeOnceAsync() {
        auto future = ($self)->RecognizeOnceAsync();
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

    FutureWrapper<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model)
    {
        auto future = ($self)->StartKeywordRecognitionAsync(model);
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopKeywordRecognitionAsync()
    {
        auto future = ($self)->StopKeywordRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }
}

%extend Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer {

    IntentRecognitionResultPtr Recognize() {
        return ($self)->RecognizeOnceAsync().get();
    }

    void StartContinuousRecognition()
    {
        ($self)->StartContinuousRecognitionAsync().get();
    }

    void StopContinuousRecognition()
    {
        ($self)->StopContinuousRecognitionAsync().get();
    }

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
    }

    FutureWrapper<IntentRecognitionResultPtr> RecognizeOnceAsync() {
        auto future = ($self)->RecognizeOnceAsync();
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

    FutureWrapper<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model)
    {
        auto future = ($self)->StartKeywordRecognitionAsync(model);
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopKeywordRecognitionAsync()
    {
        auto future = ($self)->StopKeywordRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }
}

%extend Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer {

    TranslationRecognitionResultPtr Recognize() {
        return ($self)->RecognizeOnceAsync().get();
    }

    void StartContinuousRecognition()
    {
        ($self)->StartContinuousRecognitionAsync().get();
    }

    void StopContinuousRecognition()
    {
        ($self)->StopContinuousRecognitionAsync().get();
    }

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
    }

    FutureWrapper<TranslationRecognitionResultPtr> RecognizeOnceAsync() {
        auto future = ($self)->RecognizeOnceAsync();
        return FutureWrapper<TranslationRecognitionResultPtr>(std::move(future));
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

    FutureWrapper<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model)
    {
        auto future = ($self)->StartKeywordRecognitionAsync(model);
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopKeywordRecognitionAsync()
    {
        auto future = ($self)->StopKeywordRecognitionAsync();
        return FutureWrapper<void>(std::move(future));
    }
}

#ifndef SWIGPYTHON
%feature("director") CallbackWrapper;

%extend Microsoft::CognitiveServices::Speech::EventSignal {

#if defined(SWIGJAVA)
    void AddEventListener(CallbackWrapper<T>& callback)
#else
    void Connect(CallbackWrapper<T>& callback)
#endif
    {
        ($self)->Connect(callback.GetFunction());
    };

#if defined(SWIGJAVA)
    void RemoveEventListener(CallbackWrapper<T>& callback)
#else
    void Disconnect(CallbackWrapper<T>& callback)
#endif
    {
        ($self)->Disconnect(callback.GetFunction());
    };
}
#endif


#ifndef SWIGPYTHON
%define %add_subscript_operator(Type, Enum)
%extend Microsoft::CognitiveServices::Speech::Type {
    ValuePtr Get(const std::string& name) {
        return std::make_shared<Value>(std::move(($self)->operator[](name)));
    }
    ValuePtr Get(Microsoft::CognitiveServices::Speech::Enum index) {
        return std::make_shared<Value>(std::move(($self)->operator[](index)));
    }
}
%enddef
#endif

%ignore Microsoft::CognitiveServices::Speech::EventSignal::EventSignal;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::CallbackFunction;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Signal;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Connect(CallbackFunction callback);
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Disconnect(CallbackFunction callback);
%ignore Microsoft::CognitiveServices::Speech::EventSignal::operator+=;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::operator-=;

%ignore RecognizeOnceAsync;
%ignore StartContinuousRecognitionAsync;
%ignore StopContinuousRecognitionAsync;
%ignore StartKeywordRecognitionAsync;
%ignore StopKeywordRecognitionAsync;

%immutable Microsoft::CognitiveServices::Speech::SpeechRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::RecognitionResult::Properties;
%immutable Microsoft::CognitiveServices::Speech::Session::Properties;

%include <speechapi_cxx_properties.h>

%include <speechapi_cxx_audio_stream_format.h>
%include <speechapi_cxx_audio_stream.h>
%include <speechapi_cxx_audio_config.h>

%include <speechapi_cxx_keyword_recognition_model.h>

%include <speechapi_cxx_eventargs.h>
%include <speechapi_cxx_eventsignal.h>

%include <speechapi_cxx_session_eventargs.h>
%include <speechapi_cxx_connection_eventargs.h>

%immutable Microsoft::CognitiveServices::Speech::RecognitionResult::Properties;
%include <speechapi_cxx_recognition_result.h>
%include <speechapi_cxx_recognition_eventargs.h>

#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(SessionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
%template(ConnectionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::ConnectionEventArgs&>;
#else
%template(SessionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
%template(ConnectionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::ConnectionEventArgs&>;
#endif

%template(SessionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
%template(ConnectionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::ConnectionEventArgs&>;

%include <speechapi_cxx_recognizer.h>
%include <speechapi_cxx_recognition_async_recognizer.h>

%template(BaseRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult, Microsoft::CognitiveServices::Speech::RecognitionEventArgs, Microsoft::CognitiveServices::Speech::RecognitionEventArgs>;
%include <speechapi_cxx_recognition_base_async_recognizer.h>

%include <speechapi_cxx_speech_recognition_result.h>
%include <speechapi_cxx_speech_recognition_eventargs.h>

#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(SpeechRecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognitionCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs&>;
#else
%template(SpeechRecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognitionCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs&>;
#endif

%template(SpeechRecognitionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognitionCanceledEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs&>;
%template(SpeechRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult, Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs, Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs>;
%include <speechapi_cxx_speech_recognizer.h>

%include <speechapi_cxx_intent_recognition_result.h>
%include <speechapi_cxx_intent_recognition_eventargs.h>
%include <speechapi_cxx_language_understanding_model.h>
%include <speechapi_cxx_intent_trigger.h>

#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(IntentEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
%template(IntentCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs&>;
#else
%template(IntentEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
%template(IntentCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs&>;
#endif

%template(IntentEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
%template(IntentCanceledEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs&>;
%template(IntentRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs>;

%include <speechapi_cxx_intent_recognizer.h>

%include <speechapi_cxx_translation_result.h>
%include <speechapi_cxx_translation_eventargs.h>

#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(TranslationTexEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs&>;
%template(TranslationTexCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs&>;
%template(TranslationSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisEventArgs&>;
#else
%template(TranslationTextEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs&>;
%template(TranslationTextCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs&>;
%template(TranslationSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisEventArgs&>;
#endif

%template(TranslationTextEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs&>;
%template(TranslationTextCanceledEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs&>;
%template(TranslationSynthesisEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisEventArgs&>;
%template(TranslationRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs>;

%include <speechapi_cxx_translation_recognizer.h>

%include <speechapi_cxx_session.h>

%include <speechapi_cxx_connection.h>

