// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.

%module(directors="1", "threads"=1) speech_py_impl

// rename all functions/methods/properties to match Python's naming convention
// https://www.python.org/dev/peps/pep-0008/#method-names-and-instance-variables
%rename("%(undercase)s", %$isfunction, %$not %$isconstructor) "";
%rename("%(undercase)s", %$ismember, %$not %$isconstructor) "";

// Python typing
%pythoncode %{
from typing import Optional
%}

%feature("nothreadallow");

// specific renamings
// implementation-defined reason, replaced by a property with a python Enum in the python API
%rename("_reason") *::Reason;
%rename("_get_result") *::GetResult;
%rename("_get_cancellation_details") *::GetCancellationDetails;

%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::PropertyCollection::GetProperty(const std::string& propertyName, const std::string& defaultValue) const;
%feature("shadow") Microsoft::CognitiveServices::Speech::PropertyCollection::GetProperty const %{
    def get_property(self, property_id: "PropertyId", default_value: str = '') -> str:
        if not isinstance(property_id, PropertyId):
            raise TypeError('wrong type, must be PropertyId')
        return _speech_py_impl.PropertyCollection_get_property(self, property_id.value, default_value)%};


%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::PropertyCollection::SetProperty(const std::string& name, const std::string& value);
%feature("shadow") Microsoft::CognitiveServices::Speech::PropertyCollection::SetProperty(PropertyId id, const std::string& value) %{
    def set_property(self, property_id: "PropertyId", value: str):
        if not isinstance(property_id, PropertyId):
            raise TypeError('wrong type, must be PropertyId')
        return _speech_py_impl.PropertyCollection_set_property(self, property_id.value, value)%};

%rename ("recognize_once") Recognize;

// Ignore overloaded methods for python
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::AudioDataStream::CanReadData(uint32_t pos, uint32_t bytesRequested);
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::AudioDataStream::ReadData(uint32_t pos, uint8_t* buffer, uint32_t bufferSize);

//don't expose all the different methods to create push and pull streams
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Audio::AudioInputStream::CreatePushStream;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Audio::AudioInputStream::CreatePullStream;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Audio::AudioOutputStream::CreatePushStream;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Audio::AudioOutputStream::CreatePullStream;

// hide constructors for internally used classes
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::RecognitionResult::RecognitionResult;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::RecognitionEventArgs::RecognitionEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs::SpeechRecognitionEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs::SpeechRecognitionCanceledEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SessionEventArgs::SessionEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechRecognitionResult::SpeechRecognitionResult;

%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechSynthesisResult::SpeechSynthesisResult;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechSynthesisEventArgs::SpeechSynthesisEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::SpeechSynthesisWordBoundaryEventArgs::SpeechSynthesisWordBoundaryEventArgs;

%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionResult::TranslationRecognitionResult;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs::TranslationRecognitionEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs::TranslationRecognitionCanceledEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisEventArgs::TranslationSynthesisEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResult::TranslationSynthesisResult;

%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult::IntentRecognitionResult;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs::IntentRecognitionCanceledEventArgs;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs::IntentRecognitionEventArgs;

%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::KeywordRecognitionResult::KeywordRecognitionResult;
%rename ("$ignore", fullname=1) Microsoft::CognitiveServices::Speech::KeywordRecognitionEventArgs::KeywordRecognitionEventArgs;

