import traceback
import sys
import time
import pytest
import azure.cognitiveservices.speech as msspeech

# the timeout to wait for session stopped event after recognition is finished
_TIMEOUT_IN_SECONDS = 10.

class _TestCallback(object):
    """helper class that keeps track of how often the callback has been called, and performs checks
    on the callback arguments"""

    def __init__(self, msg, event_checks=None, fn=None):
        self._msg = msg
        self.num_calls = 0
        self._events = []
        self._event_checks = event_checks or (lambda x: None)
        self._fn = fn

    def __call__(self, evt):
        self.num_calls += 1
        try:
            print(self._msg.format(evt=evt))
        except Exception:
            try:
                exc_info = sys.exc_info()
                excstring = traceback.format_exception(*exc_info)
                print('Generic Error with {}: {}'.format(evt, excstring))
            except Exception:
                print('error formatting exception')

        if self._fn:
            self._fn()

        exc_info = None
        try:
            self.check_event(evt)
        except AssertionError:
            try:
                exc_info = sys.exc_info()
                excstring = traceback.format_exception(*exc_info)
                print('Assertion Error with {}: {}'.format(evt, excstring))
            except Exception:
                print('error formatting exception')
        except Exception:
            try:
                exc_info = sys.exc_info()
                excstring = traceback.format_exception(*exc_info)
                print('Generic Error with {}: {}'.format(evt, excstring))
            except Exception:
                print('error formatting exception')

        self._events.append((evt, exc_info))

    def check_event(self, evt):
        self._event_checks(evt)

    @property
    def events(self):
        return self._events

    def reset(self):
        self.num_calls = 0
        self._events = []


def _setup_callbacks():
    callbacks = {
            'session_started': _TestCallback('SESSION_STARTED: {evt}'),
            'session_stopped': _TestCallback('SESSION_STOPPED: {evt}'),
            'canceled': _TestCallback('CANCELED: {evt}'),
            'recognizing': _TestCallback('RECOGNIZING: {evt}'),
            'recognized': _TestCallback('RECOGNIZED: {evt}'),
    }
    return callbacks


def _check_callbacks(callbacks):
    start = time.time()
    while callbacks['session_stopped'].num_calls == 0:
        if time.time() - start > _TIMEOUT_IN_SECONDS:
            pytest.fail("Waiting for SessionStopped timed out, giving up.")
            break
        time.sleep(1.)

    assert callbacks['session_started'].num_calls == 1
    assert callbacks['session_stopped'].num_calls == 1
    assert callbacks['recognizing'].num_calls >= 1
    assert callbacks['recognized'].num_calls == 1


def _check_sr_result(result, speech_input, utterance_index):
    assert isinstance(result, msspeech.SpeechRecognitionResult)
    assert speech_input.transcription[utterance_index] == result.text
    assert msspeech.ResultReason.RecognizedSpeech == result.reason
    assert speech_input.duration[utterance_index] == result.duration
    assert speech_input.offset[utterance_index] == result.offset
    assert isinstance(result.properties, msspeech.PropertyCollection)
    assert isinstance(result.result_id, str)
    assert result.result_id
    assert result.cancellation_details is None


def _connect_all_callbacks(reco, callbacks):
    for cb_name, callback in callbacks.items():
        callback.reset()
        getattr(reco, cb_name).connect(callback)


