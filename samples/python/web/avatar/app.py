# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license.

import azure.cognitiveservices.speech as speechsdk
import json
import os
import requests
import traceback
from flask import Flask, Response, render_template, request

app = Flask(__name__, template_folder='.')

# Environment variables
speech_region = os.environ.get('SPEECH_REGION')
speech_key = os.environ.get('SPEECH_KEY')

# Global variables
speech_synthesizer = None
ice_token = None

@app.route("/")
def index():
    return render_template("basic.html", methods=["GET"])

@app.route("/api/getIceToken", methods=["GET"])
def getIceToken() -> Response:
    global ice_token
    try:
        private_endpoint = request.headers.get('PrivateEndpoint')
        ice_token = getIceTokenInternal(private_endpoint)
        return Response(ice_token, status=200)
    
    except:
        return Response(traceback.format_exc(), status=400)

@app.route("/api/connectAvatar", methods=["POST"])
def connectAvatar() -> Response:
    global ice_token
    global speech_synthesizer
    try:
        private_endpoint = request.headers.get('PrivateEndpoint')
        if private_endpoint:
            if private_endpoint.startswith('https://'):
                private_endpoint = private_endpoint.replace('https://', '')
            speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=f'wss://{private_endpoint}/tts/cognitiveservices/websocket/v1?enableTalkingAvatar=true')
        else:
            speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=f'wss://{speech_region}.tts.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true')

        custom_voice_endpoint_id = request.headers.get('CustomVoiceEndpointId')
        if custom_voice_endpoint_id:
            speech_config.endpoint_id = custom_voice_endpoint_id

        speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=None)

        if not ice_token:
            ice_token = getIceTokenInternal(private_endpoint)
        
        ice_token_obj = json.loads(ice_token)
        local_sdp = request.headers.get('LocalSdp')
        avatar_character = request.headers.get('AvatarCharacter')
        avatar_style = request.headers.get('AvatarStyle')
        background_color = request.headers.get('BackgroundColor')
        is_custom_avatar = request.headers.get('IsCustomAvatar')
        transparent_background = request.headers.get('TransparentBackground')
        video_crop = request.headers.get('VideoCrop')
        avatar_config = {
            'synthesis': {
                'video': {
                    'protocol': {
                        'name': "WebRTC",
                        'webrtcConfig': {
                            'clientDescription': local_sdp,
                            'iceServers': [{
                                'urls': [ ice_token_obj['Urls'][0] ],
                                'username': ice_token_obj['Username'],
                                'credential': ice_token_obj['Password']
                            }]
                        },
                    },
                    'format':{
                        'crop':{
                            'topLeft':{
                                'x': 600 if video_crop.lower() == 'true' else 0,
                                'y': 0
                            },
                            'bottomRight':{
                                'x': 1320 if video_crop.lower() == 'true' else 1920,
                                'y': 1080
                            }
                        },
                        'bitrate': 2000000
                    },
                    'talkingAvatar': {
                        'customized': is_custom_avatar.lower() == 'true',
                        'character': avatar_character,
                        'style': avatar_style,
                        'background': {
                            'color': '#00FF00FF' if transparent_background.lower() == 'true' else background_color
                        }
                    }
                }
            }
        }
        
        connection = speechsdk.Connection.from_speech_synthesizer(speech_synthesizer)
        connection.set_message_property('speech.config', 'context', json.dumps(avatar_config))

        speech_sythesis_result = speech_synthesizer.speak_text_async('').get()
        print(f'Result ID: {speech_sythesis_result.result_id}')
        if speech_sythesis_result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = speech_sythesis_result.cancellation_details
            print(f"Speech synthesis canceled: {cancellation_details.reason}")
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                print(f"Error details: {cancellation_details.error_details}")

        turn_start_message = speech_synthesizer.properties.get_property_by_name('SpeechSDKInternal-ExtraTurnStartMessage')
        remoteSdp = json.loads(turn_start_message)['webrtc']['connectionString']

        return Response(remoteSdp, status=200)

    except:
        return Response(traceback.format_exc(), status=400)
    
@app.route("/api/speak", methods=["POST"])
def speak() -> Response:
    global speech_synthesizer
    try:
        ssml = request.data.decode('utf-8')
        speech_sythesis_result = speech_synthesizer.speak_ssml_async(ssml).get()
        if speech_sythesis_result.reason == speechsdk.ResultReason.Canceled:
            cancellation_details = speech_sythesis_result.cancellation_details
            print(f"Speech synthesis canceled: {cancellation_details.reason}")
            if cancellation_details.reason == speechsdk.CancellationReason.Error:
                print(f"Error details: {cancellation_details.error_details}")
        return Response(speech_sythesis_result.result_id, status=200)
    except:
        return Response(speech_sythesis_result.result_id, status=400)

@app.route("/api/stopSpeaking", methods=["POST"])
def stopSpeaking() -> Response:
    global speech_synthesizer
    try:
        speech_synthesizer.stop_speaking_async().get()
        return Response('Stopped speaking', status=200)
    except:
        return Response(traceback.format_exc(), status=400)

@app.route("/api/disconnectAvatar", methods=["POST"])
def disconnectAvatar() -> Response:
    global speech_synthesizer
    try:
        connection = speechsdk.Connection.from_speech_synthesizer(speech_synthesizer)
        connection.close()
        return Response('Disconnected avatar', status=200)
    except:
        return Response(traceback.format_exc(), status=400)

def getIceTokenInternal(private_endpoint: str) -> str:
    if private_endpoint:
        if not private_endpoint.startswith('https://'):
            private_endpoint = f'https://{private_endpoint}'
        ice_token_response = requests.get(f'{private_endpoint}/tts/cognitiveservices/avatar/relay/token/v1', headers={'Ocp-Apim-Subscription-Key': speech_key})
    else:
        ice_token_response = requests.get(f'https://{speech_region}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1', headers={'Ocp-Apim-Subscription-Key': speech_key})

    return ice_token_response.text
