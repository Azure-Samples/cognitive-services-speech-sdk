#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Speech Language Detection samples for the Microsoft Cognitive Services Speech SDK
"""

import time
import json

try:
    import azure.cognitiveservices.speech as speechsdk
except ImportError:
    print("""
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python for
    installation instructions.
    """)
    import sys
    sys.exit(1)


# Set up the subscription info for the Speech Service:
# Replace with your own subscription key and service region (e.g., "westus").
speech_key, service_region = "YourSubscriptionKey", "YourServiceRegion"

# Specify the path to audio files containing speech (mono WAV / PCM with a sampling rate of 16
# kHz).
single_language_wav_file = "whatstheweatherlike.wav"
multilingual_wav_file = "en-us_zh-cn.wav"


def speech_language_detection_once_from_mic():
    """performs one-shot speech language detection from the default microphone"""
    # <SpeechLanguageDetectionWithMicrophone>
    # Creates an AutoDetectSourceLanguageConfig, which defines a number of possible spoken languages
    auto_detect_source_language_config = \
        speechsdk.languageconfig.AutoDetectSourceLanguageConfig(languages=["de-DE", "en-US"])

    # Creates a SpeechConfig from your speech key and region
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)

    # Creates a source language recognizer using microphone as audio input.
    # The default language is "en-us".
    speech_language_detection = speechsdk.SourceLanguageRecognizer(
        speech_config=speech_config, auto_detect_source_language_config=auto_detect_source_language_config)

    print("Say something in English or German...")

    # Starts speech language detection, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of 15
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = speech_language_detection.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        detected_src_lang = result.properties[
            speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]
        print("Detected Language: {}".format(detected_src_lang))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Recognition canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechLanguageDetectionWithMicrophone>


def speech_language_detection_once_from_file():
    """performs one-shot speech language detection with input from an audio file"""
    # <SpeechLanguageDetectionWithFile>
    # Creates an AutoDetectSourceLanguageConfig, which defines a number of possible spoken languages
    auto_detect_source_language_config = \
        speechsdk.languageconfig.AutoDetectSourceLanguageConfig(languages=["de-DE", "en-US"])

    # Creates a SpeechConfig from your speech key and region
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)

    # Creates an AudioConfig from a given WAV file
    audio_config = speechsdk.audio.AudioConfig(filename=single_language_wav_file)

    # Creates a source language recognizer using a file as audio input, also specify the speech language
    source_language_recognizer = speechsdk.SourceLanguageRecognizer(
        speech_config=speech_config,
        auto_detect_source_language_config=auto_detect_source_language_config,
        audio_config=audio_config)

    # Starts speech language detection, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of 15
    # seconds of audio is processed. It returns the detection text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot detection like command or query.
    # For long-running multi-utterance detection, use start_continuous_recognition() instead.
    result = source_language_recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        detected_src_lang = result.properties[
            speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]
        print("Detected Language: {}".format(detected_src_lang))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech Language Detection canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
    # </SpeechLanguageDetectionWithFile>


def speech_language_detection_once_from_continuous():
    """performs continuous speech language detection with input from an audio file"""
    # <SpeechContinuousLanguageDetectionWithFile>
    # Creates an AutoDetectSourceLanguageConfig, which defines a number of possible spoken languages
    auto_detect_source_language_config = \
        speechsdk.languageconfig.AutoDetectSourceLanguageConfig(languages=["zh-CN", "en-US"])

    # Creates a SpeechConfig from your speech key and region
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)

    # Set continuous language detection (override the default of "AtStart")
    speech_config.set_property(
        property_id=speechsdk.PropertyId.SpeechServiceConnection_LanguageIdMode, value='Continuous')

    audio_config = speechsdk.audio.AudioConfig(filename=multilingual_wav_file)

    source_language_recognizer = speechsdk.SourceLanguageRecognizer(
        speech_config=speech_config,
        auto_detect_source_language_config=auto_detect_source_language_config,
        audio_config=audio_config)

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print('CLOSING on {}'.format(evt))
        nonlocal done
        done = True

    def audio_recognized(evt: speechsdk.SpeechRecognitionEventArgs):
        """
        callback that catches the recognized result of audio from an event 'evt'.
        :param evt: event listened to catch recognition result.
        :return:
        """
        if evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
            if evt.result.properties.get(
                    speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult) is None:
                print("Unable to detect any language")
            else:
                detected_src_lang = evt.result.properties[
                    speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]
                json_result = evt.result.properties[speechsdk.PropertyId.SpeechServiceResponse_JsonResult]
                detail_result = json.loads(json_result)
                start_offset = detail_result['Offset']
                duration = detail_result['Duration']
                if duration >= 0:
                    end_offset = duration + start_offset
                else:
                    end_offset = 0
                print("Detected language = " + detected_src_lang)
                print(f"Start offset = {start_offset}, End offset = {end_offset}, "
                      f"Duration = {duration} (in units of hundreds of nanoseconds (HNS))")
                global language_detected
                language_detected = True

    # Connect callbacks to the events fired by the speech recognizer
    source_language_recognizer.recognized.connect(audio_recognized)
    source_language_recognizer.session_started.connect(lambda evt: print('SESSION STARTED: {}'.format(evt)))
    source_language_recognizer.session_stopped.connect(lambda evt: print('SESSION STOPPED {}'.format(evt)))
    source_language_recognizer.canceled.connect(lambda evt: print('CANCELED {}'.format(evt)))
    # stop continuous recognition on either session stopped or canceled events
    source_language_recognizer.session_stopped.connect(stop_cb)
    source_language_recognizer.canceled.connect(stop_cb)

    # Start continuous speech recognition
    source_language_recognizer.start_continuous_recognition()
    while not done:
        time.sleep(.5)

    source_language_recognizer.stop_continuous_recognition()
    # </SpeechContinuousLanguageDetectionWithFile>
