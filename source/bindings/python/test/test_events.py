# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import itertools
import pytest
from typing import Union

import azure.cognitiveservices.speech as msspeech

from .utils import (_TestCallback, _check_sr_result, _check_translation_result,
        _check_intent_result, _check_callbacks, _wait_for_event, _check_result_properties)
from .conftest import SpeechInput, IntentInput


def session_checks(evt: msspeech.SessionEventArgs):
    assert isinstance(evt, msspeech.SessionEventArgs)
    assert evt.session_id
    if type(evt) is msspeech.SessionEventArgs:
        assert str(evt) == 'SessionEventArgs(session_id={})'.format(evt.session_id)


def recognition_checks(evt: msspeech.RecognitionEventArgs):
    session_checks(evt)
    assert isinstance(evt, msspeech.RecognitionEventArgs)
    assert isinstance(evt.offset, int)
    assert evt.offset >= 0
    if type(evt) is msspeech.RecognitionEventArgs:
        assert str(evt) == 'RecognitionEventArgs(session_id={})'.format(evt.session_id)


def speech_recognition_checks(evt: msspeech.SpeechRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.SpeechRecognitionEventArgs)
    assert evt.result
    if type(evt) is msspeech.SpeechRecognitionEventArgs:
        assert str(evt) == 'SpeechRecognitionEventArgs(session_id={}, result={})'.format(
                evt.session_id, evt.result)

    _check_result_properties(evt.result)


def speech_recognition_canceled_checks(evt: msspeech.SpeechRecognitionCanceledEventArgs):
    speech_recognition_checks(evt)
    assert isinstance(evt, msspeech.SpeechRecognitionCanceledEventArgs)
    assert evt.cancellation_details
    assert isinstance(evt.cancellation_details, msspeech.CancellationDetails)
    assert str(evt) == 'SpeechRecognitionCanceledEventArgs(session_id={}, result={})'.format(
            evt.session_id, evt.result)


def intent_recognition_checks(evt: msspeech.intent.IntentRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.intent.IntentRecognitionEventArgs)
    assert evt.result
    if type(evt) is msspeech.intent.IntentRecognitionEventArgs:
        assert str(evt) == 'IntentRecognitionEventArgs(session_id={}, result={})'.format(
                evt.session_id, evt.result)

    _check_result_properties(evt.result)


def intent_recognition_canceled_checks(evt: msspeech.intent.IntentRecognitionCanceledEventArgs):
    intent_recognition_checks(evt)
    assert isinstance(evt, msspeech.intent.IntentRecognitionCanceledEventArgs)
    assert evt.cancellation_details
    assert str(evt) == 'IntentRecognitionCanceledEventArgs(session_id={}, result={})'.format(
            evt.session_id, evt.result)


def translation_recognition_checks(evt: msspeech.translation.TranslationRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.translation.TranslationRecognitionEventArgs)
    assert evt.result
    if type(evt) is msspeech.translation.TranslationRecognitionEventArgs:
        assert str(evt) == 'TranslationRecognitionEventArgs(session_id={}, result={})'.format(
                evt.session_id, evt.result)

    _check_result_properties(evt.result)


def translation_synthesis_checks(evt: msspeech.translation.TranslationSynthesisEventArgs):
    session_checks(evt)
    assert isinstance(evt, msspeech.translation.TranslationSynthesisEventArgs)
    assert evt.result
    assert isinstance(evt.result, msspeech.translation.TranslationSynthesisResult)
    if evt.result.reason == msspeech.ResultReason.SynthesizingAudio:
        assert evt.result.audio
        assert isinstance(evt.result.audio, bytes)
    elif evt.result.reason == msspeech.ResultReason.SynthesizingAudioCompleted:
        assert not evt.result.audio
    else:
        raise ValueError('unexpected reason: {}'.format(evt.result.reason))

    assert str(evt.result) == 'TranslationSynthesisResult(audio=<{} bytes of audio>, reason={})'.format(
            len(evt.result.audio), evt.result.reason)
    assert str(evt) == 'TranslationSynthesisEventArgs(session_id={}, result={})'.format(
            evt.session_id, evt.result)


def translation_recognition_canceled_checks(evt: msspeech.translation.TranslationRecognitionCanceledEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.translation.TranslationRecognitionCanceledEventArgs)
    assert evt.cancellation_details
    assert str(evt) == 'TranslationRecognitionCanceledEventArgs(session_id={}, result={})'.format(
            evt.session_id, evt.result)


def bad_callback_check(_):
    """used to check that the re-raising of exceptions works"""
    assert False


def _check_bad_callback(callbacks):
    """
    Verify that assertions raised in an event are registered during evaluation.
    Removes the 'bad_callback' used for checking from `callbacks` when done.
    """
    bad_callback = callbacks['bad_callback']
    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])

    del callbacks['bad_callback']


