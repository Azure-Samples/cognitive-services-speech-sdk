import pytest
import time

import azure.cognitiveservices.speech as msspeech

from .utils import (_TestCallback, _check_callbacks, _check_sr_result, _TIMEOUT_IN_SECONDS)


speech_config_types = (msspeech.SpeechConfig, msspeech.translation.SpeechTranslationConfig)
recognizer_types = (msspeech.SpeechRecognizer, msspeech.translation.TranslationRecognizer,
                    msspeech.intent.IntentRecognizer)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_connection_with_recognize_once(from_file_speech_reco_with_callbacks, speech_input):
    reco, callbacks = from_file_speech_reco_with_callbacks(setup_stop_callback=False)

    # setup connection
    connection = reco.connection
    connected_callback = _TestCallback('CONNECTED: {evt}')
    disconnected_callback = _TestCallback('DISCONNECTED: {evt}')
    connection.connected.connect(connected_callback)
    connection.disconnected.connect(disconnected_callback)

    # open the connection
    connection.open(for_continuous_recognition=False)
    # wait for connection
    start = time.time()
    while connected_callback.num_calls == 0:
        if time.time() - start > _TIMEOUT_IN_SECONDS:
            pytest.fail("Waiting for Connected timed out, giving up.")
            break
        time.sleep(1.)

    result = reco.recognize_once()

    # close the connection
    connection.close()
    while disconnected_callback.num_calls == 0:
        if time.time() - start > _TIMEOUT_IN_SECONDS:
            pytest.fail("Waiting for Disconnected timed out, giving up.")
            break
        time.sleep(1.)

    _check_sr_result(result, speech_input, 0)
    _check_callbacks(callbacks)


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_connection_with_continuous_recognition(from_file_speech_reco_with_callbacks, speech_input):
    reco, callbacks = from_file_speech_reco_with_callbacks()

    # setup connection
    connection = reco.connection
    connected_callback = _TestCallback('CONNECTED: {evt}')
    disconnected_callback = _TestCallback('DISCONNECTED: {evt}')
    connection.connected.connect(connected_callback)
    connection.disconnected.connect(disconnected_callback)

    # open the connection
    connection.open(for_continuous_recognition=True)
    # wait for connection
    start = time.time()
    while connected_callback.num_calls == 0:
        if time.time() - start > _TIMEOUT_IN_SECONDS:
            pytest.fail("Waiting for Connected timed out, giving up.")
            break
        time.sleep(1.)

    reco.start_continuous_recognition()

    # wait for session stopped event and check callbacks
    _check_callbacks(callbacks)

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result
    _check_sr_result(recognized_result, speech_input, 0)

    # close the connection
    connection.close()
    while disconnected_callback.num_calls == 0:
        if time.time() - start > _TIMEOUT_IN_SECONDS:
            pytest.fail("Waiting for Disconnected timed out, giving up.")
            break
        time.sleep(1.)

