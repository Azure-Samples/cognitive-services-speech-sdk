#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

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

# Since we are working with async, we have to monitor until we know we have an answer
done = False

# A successful return from both speech processing and intent classification
def success_callback(args):
    result = args.result
    print("Recognized: \"{}\" with intent id `{}`".format(result.text, result.intent_id))
    global done
    done = True

# A failure by either speech or language processing
def failure_callback(args):
    result = args.result
    print("Intent recognition canceled: {}".format(result.cancellation_details.reason))
    if intent_result.cancellation_details.reason == speechsdk.CancellationReason.Error:
        print("Error details: {}".format(result.cancellation_details.error_details))
    global done
    done = True

# The intent recognizer has started processing
def in_progress_callback(args):
    print("Speech processing is working")
    
def recognize_intent_once_from_mic():
    """performs one-shot intent recognition from input from the default microphone"""
    # <IntentRecognitionOnceWithMic>
    # Set up the config for the intent recognizer (remember that this uses the Language Understanding key, not the Speech Services key)!
    intent_config = speechsdk.SpeechConfig(subscription=intent_key, region=intent_service_region)
    audio_config = speechsdk.audio.AudioConfig(use_default_microphone=True)

    # Set up the intent recognizer
    intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config, audio_config=audio_config)

    # Add the callbacks
    intent_recognizer.recognized.connect(success_callback)
    intent_recognizer.canceled.connect(failure_callback)
    intent_recognizer.recognizing.connect(in_progress_callback)

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
    intent_result = intent_recognizer.recognize_once_async()

    global done

    # wait until we know we are complete
    while not done:
        time.sleep(1)
        print(done)
    
recognize_intent_once_from_mic()