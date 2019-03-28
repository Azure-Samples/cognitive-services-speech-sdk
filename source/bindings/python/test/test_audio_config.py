# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import pytest
import azure.cognitiveservices.speech as msspeech

def test_audio_config():
    with pytest.raises(ValueError, match="cannot construct AudioConfig with the given arguments"):
        cfg = msspeech.audio.AudioConfig()

    with pytest.raises(ValueError, match="use_default_microphone must be a bool, is \"abc\""):
        cfg = msspeech.audio.AudioConfig('abc')

    cfg = msspeech.audio.AudioConfig(use_default_microphone=True)
    assert cfg

    class MyCallback(msspeech.audio.PullAudioInputStreamCallback):
        def __init__(self):
            super().__init__()

        def read(self, buffer, size):
            pass

        def close(self):
            pass

    callback = MyCallback()
    stream = msspeech.audio.PullAudioInputStream(pull_stream_callback=callback)
    assert stream

    cfg = msspeech.audio.AudioConfig(stream=stream)
    assert cfg

    cfg = msspeech.audio.AudioConfig(filename='a')
    assert cfg

    cfg = msspeech.audio.AudioConfig(device_name='a')
    assert cfg

    with pytest.raises(ValueError):
        cfg = msspeech.audio.AudioConfig(filename='abc', device_name='dev')

    with pytest.raises(ValueError, match="default microphone can not be combined with any other options"):
        cfg = msspeech.audio.AudioConfig(True, device_name='nondefault')

