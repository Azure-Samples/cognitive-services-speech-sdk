import itertools
import time
import pytest
from typing import Union

import azure.cognitiveservices.speech as msspeech

from .utils import _TestCallback
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


def intent_recognition_checks(evt: msspeech.IntentRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.IntentRecognitionEventArgs)
    assert evt.result


def intent_recognition_canceled_checks(evt: msspeech.IntentRecognitionCanceledEventArgs):
    intent_recognition_checks(evt)
    assert isinstance(evt, msspeech.IntentRecognitionCanceledEventArgs)
    assert evt.cancellation_details


def translation_recognition_checks(evt: msspeech.TranslationRecognitionEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.TranslationRecognitionEventArgs)
    assert evt.result


def translation_synthesis_checks(evt: msspeech.TranslationSynthesisEventArgs):
    session_checks(evt)
    assert isinstance(evt, msspeech.TranslationSynthesisEventArgs)
    assert evt.result
    assert isinstance(evt.result, msspeech.TranslationSynthesisResult)
    if evt.result.reason == msspeech.ResultReason.SynthesizingAudio:
        assert evt.result.audio
        assert isinstance(evt.result.audio, bytes)
    elif evt.result.reason == msspeech.ResultReason.SynthesizingAudioCompleted:
        assert not evt.result.audio
    else:
        raise ValueError('unexpected reason: {}'.format(evt.result.reason))


def translation_recognition_canceled_checks(evt: msspeech.TranslationRecognitionCanceledEventArgs):
    recognition_checks(evt)
    assert isinstance(evt, msspeech.TranslationRecognitionCanceledEventArgs)
    assert evt.cancellation_details


def bad_callback_check(_):
    """used to check that the re-raising of exceptions works"""
    assert False


def _setup_callbacks(reco: Union[msspeech.SpeechRecognizer, msspeech.TranslationRecognizer,
        msspeech.IntentRecognizer]):
    callbacks = {}
    callbacks['session_started'] = _TestCallback("session started: {evt}", session_checks)
    callbacks['session_stopped'] = _TestCallback("session stopped: {evt}", session_checks)
    callbacks['speech_start_detected'] = _TestCallback("speech start detected: {evt}", recognition_checks)
    callbacks['speech_end_detected'] = _TestCallback("speech end detected: {evt}", recognition_checks)

    if isinstance(reco, msspeech.IntentRecognizer):
        callbacks['recognized'] = _TestCallback("recognized: {evt}", intent_recognition_checks)
        callbacks['recognizing'] = _TestCallback("recognizing: {evt}", intent_recognition_checks)
        callbacks['canceled'] = _TestCallback("canceled: {evt}", intent_recognition_canceled_checks)
    elif isinstance(reco, msspeech.TranslationRecognizer):
        callbacks['recognized'] = _TestCallback("recognized: {evt}", translation_recognition_checks)
        callbacks['recognizing'] = _TestCallback("recognizing: {evt}", translation_recognition_checks)
        callbacks['canceled'] = _TestCallback("canceled: {evt}", translation_recognition_canceled_checks)
        callbacks['synthesizing'] = _TestCallback("synthesizing: {evt}",
            translation_synthesis_checks)
    elif isinstance(reco, msspeech.SpeechRecognizer):
        callbacks['recognized'] = _TestCallback("recognized: {evt}", speech_recognition_checks)
        callbacks['recognizing'] = _TestCallback("recognizing: {evt}", speech_recognition_checks)
        callbacks['canceled'] = _TestCallback("canceled: {evt}", speech_recognition_canceled_checks)
    else:
        raise TypeError('unknown recognizer type: {}'.format(type(reco)))

    for label, cb in callbacks.items():
        getattr(reco, label).connect(cb)

    return callbacks


def _check_callbacks(callbacks):
    """re-raise exceptions that happened when processing the callbacks"""
    all_events = itertools.chain(*[x.events for x in callbacks.values()])
    for evt, exc_info in all_events:
        if exc_info:
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_speech_recognition_events(speech_input: SpeechInput, subscription: str, speech_region: str):
    audio_cfg = msspeech.AudioConfig(filename=speech_input.path)
    cfg = msspeech.SpeechConfig(subscription=subscription, region=speech_region)
    reco = msspeech.SpeechRecognizer(cfg, audio_cfg)

    callbacks = _setup_callbacks(reco)

    bad_callback = _TestCallback("bad", bad_callback_check)
    reco.recognizing.connect(bad_callback)

    reco.start_continuous_recognition()
    time.sleep(5)
    reco.stop_continuous_recognition()
    time.sleep(1)

    _check_callbacks(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_intent_recognition_events(speech_input: SpeechInput, luis_subscription: str,
        luis_region: str, language_understanding_app_id: str):
    audio_config = msspeech.AudioConfig(filename=speech_input.path)
    intent_config = msspeech.SpeechConfig(subscription=luis_subscription,
            region=luis_region)

    intent_recognizer = msspeech.IntentRecognizer(intent_config, audio_config)

    model = msspeech.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intent_recognizer.add_intent(model, "TV.ChangeChannel")
    intent_recognizer.add_intent(model, "TV.WatchTV")
    intent_recognizer.add_intent(model, "TV.ShowGuide")

    intent_recognizer.add_intent("This is a test.", "test")
    intent_recognizer.add_intent("Switch the to channel 34.", "34")
    intent_recognizer.add_intent("what's the weather like", "weather")

    callbacks = _setup_callbacks(intent_recognizer)

    bad_callback = _TestCallback("bad", bad_callback_check)
    intent_recognizer.recognizing.connect(bad_callback)

    intent_recognizer.start_continuous_recognition()
    time.sleep(5)
    intent_recognizer.stop_continuous_recognition()
    time.sleep(1)

    _check_callbacks(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_translation_recognition_events(speech_input: SpeechInput, subscription: str,
        speech_region: str):
    translation_config = msspeech.SpeechTranslationConfig(subscription=subscription,
            region=speech_region)
    translation_config.speech_recognition_language = 'en-US'
    translation_config.add_target_language('de')
    translation_config.voice_name = "de-DE-Hedda"
    translation_config.add_target_language('fr')
    audio_config = msspeech.AudioConfig(filename=speech_input.path)

    translation_recognizer = msspeech.TranslationRecognizer(translation_config, audio_config)

    callbacks = _setup_callbacks(translation_recognizer)

    bad_callback = _TestCallback("bad", bad_callback_check)
    translation_recognizer.recognizing.connect(bad_callback)

    translation_recognizer.start_continuous_recognition()
    time.sleep(5)
    translation_recognizer.stop_continuous_recognition()
    time.sleep(1)

    _check_callbacks(callbacks)

    assert bad_callback.events
    for evt, exc_info in bad_callback.events:
        assert exc_info
        with pytest.raises(AssertionError):
            raise exc_info[0].with_traceback(exc_info[1], exc_info[2])

