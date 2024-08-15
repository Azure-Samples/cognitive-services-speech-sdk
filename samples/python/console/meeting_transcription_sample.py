#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
"""
Meeting transcription samples for the Microsoft Cognitive Services Speech SDK
"""

import time
import uuid

from scipy.io import wavfile

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
# Replace with your own subscription key and service region (e.g., "centralus").
# See the limitations in supported regions,
# https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-meeting-transcription
speech_key, service_region = "YourSubscriptionKey", "YourServiceRegion"

# This sample uses a wavfile which is captured using a supported Speech SDK devices (8 channel, 16kHz, 16-bit PCM)
# See https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk-microphone
meetingfilename = "YourMeetingWavFile"


# This sample demonstrates how to differentiate speakers using meeting transcription service.
# Differentiation of speakers do not require voice signatures. In case more enhanced speaker identification is required,
# please use https://signature.centralus.cts.speech.microsoft.com/UI/index.html REST API to create your own voice signatures
def meeting_transcription_differentiate_speakers():
    """differentiates speakers using meeting transcription service"""
    # Creates speech configuration with subscription information
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)
    speech_config.set_property_by_name("ConversationTranscriptionInRoomAndOnline", "true")
    speech_config.set_property_by_name("DifferentiateGuestSpeakers", "true")

    channels = 8
    bits_per_sample = 16
    samples_per_second = 16000

    # Create audio configuration using the push stream
    wave_format = speechsdk.audio.AudioStreamFormat(samples_per_second, bits_per_sample, channels)
    stream = speechsdk.audio.PushAudioInputStream(stream_format=wave_format)
    audio_config = speechsdk.audio.AudioConfig(stream=stream)

    # Meeting identifier is required when creating meeting.
    meeting_id = str(uuid.uuid4())
    meeting = speechsdk.transcription.Meeting(speech_config, meeting_id)
    transcriber = speechsdk.transcription.MeetingTranscriber(audio_config)

    done = False

    def stop_cb(evt: speechsdk.SessionEventArgs):
        """callback that signals to stop continuous transcription upon receiving an event `evt`"""
        print('CLOSING {}'.format(evt))
        nonlocal done
        done = True

    # Subscribe to the events fired by the meeting transcriber
    transcriber.transcribed.connect(lambda evt: print('TRANSCRIBED: {}'.format(evt)))
    transcriber.session_started.connect(lambda evt: print('SESSION STARTED: {}'.format(evt)))
    transcriber.session_stopped.connect(lambda evt: print('SESSION STOPPED {}'.format(evt)))
    transcriber.canceled.connect(lambda evt: print('CANCELED {}'.format(evt)))
    # stop continuous transcription on either session stopped or canceled events
    transcriber.session_stopped.connect(stop_cb)
    transcriber.canceled.connect(stop_cb)

    # Add participants to the meeting.
    # Note user voice signatures are not required for speaker differentiation.
    # Use voice signatures when adding participants when more enhanced speaker identification is required.
    katie = speechsdk.transcription.Participant("katie@example.com", "en-us")
    stevie = speechsdk.transcription.Participant("stevie@example.com", "en-us")

    meeting.add_participant_async(katie).get()
    meeting.add_participant_async(stevie).get()
    transcriber.join_meeting_async(meeting).get()
    transcriber.start_transcribing_async()

    # Read the whole wave files at once and stream it to sdk
    _, wav_data = wavfile.read(meetingfilename)
    stream.write(wav_data.tobytes())
    stream.close()
    while not done:
        time.sleep(.5)

    transcriber.stop_transcribing_async()
