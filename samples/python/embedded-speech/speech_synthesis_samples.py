#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Embedded (offline) speech synthesis samples for the Speech SDK for Python.
"""

import azure.cognitiveservices.speech as speechsdk

import settings


def embedded_synthesis_list_voices():
    """List the embedded speech synthesis voices available at the configured voice path."""
    if not settings.has_speech_synthesis_voice_path():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    synthesizer = speechsdk.SpeechSynthesizer(speech_config=config, audio_config=None)
    result = synthesizer.get_voices_async("").get()

    if result.reason == speechsdk.ResultReason.VoicesListRetrieved:
        print("Voices found:")
        for voice in result.voices:
            print(voice.name)
            print(" Gender: {}".format(voice.gender))
            print(" Locale: {}".format(voice.locale))
            print(" Path:   {}".format(voice.voice_path))
    elif result.reason == speechsdk.ResultReason.Canceled:
        print("CANCELED: ErrorDetails={}".format(result.error_details))


def _synthesize_to(synthesizer):
    """Reads text from console input and synthesizes it until an empty line is entered."""

    def synthesis_started_cb(evt: speechsdk.SessionEventArgs):
        print("Synthesis started.")

    def word_boundary_cb(evt: speechsdk.SpeechSynthesisWordBoundaryEventArgs):
        # Unit of audio_offset is tick (1 tick = 100 nanoseconds).
        print("Word \"{}\" | Text offset {} | Audio offset {}ms".format(
            evt.text, evt.text_offset, (evt.audio_offset + 5000) // 10000))

    synthesizer.synthesis_started.connect(synthesis_started_cb)
    synthesizer.synthesis_word_boundary.connect(word_boundary_cb)

    while True:
        text = input("Enter some text that you want to speak, or empty to exit.\n> ")
        if not text:
            break

        result = synthesizer.speak_text(text)

        if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
            print("Synthesis completed for text \"{}\".".format(text))
        elif result.reason == speechsdk.ResultReason.Canceled:
            cancellation = result.cancellation_details
            print("CANCELED: Reason={}".format(cancellation.reason))
            if cancellation.reason == speechsdk.CancellationReason.Error:
                print("CANCELED: ErrorCode={}".format(cancellation.error_code))
                print("CANCELED: ErrorDetails={}".format(cancellation.error_details))


def embedded_synthesis_to_speaker():
    """Synthesize speech to the default speaker using an embedded voice."""
    if not settings.has_speech_synthesis_voice():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioOutputConfig(use_default_speaker=True)
    synthesizer = speechsdk.SpeechSynthesizer(speech_config=config, audio_config=audio_config)
    _synthesize_to(synthesizer)


def embedded_synthesis_to_wave_file():
    """Synthesize speech to a WAV file using an embedded voice."""
    if not settings.has_speech_synthesis_voice():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    text = input("Enter some text that you want to speak.\n> ")
    if not text:
        return

    file_name = "embedded_synthesis_output.wav"
    audio_config = speechsdk.audio.AudioOutputConfig(filename=file_name)
    synthesizer = speechsdk.SpeechSynthesizer(speech_config=config, audio_config=audio_config)

    result = synthesizer.speak_text(text)

    if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
        print("Synthesized audio was saved to \"{}\".".format(file_name))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation = result.cancellation_details
        print("CANCELED: Reason={}".format(cancellation.reason))
        if cancellation.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorDetails={}".format(cancellation.error_details))
