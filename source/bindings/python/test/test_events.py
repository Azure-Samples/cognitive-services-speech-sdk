import itertools
import pytest
from typing import Union

import azure.cognitiveservices.speech as msspeech

from .utils import (_TestCallback, _check_sr_result, _check_translation_result,
        _check_intent_result, _check_callbacks)
from .conftest import SpeechInput


def session_checks(evt: msspeech.SessionEventArgs):
    assert isinstance(evt, msspeech.SessionEventArgs)
    assert evt.session_id


def recognition_checks(evt: msspeech.RecognitionEventArgs):
    session_checks(evt)
    assert isinstance(evt, msspeech.RecognitionEventArgs)
    assert isinstance(evt.offset, int)
    assert evt.offset >= 0


def speech_recognition_checks(evt: msspeech.SpeechRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.SpeechRecognitionEventArgs)
    assert evt.result


def speech_recognition_canceled_checks(evt: msspeech.SpeechRecognitionCanceledEventArgs):
    speech_recognition_checks(evt)
    assert isinstance(evt, msspeech.SpeechRecognitionCanceledEventArgs)
    assert evt.cancellation_details
    assert isinstance(evt.cancellation_details, msspeech.CancellationDetails)


def intent_recognition_checks(evt: msspeech.intent.IntentRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.intent.IntentRecognitionEventArgs)
    assert evt.result


def intent_recognition_canceled_checks(evt: msspeech.intent.IntentRecognitionCanceledEventArgs):
    intent_recognition_checks(evt)
    assert isinstance(evt, msspeech.intent.IntentRecognitionCanceledEventArgs)
    assert evt.cancellation_details


def translation_recognition_checks(evt: msspeech.translation.TranslationRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.translation.TranslationRecognitionEventArgs)
    assert evt.result


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


def translation_recognition_canceled_checks(evt: msspeech.translation.TranslationRecognitionCanceledEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.translation.TranslationRecognitionCanceledEventArgs)
    assert evt.cancellation_details


def bad_callback_check(_):
    """used to check that the re-raising of exceptions works"""
    assert False


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

    return callbacks


def _check_events(callbacks):
    """re-raise exceptions that happened when processing the callbacks"""
    all_events = itertools.chain(*[x.events for x in callbacks.values()])
    for evt, exc_info in all_events:
        if exc_info:
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_events(speech_input: SpeechInput, subscription: str, speech_region: str):
    audio_cfg = msspeech.audio.AudioConfig(filename=speech_input.path)
    cfg = msspeech.SpeechConfig(subscription=subscription, region=speech_region)
    reco = msspeech.SpeechRecognizer(cfg, audio_cfg)

    callbacks = _setup_callbacks_for_event_check(reco)

    bad_callback = _TestCallback(None, bad_callback_check, quiet=True)
    reco.recognizing.connect(bad_callback)

    reco.start_continuous_recognition()

    _check_callbacks(callbacks)
    _check_events(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result

    _check_sr_result(recognized_result, speech_input, 0)


@pytest.mark.parametrize('intent_input,', ['lamp'], indirect=True)
def test_intent_recognition_events(intent_input: SpeechInput, luis_subscription: str,
        luis_region: str, language_understanding_app_id: str):
    audio_config = msspeech.audio.AudioConfig(filename=intent_input.path)
    intent_config = msspeech.SpeechConfig(subscription=luis_subscription,
            region=luis_region)

    intent_recognizer = msspeech.intent.IntentRecognizer(intent_config, audio_config)

    model = msspeech.intent.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intent_recognizer.add_intent(model, "HomeAutomation.TurnOn")
    intent_recognizer.add_intent(model, "HomeAutomation.TurnOff")

    intent_recognizer.add_intent("This is a test.", "test")
    intent_recognizer.add_intent("Switch the to channel 34.", "34")
    intent_recognizer.add_intent("what's the weather like", "weather")

    callbacks = _setup_callbacks_for_event_check(intent_recognizer)

    bad_callback = _TestCallback(None, bad_callback_check, quiet=True)
    intent_recognizer.recognizing.connect(bad_callback)

    intent_recognizer.start_continuous_recognition()

    _check_callbacks(callbacks, check_num_recognized=False)
    _check_events(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])

    # TODO remove workaround once intent is fixed and delivers only a single valid result
    # recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    recognized_events = [evt for (evt, _) in callbacks['recognized'].events if
            evt.result.reason == msspeech.ResultReason.RecognizedIntent]

    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result

    _check_intent_result(recognized_result, intent_input, 0)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_recognition_events(speech_input: SpeechInput, subscription: str,
        speech_region: str):
    translation_config = msspeech.translation.SpeechTranslationConfig(subscription=subscription,
            region=speech_region)
    translation_config.speech_recognition_language = 'en-US'
    translation_config.add_target_language('de')
    translation_config.voice_name = "de-DE-Hedda"
    translation_config.add_target_language('fr')
    audio_config = msspeech.audio.AudioConfig(filename=speech_input.path)

    translation_recognizer = msspeech.translation.TranslationRecognizer(translation_config, audio_config)

    callbacks = _setup_callbacks_for_event_check(translation_recognizer)

    bad_callback = _TestCallback(None, bad_callback_check, quiet=True)
    translation_recognizer.recognizing.connect(bad_callback)

    translation_recognizer.start_continuous_recognition()

    _check_callbacks(callbacks)
    _check_events(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result

    _check_translation_result(recognized_result, speech_input, 0, ['de', 'fr'])

