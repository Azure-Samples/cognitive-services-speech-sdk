# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license.

import azure.cognitiveservices.speech as speechsdk
import datetime
import html
import json
import os
import random
import re
import requests
import threading
import time
import traceback
from flask import Flask, Response, render_template, request

# Create the Flask app
app = Flask(__name__, template_folder='.')

# Environment variables
# Speech resource
speech_region = os.environ.get('SPEECH_REGION') # e.g. westus2
speech_key = os.environ.get('SPEECH_KEY')
speech_private_endpoint = os.environ.get('SPEECH_PRIVATE_ENDPOINT') # e.g. https://my-speech-service.cognitiveservices.azure.com/
# OpenAI resource
azure_openai_endpoint = os.environ.get('AZURE_OPENAI_ENDPOINT') # e.g. https://my-aoai.openai.azure.com/
azure_openai_api_key = os.environ.get('AZURE_OPENAI_API_KEY')
azure_openai_deployment_name = os.environ.get('AZURE_OPENAI_DEPLOYMENT_NAME') # e.g. my-gpt-35-turbo-deployment
# Cognitive search resource
cognitive_search_endpoint = os.environ.get('COGNITIVE_SEARCH_ENDPOINT') # e.g. https://my-cognitive-search.search.windows.net/
cognitive_search_api_key = os.environ.get('COGNITIVE_SEARCH_API_KEY')
cognitive_search_index_name = os.environ.get('COGNITIVE_SEARCH_INDEX_NAME') # e.g. my-search-index

# Configurations which can be overridden by the client
tts_voice = 'en-US-JennyMultilingualV2Neural'
personal_voice_speaker_profile_id = None

# Global variables
speech_synthesizer = None
speech_token = None
ice_token = None
chat_initiated = False
messages = []
data_sources = []
sentence_level_punctuations = [ '.', '?', '!', ':', ';', '。', '？', '！', '：', '；' ]
enable_quick_reply = False
quick_replies = [ 'Let me take a look.', 'Let me check.', 'One moment, please.' ]
byod_doc_regex = re.compile(r'\[doc(\d+)\]')
is_speaking = False
spoken_text_queue = []
last_speak_time = datetime.datetime.now()

# The default route, which shows the default web page (basic.html)
@app.route("/")
def index():
    return render_template("basic.html", methods=["GET"])

# The basic route, which shows the basic web page
@app.route("/basic")
def basicView():
    return render_template("basic.html", methods=["GET"])

@app.route("/chat")
def chatView():
    return render_template("chat.html", methods=["GET"])

@app.route("/api/getSpeechToken", methods=["GET"])
def getSpeechToken() -> Response:
    global speech_token
    response = Response(speech_token, status=200)
    response.headers['SpeechRegion'] = speech_region
    return response

# The API route to get the ICE token
@app.route("/api/getIceToken", methods=["GET"])
def getIceToken() -> Response:
    return Response(ice_token, status=200)

# The API route to connect the TTS avatar
@app.route("/api/connectAvatar", methods=["POST"])
def connectAvatar() -> Response:
    global ice_token
    global speech_synthesizer
    global last_speak_time
    try:
        if speech_private_endpoint:
            speech_private_endpoint_wss = speech_private_endpoint.replace('https://', 'wss://')
            speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=f'{speech_private_endpoint_wss}/tts/cognitiveservices/websocket/v1?enableTalkingAvatar=true')
        else:
            speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=f'wss://{speech_region}.tts.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true')

        custom_voice_endpoint_id = request.headers.get('CustomVoiceEndpointId')
        if custom_voice_endpoint_id:
            speech_config.endpoint_id = custom_voice_endpoint_id

        speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=None)
        
        ice_token_obj = json.loads(ice_token)
        local_sdp = request.headers.get('LocalSdp')
        avatar_character = request.headers.get('AvatarCharacter')
        avatar_style = request.headers.get('AvatarStyle')
        background_color = '#FFFFFFFF' if request.headers.get('BackgroundColor') is None else request.headers.get('BackgroundColor')
        is_custom_avatar = request.headers.get('IsCustomAvatar')
        transparent_background = 'false' if request.headers.get('TransparentBackground') is None else request.headers.get('TransparentBackground')
        video_crop = 'false' if request.headers.get('VideoCrop') is None else request.headers.get('VideoCrop')
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
                raise Exception(cancellation_details.error_details)
        turn_start_message = speech_synthesizer.properties.get_property_by_name('SpeechSDKInternal-ExtraTurnStartMessage')
        remoteSdp = json.loads(turn_start_message)['webrtc']['connectionString']
        last_speak_time = datetime.datetime.now()

        return Response(remoteSdp, status=200)

    except Exception as e:
        return Response(f"Result ID: {speech_sythesis_result.result_id}. Error message: {e}", status=400)

