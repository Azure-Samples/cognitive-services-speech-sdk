# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes that are concerned with the handling of audio input to the various recognizers.
"""

from . import speech_py_impl as impl

from typing import Optional
OptionalStr = Optional[str]


class AudioStreamFormat():
    """
    Represents specific audio configuration, such as microphone, file, or custom audio streams

    When called without arguments, returns the default `AudioStreamFormat` (16kHz, 16 bit,
    mono).

    Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel
    (Mono) is supported.

    :param samples_per_second: The sample rate for the stream.
    :param bits_per_sample: The number of bits per audio sample
    :param channels: The number of audio channels
    """

    def __init__(self, samples_per_second: int = None, bits_per_sample: int = 16,
                 channels: int = 1):
        if samples_per_second is None:
            self._impl = impl.AudioStreamFormat_get_default_input_format()
        else:
            self._impl = impl.AudioStreamFormat_get_wave_format_pcm(samples_per_second,
                                                                    bits_per_sample, channels)


class AudioInputStream():
    """
    Base class for Input Streams
    """
    pass


class PullAudioInputStreamCallback(impl.PullAudioInputStreamCallback):
    """
    An interface that defines callback methods for an audio input stream.

    Derive from this class and implement its function to provide your own
    data as an audio input stream.
    """

    def __init__(self):
        super().__init__()

    def read(self, buffer: memoryview) -> int:
        """
        This function is called to synchronously get data from the audio stream.
        The buffer returned by read() should not contain any audio header.

        :param buffer: the buffer that audio data should be passed in.
        :return: The number of bytes passed into the stream.
        """
        return 0

    def close(self) -> None:
        """
        The callback that is called when the stream is closed.
        """
        pass


class PullAudioInputStream(AudioInputStream):
    """
    Pull audio input stream class.

    :param pull_stream_callback: The object containing the callback functions for the pull stream
    :param stream_format: The `AudioStreamFormat` the stream uses for audio data.
    """

    def __init__(self, pull_stream_callback: PullAudioInputStreamCallback,
                 stream_format: Optional[AudioStreamFormat] = None):
        if pull_stream_callback is not None:
            if stream_format is None:
                self._impl = impl.PullAudioInputStream.create(pull_stream_callback)
            else:
                self._impl = impl.PullAudioInputStream.create(stream_format._impl,
                                                              pull_stream_callback)


class PushAudioInputStream(AudioInputStream):
    """
    Represents memory backed push audio input stream used for custom audio input configurations.

    :param stream_format: The `AudioStreamFormat` the stream uses for audio data.
    """

    def __init__(self, stream_format: Optional[AudioStreamFormat] = None):
        if stream_format is None:
            self._impl = impl.PushAudioInputStream.create()
        else:
            self._impl = impl.PushAudioInputStream.create(stream_format._impl)

    def write(self, buffer: bytes):
        """
        Writes the audio data specified by making an internal copy of the data.
        The buffer should not contain any audio header.

        :param buffer: The audio data.
        """
        self._impl.write(buffer)

    def close(self):
        """
        Closes the stream.
        """
        self._impl.close()


class AudioConfig():
    """
    Represents specific audio configuration, such as microphone, file, or custom audio streams

    Generates an audio configuration for the various recognizers. Only one argument can be
    passed at a time.

    :param use_default_microphone: Specifies to use the default system microphone for audio
        input.
    :param device_name: Specifies the id of the audio device to use.
         Please refer to `this page <https://aka.ms/csspeech/microphone-selection>`_
         on how to retrieve platform-specific microphone names.
         This functionality was added in version 1.3.0.
    :param filename: Specifies an audio input file. Currently, only WAV / PCM with 16-bit
        samples, 16 kHz sample rate, and a single channel (Mono) is supported.
    :param stream: Creates an AudioConfig object representing the specified stream.
    """

    def __init__(self, use_default_microphone: bool = False, filename: OptionalStr = None,
            stream: Optional[AudioInputStream] = None, device_name: OptionalStr = None):
        if not isinstance(use_default_microphone, bool):
            raise ValueError('use_default_microphone must be a bool, is "{}"'.format(
                use_default_microphone))
        if use_default_microphone:
            if filename is None and stream is None and device_name is None:
                self._impl = impl.AudioConfig._from_default_microphone_input()
                return
            else:
                raise ValueError('default microphone can not be combined with any other options')

        if sum(x is not None for x in (filename, stream, device_name)) > 1:
            raise ValueError('only one of filename, stream, and device_name can be given')

        if filename is not None:
            self._impl = impl.AudioConfig._from_wav_file_input(filename)
            return
        if stream is not None:
            self._impl = impl.AudioConfig._from_stream_input(stream._impl)
            return
        if device_name is not None:
            self._impl = impl.AudioConfig._from_microphone_input(device_name)
            return

        raise ValueError('cannot construct AudioConfig with the given arguments')

