from functools import partial
import pytest

from .utils import (_TestCallback, _check_callbacks, _check_sr_result, _setup_callbacks, _wait_for_event)

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_connection_with_recognize_once(from_file_speech_reco_with_callbacks, speech_input):
    reco, callbacks = from_file_speech_reco_with_callbacks(
            partial(_setup_callbacks, setup_stop_callbacks=False))

    # setup connection
    connection = reco.connection

    connected_callback = _TestCallback('CONNECTED: {evt}')
    disconnected_callback = _TestCallback('DISCONNECTED: {evt}')
    connection.connected.connect(connected_callback)
    connection.disconnected.connect(disconnected_callback)
    callbacks.update({'connected': connected_callback,
        'disconnected': disconnected_callback})

    # open the connection
    connection.open(for_continuous_recognition=False)
    # wait for connection
    _wait_for_event(callbacks, 'connected')

    result = reco.recognize_once()

    # close the connection
    connection.close()
    _wait_for_event(callbacks, 'disconnected')

    _check_sr_result(result, speech_input, 0)
    _wait_for_event(callbacks, 'session_stopped')
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
    callbacks.update({'connected': connected_callback,
        'disconnected': disconnected_callback})

    # open the connection
    connection.open(for_continuous_recognition=True)
    # wait for connection
    _wait_for_event(callbacks, 'connected')

    reco.start_continuous_recognition()

    # wait for session stopped event and check callbacks
    _wait_for_event(callbacks, 'session_stopped')
    _check_callbacks(callbacks)

    recognized_events = [evt for (evt, _) in callbacks['recognized'].events]
    assert 1 == len(recognized_events)
    recognized_result = recognized_events[-1].result
    _check_sr_result(recognized_result, speech_input, 0)

    # close the connection
    connection.close()
    _wait_for_event(callbacks, 'disconnected')