// Release GIL:
// * when invoking a blocking Recognize/Synthesize.
%threadallow Microsoft::CognitiveServices::Speech::SpeechRecognizer::Recognize;
%threadallow Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::Recognize;
%threadallow Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::Recognize;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::SpeakText;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::SpeakSsml;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::StartSpeakingText;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::StartSpeakingSsml;
// * when invoking a nonblocking Recognize/Synthesize.
%threadallow Microsoft::CognitiveServices::Speech::SpeechRecognizer::RecognizeOnceAsync;
%threadallow Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::RecognizeOnceAsync;
%threadallow Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::RecognizeOnceAsync;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::SpeakTextAsync;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::SpeakSsmlAsync;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::StartSpeakingTextAsync;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::StartSpeakingSsmlAsync;
%threadallow Microsoft::CognitiveServices::Speech::KeywordRecognizer::RecognizeOnceAsync;
// * when invoking SpeechRecognizer/SpeechSynthesizer dtor (otherwise there might be a deadlock if a
//   callback is concurrently invoked from the speech client.)
%threadallow Microsoft::CognitiveServices::Speech::SpeechRecognizer::~SpeechRecognizer;
%threadallow Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::~IntentRecognizer;
%threadallow Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::~TranslationRecognizer;;
%threadallow Microsoft::CognitiveServices::Speech::SpeechSynthesizer::~SpeechSynthesizer;
%threadallow Microsoft::CognitiveServices::Speech::KeywordRecognizer::~KeywordRecognizer;
// * when waiting on a future.
%threadallow FutureWrapper::Get;
// * when invoking disconnect (which involves acquiring an event signal lock) to
// avoid a deadlock (in-progress callback waiting for GIL might be holding
// the event signal lock).
%threadallow Microsoft::CognitiveServices::Speech::EventSignal::_Disconnect;
%threadallow Microsoft::CognitiveServices::Speech::EventSignal::DisconnectAll;
// * when reading data from stream, which will block until enough data is available
%threadallow Microsoft::CognitiveServices::Speech::Audio::PullAudioOutputStream::Read;
%threadallow Microsoft::CognitiveServices::Speech::AudioDataStream::ReadData;

%extend Microsoft::CognitiveServices::Speech::CancellationDetails {
    %pythoncode %{
    def __str__(self):
        return u'{}(reason={}, error_details="{}")'.format(type(self).__name__,
            self.reason, self.error_details)
    %}
}

%extend Microsoft::CognitiveServices::Speech::NoMatchDetails {
    %pythoncode %{
    def __str__(self):
        return u'{}(reason={})'.format(type(self).__name__, self.reason)
    %}
}

%extend Microsoft::CognitiveServices::Speech::RecognitionResult {
    %pythoncode %{
    def __str__(self):
        return u'{}:(result_id={}, text={})'.format(type(self), self.result_id, self.text)
    %}
}

%extend Microsoft::CognitiveServices::Speech::SessionEventArgs {
    %pythoncode %{
    def __str__(self):
        return u'{}:(session_id={})'.format(type(self), self.session_id)
    %}
}

%extend Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs {
    %pythoncode %{
    def __str__(self):
        return u'{}:(session_id={}, result={})'.format(type(self), self.session_id, self.result)
    %}
}

%extend Microsoft::CognitiveServices::Speech::SpeechSynthesisResult {
    %pythoncode %{
    def __str__(self):
        return u'{}:(result_id={})'.format(type(self), self.result_id)
    %}
}

%extend Microsoft::CognitiveServices::Speech::SpeechSynthesisCancellationDetails {
    %pythoncode %{
    def __str__(self):
        return u'{}(reason={}, error_details="{}")'.format(type(self).__name__,
            self.reason, self.error_details)
    %}
}

%extend Microsoft::CognitiveServices::Speech::PropertyCollection {
    %pythoncode %{
    def get_property_by_name(self, property_name: str):
        return _speech_py_impl.PropertyCollection_get_property(self, property_name)
    %}
}

