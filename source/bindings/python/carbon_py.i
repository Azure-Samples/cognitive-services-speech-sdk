%module(directors="1", "threads"=1) carbon_py

// rename all functions/methods/properties to match Python's naming convention 
// https://www.python.org/dev/peps/pep-0008/#method-names-and-instance-variables
%rename("%(undercase)s", %$isfunction) "";
%rename("%(undercase)s", %$ismember) "";

%feature("nothreadallow");

// Release GIL:
// * when invoking a blocking Recognize.
%threadallow Microsoft::CognitiveServices::Speech::SpeechRecognizer::Recognize;
// * when invoking SpeechRecognizer dtor (otherwise there might be a deadlock if a 
//   callback is concurrently invoked from the speech client.)
%threadallow Microsoft::CognitiveServices::Speech::SpeechRecognizer::~SpeechRecognizer;
// * when waiting on a future.
%threadallow FutureWrapper::Get;
// * when invoking disconnect (which involves acquiring an event signal lock) to
// avoid a deadlock (in-progress callback waiting for GIL might be holding 
// the event signal lock).
%threadallow Microsoft::CognitiveServices::Speech::EventSignal::_Disconnect;
%threadallow Microsoft::CognitiveServices::Speech::EventSignal::DisconnectAll;

%extend Microsoft::CognitiveServices::Speech::Recognition::RecognitionResult {
    %pythoncode %{
    def __str__(self):
        return u'{}:(result_id={}, reason={}, text={})'.format(type(self), self.result_id, self.reason, self.text)
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
        return u'{}:(session_id={}, result={})'.format(type(self), self.session_id, self.get_result())
    %}
}

%include "carbon.i"

// TODO (#1137487): monkey patch event signals to keep the callbacks alive, 
// until they are disconnected. 
%define %py_wrap_callbacks(T)
%pythoncode %{
def _generate_signal_methods(callback_wrapper):
    def check_type(callback):
        if not isinstance(callback, callback_wrapper):
            raise ValueError('Unexpected callback type (%s) in place of (%s).'
                         % type(callback), callback_wrapper.__class__.__name__)

    def connect(event_signal, callback):
        check_type(callback)
        event_signal._connect(callback)

    def disconnect(event_signal, callback):
        check_type(callback)
        event_signal._disconnect(callback)

    return connect, disconnect;

class T ## Callback(_ ## T ## Callback):
    def __init__(self, callback):
        super(T ## Callback, self).__init__()
        self.callback = callback

    def __call__(self, event_args):
        self.callback(event_args)

T ## Signal.connect, T ## Signal.disconnect = _generate_signal_methods(T ## Callback)
%}
%enddef


%extend Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory {
    %pythoncode %{
    def create_speech_recognizer(self, filename=None):
        if not filename:
            return self.create_speech_recognizer()

        audioInput = AudioConfig.from_wav_file_input(filename)
        return self.create_speech_recognizer_from_config(audioInput)
    %}
}

%extend Microsoft::CognitiveServices::Speech::RecognitionResult {
    %pythoncode %{
    @property
    def json(self):
        return self.properties.get_property(SpeechPropertyId_SpeechServiceResponse_Json)
    %}
}


%py_wrap_callbacks(SessionEvent)
%py_wrap_callbacks(RecognitionEvent)
%py_wrap_callbacks(SpeechRecognitionEvent)
%py_wrap_callbacks(IntentEvent)
