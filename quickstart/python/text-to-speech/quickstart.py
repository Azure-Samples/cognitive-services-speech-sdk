# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

# <code>
import azure.cognitiveservices.speech as speechsdk
import json

# Load the configuration from the config.json file
# Creates an instance of a speech config with specified subscription key and endpoint.
# Replace with your own subscription key and endpoint in config.json.
with open('config.json', 'r') as config_file:
    config = json.load(config_file)

# Creates an instance of a speech config with specified endpoint and subscription key.
# Replace with your own endpoint and subscription key in config file.
speech_key = config.get("SubscriptionKey")
speech_endpoint = config.get("Endpoint")
speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)

# Configure the voice for speech synthesis. For a complete list of available voices,
# visit https://aka.ms/speech/voices/neural
#
# To use the latest LLM-based HD neural voice, set the voice name to "en-us-Ava:DragonHDLatestNeural"
# (available in regions such as East US). Alternatively, use standard neural voices like
# "en-US-AriaNeural" or "en-US-AvaMultilingualNeural".
#
# For HD neural voice region availability, see https://aka.ms/speech/regions
# speech_config.speech_synthesis_voice_name = "en-US-AriaNeural"
speech_config.speech_synthesis_voice_name = "en-us-Ava:DragonHDLatestNeural"

# Creates a speech synthesizer using the default speaker as audio output.
speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config)

# Receives a text from console input.
print("Type some text that you want to speak...")
text = input()

# Synthesizes the received text to speech.
# The synthesized speech is expected to be heard on the speaker with this line executed.
result = speech_synthesizer.speak_text_async(text).get()

# Checks result.
if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
    print("Speech synthesized to speaker for text [{}]".format(text))
elif result.reason == speechsdk.ResultReason.Canceled:
    cancellation_details = result.cancellation_details
    print("Speech synthesis canceled: {}".format(cancellation_details.reason))
    if cancellation_details.reason == speechsdk.CancellationReason.Error:
        if cancellation_details.error_details:
            print("Error details: {}".format(cancellation_details.error_details))
    print("Did you update the subscription info?")
# </code>