// hide factories. %feature("shadow") doesn't work for static methods, so need to
// rename the original factory to a private one and then extend with the new one.
%rename ("_from_default_microphone_input") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromDefaultMicrophoneInput;
%rename ("_from_stream_input") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromStreamInput;
%rename ("_from_wav_file_input") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromWavFileInput;
%rename ("_from_microphone_input") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromMicrophoneInput;
%rename ("_from_default_speaker_output") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromDefaultSpeakerOutput;
%rename ("_from_wav_file_output") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromWavFileOutput;
%rename ("_from_stream_output") Microsoft::CognitiveServices::Speech::Audio::AudioConfig::FromStreamOutput;
%rename ("_from_result") Microsoft::CognitiveServices::Speech::CancellationDetails::FromResult;
%rename ("_from_result") Microsoft::CognitiveServices::Speech::SpeechSynthesisCancellationDetails::FromResult;
%rename ("_from_stream") Microsoft::CognitiveServices::Speech::SpeechSynthesisCancellationDetails::FromStream;
%rename ("_from_result") Microsoft::CognitiveServices::Speech::AudioDataStream::FromResult;
%rename ("_from_config") Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer::FromConfig;
%rename ("_from_file") Microsoft::CognitiveServices::Speech::KeywordRecognitionModel::FromFile;
%rename ("_from_app_id") Microsoft::CognitiveServices::Speech::Intent::LanguageUnderstandingModel::FromAppId;
%rename ("_from_endpoint") Microsoft::CognitiveServices::Speech::Intent::LanguageUnderstandingModel::FromEndpoint;
%rename ("_from_subscription") Microsoft::CognitiveServices::Speech::Intent::LanguageUnderstandingModel::FromSubscription;
%rename ("_from_result") Microsoft::CognitiveServices::Speech::NoMatchDetails::FromResult;
%rename ("_from_authorization_token") Microsoft::CognitiveServices::Speech::SpeechConfig::FromAuthorizationToken;
%rename ("_from_endpoint") Microsoft::CognitiveServices::Speech::SpeechConfig::FromEndpoint;
%rename ("_from_host") Microsoft::CognitiveServices::Speech::SpeechConfig::FromHost;
%rename ("_from_subscription") Microsoft::CognitiveServices::Speech::SpeechConfig::FromSubscription;
%rename ("_from_config") Microsoft::CognitiveServices::Speech::SpeechRecognizer::FromConfig;
%rename ("_from_config") Microsoft::CognitiveServices::Speech::SpeechSynthesizer::FromConfig;
%rename ("_from_authorization_token") Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig::FromAuthorizationToken;
%rename ("_from_endpoint") Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig::FromEndpoint;
%rename ("_from_host") Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig::FromHost;
%rename ("_from_subscription") Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig::FromSubscription;
%rename ("_from_config") Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer::FromConfig;
%rename ("from_recognizer") Microsoft::CognitiveServices::Speech::PhraseListGrammar::FromRecognizer;
%rename ("_from_language") Microsoft::CognitiveServices::Speech::SourceLanguageConfig::FromLanguage;
%rename ("_from_open_range") Microsoft::CognitiveServices::Speech::AutoDetectSourceLanguageConfig::FromOpenRange;
%rename ("_from_languages") Microsoft::CognitiveServices::Speech::AutoDetectSourceLanguageConfig::FromLanguages;
%rename ("_from_source_language_configs") Microsoft::CognitiveServices::Speech::AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs;
%rename ("_from_result") Microsoft::CognitiveServices::Speech::AutoDetectSourceLanguageResult::FromResult;
%rename ("_from_config") Microsoft::CognitiveServices::Speech::KeywordRecognizer::FromConfig;

// return synthesized audio as bytes object
%rename ("_audio") Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResult::Audio;
%extend Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisResult {
    %pythoncode %{
    @property
    def audio(self) -> bytes:
        """The synthesized translation result as a `bytes` object"""
        return bytes(_speech_py_impl.TranslationSynthesisResult__audio_get(self))%}
}

// return TTS synthesized audio as bytes object
%rename ("_get_audio_data") Microsoft::CognitiveServices::Speech::SpeechSynthesisResult::GetAudioData;
%extend Microsoft::CognitiveServices::Speech::SpeechSynthesisResult {
    %pythoncode %{
    @property
    def audio_data(self) -> bytes:
        """The synthesized TTS result as a `bytes` object"""
        audio_length = _speech_py_impl.SpeechSynthesisResult_get_audio_length(self)
        if audio_length > 0:
            return bytes(_speech_py_impl.SpeechSynthesisResult__get_audio_data(self))
        else:
            return bytes(0)
    %}
}

%feature("director") PullAudioInputStreamCallback;
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

%exception Read {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}

%feature("director") PushAudioOutputStreamCallback;
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

%exception Write {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}

//typemaps for PullAudioStream and AudioDataStream
namespace Microsoft::CognitiveServices::Speech::Audio {
    %typemap(directorin) (uint8_t* dataBuffer, uint32_t ) {
        // in typemap: convert the C pointer to the buffer into a python `memoryview`
        $input = PyMemoryView_FromMemory((char*) $1, $2, PyBUF_WRITE);
    }

