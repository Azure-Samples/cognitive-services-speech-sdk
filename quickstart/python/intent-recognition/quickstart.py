# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

# <skeleton>
import azure.cognitiveservices.speech as speechsdk

print("Say something...")
# </skeleton>

"""performs one-shot intent recognition from input from the default microphone"""

# <create_speech_configuration>
# Set up the config for the intent recognizer (remember that this uses the Language Understanding key, not the Speech Services key)!
intent_config = speechsdk.SpeechConfig(
    subscription="YourLanguageUnderstandingSubscriptionKey",
    region="YourLanguageUnderstandingServiceRegion")
# </create_speech_configuration>

# <create_intent_recognizer>
# Set up the intent recognizer
intent_recognizer = speechsdk.intent.IntentRecognizer(speech_config=intent_config)
# </create_intent_recognizer>

# <add_intents>
# set up the intents that are to be recognized. These can be a mix of simple phrases and
# intents specified through a LanguageUnderstanding Model.
model = speechsdk.intent.LanguageUnderstandingModel(app_id="YourLanguageUnderstandingAppId")
intents = [
    (model, "HomeAutomation.TurnOn"),
    (model, "HomeAutomation.TurnOff"),
    ("This is a test.", "test"),
    ("Switch to channel 34.", "34"),
    ("what's the weather like", "weather"),
]
intent_recognizer.add_intents(intents)
# </add_intents>

# To add all of the possible intents from a LUIS model to the recognizer, uncomment the line below:
# intent_recognizer.add_all_intents(model)

# Starts intent recognition, and returns after a single utterance is recognized. The end of a
# single utterance is determined by listening for silence at the end or until a maximum of about 30
# seconds of audio is processed. It returns the recognition text as result.
# Note: Since recognize_once() returns only a single utterance, it is suitable only for single
# shot recognition like command or query.
# For long-running multi-utterance recognition, use start_continuous_recognition() instead.
# <recognize_intent>
intent_result = intent_recognizer.recognize_once()
# </recognize_intent>

# <print_results>
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
# </print_results>
