#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import os
import logging
from dotenv import load_dotenv
try:
    import customvoice
except ImportError:
    print('Please copy folder https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/'
          'custom-voice/python/customvoice and keep the same folder structure as github.')
    quit()
import azure.cognitiveservices.speech as speechsdk

# Load .env file from parent directory (samples/custom-voice/.env).
# This also works when running from the command line: python personal_voice_sample.py
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
load_dotenv(os.path.join(_SCRIPT_DIR, '..', '.env'))

# Settings are loaded from environment variables.
# Copy ../.env.sample to ../.env and fill in your values.
# See ../.env.sample for the full list of available variables.

# Speech resource
REGION = os.environ.get('SPEECH_REGION', 'eastus')
SPEECH_KEY = os.environ.get('SPEECH_KEY', '')

# Voice consent file path
CONSENT_FILE_PATH = os.environ.get('CONSENT_FILE_PATH', os.path.join(_SCRIPT_DIR, 'TestData', 'VoiceTalentVerbalStatement.wav'))
VOICE_TALENT_NAME = os.environ.get('VOICE_TALENT_NAME', 'Sample Voice Actor')
COMPANY_NAME = os.environ.get('COMPANY_NAME', 'Contoso')

# Audio folder containing 5-90 seconds audio file(s) for personal voice
AUDIO_FOLDER = os.environ.get('AUDIO_FOLDER', os.path.join(_SCRIPT_DIR, 'TestData', 'voice'))

# Resource IDs (can keep defaults)
LOCALE = os.environ.get('LOCALE', 'en-us')
PROJECT_ID = os.environ.get('PERSONAL_PROJECT_ID', 'personal-voice-project-1')
CONSENT_ID = os.environ.get('PERSONAL_CONSENT_ID', 'personal-voice-consent-1')
PERSONAL_VOICE_ID = os.environ.get('PERSONAL_VOICE_ID', 'personal-voice-1')

# ==================================================================================


def create_personal_voice():
    # create project
    project = customvoice.Project.create(config, PROJECT_ID, customvoice.ProjectKind.PersonalVoice)
    print('Project created. project id: %s' % project.id)

    # upload consent
    consent = customvoice.Consent.create(config, PROJECT_ID, CONSENT_ID, VOICE_TALENT_NAME, COMPANY_NAME,
                                         CONSENT_FILE_PATH, LOCALE)
    if consent.status == customvoice.Status.Failed:
        print('Create consent failed. consent id: %s' % consent.id)
        raise Exception
    elif consent.status == customvoice.Status.Succeeded:
        print('Create consent succeeded. consent id: %s' % consent.id)

    # create personal voice
    personal_voice = customvoice.PersonalVoice.create(config, PROJECT_ID, PERSONAL_VOICE_ID, CONSENT_ID, AUDIO_FOLDER)
    if personal_voice.status == customvoice.Status.Failed:
        print('Create personal voice failed. personal voice id: %s' % personal_voice.id)
        raise Exception
    elif personal_voice.status == customvoice.Status.Succeeded:
        print('Create personal voice succeeded. personal voice id: %s, speaker profile id: %s' %
              (personal_voice.id, personal_voice.speaker_profile_id))
    return personal_voice.speaker_profile_id


def speech_synthesis_to_wave_file(text: str, output_file_path: str, speaker_profile_id: str):
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
        print(f"Word Boundary: Text='{evt.text}', Audio offset={evt.audio_offset / 10000}ms, "
              f"Duration={evt.duration / 10000}ms")

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


def clean_up():
    customvoice.PersonalVoice.delete(config, PERSONAL_VOICE_ID)
    customvoice.Consent.delete(config, CONSENT_ID)
    customvoice.Project.delete(config, PROJECT_ID)


if __name__ == '__main__':
    # Validate required settings
    if not SPEECH_KEY:
        print('ERROR: SPEECH_KEY environment variable is not set.')
        print()
        print('Please copy .env.sample to .env and fill in your values, or set them in .vscode/launch.json.')
        quit()

    logging.basicConfig(filename="customvoice.log",
                        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                        filemode='w')
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)

    config = customvoice.Config(SPEECH_KEY, REGION, logger)

    try:
        # step 1: create personal voice
        speaker_profile_id = create_personal_voice()

        # step 2: synthesis wave
        text = 'This is zero shot voice. Test 2.'
        output_wave_file_path = 'output_sdk.wav'
        speech_synthesis_to_wave_file(text, output_wave_file_path, speaker_profile_id)
    except Exception as e:
        print(e)
    finally:
        # Optional step 3: clean up, if you don't need this voice to synthesize more content.
        clean_up()
