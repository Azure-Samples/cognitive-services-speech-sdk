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

%include <exception.i>
%include <std_except.i>
%include <std_shared_ptr.i>
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

%shared_ptr(Microsoft::CognitiveServices::Speech::Recognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult, Microsoft::CognitiveServices::Speech::RecognitionEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::BaseAsyncRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::RecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechRecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult, Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentTrigger)
%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::LanguageUnderstandingModel)
%shared_ptr(Microsoft::CognitiveServices::Speech::KeywordRecognitionModel)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationTextResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationTextResult, Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::ISpeechFactory)
%shared_ptr(Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory)
%shared_ptr(Microsoft::CognitiveServices::Speech::Value)

%template(WstringVector) std::vector<std::wstring>;
%template(StdMapWStringWString) std::map<std::wstring, std::wstring>;

%ignore CallbackWrapper::GetFunction();
%ignore FutureWrapper::FutureWrapper;
%include <wrappers.h>

%ignore Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory::CreateSpeechRecognizerWithStream;
%ignore Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory::CreateIntentRecognizerWithStream;
%ignore Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory::CreateTranslationRecognizerWithStream;

%include <speechapi_cxx_common.h>

// %include <speechapi_c_property_names.h>
// %include <speechapi_cxx_property_names.h>

%ignore Microsoft::CognitiveServices::Speech::NotYetImplementedException;

%ignore operator=;
%ignore operator[];
%ignore operator const std::wstring;
%ignore operator int32_t;
%ignore operator SPXLUISHANDLE;
%ignore operator SPXTRIGGERHANDLE;

%inline %{
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult> IntentRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Translation::TranslationTextResult> TranslationTextResultPtr;
%}

%template(SpeechRecognitionResultPtrFuture) FutureWrapper<SpeechRecognitionResultPtr>;
%template(IntentRecognitionResultPtrFuture) FutureWrapper<IntentRecognitionResultPtr>;
%template(TranslationTextResultPtrFuture) FutureWrapper<TranslationTextResultPtr>;
%template(VoidFuture) FutureWrapper<void>;

