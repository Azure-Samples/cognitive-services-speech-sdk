#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Intent recognition samples for the Microsoft Cognitive Services Speech SDK
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

# Set up the subscription info for the Language Understanding Service: Note that this is not the
# same subscription as the one for the Speech Service. Replace with your own language understanding
# subscription key and service region (e.g., "westus").
intent_key = "YourLanguageUnderstandingSubscriptionKey"
intent_service_region = "YourLanguageUnderstandingServiceRegion"
language_understanding_app_id = "YourLanguageUnderstandingAppId"


# Specify the path to an audio file containing speech (mono WAV / PCM with a sampling rate of 16kHz).
lampfilename = "TurnOnTheLamp.wav"


def recognize_intent_once_from_mic():
    """performs one-shot intent recognition from input from the default microphone"""
    # <IntentRecognitionOnceWithMic>
    # Set up the config for the intent recognizer
    # remember that this uses the Language Understanding key, not the Speech Services key!
    intent_config = speechsdk.SpeechConfig(subscription=intent_key, region=intent_service_region)
    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)

    # Set up the intent recognizer
    intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config, audio_config=audio_config)

    # set up the intents that are to be recognized. These can be a mix of simple phrases and
    # intents specified through a LanguageUnderstanding Model.
    model = speechsdk.intent.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intents = [
        (model, "HomeAutomation.TurnOn"),
        (model, "HomeAutomation.TurnOff"),
        ("This is a test.", "test"),
        ("Switch to channel 34.", "34"),
        ("what's the weather like", "weather"),
    ]
    intent_recognizer.add_intents(intents)

    # Starts intent recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of 15
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    intent_result = intent_recognizer.recognize_once()

    # Check the results
    if intent_result.reason == speechsdk.ResultReason.RecognizedIntent:
        print("Recognized: \"{}\" with intent id `{}`".format(intent_result.text, intent_result.intent_id))
    elif intent_result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(intent_result.text))
    elif intent_result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(intent_result.no_match_details))
    elif intent_result.reason == speechsdk.ResultReason.Canceled:
        print("Intent recognition canceled: {}".format(intent_result.cancellation_details.reason))
        if intent_result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(intent_result.cancellation_details.error_details))
    # </IntentRecognitionOnceWithMic>


def recognize_intent_once_from_file():
    """performs one-shot intent recognition from input from an audio file"""
    # <IntentRecognitionOnceWithFile>
    # Set up the config for the intent recognizer
    # remember that this uses the Language Understanding key, not the Speech Services key!
    intent_config = speechsdk.SpeechConfig(subscription=intent_key, region=intent_service_region)
    audio_config = speechsdk.audio.AudioConfig(filename=lampfilename)

    # Set up the intent recognizer
    intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config, audio_config=audio_config)

    # set up the intents that are to be recognized. These can be a mix of simple phrases and
    # intents specified through a LanguageUnderstanding Model.
    model = speechsdk.intent.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intents = [
        (model, "HomeAutomation.TurnOn"),
        (model, "HomeAutomation.TurnOff"),
        ("This is a test.", "test"),
        ("Switch the to channel 34.", "34"),
        ("what's the weather like", "weather"),
    ]
    intent_recognizer.add_intents(intents)

    # Starts intent recognition, and returns after a single utterance is recognized. The end of a
    # single utterance is determined by listening for silence at the end or until a maximum of 15
    # seconds of audio is processed. It returns the recognition text as result.
    # Note: Since recognize_once() returns only a single utterance, it is suitable only for single
    # shot recognition like command or query.
    # For long-running multi-utterance recognition, use start_continuous_recognition() instead.
    intent_result = intent_recognizer.recognize_once()

    # Check the results
    if intent_result.reason == speechsdk.ResultReason.RecognizedIntent:
        print("Recognized: \"{}\" with intent id `{}`".format(intent_result.text, intent_result.intent_id))
    elif intent_result.reason == speechsdk.ResultReason.RecognizedSpeech:
        print("Recognized: {}".format(intent_result.text))
    elif intent_result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized: {}".format(intent_result.no_match_details))
    elif intent_result.reason == speechsdk.ResultReason.Canceled:
        print("Intent recognition canceled: {}".format(intent_result.cancellation_details.reason))
        if intent_result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(intent_result.cancellation_details.error_details))
    # </IntentRecognitionOnceWithFile>


