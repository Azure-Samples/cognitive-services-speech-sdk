# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.

import azure.cognitiveservices.speech as msspeech

from concurrent.futures import ThreadPoolExecutor


class _PushAudioOutputStreamTestCallback(msspeech.audio.PushAudioOutputStreamCallback):
    """
    A test class derived from PushAudioOutputStreamCallback, which is used to test
    speech synthesis with output to PushAudioOutputStream
    """

    def __init__(self):
        super().__init__()
        self._audio_data = bytes(0)
        self._closed = False

    def write(self, audio_buffer: memoryview) -> int:
        self._audio_data += audio_buffer
        return audio_buffer.nbytes

    def close(self):
        self._closed = True

    def is_closed(self) -> bool:
        return self._closed

    def get_audio_size(self) -> int:
        return len(self._audio_data)


def _do_something_with_audio_in_push_stream(callback, canceled):
    if not canceled:
        assert callback.get_audio_size() > 0

    assert callback.is_closed()


def _do_something_with_audio_in_pull_stream(stream, canceled):
    audio_buffer = bytes(1024)
    total_size = 0

    filled_size = stream.read(audio_buffer)
    while filled_size > 0:
        # do something with audio buffer
        total_size += filled_size
        filled_size = stream.read(audio_buffer)
        # blocks until atleast 1024 bytes are available

    if not canceled[0]:
        assert total_size > 0
    else:
        assert total_size == 0


def _do_something_with_audio_in_pull_stream_in_background(stream, canceled):
    executor = ThreadPoolExecutor()
    return executor.submit(_do_something_with_audio_in_pull_stream, stream, canceled)


def _do_something_with_audio_in_vector(audio_data):
    assert audio_data is not None
    assert isinstance(audio_data, bytes)
    assert len(audio_data) > 0


def _synthesizing_event_check(e):
    result_reason = e.result.reason
    assert msspeech.ResultReason.SynthesizingAudio == result_reason

    audio_data = e.result.audio_data
    _do_something_with_audio_in_vector(audio_data)


def _do_something_with_audio_in_data_stream(stream, after_synthesis_done):
    if after_synthesis_done and stream.status != msspeech.StreamStatus.Canceled:
        assert msspeech.StreamStatus.AllData == stream.status

    audio_buffer = bytes(1024)
    total_size = 0

    filled_size = stream.read_data(audio_buffer)
    while filled_size > 0:
        # do something with audio buffer
        total_size += filled_size
        filled_size = stream.read_data(audio_buffer)
        # blocks until atleast 1024 bytes are available

    if msspeech.StreamStatus.Canceled != stream.status:
        assert total_size > 0

    if not after_synthesis_done and stream.status != msspeech.StreamStatus.Canceled:
        assert msspeech.StreamStatus.AllData == stream.status


def _do_something_with_audio_in_data_stream_in_background(stream, after_synthesis_done):
    executor = ThreadPoolExecutor()
    return executor.submit(_do_something_with_audio_in_data_stream, stream, after_synthesis_done)


def _synthesis_started_event_check(e):
    result_reason = e.result.reason
    assert msspeech.ResultReason.SynthesizingAudioStarted == result_reason

    audio_data = e.result.audio_data
    assert 0 == len(audio_data)

    stream = msspeech.AudioDataStream(e.result)  # of type AudioDataStream, does not block
    _do_something_with_audio_in_data_stream_in_background(stream, False)


def _synthesis_started_counter(e, counter):
    counter['_started_speak_requests'] += 1


def _synthesis_completed_counter(e, counter):
    counter['_done_speak_requests'] += 1

    if counter['_done_speak_requests'] == 1:
        assert 1 == counter['_started_speak_requests']


def _synthesis_canceled_counter(e, counter):
    counter['_done_speak_requests'] += 1

    if counter['_done_speak_requests'] == 1:
        assert 1 == counter['_started_speak_requests']


def _synthesis_word_boundary_event_check_plain_text(e, counter,
    expected_audio_offsets, expected_text_offsets, expected_word_lengths):
    assert expected_audio_offsets[counter['_word_boundary_events']] == e.audio_offset
    assert expected_text_offsets[counter['_word_boundary_events']] == e.text_offset
    assert expected_word_lengths[counter['_word_boundary_events']] == e.word_length

    counter['_word_boundary_events'] += 1


def _synthesis_word_boundary_event_check_ssml(e, counter,
    expected_audio_offsets, expected_ssml_offsets, expected_word_lengths):
    assert e.audio_offset > 0
    assert expected_ssml_offsets[counter['_word_boundary_events']] == e.text_offset
    assert expected_word_lengths[counter['_word_boundary_events']] == e.word_length

    counter['_word_boundary_events'] += 1


def _do_something_with_audio_in_result(future_result, after_synthesis_done):
    stream = msspeech.AudioDataStream(future_result.get())
    _do_something_with_audio_in_data_stream(stream, after_synthesis_done)


def _do_something_with_audio_in_result_in_background(future_result, after_synthesis_done):
    executor = ThreadPoolExecutor()
    return executor.submit(_do_something_with_audio_in_result, future_result, after_synthesis_done)