# The API route to speak a given SSML
@app.route("/api/speak", methods=["POST"])
def speak() -> Response:
    global speech_synthesizer
    try:
        ssml = request.data.decode('utf-8')
        result_id = speakSsml(ssml)
        return Response(result_id, status=200)
    except Exception as e:
        return Response(f"Result ID: {result_id}. Error message: {e}", status=400)

@app.route("/api/chat", methods=["POST"])
def chat() -> Response:
    global chat_initiated
    if not chat_initiated:
        initializeChatContext(request.headers.get('SystemPrompt'))
        chat_initiated = True
    user_query = request.data.decode('utf-8')
    return Response(handleUserQuery(user_query), mimetype='text/plain', status=200)

# The API route to disconnect the TTS avatar
@app.route("/api/disconnectAvatar", methods=["POST"])
def disconnectAvatar() -> Response:
    global speech_synthesizer
    try:
        connection = speechsdk.Connection.from_speech_synthesizer(speech_synthesizer)
        connection.close()
        return Response('Disconnected avatar', status=200)
    except:
        return Response(traceback.format_exc(), status=400)

def refreshIceToken() -> None:
    global ice_token
    if speech_private_endpoint:
        ice_token = requests.get(f'{speech_private_endpoint}/tts/cognitiveservices/avatar/relay/token/v1', headers={'Ocp-Apim-Subscription-Key': speech_key}).text
    else:
        ice_token = requests.get(f'https://{speech_region}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1', headers={'Ocp-Apim-Subscription-Key': speech_key}).text

def refreshSpeechToken() -> None:
    global speech_token
    while True:
        # Refresh the speech token every 9 minutes
        speech_token = requests.post(f'https://{speech_region}.api.cognitive.microsoft.com/sts/v1.0/issueToken', headers={'Ocp-Apim-Subscription-Key': speech_key}).text
        time.sleep(60 * 9)

def initializeChatContext(system_prompt: str) -> None:
    global messages
    global data_sources
    global enable_quick_reply
    global is_speaking
    global byod_doc_regex

    # Initialize data sources for 'bring your data' scenario
    data_sources = []
    if cognitive_search_endpoint and cognitive_search_api_key and cognitive_search_index_name:
        # On-your-data scenario
        data_source = {
            'type': 'AzureCognitiveSearch',
            'parameters': {
                'endpoint': cognitive_search_endpoint,
                'key': cognitive_search_api_key,
                'indexName': cognitive_search_index_name,
                'semanticConfiguration': '',
                'queryType': 'simple',
                'fieldsMapping': {
                    'contentFieldsSeparator': '\n',
                    'contentFields': ['content'],
                    'filepathField': None,
                    'titleField': 'title',
                    'urlField': None
                },
                'inScope': True,
                'roleInformation': system_prompt
            }
        }
        data_sources.append(data_source)

    # Initialize messages
    messages = []
    if len(data_sources) == 0:
        system_message = {
            'role': 'system',
            'content': system_prompt
        }
        messages.append(system_message)

