#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Speech synthesis sample with style auto prediction from GPT
"""

try:
    import azure.cognitiveservices.speech as speechsdk
except ImportError:
    print("""
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-python for
    installation instructions.
    """)
    import sys
    sys.exit(1)

import json
import os
import openai
openai.api_type = "azure"
openai.api_base = os.getenv("AZURE_OPENAI_ENDPOINT")
openai.api_version = "2023-05-15"
openai.api_key = os.getenv("AZURE_OPENAI_KEY")

STYLE_PROMPT = """You are requested to split the following input into sentences and tag a correct style from {0} and `general`.
Please response with json format, e.g., [{{"sentence": text, "style": style }}]
Input:  {1}"""

SPEECH_REGION = os.getenv("SPEECH_REGION")
SPEECH_KEY = os.getenv("SPEECH_KEY")

# Set the voice you want to use
VOICE = "en-US-JennyNeural"

# paste or read the text you want to synthesize
TEXT = """Elizabeth Bennet had been obliged, by the scarcity of gentlemen, to sit down for two dances; and during part of that time, Mr. Darcy had been standing near enough for her to overhear a conversation between him and Mr. Bingley, who came from the dance for a few minutes to press his friend to join it.

“Come, Darcy,” said he, “I must have you dance. I hate to see you standing about by yourself in this stupid manner. You had much better dance.”

“I certainly shall not. You know how I detest it, unless I am particularly acquainted with my partner. At such an assembly as this, it would be insupportable. Your sisters are engaged, and there is not another woman in the room whom it would not be a punishment to me to stand up with.”

“I would not be so fastidious as you are,” cried Bingley, “for a kingdom! Upon my honour, I never met with so many pleasant girls in my life as I have this evening; and there are several of them, you see, uncommonly pretty.”

“You are dancing with the only handsome girl in the room,” said Mr. Darcy, looking at the eldest Miss Bennet.

“Oh, she is the most beautiful creature I ever beheld! But there is one of her sisters sitting down just behind you, who is very pretty, and I dare say very agreeable. Do let me ask my partner to introduce you.”"""


if __name__ == "__main__":
    speech_config = speechsdk.SpeechConfig(subscription=SPEECH_KEY, region=SPEECH_REGION)
    speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Audio24Khz96KBitRateMonoMp3)
    speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=None)
    voices = speech_synthesizer.get_voices_async().get().voices
    for v in voices:
        if VOICE == v.name or VOICE == v.short_name:
            styles = v.style_list
            break
    else:
        print(f"The voice {VOICE} is not found")
        exit(1)

    if len(styles) == 0:
        print(f"The voice {VOICE} does not support styles")
        exit(1)

    # refer to AOAI document: https://learn.microsoft.com/azure/cognitive-services/openai/chatgpt-quickstart?tabs=command-line&pivots=programming-language-python
    aoi_response = openai.ChatCompletion.create(
        engine="gpt-35-turbo",  # engine = "deployment_name".
        messages=[
            {"role": "system", "content": "You are an assistant."},
            {"role": "user", "content": STYLE_PROMPT.format(styles, TEXT)},
        ]
    )

    prediction = aoi_response['choices'][0]['message']['content']
    print("Style prediction: {}".format(prediction))
    prediction = json.loads(prediction)
    # style elements in SSML: https://learn.microsoft.com/azure/cognitive-services/speech-service/speech-synthesis-markup-voice#style-and-degree-example
    ssml = "<speak version='1.0' xmlns='https://www.w3.org/2001/10/synthesis' xmlns:mstts='https://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='{}'>{}</voice></speak>"
    style_elements = ""
    for s in prediction:
        style_elements += "<mstts:express-as style='{}'>{}</mstts:express-as>".format(s["style"], s["sentence"])

    ssml = ssml.format(VOICE, style_elements)
    result = speech_synthesizer.speak_ssml_async(ssml).get()
    if result.reason == speechsdk.ResultReason.Canceled:
        print("Speech synthesis canceled: {}".format(result.cancellation_details.error_details))
        exit(1)
    audio_data_stream = speechsdk.AudioDataStream(result)
    # save the audio file
    audio_data_stream.save_to_wav_file("output.mp3")
