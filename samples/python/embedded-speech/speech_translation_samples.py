#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Embedded (offline) speech translation samples for the Speech SDK for Python.

The source and target languages of an embedded translation model are fixed by the
model itself, so they are not configured in code. Use ``get_speech_translation_models()``
to inspect what each model supports.
"""

import threading

import azure.cognitiveservices.speech as speechsdk

import settings


def embedded_translation_list_models():
    """List the embedded speech translation models available at the configured model path."""
    if not settings.has_speech_translation_model_path():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    models = config.get_speech_translation_models()
    if models:
        print("Embedded speech translation models found:")
        for model in models:
            print("Name:    {}".format(model.name))
            print(" Source languages: {}".format(", ".join(model.source_languages)))
            print(" Target languages: {}".format(", ".join(model.target_languages)))
            print(" Path:    {}".format(model.path))
            print(" Version: {}".format(model.version))
    else:
        print("No embedded speech translation models found. "
              "Check the model path in settings.py.")


def _print_translations(result):
    for target_language, translation in result.translations.items():
        print("TRANSLATED into '{}': {}".format(target_language, translation))


def embedded_translation_once_from_microphone():
    """Translate speech once from the default microphone using an embedded model."""
    if not settings.has_speech_translation_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=config, audio_config=audio_config)

    print("Speak into your microphone.")
    result = recognizer.recognize_once()

    if result.reason == speechsdk.ResultReason.TranslatedSpeech:
        _print_translations(result)
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("NOMATCH: Speech could not be recognized.")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation = result.cancellation_details
        print("CANCELED: Reason={}".format(cancellation.reason))
        if cancellation.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorCode={}".format(cancellation.error_code))
            print("CANCELED: ErrorDetails={}".format(cancellation.error_details))


def embedded_translation_continuous_from_microphone():
    """Translate speech continuously from the default microphone using an embedded model."""
    if not settings.has_speech_translation_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=config, audio_config=audio_config)

    done = threading.Event()

    def recognized_cb(evt: speechsdk.translation.TranslationRecognitionEventArgs):
        if evt.result.reason == speechsdk.ResultReason.TranslatedSpeech:
            _print_translations(evt.result)
        elif evt.result.reason == speechsdk.ResultReason.NoMatch:
            print("NOMATCH: Speech could not be recognized.")

    def canceled_cb(evt: speechsdk.translation.TranslationRecognitionCanceledEventArgs):
        print("CANCELED: Reason={}".format(evt.reason))
        if evt.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorDetails={}".format(evt.error_details))
        done.set()

    def stop_cb(evt: speechsdk.SessionEventArgs):
        print("Session stopped.")
        done.set()

    recognizer.recognized.connect(recognized_cb)
    recognizer.canceled.connect(canceled_cb)
    recognizer.session_stopped.connect(stop_cb)

    print("Speak into your microphone. Press Enter to stop.")
    recognizer.start_continuous_recognition()

    input()
    recognizer.stop_continuous_recognition()
    done.wait()
