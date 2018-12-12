import pytest
import azure.cognitiveservices.speech as msspeech

def test_audio_config():
    with pytest.raises(ValueError) as excinfo:
        cfg = msspeech.AudioConfig()

        assert "no valid audio source" == str(excinfo.value)

    with pytest.raises(ValueError) as excinfo:
        cfg = msspeech.AudioConfig('abc')

        assert "use_default_microphone must be a bool, is \"abc\"" in str(excinfo.value)

    cfg = msspeech.AudioConfig(use_default_microphone=True)
    assert cfg

    class MyCallback(msspeech.PullAudioInputStreamCallback):
        def __init__(self):
            super().__init__()

        def read(self, buffer, size):
            pass

        def close(self):
            pass

    callback = MyCallback()
    stream = msspeech.PullAudioInputStream(pull_stream_callback=callback)
    assert stream

    cfg = msspeech.AudioConfig(stream=stream)
    assert cfg

    cfg = msspeech.AudioConfig(filename='a')
    assert cfg

