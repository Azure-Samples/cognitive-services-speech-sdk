#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import requests
from time import sleep
import os
import logging
import customvoice
import azure.cognitiveservices.speech as speechsdk


def create_personal_voice(project_id: str,
                          consent_id: str, consent_file_path: str, voice_talent_name: str, company_name: str,
                          personal_voice_id: str, audio_folder: str):
    # create project
    project = customvoice.Project.create(config, project_id, customvoice.ProjectKind.PersonalVoice)
    print('Project created. project id: %s' % project.id)

    # upload consent
    consent = customvoice.Consent.create(config, project_id, consent_id, voice_talent_name, company_name, consent_file_path, 'en-us')
    if consent.status == customvoice.Status.Failed:
        print('Create consent failed. consent id: %s' % consent.id)
        raise Exception
    elif consent.status == customvoice.Status.Succeeded:
        print('Create consent succeeded. consent id: %s' % consent.id)

    # create personal voice
    personal_voice = customvoice.PersonalVoice.create(config, project_id, personal_voice_id, consent_id, audio_folder)
    if personal_voice.status == customvoice.Status.Failed:
        print('Create personal voice failed. personal voice id: %s' % personal_voice.id)
        raise Exception
    elif personal_voice.status == customvoice.Status.Succeeded:
        print('Create personal voice succeeded. personal voice id: %s, speaker profile id: %s' % (personal_voice.id, personal_voice.speaker_profile_id))
    return personal_voice.speaker_profile_id


def speech_synthesis_to_wave_file(text: str, output_file_path: str, speaker_profile_id: str):
    # Creates an instance of a speech config with specified subscription key and service region.
    speech_config = speechsdk.SpeechConfig(subscription=config.key, region=config.region)
    speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm)
    file_config = speechsdk.audio.AudioOutputConfig(filename=output_file_path)
    speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=file_config)


    # use PhoenixLatestNeural if you want word boundary event.  We will support events on DragonLatestNeural in the future.
    ssml = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' " \
           "xmlns:mstts='http://www.w3.org/2001/mstts'>" \
           "<voice name='DragonLatestNeural'>" \
           "<mstts:ttsembedding speakerProfileId='%s'/>" \
           "<mstts:express-as style='Prompt'>" \
           "<lang xml:lang='en-US'> %s </lang>" \
           "</mstts:express-as>" \
           "</voice></speak> " % (speaker_profile_id, text)

    def word_boundary(evt):
        print(f"Word Boundary: Text='{evt.text}', Audio offset={evt.audio_offset / 10000}ms, Duration={evt.duration / 10000}ms, text={evt.text}")

    speech_synthesizer.synthesis_word_boundary.connect(word_boundary)
    result = speech_synthesizer.speak_ssml_async(ssml).get()

    # Check result
    if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
        print("Speech synthesized for text [{}], and the audio was saved to [{}]".format(text, output_file_path))
        print("result id: {}".format(result.result_id))
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print("Speech synthesis canceled: {}".format(cancellation_details.reason))
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print("Error details: {}".format(cancellation_details.error_details))
            print("result id: {}".format(result.result_id))


def clean_up(project_id: str, consent_id: str, personal_voice_id: str):
    customvoice.PersonalVoice.delete(config, personal_voice_id)
    customvoice.Consent.delete(config, consent_id)
    customvoice.Project.delete(config, project_id)


region = 'eastus' # eastus, westeurope, southeastasia
key = 'your speech key here'


logging.basicConfig(filename="customvoice.log",
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                    filemode='w')
logger = logging.getLogger()
logger.setLevel(logging.DEBUG)

config = customvoice.Config(key, region, logger)


project_id = 'personal-voice-project-1'
consent_id = 'personal-voice-consent-1'
personal_voice_id  = 'personal-voice-1'

try:
    # step 1: create personal voice
    # Need consent file and audio file to create personal vocie.
    # This is consent file template.
    # I [voice talent name] am aware that recordings of my voice will be used by [company name] to create and use a synthetic version of my voice.
    # You can find sample consent file here
    # https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/VoiceTalentVerbalStatement.wav
    consent_file_path = r'voice\\VoiceTalentVerbalStatement.wav'
    voice_talent_name = 'Sample Voice Actor'
    company_name = 'Contoso'

    # Need 50 - 90 seconds audio file.
    # You can find sample audio file here.
    # https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/SampleAudios.zip
    # Pleae unzip audio file, and put the first 14 files in folder below.
    audio_folder = r'voice\\'
    speaker_profile_id = create_personal_voice(project_id, 
                                            consent_id, consent_file_path, voice_talent_name, company_name,
                                            personal_voice_id, audio_folder)

    # step 2: synthesis wave
    text = 'This is zero shot voice. Test 2.'
    output_wave_file_path = 'output_sdk.wav'
    speech_synthesis_to_wave_file(text, output_wave_file_path, speaker_profile_id)
except Exception as e:
    print(e)
finally:
    # Optional step 3: clean up, if you don't need this voice to synthesis more content.
    clean_up(project_id, consent_id, personal_voice_id)