# Handle the user query and return the assistant reply. For chat scenario.
# The function is a generator, which yields the assistant reply in chunks.
def handleUserQuery(user_query: str):
    global messages
    global data_sources
    global is_speaking
    global spoken_text_queue

    chat_message = {
        'role': 'user',
        'content': user_query
    }

    messages.append(chat_message)

    # Stop previous speaking if there is any
    if is_speaking:
        # To-do: also stop the current speaking after stop speaking is supported
        spoken_text_queue = []

    # For 'bring your data' scenario, chat API currently has long (4s+) latency
    # We return some quick reply here before the chat API returns to mitigate.
    if len(data_sources) > 0 and enable_quick_reply:
        speak(random.choice(quick_replies), 2000)

    url = f"{azure_openai_endpoint}/openai/deployments/{azure_openai_deployment_name}/chat/completions?api-version=2023-06-01-preview"
    body = json.dumps({
        'messages': messages,
        'stream': True
    })

    if len(data_sources) > 0:
        url = f"{azure_openai_endpoint}/openai/deployments/{azure_openai_deployment_name}/extensions/chat/completions?api-version=2023-06-01-preview"
        body = json.dumps({
            'dataSources': data_sources,
            'messages': messages,
            'stream': True
        })

    assistant_reply = ''
    tool_content = ''
    spoken_sentence = ''
    display_sentence = ''

    response = requests.post(url, stream=True, headers={
        'api-key': azure_openai_api_key,
        'Content-Type': 'application/json'
    }, data=body)

    if not response.ok:
        raise Exception(f"Chat API response status: {response.status_code} {response.reason}")

    # Iterate chunks from the response stream
    iterator = response.iter_content(chunk_size=None)
    for chunk in iterator:
        if not chunk:
            # End of stream
            return

        # Process the chunk of data (value)
        chunk_string = chunk.decode()

        if not chunk_string.endswith('}\n\n') and not chunk_string.endswith('[DONE]\n\n'):
            # This is an incomplete chunk, read the next chunk
            while not chunk_string.endswith('}\n\n') and not chunk_string.endswith('[DONE]\n\n'):
                chunk_string += next(iterator).decode()

        for line in chunk_string.split('\n\n'):
            try:
                if line.startswith('data:') and not line.endswith('[DONE]'):
                    response_json = json.loads(line[5:].strip())
                    response_token = None
                    if len(response_json['choices']) > 0:
                        choice = response_json['choices'][0]
                        if len(data_sources) == 0:
                            if len(choice['delta']) > 0 and 'content' in choice['delta']:
                                response_token = choice['delta']['content']
                        elif len(choice['messages']) > 0 and 'delta' in choice['messages'][0]:
                            delta = choice['messages'][0]['delta']
                            if 'role' in delta and delta['role'] == 'tool' and 'content' in delta:
                                tool_content = response_json['choices'][0]['messages'][0]['delta']['content']
                            elif 'content' in delta:
                                response_token = response_json['choices'][0]['messages'][0]['delta']['content']
                                if response_token is not None:
                                    if byod_doc_regex.search(response_token):
                                        response_token = byod_doc_regex.sub('', response_token).strip()
                                    if response_token == '[DONE]':
                                        response_token = None

                    if response_token is not None:
                        # print(f'Current token: {response_token}')
                        yield response_token
                        assistant_reply += response_token  # build up the assistant message
                        display_sentence += response_token  # build up the display sentence
                        if response_token == '\n' or response_token == '\n\n':
                            speakWithQueue(spoken_sentence.strip())
                            # print(f"Spoken sentence: {spoken_sentence.strip()}")
                            spoken_sentence = ''
                        else:
                            response_token = response_token.replace('\n', '')
                            spoken_sentence += response_token  # build up the spoken sentence
                            if len(response_token) == 1 or len(response_token) == 2:
                                for punctuation in sentence_level_punctuations:
                                    if response_token.startswith(punctuation):
                                        speakWithQueue(spoken_sentence.strip())
                                        # print(f"Spoken sentence: {spoken_sentence.strip()}")
                                        spoken_sentence = ''
                                        break
            except Exception as e:
                print(f"Error occurred while parsing the response: {e}")
                print(line)

        # print(f'Display sentence: {display_sentence}')
        # yield display_sentence
        display_sentence = ''

    if spoken_sentence != '':
        speakWithQueue(spoken_sentence.strip())
        print(f"Spoken sentence: {spoken_sentence.strip()}")
        spoken_sentence = ''

    if len(data_sources) > 0:
        tool_message = {
            'role': 'tool',
            'content': tool_content
        }
        messages.append(tool_message)

    assistant_message = {
        'role': 'assistant',
        'content': assistant_reply
    }
    messages.append(assistant_message)

# Speak the given text. If there is already a speaking in progress, add the text to the queue. For chat scenario.
def speakWithQueue(text: str, ending_silence_ms: int = 0) -> None:
    global is_speaking
    global spoken_text_queue
    if is_speaking:
        spoken_text_queue.append(text)
        return
    speakNext(text, ending_silence_ms)

# Speak the given text, after the speaking is done, speak the next text in the queue. For chat scenario.
def speakNext(text: str, ending_silence_ms: int = 0) -> None:
    global tts_voice
    global personal_voice_speaker_profile_id
    global is_speaking
    global last_speak_time
    ssml = f"""<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                 <voice name='{tts_voice}'>
                     <mstts:ttsembedding speakerProfileId='{personal_voice_speaker_profile_id}'>
                         <mstts:leadingsilence-exact value='0'/>
                         {html.escape(text)}
                     </mstts:ttsembedding>
                 </voice>
               </speak>"""
    if ending_silence_ms > 0:
        ssml = f"""<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                     <voice name='{tts_voice}'>
                         <mstts:ttsembedding speakerProfileId='{personal_voice_speaker_profile_id}'>
                             <mstts:leadingsilence-exact value='0'/>
                             {html.escape(text)}
                             <break time='{ending_silence_ms}ms' />
                         </mstts:ttsembedding>
                     </voice>
                   </speak>"""
    last_speak_time = datetime.datetime.now()
    is_speaking = True
    try:
        speakSsml(ssml)
        if len(spoken_text_queue) > 0:
            speakNext(spoken_text_queue.pop(0))
        else:
            is_speaking = False
    except Exception as e:
        is_speaking = False
        print(f"Error occurred while speaking: {e}")
    return

# Speak the given ssml with speech sdk
def speakSsml(ssml: str) -> str:
    speech_sythesis_result = speech_synthesizer.speak_ssml_async(ssml).get()
    if speech_sythesis_result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = speech_sythesis_result.cancellation_details
        print(f"Speech synthesis canceled: {cancellation_details.reason}")
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print(f"Error details: {cancellation_details.error_details}")
            raise Exception(cancellation_details.error_details)
    return speech_sythesis_result.result_id

# Start the speech token refresh thread
speechTokenRefereshThread = threading.Thread(target=refreshSpeechToken)
speechTokenRefereshThread.daemon = True
speechTokenRefereshThread.start()

# Fetch ICE token at startup
refreshIceToken()
