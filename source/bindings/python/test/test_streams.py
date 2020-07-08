# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import sys
import pytest
import time
import wave
import azure.cognitiveservices.speech as msspeech

from .conftest import SpeechInput
from .utils import (_setup_callbacks, _check_callbacks, _check_sr_result, _wait_for_event)


class WavFileReaderCallback(msspeech.audio.PullAudioInputStreamCallback):
    def __init__(self, filename: str):
        super().__init__()
        self._file_h = wave.open(filename, mode=None)

        assert self._file_h.getnchannels() == 1
        assert self._file_h.getsampwidth() == 2
        self.sample_width = self._file_h.getsampwidth()
        assert self._file_h.getframerate() == 16000
        assert self._file_h.getcomptype() == 'NONE'

    def read(self, buffer: memoryview) -> int:
        print('trying to read {} frames'.format(buffer.nbytes))
        try:
            size = buffer.nbytes
            frames = self._file_h.readframes(size // self.sample_width)

            buffer[:len(frames)] = frames
            print('read {} frames'.format(len(frames)))

            return len(frames)
        except Exception as ex:
            print('Exception in `read`: {}'.format(ex))
            raise

    def close(self) -> None:
        print('closing file')
        try:
            self._file_h.close()
        except Exception as ex:
            print('Exception in `close`: {}'.format(ex))
            raise

class BinaryFileReaderCallback(msspeech.audio.PullAudioInputStreamCallback):
    def __init__(self, filename: str):
        super().__init__()
        self._file_h = open(filename, "rb")

    def read(self, buffer: memoryview) -> int:
        print('trying to read {} frames'.format(buffer.nbytes))
        try:
            size = buffer.nbytes
            frames = self._file_h.read(size)

            buffer[:len(frames)] = frames
            print('read {} frames'.format(len(frames)))

            return len(frames)
        except Exception as ex:
            print('Exception in `read`: {}'.format(ex))
            raise

    def close(self) -> None:
        print('closing file')
        try:
            self._file_h.close()
        except Exception as ex:
            print('Exception in `close`: {}'.format(ex))
            raise

def compressed_stream_test_helper(compressed_format,
        speech_input,
        default_speech_auth):
    callback = BinaryFileReaderCallback(speech_input.path)
    stream = msspeech.audio.PullAudioInputStream(stream_format=compressed_format, pull_stream_callback=callback)

    speech_config = msspeech.SpeechConfig(**default_speech_auth)
    audio_config = msspeech.audio.AudioConfig(stream=stream)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    callbacks = _setup_callbacks(reco, setup_stop_callbacks=True)

    reco.start_continuous_recognition()

    _wait_for_event(callbacks, 'session_stopped')
    _check_callbacks(callbacks, check_num_recognized=False)

    valid_events = [evt for (evt, _) in callbacks['recognized'].events
            if evt.result.reason == msspeech.ResultReason.RecognizedSpeech]

    assert 1 == len(valid_events)
    _check_sr_result(valid_events[-1].result, speech_input, 0)

@pytest.mark.skipif(sys.platform == 'darwin', reason="Compressed stream is not available on macOS")
@pytest.mark.parametrize('speech_input,', ['weathermp3'], indirect=True)
def test_pull_audio_input_stream_compressed_mp3(speech_input: SpeechInput,
        default_speech_auth):
    compressed_format = msspeech.audio.AudioStreamFormat(compressed_stream_format=msspeech.AudioStreamContainerFormat.MP3)
    compressed_stream_test_helper(compressed_format, speech_input, default_speech_auth)

@pytest.mark.skipif(sys.platform == 'darwin', reason="Compressed stream is not available on macOS")
@pytest.mark.parametrize('speech_input,', ['weatheropus'], indirect=True)
def test_pull_audio_input_stream_compressed_opus(speech_input: SpeechInput,
        default_speech_auth):
    compressed_format = msspeech.audio.AudioStreamFormat(compressed_stream_format=msspeech.AudioStreamContainerFormat.OGG_OPUS)
    compressed_stream_test_helper(compressed_format, speech_input, default_speech_auth)

@pytest.mark.skipif(sys.platform == 'darwin', reason="Compressed stream is not available on macOS")
@pytest.mark.parametrize('speech_input,', ['weatherflac'], indirect=True)
def test_pull_audio_input_stream_compressed_flac(speech_input: SpeechInput,
        default_speech_auth):
    compressed_format = msspeech.audio.AudioStreamFormat(compressed_stream_format=msspeech.AudioStreamContainerFormat.FLAC)
    compressed_stream_test_helper(compressed_format, speech_input, default_speech_auth)

@pytest.mark.skipif(sys.platform == 'darwin', reason="Compressed stream is not available on macOS")
@pytest.mark.parametrize('speech_input,', ['weatheralaw'], indirect=True)
def test_pull_audio_input_stream_compressed_alaw(speech_input: SpeechInput,
        default_speech_auth):
    compressed_format = msspeech.audio.AudioStreamFormat(compressed_stream_format=msspeech.AudioStreamContainerFormat.ALAW)
    compressed_stream_test_helper(compressed_format, speech_input, default_speech_auth)

@pytest.mark.skipif(sys.platform == 'darwin', reason="Compressed stream is not available on macOS")
@pytest.mark.parametrize('speech_input,', ['weathermulaw'], indirect=True)
def test_pull_audio_input_stream_compressed_mulaw(speech_input: SpeechInput,
        default_speech_auth):
    compressed_format = msspeech.audio.AudioStreamFormat(compressed_stream_format=msspeech.AudioStreamContainerFormat.MULAW)
    compressed_stream_test_helper(compressed_format, speech_input, default_speech_auth)

@pytest.mark.parametrize("use_default_wave_format", (False, True))
@pytest.mark.parametrize('speech_input,', ['weather', 'lamp'], indirect=True)
def test_pull_audio_input_stream_callback(speech_input: SpeechInput, use_default_wave_format: bool,
        default_speech_auth):
    callback = WavFileReaderCallback(speech_input.path)
    if use_default_wave_format:
        stream = msspeech.audio.PullAudioInputStream(pull_stream_callback=callback)
    else:
        channels = 1
        bits_per_sample = 16
        samples_per_second = 16000

        wave_format = msspeech.audio.AudioStreamFormat(samples_per_second, bits_per_sample, channels)
        stream = msspeech.audio.PullAudioInputStream(stream_format=wave_format, pull_stream_callback=callback)

    speech_config = msspeech.SpeechConfig(**default_speech_auth)
    audio_config = msspeech.audio.AudioConfig(stream=stream)

    reco = msspeech.SpeechRecognizer(speech_config, audio_config)

    callbacks = _setup_callbacks(reco, setup_stop_callbacks=True)

    reco.start_continuous_recognition()

    # TODO: expected number of callback calls should be a property of the input
    _wait_for_event(callbacks, 'session_stopped')
    _check_callbacks(callbacks, check_num_recognized=False)

    valid_events = [evt for (evt, _) in callbacks['recognized'].events
            if evt.result.reason == msspeech.ResultReason.RecognizedSpeech]

    assert 1 == len(valid_events)
    _check_sr_result(valid_events[-1].result, speech_input, 0)

@pytest.mark.parametrize("use_default_wave_format", (False, True))
@pytest.mark.parametrize('speech_input,', ['weather', 'lamp'], indirect=True)
def test_push_audio_input_stream(speech_input: SpeechInput, use_default_wave_format: bool,
        default_speech_auth):
    speech_config = msspeech.SpeechConfig(**default_speech_auth)

    if use_default_wave_format:
        stream = msspeech.audio.PushAudioInputStream()
    else:
        channels = 1
        bits_per_sample = 16
        samples_per_second = 16000

        wave_format = msspeech.audio.AudioStreamFormat(samples_per_second, bits_per_sample, channels)
        stream = msspeech.audio.PushAudioInputStream(stream_format=wave_format)

    audio_cfg = msspeech.audio.AudioConfig(stream=stream)

    reco = msspeech.SpeechRecognizer(speech_config, audio_cfg)

    callbacks = _setup_callbacks(reco, setup_stop_callbacks=False)

    n_bytes = 3200
    wav_fh = wave.open(speech_input.path)

    reco.start_continuous_recognition_async()
    try:
        while(True):
            frames = wav_fh.readframes(n_bytes // 2)
            print('read {} frames'.format(len(frames)))
            if not frames:
                break

            stream.write(frames)
            time.sleep(.2)

    finally:
        print('closing')
        reco.stop_continuous_recognition_async()
        wav_fh.close()
        stream.close()

    _wait_for_event(callbacks, 'session_stopped')
    _check_callbacks(callbacks)

    # TODO: investigate different offsets between stream methods (VSTS1550242).
    # _check_sr_result(callbacks['recognized'].events[-1][0].result, speech_input, 0)

