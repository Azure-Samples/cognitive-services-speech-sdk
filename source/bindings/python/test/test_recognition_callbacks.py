# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

"""
This test checks that recognition functions, even if arguments passed to callback functions cannot
be constructed. To create this situation, the constructor of RecognitionResult is monkeypatched to
raise an exception, which still has to be handled correctly by the mechanism executing the
callback.
"""
import pytest
from .utils import _wait_for_event
from unittest.mock import patch

import azure.cognitiveservices.speech as msspeech

def newinit(obj, result_impl):
    """
    replacement constructor for Recognition Result, which reads the latency from the result
    implementation, and then raises an exception
    """
    latency_id = msspeech.speech.PropertyId.SpeechServiceResponse_RecognitionLatencyMs
    try:
        obj.recognition_latency_ms = int(result_impl.properties.get_property(latency_id))
    except Exception as e:
        print(e)
        obj.recognition_latency_ms = None

    obj._old_init(result_impl)
    raise Exception('my exception')

@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_bad_callback_args_async(from_file_speech_reco_with_callbacks, speech_input):
    """test that async recognition handles bad arguments passed to callback functions
    gracefully."""
    # patching needs to happen inside the function so as not to clobber the module for other tests
    msspeech.RecognitionResult._old_init = msspeech.RecognitionResult.__init__
    with patch.object(msspeech.RecognitionResult, '__init__', newinit):
        reco, callbacks = from_file_speech_reco_with_callbacks()

        with pytest.raises(Exception):
            reco.recognize_once()


@pytest.mark.parametrize('speech_input,', ['weather'], indirect=True)
def test_bad_callback_args_continuous(from_file_speech_reco_with_callbacks, speech_input):
    """test that continuous recognition handles bad arguments passed to callback functions
    gracefully."""
    msspeech.RecognitionResult._old_init = msspeech.RecognitionResult.__init__
    with patch.object(msspeech.RecognitionResult, '__init__', newinit):
        reco, callbacks = from_file_speech_reco_with_callbacks()
        reco.start_continuous_recognition()
        _wait_for_event(callbacks, 'session_stopped')

