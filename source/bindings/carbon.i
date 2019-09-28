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
%ignore Microsoft::CognitiveServices::Speech::Utils::NonCopyable;
%ignore Microsoft::CognitiveServices::Speech::Utils::NonMovable;
%ignore Microsoft::CognitiveServices::Speech::Utils::HandleOrInvalid;

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

%shared_ptr(Microsoft::CognitiveServices::Speech::Grammar)
%shared_ptr(Microsoft::CognitiveServices::Speech::GrammarList)
%shared_ptr(Microsoft::CognitiveServices::Speech::PhraseListGrammar)
%shared_ptr(Microsoft::CognitiveServices::Speech::ClassLanguageModel)
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
%shared_ptr(Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriber)
%shared_ptr(Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult)

%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs, Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs>::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs>)
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs>::PrivatePropertyCollection)


%shared_ptr(Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::ConversationTranscriber)
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
%shared_ptr(Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult,Microsoft::CognitiveServices::Speech::ConversationTranscriptionEventArgs,Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs >::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechSynthesisResult)
%shared_ptr(Microsoft::CognitiveServices::Speech::AudioDataStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechSynthesisCancellationDetails)
%shared_ptr(Microsoft::CognitiveServices::Speech::SpeechSynthesizer)
%shared_ptr(Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector)
%shared_ptr(Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::AutoDetectSourceLanguageConfig)
%shared_ptr(std::vector<uint8_t>)

%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioConfig)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioOutputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::AudioStreamFormat)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PushAudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStream::FunctionCallbackWrapper)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioInputStreamCallback)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PullAudioOutputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PushAudioOutputStream)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PushAudioOutputStream::FunctionCallbackWrapper)
%shared_ptr(Microsoft::CognitiveServices::Speech::Audio::PushAudioOutputStreamCallback)

%shared_ptr(Microsoft::CognitiveServices::Speech::Conversation::Participant)
%shared_ptr(Microsoft::CognitiveServices::Speech::Conversation::Participant::PrivatePropertyCollection)
%shared_ptr(Microsoft::CognitiveServices::Speech::Conversation::User)

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
%ignore operator SPXPARTICIPANTHANDLE;
%ignore operator SPXSUSERHANDLE;
%ignore operator SPXAUTODETECTSOURCELANGCONFIGHANDLE;

%ignore *::PropertyId;

%inline %{
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> SpeechRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult> IntentRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult> TranslationRecognitionResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechSynthesisResult> SpeechSynthesisResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult> ConversationTranscriberResultPtr;
    typedef std::shared_ptr<Microsoft::CognitiveServices::Speech::Conversation::Participant> ParticipantPtr;
%}

%template(SpeechRecognitionResultPtrFuture) FutureWrapper<SpeechRecognitionResultPtr>;
%template(IntentRecognitionResultPtrFuture) FutureWrapper<IntentRecognitionResultPtr>;
%template(TranslationRecognitionResultPtrFuture) FutureWrapper<TranslationRecognitionResultPtr>;
%template(SpeechSynthesisResultPtrFuture) FutureWrapper<SpeechSynthesisResultPtr>;
%template(StringFuture) FutureWrapper<std::string>;
%template(VoidFuture) FutureWrapper<void>;
%template(ConversationTranscriberResultPtrFuture) FutureWrapper<ConversationTranscriberResultPtr>;


%include <speechapi_cxx_speech_config.h>
%include <speechapi_cxx_speech_translation_config.h>
%include <speechapi_cxx_dialog_service_config.h>
%include <speechapi_cxx_auto_detect_source_lang_config.h>

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

