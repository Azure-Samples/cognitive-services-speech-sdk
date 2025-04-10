#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Translation recognition samples for the Microsoft Cognitive Services Speech SDK
"""

import time

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
# Replace with your own subscription key and endpoint.
speech_key, speech_endpoint = "YourSubscriptionKey", "https://YourServiceRegion.api.cognitive.microsoft.com"

# Specify the path to audio files containing speech (mono WAV / PCM with a sampling rate of 16
# kHz).
weatherfilename = "whatstheweatherlike.wav"
multilingual_wav_file = "en-us_zh-cn.wav"


def translation_once_from_mic():
    """performs one-shot speech translation from input from the default microphone"""
    # <TranslationOnceWithMic>
    # set up translation parameters: source language and target languages
    translation_config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key, endpoint=speech_endpoint,
        speech_recognition_language='en-US',
        target_languages=('de', 'fr', 'zh-Hans'))
    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)

    # Creates a translation recognizer using and audio file as input.
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config, audio_config=audio_config)

    # Starts translation, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. It returns the recognized text as well as the translation.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.TranslatedSpeech:
        print("""Recognized: {}
        German translation: {}
        French translation: {}
        Chinese translation: {}""".format(
            result.text, result.translations['de'],
            result.translations['fr'],
            result.translations['zh-Hans'],))
    elif result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        print("Translation canceled: {}".format(result.cancellation_details.reason))
        if result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(result.cancellation_details.error_details))
    # </TranslationOnceWithMic>


def translation_once_from_file():
    """performs one-shot speech translation from input from an audio file"""
    # <TranslationOnceWithFile>
    # set up translation parameters: source language and target languages
    translation_config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key, endpoint=speech_endpoint,
        speech_recognition_language='en-US',
        target_languages=('de', 'fr'))
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    # Creates a translation recognizer using and audio file as input.
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config, audio_config=audio_config)

    # Starts translation, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of about 30
    # seconds of audio is processed. The task returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    result = recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.TranslatedSpeech:
        print("""Recognized: {}
        German translation: {}
        French translation: {}""".format(
            result.text, result.translations['de'], result.translations['fr']))
    elif result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        print("Translation canceled: {}".format(result.cancellation_details.reason))
        if result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(result.cancellation_details.error_details))
    # </TranslationOnceWithFile>


def translation_continuous():
    """performs continuous speech translation from an audio file"""
    # <TranslationContinuous>
    # set up translation parameters: source language and target languages
    translation_config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key, endpoint=speech_endpoint,
        speech_recognition_language='en-US',
        target_languages=('de', 'fr'), voice_name="de-DE-KatjaNeural")
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    # Creates a translation recognizer using and audio file as input.
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config, audio_config=audio_config)

    def result_callback(event_type: str, evt: speechsdk.translation.TranslationRecognitionEventArgs):
        """callback to display a translation result"""
        print("{}:\n {}\n\tTranslations: {}\n\tResult Json: {}\n".format(
            event_type, evt, evt.result.translations.items(), evt.result.json))

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print('CLOSING on {}'.format(evt))
        nonlocal done
        done = True

    def canceled_cb(evt: speechsdk.translation.TranslationRecognitionCanceledEventArgs):
        print('CANCELED:\n\tReason:{}\n'.format(evt.result.reason))
        print('\tDetails: {} ({})'.format(evt, evt.result.cancellation_details.error_details))

    # connect callback functions to the events fired by the recognizer
    recognizer.session_started.connect(lambda evt: print('SESSION STARTED: {}'.format(evt)))
    recognizer.session_stopped.connect(lambda evt: print('SESSION STOPPED {}'.format(evt)))
    # event for intermediate results
    recognizer.recognizing.connect(lambda evt: result_callback('RECOGNIZING', evt))
    # event for final result
    recognizer.recognized.connect(lambda evt: result_callback('RECOGNIZED', evt))
    # cancellation event
    recognizer.canceled.connect(canceled_cb)

    # stop continuous recognition on either session stopped or canceled events
    recognizer.session_stopped.connect(stop_cb)
    recognizer.canceled.connect(stop_cb)

    def synthesis_callback(evt: speechsdk.translation.TranslationRecognitionEventArgs):
        """
        callback for the synthesis event
        """
        print('SYNTHESIZING {}\n\treceived {} bytes of audio. Reason: {}'.format(
            evt, len(evt.result.audio), evt.result.reason))

    # connect callback to the synthesis event
    recognizer.synthesizing.connect(synthesis_callback)

    # start translation
    recognizer.start_continuous_recognition()

    while not done:
        time.sleep(.5)

    recognizer.stop_continuous_recognition()
    # </TranslationContinuous>


def translation_once_with_lid_from_file():
    """performs a one-shot speech translation from an audio file, with at-start language identification"""
    # <TranslationOnceWithLID>

    # Set up translation parameters, including the list of target (translated) languages.
    translation_config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key,
        endpoint=speech_endpoint,
        target_languages=('de', 'fr'))
    audio_config = speechsdk.audio.AudioConfig(filename=weatherfilename)

    # Specify the AutoDetectSourceLanguageConfig, which defines the number of possible source (input) languages
    auto_detect_source_language_config = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(
        languages=["en-US", "de-DE", "zh-CN"])

    # Creates a translation recognizer using an audio file as input.
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config,
        audio_config=audio_config,
        auto_detect_source_language_config=auto_detect_source_language_config)

    # Starts translation, with single-utterance (one-shot) recognition and language identification
    result = recognizer.recognize_once()

    # Check the result
    if result.reason == speechsdk.ResultReason.TranslatedSpeech:
        src_lang = result.properties[speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]
        print("""Recognized:
        Detected language: {}
        Recognition result: {}
        German translation: {}
        French translation: {}""".format(
            src_lang,
            result.text,
            result.translations['de'],
            result.translations['fr']))
    elif result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized:\n {}".format(result.text))
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(result.no_match_details))
    elif result.reason == speechsdk.ResultReason.Canceled:
        print("Translation canceled: {}".format(result.cancellation_details.reason))
        if result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(result.cancellation_details.error_details))
    # </TranslationOnceWithLID>


def translation_continuous_with_lid_from_multilingual_file():
    """performs continuous speech translation from a multi-lingual audio file, with continuous language identification"""
    # <TranslationContinuousWithLID>

    # Set up translation parameters, including the list of target (translated) languages.
    translation_config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key,
        endpoint=speech_endpoint,
        target_languages=('de', 'fr'))
    audio_config = speechsdk.audio.AudioConfig(filename=multilingual_wav_file)

    # Since the spoken language in the input audio changes, you need to set the language identification to "Continuous" mode.
    # (override the default value of "AtStart").
    translation_config.set_property(
        property_id=speechsdk.PropertyId.SpeechServiceConnection_LanguageIdMode, value='Continuous')

    # Specify the AutoDetectSourceLanguageConfig, which defines the number of possible languages
    auto_detect_source_language_config = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(
        languages=["en-US", "zh-CN"])

    # Creates a translation recognizer using and audio file as input.
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config,
        audio_config=audio_config,
        auto_detect_source_language_config=auto_detect_source_language_config)

    def result_callback(evt):
        """callback to display a translation result"""
        if evt.result.reason == speechsdk.ResultReason.TranslatedSpeech:
            src_lang = evt.result.properties[speechsdk.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult]
            print("""Recognized:
            Detected language: {}
            Recognition result: {}
            German translation: {}
            French translation: {}""".format(
                src_lang,
                evt.result.text,
                evt.result.translations['de'],
                evt.result.translations['fr']))
        elif evt.result.reason == speechsdk.ResultReason.RecognizedSpeech:
            print("Recognized:\n {}".format(evt.result.text))
        elif evt.result.reason == speechsdk.ResultReason.NoMatch:
            print("No speech could be recognized: {}".format(evt.result.no_match_details))
        elif evt.result.reason == speechsdk.ResultReason.Canceled:
            print("Translation canceled: {}".format(evt.result.cancellation_details.reason))
            if evt.result.cancellation_details.reason == speechsdk.CancellationReason.Error:
                print("Error details: {}".format(evt.result.cancellation_details.error_details))

    done = False

    def stop_cb(evt):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print('CLOSING on {}'.format(evt))
        nonlocal done
        done = True

    # connect callback functions to the events fired by the recognizer
    recognizer.session_started.connect(lambda evt: print('SESSION STARTED: {}'.format(evt)))
    recognizer.session_stopped.connect(lambda evt: print('SESSION STOPPED {}'.format(evt)))

    # event for final result
    recognizer.recognized.connect(lambda evt: result_callback(evt))

    # cancellation event
    recognizer.canceled.connect(lambda evt: print('CANCELED: {} ({})'.format(evt, evt.reason)))

    # stop continuous recognition on either session stopped or canceled events
    recognizer.session_stopped.connect(stop_cb)
    recognizer.canceled.connect(stop_cb)

    # start translation
    recognizer.start_continuous_recognition()

    while not done:
        time.sleep(.5)

    recognizer.stop_continuous_recognition()
    # </TranslationContinuousWithLID>