    %typemap(directorin) (uint8_t* buffer, uint32_t) = (uint8_t* dataBuffer, uint32_t);

    // view is local to the function. SWIG renames it to view2,
    // presumably because it assigns it to the second argument.
    %typemap(in) (uint8_t* dataBuffer, uint32_t size) (Py_buffer view) {
        // in typemap: convert a python `memoryview` to a raw pointer and size
        int res; Py_ssize_t sz = 0; void *buf = 0;
        res = PyObject_CheckBuffer($input);
        if (!res)
        {
            PyErr_Clear();
            %argument_fail(res, "(TYPEMAP, SIZE)", $symname, $argnum);
        }
        res = PyObject_GetBuffer($input, &view, PyBUF_CONTIG_RO);
        if (res < 0)
        {
            PyErr_Clear();
            %argument_fail(res, "(TYPEMAP, SIZE)", $symname, $argnum);
        }
        sz = view.len;
        buf = view.buf;
        // prepare the arguments for the wrapped function
        $1 = ($1_ltype) buf; // dataBuffer
        $2 = ($2_ltype) (sz/sizeof($*1_type)); // size
    }

    %typemap(in) (uint8_t* buffer, uint32_t bufferSize) = (uint8_t* dataBuffer, uint32_t size);

    %typemap(freearg) (uint8_t* dataBuffer, uint32_t size) {
        // in typemap(freearg): cleanup the buffer from the typemap above
        PyBuffer_Release(&view2);
        }

    %typemap(freearg) (uint8_t* buffer, uint32_t bufferSize) = (uint8_t* dataBuffer, uint32_t size);
}