def _setup_callbacks_for_event_check(reco: Union[msspeech.SpeechRecognizer,
        msspeech.translation.TranslationRecognizer, msspeech.intent.IntentRecognizer]):
    callbacks = {
        'session_started': _TestCallback("SESSION STARTED: {evt}", session_checks),
        'session_stopped': _TestCallback("SESSION STOPPED: {evt}", session_checks),
        'speech_start_detected': _TestCallback("SPEECH START DETECTED: {evt}", recognition_checks),
        'speech_end_detected': _TestCallback("SPEECH END DETECTED: {evt}", recognition_checks),
    }

    if isinstance(reco, msspeech.intent.IntentRecognizer):
        callbacks['recognized'] = _TestCallback("RECOGNIZED: {evt}", intent_recognition_checks)
        callbacks['recognizing'] = _TestCallback("RECOGNIZING: {evt}", intent_recognition_checks)
        callbacks['canceled'] = _TestCallback("CANCELED: {evt}", intent_recognition_canceled_checks)
    elif isinstance(reco, msspeech.translation.TranslationRecognizer):
        callbacks['recognized'] = _TestCallback("RECOGNIZED: {evt}", translation_recognition_checks)
        callbacks['recognizing'] = _TestCallback("RECOGNIZING: {evt}", translation_recognition_checks)
        callbacks['canceled'] = _TestCallback("CANCELED: {evt}", translation_recognition_canceled_checks)
        callbacks['synthesizing'] = _TestCallback("SYNTHESIZING: {evt}",
            translation_synthesis_checks)
    elif isinstance(reco, msspeech.SpeechRecognizer):
        callbacks['recognized'] = _TestCallback("RECOGNIZED: {evt}", speech_recognition_checks)
        callbacks['recognizing'] = _TestCallback("RECOGNIZING: {evt}", speech_recognition_checks)
        callbacks['canceled'] = _TestCallback("CANCELED: {evt}", speech_recognition_canceled_checks)
    else:
        raise TypeError('unknown recognizer type: {}'.format(type(reco)))

    for label, cb in callbacks.items():
        getattr(reco, label).connect(cb)

    def stop(evt):
        print('STOPPING: {}'.format(evt))
        reco.stop_continuous_recognition()

    reco.session_stopped.connect(stop)
    reco.canceled.connect(stop)

    bad_callback = _TestCallback(None, bad_callback_check, quiet=True)
    reco.recognizing.connect(bad_callback)
    callbacks['bad_callback'] = bad_callback

    return callbacks


def _check_events(callbacks):
    """re-raise exceptions that happened when processing the callbacks"""
    all_events = itertools.chain(*[x.events for x in callbacks.values()])
    for evt, exc_info in all_events:
        if exc_info:
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_events(speech_input: SpeechInput, from_file_speech_reco_with_callbacks):
    reco, callbacks = from_file_speech_reco_with_callbacks(
            setup_callback_handle=_setup_callbacks_for_event_check)

    reco.start_continuous_recognition()

    _wait_for_event(callbacks, 'session_stopped')
    _check_bad_callback(callbacks)
    _check_callbacks(callbacks, check_num_recognized=False)
    _check_events(callbacks)

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events if
            evt.result.reason == msspeech.ResultReason.RecognizedSpeech]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result

    _check_sr_result(recognized_result, speech_input, 0)


# @pytest.mark.parametrize('intent_input,', ['lamp'], indirect=True)
# def test_intent_recognition_events(intent_input: IntentInput, from_file_intent_reco_with_callbacks):
#     intent_recognizer, callbacks = from_file_intent_reco_with_callbacks(
#             setup_callback_handle=_setup_callbacks_for_event_check)

#     intent_recognizer.start_continuous_recognition()

#     _wait_for_event(callbacks, 'session_stopped')
#     _check_bad_callback(callbacks)
#     _check_callbacks(callbacks, check_num_recognized=False)
#     _check_events(callbacks)

#     # TODO remove workaround once intent is fixed and delivers only a single valid result
#     # recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
#     recognized_events = [evt for (evt, _) in callbacks['recognized'].events if
#             evt.result.reason == msspeech.ResultReason.RecognizedIntent]

#     assert 1 == len(recognized_events)
#     recognized_result = recognized_events[-1].result

#     _check_intent_result(recognized_result, intent_input, 0)

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_recognition_events(speech_input: SpeechInput,
        from_file_translation_reco_with_callbacks):
    translation_recognizer, callbacks = from_file_translation_reco_with_callbacks(
            setup_callback_handle=_setup_callbacks_for_event_check)

    translation_recognizer.start_continuous_recognition()

    _wait_for_event(callbacks, 'session_stopped')
    _check_bad_callback(callbacks)
    _check_callbacks(callbacks)
    _check_events(callbacks)

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result

    _check_translation_result(recognized_result, speech_input, 0, ['de', 'fr'])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_canceled_events(speech_input: SpeechInput,
        from_file_speech_reco_with_callbacks):
    reco, callbacks = from_file_speech_reco_with_callbacks(
            setup_callback_handle=_setup_callbacks_for_event_check, endpoint='invalid',
            subscription='invalid', speech_region=None)

    reco.start_continuous_recognition()

    _wait_for_event(callbacks, 'canceled')
    _check_events(callbacks)
    assert callbacks['session_started'].num_calls == 1
    assert callbacks['canceled'].num_calls == 1


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_recognition_canceled_events(speech_input: SpeechInput,
        from_file_translation_reco_with_callbacks):
    translation_recognizer, callbacks = from_file_translation_reco_with_callbacks(
            setup_callback_handle=_setup_callbacks_for_event_check, endpoint='invalid',
            subscription='invalid', speech_region=None)

    translation_recognizer.start_continuous_recognition()

    _wait_for_event(callbacks, 'canceled')
    _check_events(callbacks)
    assert callbacks['session_started'].num_calls == 1
    assert callbacks['canceled'].num_calls == 1


@pytest.mark.parametrize('intent_input,', ['lamp'], indirect=True)
def test_intent_recognition_canceled_events(intent_input: IntentInput,
        from_file_intent_reco_with_callbacks):
    intent_recognizer, callbacks = from_file_intent_reco_with_callbacks(
            setup_callback_handle=_setup_callbacks_for_event_check, endpoint='invalid',
            subscription='invalid', luis_region=None)

    intent_recognizer.start_continuous_recognition()

    _wait_for_event(callbacks, 'canceled')
    _check_events(callbacks)
    assert callbacks['session_started'].num_calls == 1
    assert callbacks['canceled'].num_calls == 1