%extend Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriber {

    FutureWrapper<void> StartTranscribingAsync()
    {
        auto future = ($self)->StartTranscribingAsync();
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> StopTranscribingAsync(ResourceHandling resourceHandling = ResourceHandling::KeepResources)
    {
        auto future = ($self)->StopTranscribingAsync(resourceHandling);
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> EndConversationAsync(ResourceHandling resourceHandling = ResourceHandling::KeepResources)
    {
        auto future = ($self)->EndConversationAsync(resourceHandling);
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

%extend Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector {

    FutureWrapper<void> ConnectAsync()
    {
        auto future = ($self)->ConnectAsync();
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<void> DisconnectAsync()
    {
        auto future = ($self)->DisconnectAsync();
        return FutureWrapper<void>(std::move(future));
    }

    FutureWrapper<std::string> SendActivityAsync(std::string activity)
    {
        auto future = ($self)->SendActivityAsync(std::move(activity));
        return FutureWrapper<std::string>(std::move(future));
    }

    FutureWrapper<void> ListenOnceAsync()
    {
        auto future = ($self)->ListenOnceAsync();
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

%extend Microsoft::CognitiveServices::Speech::SpeechSynthesizer {

    FutureWrapper<SpeechSynthesisResultPtr> SpeakTextAsync(const std::string& text)
    {
        auto future = ($self)->SpeakTextAsync(text);
        return FutureWrapper<SpeechSynthesisResultPtr>(std::move(future));
    }

    FutureWrapper<SpeechSynthesisResultPtr> SpeakSsmlAsync(const std::string& ssml)
    {
        auto future = ($self)->SpeakSsmlAsync(ssml);
        return FutureWrapper<SpeechSynthesisResultPtr>(std::move(future));
    }

    FutureWrapper<SpeechSynthesisResultPtr> StartSpeakingTextAsync(const std::string& text)
    {
        auto future = ($self)->StartSpeakingTextAsync(text);
        return FutureWrapper<SpeechSynthesisResultPtr>(std::move(future));
    }

    FutureWrapper<SpeechSynthesisResultPtr> StartSpeakingSsmlAsync(const std::string& ssml)
    {
        auto future = ($self)->StartSpeakingSsmlAsync(ssml);
        return FutureWrapper<SpeechSynthesisResultPtr>(std::move(future));
    }
}

%extend Microsoft::CognitiveServices::Speech::AudioDataStream {

    FutureWrapper<void> SaveToWavFileAsync(std::string fileName)
    {
        auto future = ($self)->SaveToWavFileAsync(fileName);
        return FutureWrapper<void>(std::move(future));
    }
}

%extend Microsoft::CognitiveServices::Speech::Connection {

    FutureWrapper<void> SendMessageAsync(std::string path, std::string payload)
    {
        auto future = ($self)->SendMessageAsync(path, payload);
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
%ignore SpeakTextAsync;
%ignore SpeakSsmlAsync;
%ignore StartSpeakingTextAsync;
%ignore StartSpeakingSsmlAsync;
%ignore SaveToWavFileAsync;
%ignore SendMessageAsync;

%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::ConnectAsync;
%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::DisconnectAsync;
%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::SendActivityAsync;
%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::ListenOnceAsync;
%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::StartKeywordRecognitionAsync;
%ignore Microsoft::CognitiveServices::Speech::Dialog::DialogServiceConnector::StopKeywordRecognitionAsync;

%ignore StartTranscribingAsync;
%ignore StopTranscribingAsync;
%ignore EndConversationAsync;

%ignore Microsoft::CognitiveServices::Speech::SpeechRecognizer::FromConfig(std::shared_ptr<SpeechConfig> speechconfig, std::nullptr_t);

%immutable Microsoft::CognitiveServices::Speech::SpeechRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::RecognitionResult::Properties;
%immutable Microsoft::CognitiveServices::Speech::Session::Properties;
%immutable Microsoft::CognitiveServices::Speech::SpeechSynthesizer::Properties;
%immutable Microsoft::CognitiveServices::Speech::SpeechSynthesisResult::Properties;
%immutable Microsoft::CognitiveServices::Speech::AudioDataStream::Properties;
%immutable Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriber::Properties;
%immutable Microsoft::CognitiveServices::Speech::Participant::Properties;

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

%include <speechapi_cxx_participant.h>
%include <speechapi_cxx_user.h>

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

%include <speechapi_cxx_conversation_transcription_result.h>
%include <speechapi_cxx_conversation_transcription_eventargs.h>
#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(ConversationTranscriberEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs&>;
%template(ConversationTranscriberCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs&>;
#else
%template(ConversationTranscriberEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs&>;
%template(ConversationTranscriberCanceledEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs&>;
#endif

%template(ConversationTranscriberEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs&>;
%template(ConversationTranscriberCanceledEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs&>;
%template(ConversationTranscriberRecognizerBase) Microsoft::CognitiveServices::Speech::AsyncRecognizer<Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionResult, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionEventArgs, Microsoft::CognitiveServices::Speech::Conversation::ConversationTranscriptionCanceledEventArgs>;

%include <speechapi_cxx_intent_recognizer.h>

%include <speechapi_cxx_conversation_transcriber.h>

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

%include <speechapi_cxx_dialog_service_connector_eventargs.h>

%template(ActivityReceivedEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::Dialog::ActivityReceivedEventArgs&>;
%template(ActivityReceivedEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Dialog::ActivityReceivedEventArgs&>;

%include <speechapi_cxx_speech_synthesis_result.h>
%include <speechapi_cxx_speech_synthesis_eventargs.h>
%include <speechapi_cxx_speech_synthesis_word_boundary_eventargs.h>

#ifdef SWIGPYTHON
#elif defined(SWIGJAVA)
%template(SpeechSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechSynthesisEventArgs&>;
%template(SpeechSynthesisWordBoundaryEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechSynthesisWordBoundaryEventArgs&>;
#else
%template(SpeechSynthesisEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechSynthesisEventArgs&>;
%template(SpeechSynthesisWordBoundaryEventListener) CallbackWrapper<const Microsoft::CognitiveServices::Speech::SpeechSynthesisWordBoundaryEventArgs&>;
#endif

%template(SpeechSynthesisEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SpeechSynthesisEventArgs&>;
%template(SpeechSynthesisWordBoundaryEventSignal) Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::SpeechSynthesisWordBoundaryEventArgs&>;

%include <speechapi_cxx_speech_synthesizer.h>

%include <speechapi_cxx_translation_recognizer.h>

%include <speechapi_cxx_dialog_service_connector.h>

%include <speechapi_cxx_session.h>

%include <speechapi_cxx_connection.h>
%include <speechapi_cxx_grammar.h>
%include <speechapi_cxx_phrase_list_grammar.h>
%include <speechapi_cxx_grammar_list.h>
%include <speechapi_cxx_class_language_model.h>

%template(FromRecognizer) Microsoft::CognitiveServices::Speech::PhraseListGrammar::FromRecognizer<Microsoft::CognitiveServices::Speech::Recognizer>;
%template(FromRecognizer) Microsoft::CognitiveServices::Speech::GrammarList::FromRecognizer<Microsoft::CognitiveServices::Speech::Recognizer>;
%template(Add) Microsoft::CognitiveServices::Speech::GrammarList::Add<Microsoft::CognitiveServices::Speech::Grammar>;
%template(AssignClass) Microsoft::CognitiveServices::Speech::ClassLanguageModel::AssignClass<Microsoft::CognitiveServices::Speech::Grammar>;