// %extend need to come first, before the %ignore for the same method (RecognizeAsync, etc.)
%extend Microsoft::CognitiveServices::Speech::SpeechRecognizer {

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

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
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

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
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

    TranslationTextResultPtr Recognize() {
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

    void StartKeywordRecognition(std::shared_ptr<KeywordRecognitionModel> model)
    {
        ($self)->StartKeywordRecognitionAsync(model).get();
    }

    void StopKeywordRecognition()
    {
        ($self)->StopKeywordRecognitionAsync().get();
    }

    FutureWrapper<TranslationTextResultPtr> RecognizeAsync() {
        auto future = ($self)->RecognizeAsync();
        return FutureWrapper<TranslationTextResultPtr>(std::move(future));
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

%feature("director") CallbackWrapper;

%extend Microsoft::CognitiveServices::Speech::EventSignal {

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

%extend Microsoft::CognitiveServices::Speech::Value {

    const std::wstring& __setitem__(const std::wstring& value) {
        return ($self)->operator=(value);
    }

    int32_t __setitem__(int32_t value) {
        return ($self)->operator=(value);
    }

    bool __setitem__(bool value) {
        return ($self)->operator=(value);
    }
}


%inline %{
    typedef Microsoft::CognitiveServices::Speech::Value Value;
    typedef std::shared_ptr<Value> ValuePtr;
%}

#ifdef SWIGPYTHON

%define %add_subscript_operator(Type, Enum)
%extend Microsoft::CognitiveServices::Speech::Type {
    ValuePtr __getitem__(const std::wstring& name) {
        return std::make_shared<Value>(std::move(($self)->operator[](name)));
    }
    ValuePtr __getitem__(Microsoft::CognitiveServices::Speech::Enum index) {
        return std::make_shared<Value>(std::move(($self)->operator[](index)));
    }
}
%enddef

#else

%define %add_subscript_operator(Type, Enum)
%extend Microsoft::CognitiveServices::Speech::Type {
    ValuePtr Get(const std::wstring& name) {
        return std::make_shared<Value>(std::move(($self)->operator[](name)));
    }
    ValuePtr Get(Microsoft::CognitiveServices::Speech::Enum index) {
        return std::make_shared<Value>(std::move(($self)->operator[](index)));
    }
}
%enddef

#endif

%add_subscript_operator(ResultPropertyValueCollection, ResultProperty)
%add_subscript_operator(FactoryParameterCollection, FactoryParameter)
%add_subscript_operator(RecognizerParameterValueCollection, RecognizerParameter)
%add_subscript_operator(SessionParameterValueCollection, SessionParameter)

%ignore Microsoft::CognitiveServices::Speech::EventSignal::EventSignal;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::CallbackFunction;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Signal;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Connect(CallbackFunction callback);
%ignore Microsoft::CognitiveServices::Speech::EventSignal::Disconnect(CallbackFunction callback);
%ignore Microsoft::CognitiveServices::Speech::EventSignal::operator+=;
%ignore Microsoft::CognitiveServices::Speech::EventSignal::operator-=;

%ignore RecognizeAsync;
%ignore StartContinuousRecognitionAsync;
%ignore StopContinuousRecognitionAsync;
%ignore StartKeywordRecognitionAsync;
%ignore StopKeywordRecognitionAsync;

%ignore Microsoft::CognitiveServices::Speech::Value::Value;
%ignore Microsoft::CognitiveServices::Speech::SessionParameterValue;
%ignore Microsoft::CognitiveServices::Speech::ResultPropertyValue;
%ignore Microsoft::CognitiveServices::Speech::RecognizerParameterValue;
%ignore Microsoft::CognitiveServices::Speech::FactoryParameterValue;

%include <speechapi_cxx_value.h>

%template(RecognizerParameterValueCollectionBase) Microsoft::CognitiveServices::Speech::HandleValueCollection<SPXRECOHANDLE, Microsoft::CognitiveServices::Speech::RecognizerParameterValue>;
%template(ResultPropertyValueCollectionBase) Microsoft::CognitiveServices::Speech::HandleValueCollection<SPXRESULTHANDLE, Microsoft::CognitiveServices::Speech::ResultPropertyValue>;
%template(FactoryParameterCollectionBase) Microsoft::CognitiveServices::Speech::HandleValueCollection<SPXFACTORYHANDLE, Microsoft::CognitiveServices::Speech::FactoryParameterValue>;
%template(SessionParameterValueCollectionBase) Microsoft::CognitiveServices::Speech::HandleValueCollection<SPXSESSIONHANDLE, Microsoft::CognitiveServices::Speech::SessionParameterValue>;

%include <speechapi_cxx_audioinputstream.h>
%include <speechapi_cxx_keyword_recognition_model.h>

%include <speechapi_cxx_eventargs.h>
%include <speechapi_cxx_eventsignal.h>

%include <speechapi_cxx_session_eventargs.h>

%immutable Microsoft::CognitiveServices::Speech::RecognitionResult::Properties;
%include <speechapi_cxx_recognition_result.h>
%include <speechapi_cxx_recognition_eventargs.h>

#ifdef SWIGPYTHON
%template(_SessionEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(_RecognitionEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(SessionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
#else
%template(SessionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;
#endif

%template(SessionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SessionEventArgs&>;
%template(RecognitionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::RecognitionEventArgs&>;

%include <speechapi_cxx_recognizer_parameter_collection.h>
%include <speechapi_cxx_recognizer.h>
%include <speechapi_cxx_recognition_async_recognizer.h>

%template(BaseRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::RecognitionResult, Microsoft::CognitiveServices::Speech::RecognitionEventArgs>;
%include <speechapi_cxx_recognition_base_async_recognizer.h>

%include <speechapi_cxx_speech_recognition_result.h>
%include <speechapi_cxx_speech_recognition_eventargs.h>

#ifdef SWIGPYTHON
%template(_SpeechRecognitionEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(SpeechRecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
#else
%template(SpeechRecognitionEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
#endif

%template(SpeechRecognitionEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs&>;
%template(SpeechRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult, Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs>;
%include <speechapi_cxx_speech_recognizer.h>

%include <speechapi_cxx_intent_recognition_result.h>
%include <speechapi_cxx_intent_recognition_eventargs.h>
%include <speechapi_cxx_language_understanding_model.h>
%include <speechapi_cxx_intent_trigger.h>

#ifdef SWIGPYTHON
%template(_IntentEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
#elif defined(SWIGJAVA)
%template(IntentEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
#else
%template(IntentEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
#endif

%template(IntentEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs&>;
%template(IntentRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs>;

%include <speechapi_cxx_intent_recognizer.h>

%ignore Microsoft::CognitiveServices::Speech::FactoryParameterValue::FactoryParameterValue(SPXFACTORYHANDLE, enum FactoryParameter);
%ignore GetLanguageResource(LanguageResourceScope scopes, std::wstring acceptLanguage);

%include <speechapi_cxx_translation_result.h>
%include <speechapi_cxx_translation_eventargs.h>

#ifdef SWIGPYTHON
%template(_TranslationTextEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs&>;
%template(_TranslationSynthesisEventCallback) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResultEventArgs&>;
#elif defined(SWIGJAVA)
%template(TranslationTexEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs&>;
%template(TranslationSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResultEventArgs&>;
#else
%template(TranslationTextEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs&>;
%template(TranslationSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResultEventArgs&>;
#endif

%template(TranslationTextEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs&>;
%template(TranslationSynthesisEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResultEventArgs&>;
%template(TranslationRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Translation::TranslationTextResult, Microsoft::CognitiveServices::Speech::Translation::TranslationTextResultEventArgs>;

%immutable Microsoft::CognitiveServices::Speech::ISpeechFactory::Parameters;
%include <speechapi_cxx_translation_recognizer.h>
%include <speechapi_cxx_factory_parameter.h>
%include <speechapi_cxx_factory.h>

%immutable Microsoft::CognitiveServices::Speech::Session::Parameters;
%include <speechapi_cxx_session_parameter_collection.h>
%include <speechapi_cxx_session.h>


