%module(directors="1", "threads"=1) carbon_py

// rename all functions/methods/properties to match Python's naming convention 
// https://www.python.org/dev/peps/pep-0008/#method-names-and-instance-variables
%rename("%(undercase)s", %$isfunction) "";
%rename("%(undercase)s", %$ismember) "";

%feature("nothreadallow");

// Release GIL:
// * when invoking a blocking Recognize.
%threadallow Carbon::Recognition::Speech::SpeechRecognizer::Recognize;
// * when invoking SpeechRecognizer dtor (otherwise there might be a deadlock if a 
//   callback is concurrently invoked from the speech client.)
%threadallow Carbon::Recognition::Speech::SpeechRecognizer::~SpeechRecognizer;
// * when waiting on a future.
%threadallow FutureWrapper::Get;

%extend Carbon::Recognition::RecognitionResult {
    %pythoncode %{
    def __str__(self):
        return '{}:(result_id={}, reason={}, text={})'.format(type(self), self.result_id, self.reason, self.text)
    %}
}

%extend Carbon::SessionEventArgs {
    %pythoncode %{
    def __str__(self):
        return '{}:(session_id={})'.format(type(self), self.session_id)
    %}
}

%extend Carbon::Recognition::Speech::SpeechRecognitionEventArgs {
    %pythoncode %{
    def __str__(self):
        return '{}:(session_id={}, result={})'.format(type(self), self.session_id, self.result)
    %}
}

%rename(_RecognizerFactory)Carbon::Recognition::RecognizerFactory;

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

%pythoncode %{
class RecognizerFactory:
    @staticmethod
    def create_speech_recognizer(filename=None):
        if not filename:
            return _RecognizerFactory.create_speech_recognizer()
        return _RecognizerFactory.create_speech_recognizer_with_file_input(filename)
%}


%py_wrap_callbacks(SessionEvent)
%py_wrap_callbacks(RecognitionEvent)
%py_wrap_callbacks(SpeechRecognitionEvent)
%py_wrap_callbacks(IntentEvent)