// wrap callbacks in C++, so that callbacks from python do not have to be typed
%{
namespace Microsoft { namespace CognitiveServices { namespace Speech {
template <class T>
/// <summary>
/// Creates a wrapper around a Python callable object that can serve as callback function to the events
/// fired by the SDK.
/// </summary>
/// <tparam name="T"> The type of EventArgs that the callback is called with. </tparam>
class PyCallback
{
    private:
        PyObject* m_func;
        swig_type_info *m_wrapped_type;
        PyObject* m_event_args_pytype;
    public:
        PyCallback<T>& operator=(const PyCallback<T>&) = delete;
        PyCallback<T>& operator=(const PyCallback<T>&&) = delete;

        PyCallback<T>() = delete;

        PyCallback<T>(const PyCallback<T>& other) :
                m_func{other.m_func},
                m_wrapped_type{other.m_wrapped_type},
                m_event_args_pytype{other.m_event_args_pytype}
        {
            if (!Py_IsInitialized())
            {
                // Interpreter is dead, don't use the Python API to avoid throwing
                throw std::runtime_error("Cannot acquire python interpreter to copy callback");
            }
            PyGILState_STATE state = PyGILState_Ensure();
            Py_XINCREF(m_func);
            Py_XINCREF(m_event_args_pytype);
            PyGILState_Release(state);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="obj">A Python callable object</param>
        /// <param name="ty">The SWIG type information for the wrapper of the EventArgs-derived
        /// <param name="event_args_pytype">The Python type that wraps the argument of the callback</param>
        PyCallback<T>(PyObject *obj, swig_type_info* ty, PyObject *event_args_pytype) :
                m_func{obj},
                m_wrapped_type{ty},
                m_event_args_pytype{event_args_pytype}
        {
            if (!Py_IsInitialized())
            {
                // Interpreter is dead, don't use the Python API to avoid throwing
                throw std::runtime_error("Cannot acquire python interpreter to create callback");
            }
            PyGILState_STATE state = PyGILState_Ensure();
            Py_XINCREF(m_func);
            Py_XINCREF(m_event_args_pytype);
            assert(PyCallable_Check(m_func));
            PyGILState_Release(state);
        }

        ~PyCallback<T>() {
            if (!Py_IsInitialized())
            {
                // Interpreter is dead, don't use the Python API to avoid throwing
                return;
            }
            PyGILState_STATE state = PyGILState_Ensure();
            Py_XDECREF(m_func);
            Py_XDECREF(m_event_args_pytype);
            PyGILState_Release(state);
        }

        /// <summary>
        /// Converts the arguments of the function call from the SWIG wrapper type to the corresponding
        /// python object and calls the callback with it.
        /// </summary>
        void operator()(const T& evt) {
            if (!Py_IsInitialized())
            {
                // Interpreter is dead, don't use the Python API to avoid throwing
                throw std::runtime_error("Cannot aquire python interpreter to call callback");
            }
            PyGILState_STATE state = PyGILState_Ensure();

            if (!m_func || Py_None == m_func || !PyCallable_Check(m_func)) {
                PyGILState_Release(state);
                return;
            }

            PyObject* obj0 = SWIG_NewPointerObj(SWIG_as_voidptr(&evt), m_wrapped_type, 0);

            // create a new python object of type event_args_pytype wrapping the original eventargs objects
            // obj0
            int typecheck = PyType_Check(m_event_args_pytype);
            if (!typecheck)
            {
                Py_XDECREF(obj0);
                PyGILState_Release(state);
                throw std::runtime_error("Error converting event signal: wrapped type needs to be a type.");
            }

            PyObject* wrap_args = PyTuple_Pack(1, (PyObject *)obj0);
            Py_XDECREF(obj0);
            PyObject* wrap_result = PyObject_CallObject(m_event_args_pytype, (PyObject *)wrap_args);
            Py_XDECREF(wrap_args);

            if (nullptr == wrap_result)
            {
                PyGILState_Release(state);
                throw std::runtime_error("Error occurred when trying to construct the arguments for the callback function.");
            }

            int result_typecheck = PyObject_TypeCheck((PyObject *)wrap_result, (PyTypeObject *)m_event_args_pytype);
            if (!result_typecheck)
            {
                Py_XDECREF(wrap_result);
                PyGILState_Release(state);
                throw std::runtime_error("Error converting event signal: wrapped object has unexpected type.");
            }

            PyObject* args = PyTuple_Pack(1, (PyObject *)wrap_result);
            Py_XDECREF(wrap_result);

            PyObject* result = PyObject_CallObject(m_func, (PyObject *)args);

            Py_XDECREF(args);
            if (nullptr == result)
            {
                PyGILState_Release(state);
                throw std::runtime_error("Error occurred when trying to call the callback function.");
            }

            Py_XDECREF(result);
            PyGILState_Release(state);
        }
};
} } } // namespace
%}


%include "pythondocs.i"
%include "carbon.i"


%define %py_wrap_callback_connect(T)
%extend Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::T&> {
    void Connect(PyObject* func, PyObject* wrapped_type) {
        $self->Connect(std::function<void(const Microsoft::CognitiveServices::Speech::T&)>
            {Microsoft::CognitiveServices::Speech::PyCallback<Microsoft::CognitiveServices::Speech::T>(
                func, SWIGTYPE_p_Microsoft__CognitiveServices__Speech__ ## T, wrapped_type)});
    }
}
%enddef

%define %py_wrap_translation_callback_connect(T)
%extend Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Translation::T&> {
    void Connect(PyObject* func, PyObject* wrapped_type) {
        $self->Connect(std::function<void(const Microsoft::CognitiveServices::Speech::Translation::T&)>
            {Microsoft::CognitiveServices::Speech::PyCallback<Microsoft::CognitiveServices::Speech::Translation::T>(
                func, SWIGTYPE_p_Microsoft__CognitiveServices__Speech__Translation__ ## T, wrapped_type)});
    }
}
%enddef

%define %py_wrap_intent_callback_connect(T)
%extend Microsoft::CognitiveServices::Speech::EventSignal<const Microsoft::CognitiveServices::Speech::Intent::T&> {
    void Connect(PyObject* func, PyObject* wrapped_type) {
        $self->Connect(std::function<void(const Microsoft::CognitiveServices::Speech::Intent::T&)>
            {Microsoft::CognitiveServices::Speech::PyCallback<Microsoft::CognitiveServices::Speech::Intent::T>(
                func, SWIGTYPE_p_Microsoft__CognitiveServices__Speech__Intent__ ## T, wrapped_type)});
    }
}
%enddef

%py_wrap_callback_connect(SessionEventArgs)
%py_wrap_callback_connect(ConnectionEventArgs)
%py_wrap_callback_connect(RecognitionEventArgs)
%py_wrap_callback_connect(SpeechRecognitionEventArgs)
%py_wrap_callback_connect(SpeechRecognitionCanceledEventArgs)
%py_wrap_callback_connect(SpeechSynthesisEventArgs)
%py_wrap_callback_connect(SpeechSynthesisWordBoundaryEventArgs)
%py_wrap_translation_callback_connect(TranslationRecognitionEventArgs)
%py_wrap_translation_callback_connect(TranslationRecognitionCanceledEventArgs)
%py_wrap_translation_callback_connect(TranslationSynthesisEventArgs)
%py_wrap_intent_callback_connect(IntentRecognitionEventArgs)
%py_wrap_intent_callback_connect(IntentRecognitionCanceledEventArgs)
%py_wrap_callback_connect(KeywordRecognitionEventArgs)

// macro to make reason properties return Python enums
%define %py_wrap_reason(BaseName, EnumName, Docstring)
%extend Microsoft::CognitiveServices::Speech:: ## BaseName {
    %pythoncode %{
    @property
    def reason(self) -> "EnumName":
        """
        Docstring
        """
        return EnumName ## (self._reason)
    %}
}
%enddef

%py_wrap_reason(RecognitionResult, ResultReason, The recognition reason)
%py_wrap_reason(CancellationDetails, CancellationReason, The reason the result was canceled.)
%py_wrap_reason(NoMatchDetails, NoMatchReason, The reason for the NoMatch result)
%py_wrap_reason(Translation::TranslationSynthesisResult, ResultReason, The reason for the result)
%py_wrap_reason(SpeechSynthesisResult, ResultReason, The speech synthesis reason)
%py_wrap_reason(SpeechSynthesisCancellationDetails, CancellationReason, The reason the synthesis was canceled)
%py_wrap_reason(KeywordRecognitionResult, ResultReason, The reason for the result)

// macro to make stream status properties return Python enums
%define %py_wrap_status(BaseName, EnumName, Docstring)
%extend Microsoft::CognitiveServices::Speech:: ## BaseName {
    %pythoncode %{
    @property
    def status(self) -> "EnumName":
        """
        Docstring
        """
        return EnumName ## (self.get_status())
    %}
}
%enddef

%py_wrap_status(AudioDataStream, StreamStatus, The audio data stream status)

// PROPERTIES

// macro to wrap read_only class attributes with a getter function into python properties with type annotation and a docstring
%define %py_ro_property(Typename, Propertyname, PropertyType, Docstring, ...)
%extend Microsoft::CognitiveServices::Speech:: ## __VA_ARGS__ ## Typename {
    %pythoncode %{
    @property
    def Propertyname(self) -> "PropertyType":
        """
        Docstring
        """
        return self._get_ ## Propertyname()
        %}
}
%enddef

// macro to wrap read_only class attributes into python properties with type annotation and a docstring
%define %py_ro_property_no_get(Typename, Propertyname, ...)
%extend Microsoft::CognitiveServices::Speech:: ## __VA_ARGS__ ## Typename {
    %pythoncode %{
    __swig_getmethods__["Propertyname"] = _speech_py_impl. ## Typename ## _ ## Propertyname
    if _newclass: Propertyname = property(_speech_py_impl. ## Typename ## _ ## Propertyname ## )%}
}
%enddef

// add properties for class attributes
%py_ro_property_no_get(Recognizer, is_enabled)

%py_ro_property_no_get(RecognitionResult, offset)
%py_ro_property_no_get(RecognitionResult, duration)

// add properties for class attributes with getter function
%py_ro_property(SpeechRecognitionEventArgs, result, SpeechRecognitionResult, Contains the speech recognition result.)
%py_ro_property(IntentRecognitionEventArgs, result, TranslationSynthesisResult, Contains the intent recognition result., Intent::)
%py_ro_property(TranslationRecognitionEventArgs, result, TranslationRecognitionResult, Contains the translation recognition result., Translation::)
%py_ro_property(TranslationSynthesisEventArgs, result, TranslationSynthesisResult, Contains the translation synthesis result., Translation::)
%py_ro_property(KeywordRecognitionEventArgs, result, KeywordRecognitionResult, Contains the keyword recognition result.)

%py_ro_property(TranslationRecognitionCanceledEventArgs, cancellation_details, CancellationDetails, Details about why translation was canceled., Translation::)
%py_ro_property(SpeechRecognitionCanceledEventArgs, cancellation_details, CancellationDetails, Details about why speech recognition was canceled.)
%py_ro_property(IntentRecognitionCanceledEventArgs, cancellation_details, CancellationDetails, Details about why intent recognition was canceled., Intent::)

// convenience accessors for RecognitionResult. We expose the cancellation
// details on the recognition result directly, so they don't have to be
// constructed explicitly.
%extend Microsoft::CognitiveServices::Speech::RecognitionResult {
    %pythoncode %{
    @property
    def json(self) -> str:
        """The bare JSON representation of the result from the Speech Service."""
        return self.properties.get_property(PropertyId.SpeechServiceResponse_JsonResult)
    @property
    def error_json(self) -> str:
        """The bare JSON representation of the error from the Speech Service."""
        return self.properties.get_property(PropertyId.SpeechServiceResponse_JsonErrorDetails)
    @property
    def cancellation_details(self) -> "CancellationDetails":
        """The reason why recognition was cancelled.

        Returns `None` if there was no cancellation."""
        if ResultReason.Canceled == self.reason:
            return CancellationDetails._from_result(self)
    @property
    def no_match_details(self) -> "NoMatchDetails":
        """Detailed information for NoMatch recognition results.

        Returns `None` if there was a match found."""
        if ResultReason.NoMatch == self.reason:
            return NoMatchDetails._from_result(self) %}
}

// convenience accessors for SpeechSynthesisResult. We expose the cancellation
// details on the synthesis result directly, so they don't have to be
// constructed explicitly.
%extend Microsoft::CognitiveServices::Speech::SpeechSynthesisResult {
    %pythoncode %{
    @property
    def cancellation_details(self) -> "SpeechSynthesisCancellationDetails":
        """The reason why speech synthesis was cancelled.

        Returns `None` if there was no cancellation."""
        if ResultReason.Canceled == self.reason:
            return SpeechSynthesisCancellationDetails._from_result(self) %}
}


%extend Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionResult {
    %pythoncode %{
    @property
    def intent_json(self) -> str:
        """The bare JSON representation of the result from the Language Understanding service."""
        return self.properties.get_property(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)
    %}
}

// convert C++ enums to python enums
%pythoncode %{
from enum import Enum
def inject_enum(prefix: str):
    # get swig-generated enum values
    swig_enum_vars = {k: v for k, v in globals().items() if k.startswith(prefix + '_')}
    for k, v in swig_enum_vars.items():
        del globals()[k]
    new_enum_vars = {k[len(prefix) + 1: ]: v for k, v in swig_enum_vars.items()}
    # create python enum and inject into module namespace
    globals()[prefix] = Enum(prefix, new_enum_vars)
    globals()[prefix].__doc__ = globals()['DocstringFor' + prefix]
    del globals()['DocstringFor' + prefix]

inject_enum('CancellationReason')
inject_enum('CancellationErrorCode')
inject_enum('NoMatchReason')
inject_enum('OutputFormat')
inject_enum('PropertyId')
inject_enum('ResultReason')
inject_enum('ServicePropertyChannel')
inject_enum('ProfanityOption')
inject_enum('StreamStatus')
inject_enum('SpeechSynthesisOutputFormat')
inject_enum('AudioStreamContainerFormat')

# clean up the exported names
del inject_enum
del Enum
%}

// clean up internal variables exported by the module
//   - remove exported variables that end with _swigregister
//   - remove exported factory functions that are replaced with explicit constructors in python
%pythoncode %{
swigregister_class_names = [variable_name for variable_name in globals() if variable_name.endswith('_swigregister')]
from_factory_method_names = [variable_name for variable_name in globals() if '__from' in variable_name.lower()]
for variable_name in swigregister_class_names + from_factory_method_names:
    del globals()[variable_name]

del from_factory_method_names, swigregister_class_names, variable_name
%}