def recognize_intent_once_async_from_mic():
    """performs one-shot asynchronous intent recognition from input from the default microphone"""
    # Set up the config for the intent recognizer
    intent_config = speechsdk.SpeechConfig(subscription=intent_key, region=intent_service_region)
    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)

    # Set up the intent recognizer
    intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config, audio_config=audio_config)

    # Add callbacks to the recognition events

    # Set up a flag to mark when asynchronous recognition is done
    done = False

    def recognized_callback(evt: speechsdk.intent.IntentRecognitionEventArgs):
        """
        Callback that is called on successful recognition of a full utterance by both speech
        processing and intent classification
        """
        result = evt.result
        print("Recognized: \"{}\" with intent id `{}`".format(result.text, result.intent_id))
        nonlocal done
        done = True

    def canceled_callback(evt: speechsdk.intent.IntentRecognitionCanceledEventArgs):
        """Callback that is called on a failure by either speech or language processing"""
        result = evt.result
        print("Intent recognition canceled: {}".format(result.cancellation_details.reason))
        if result.cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(result.cancellation_details.error_details))
        nonlocal done
        done = True

    def recognizing_callback(evt: speechsdk.intent.IntentRecognitionEventArgs):
        """Callback that is called on intermediate results from speech transcription"""
        result = evt.result
        print("Intermediate transcription: \"{}\"".format(result.text))

    # Connect the callbacks
    intent_recognizer.recognized.connect(recognized_callback)
    intent_recognizer.canceled.connect(canceled_callback)
    intent_recognizer.recognizing.connect(recognizing_callback)

    # set up the intents that are to be recognized. These can be a mix of simple phrases and
    # intents specified through a LanguageUnderstanding Model.
    model = speechsdk.intent.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intents = [
        (model, "HomeAutomation.TurnOn"),
        (model, "HomeAutomation.TurnOff"),
        ("This is a test.", "test"),
        ("Switch to channel 34.", "34"),
        ("what's the weather like", "weather"),
    ]
    intent_recognizer.add_intents(intents)

    # Starts non-blocking intent recognition and stop after a single utterance has been recognized.
    # The end of a single utterance is determined by listening for silence at the end or until a
    # maximum of 15 seconds of audio is processed.
    # Note: Since recognize_once() stops after a single utterance, it is suitable only for single
    # shot recognition like command or query. For long-running multi-utterance recognition, use
    # start_continuous_recognition() instead.
    intent_recognizer.recognize_once_async()

    # wait until recognition is complete
    while not done:
        time.sleep(1)


def recognize_intent_continuous():
    """performs continuous intent recognition from input from an audio file"""
    # <IntentContinuousRecognitionWithFile>
    intent_config = speechsdk.SpeechConfig(subscription=intent_key, region=intent_service_region)
    audio_config = speechsdk.audio.AudioConfig(filename=lampfilename)

    # Set up the intent recognizer
    intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config, audio_config=audio_config)

    # set up the intents that are to be recognized. These can be a mix of simple phrases and
    # intents specified through a LanguageUnderstanding Model.
    model = speechsdk.intent.LanguageUnderstandingModel(app_id=language_understanding_app_id)
    intents = [
        (model, "HomeAutomation.TurnOn"),
        (model, "HomeAutomation.TurnOff"),
        ("This is a test.", "test"),
        ("Switch the channel to 34.", "34"),
        ("what's the weather like", "weather"),
    ]
    intent_recognizer.add_intents(intents)

    # Connect callback functions to the signals the intent recognizer fires.
    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous recognition upon receiving an event `evt`"""
        print('CLOSING on {}'.format(evt))
        nonlocal done
        done = True

    intent_recognizer.session_started.connect(lambda evt: print("SESSION_START: {}".format(evt)))
    intent_recognizer.speech_end_detected.connect(lambda evt: print("SPEECH_END_DETECTED: {}".format(evt)))
    # event for intermediate results
    intent_recognizer.recognizing.connect(lambda evt: print("RECOGNIZING: {}".format(evt)))
    # event for final result
    intent_recognizer.recognized.connect(lambda evt: print(
        "RECOGNIZED: {}\n\tText: {} (Reason: {})\n\tIntent Id: {}\n\tIntent JSON: {}".format(
            evt, evt.result.text, evt.result.reason, evt.result.intent_id, evt.result.intent_json)))

    # cancellation event
    intent_recognizer.canceled.connect(lambda evt: print(f"CANCELED: {evt.cancellation_details} ({evt.reason})"))

    # stop continuous recognition on session stopped, end of speech or canceled events
    intent_recognizer.session_stopped.connect(stop_cb)
    intent_recognizer.speech_end_detected.connect(stop_cb)
    intent_recognizer.canceled.connect(stop_cb)

    # And finally run the intent recognizer. The output of the callbacks should be printed to the console.
    intent_recognizer.start_continuous_recognition()
    while not done:
        time.sleep(.5)

    intent_recognizer.stop_continuous_recognition()
    # </IntentContinuousRecognitionWithFile>
