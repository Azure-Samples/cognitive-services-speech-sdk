#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Embedded (offline) speech recognition samples for the Speech SDK for Python.
"""

import threading

import azure.cognitiveservices.speech as speechsdk

import settings


def embedded_recognition_list_models():
    """List the embedded speech recognition models available at the configured model path."""
    if not settings.has_speech_recognition_model_path():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    models = config.get_speech_recognition_models()
    if models:
        print("Embedded speech recognition models found:")
        for model in models:
            print("Name:    {}".format(model.name))
            print(" Locales: {}".format(", ".join(model.locales)))
            print(" Path:    {}".format(model.path))
            print(" Version: {}".format(model.version))
    else:
        print("No embedded speech recognition models found. "
              "Check the model path in settings.py.")


def embedded_recognition_from_wave_file():
    """Recognize speech once from a WAV file using an embedded model."""
    if not settings.has_speech_recognition_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioConfig(filename=settings.WAV_AUDIO_FILE_NAME)
    recognizer = speechsdk.SpeechRecognizer(speech_config=config, audio_config=audio_config)

    result = recognizer.recognize_once()

    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("RECOGNIZED: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("NOMATCH: Speech could not be recognized.")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation = result.cancellation_details
        print("CANCELED: Reason={}".format(cancellation.reason))
        if cancellation.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorCode={}".format(cancellation.error_code))
            print("CANCELED: ErrorDetails={}".format(cancellation.error_details))


def embedded_recognition_once_from_microphone():
    """Recognize speech once from the default microphone using an embedded model."""
    if not settings.has_speech_recognition_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)
    recognizer = speechsdk.SpeechRecognizer(speech_config=config, audio_config=audio_config)

    print("Speak into your microphone.")
    result = recognizer.recognize_once()

    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("RECOGNIZED: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("NOMATCH: Speech could not be recognized.")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation = result.cancellation_details
        print("CANCELED: Reason={}".format(cancellation.reason))
        if cancellation.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorCode={}".format(cancellation.error_code))
            print("CANCELED: ErrorDetails={}".format(cancellation.error_details))


def embedded_recognition_continuous_from_microphone():
    """Recognize speech continuously from the default microphone using an embedded model."""
    if not settings.has_speech_recognition_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)
    recognizer = speechsdk.SpeechRecognizer(speech_config=config, audio_config=audio_config)

    done = threading.Event()

    def recognizing_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        print("RECOGNIZING: {}".format(evt.result.text))

    def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        if evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print("RECOGNIZED: {}".format(evt.result.text))
        elif evt.result.reason == speechsdk.ResultReason.NoMatch:
            print("NOMATCH: Speech could not be recognized.")

    def canceled_cb(evt: speechsdk.SpeechRecognitionCanceledEventArgs):
        print("CANCELED: Reason={}".format(evt.reason))
        if evt.reason == speechsdk.CancellationReason.Error:
            print("CANCELED: ErrorCode={}".format(evt.error_code))
            print("CANCELED: ErrorDetails={}".format(evt.error_details))
        done.set()

    def stop_cb(evt: speechsdk.SessionEventArgs):
        print("Session stopped.")
        done.set()

    recognizer.recognizing.connect(recognizing_cb)
    recognizer.recognized.connect(recognized_cb)
    recognizer.canceled.connect(canceled_cb)
    recognizer.session_stopped.connect(stop_cb)

    print("Speak into your microphone. Press Enter to stop.")
    recognizer.start_continuous_recognition()

    input()
    recognizer.stop_continuous_recognition()
    done.wait()


def embedded_recognition_keyword_from_microphone():
    """
    Recognize speech using keyword-triggered recognition from the default microphone.

    The recognizer listens for the keyword and starts recognition only after the
    keyword is spoken. A keyword recognition model file (.table) is required.
    See https://aka.ms/speech/kws for information on creating a keyword model.
    """
    if not settings.has_speech_recognition_model():
        return
    config = settings.create_embedded_speech_config()
    if config is None:
        return

    # The keyword recognition model file and matching keyword phrase. The default
    # data/keyword_computer.table model triggers on "Computer". See settings.py.
    keyword_model_file = settings.KEYWORD_MODEL_FILE_NAME
    keyword_phrase = settings.KEYWORD_PHRASE

    keyword_model = speechsdk.KeywordRecognitionModel(keyword_model_file)

    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)
    recognizer = speechsdk.SpeechRecognizer(speech_config=config, audio_config=audio_config)

    done = threading.Event()

    def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
        if evt.result.reason == speechsdk.ResultReason.RecognizedKeyword:
            print("RECOGNIZED KEYWORD: {}".format(evt.result.text))
        elif evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print("RECOGNIZED: {}".format(evt.result.text))
        elif evt.result.reason == speechsdk.ResultReason.NoMatch:
            print("NOMATCH: Speech could not be recognized.")

    def canceled_cb(evt: speechsdk.SpeechRecognitionCanceledEventArgs):
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

    print("Say \"{}\" followed by whatever you want to recognize. Press Enter to stop."
          .format(keyword_phrase))
    recognizer.start_keyword_recognition(keyword_model)

    input()
    recognizer.stop_keyword_recognition()
    done.wait()
