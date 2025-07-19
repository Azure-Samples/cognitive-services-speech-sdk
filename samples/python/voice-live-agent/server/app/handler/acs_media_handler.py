import asyncio
import base64
import json
import uuid
from websockets.asyncio.client import connect as ws_connect
from websockets.typing import Data
from azure.identity.aio import ManagedIdentityCredential

def session_config():
    return {
        "type": "session.update",
        "session": {
            "instructions": "You are a helpful AI assistant responding in natural, engaging language.",
            "turn_detection": {
                "type": "azure_semantic_vad",
                "threshold": 0.3,
                "prefix_padding_ms": 200,
                "silence_duration_ms": 200,
                "remove_filler_words": False,
                "end_of_utterance_detection": {
                    "model": "semantic_detection_v1",
                    "threshold": 0.01,
                    "timeout": 2,
                },
            },
            "input_audio_noise_reduction": {"type": "azure_deep_noise_suppression"},
            "input_audio_echo_cancellation": {"type": "server_echo_cancellation"},
            "voice": {
                "name": "en-US-Aria:DragonHDLatestNeural",
                "type": "azure-standard",
                "temperature": 0.8
            }
        }
    }

class ACSMediaHandler:
    incoming_websocket = None
    ws = None
    send_queue = None
    send_task = None
    is_raw_audio = True

    def __init__(self, config):
        self.endpoint = config["AZURE_VOICE_LIVE_ENDPOINT"]
        self.model = config["VOICE_LIVE_MODEL"]
        self.api_key = config["AZURE_VOICE_LIVE_API_KEY"]
        self.client_id = config["AZURE_USER_ASSIGNED_IDENTITY_CLIENT_ID"]
        self.send_queue = asyncio.Queue()

    def _generate_guid(self):
        return str(uuid.uuid4())

    # API for now, Use Voice Live SDK once that is ready
    async def connect(self):
        url = f"{self.endpoint}/voice-live/realtime?api-version=2025-05-01-preview&model={self.model}"
        url = url.replace("https://", "wss://")

        headers = {
            "x-ms-client-request-id": self._generate_guid(),
        }

        if self.client_id:
            scopes = "https://cognitiveservices.azure.com/.default"
            # Use the user-assigned managed identity
            credential = ManagedIdentityCredential(
                managed_identity_client_id=self.client_id
            )
            token = await credential.get_token(scopes)
            headers["Authorization"] = f"Bearer {token.token}"
            print(token)
            print(self.endpoint)
            print(url)
            print(headers)
        else:
            headers["api-key"] = self.api_key

        self.ws = await ws_connect(url, additional_headers=headers)
        print("[VoiceLiveACSHandler] Connected to Voice Live API")

        # Send session config
        await self._send_json(session_config())
        await self._send_json({"type": "response.create"})

        # Start receiver and sender loops
        asyncio.create_task(self._receiver_loop())
        self.send_task = asyncio.create_task(self._sender_loop())

    async def init_incoming_websocket(self, socket, is_raw_audio = True):
        self.incoming_websocket = socket
        self.is_raw_audio = is_raw_audio

    async def audio_to_voicelive(self, audio_b64: str):
        await self.send_queue.put(json.dumps({
            "type": "input_audio_buffer.append",
            "audio": audio_b64
        }))

    async def _send_json(self, obj):
        if self.ws:
            await self.ws.send(json.dumps(obj))

    async def _sender_loop(self):
        try:
            while True:
                msg = await self.send_queue.get()
                if self.ws:
                    await self.ws.send(msg)
        except Exception as e:
            print(f"[VoiceLiveACSHandler] Sender loop error: {e}")

    async def _receiver_loop(self):
        try:
            async for message in self.ws:
                event = json.loads(message)
                event_type = event.get("type")

                match event_type:
                    case "session.created":
                        session_id = event.get("session", {}).get("id")
                        print(f"[VoiceLiveACSHandler] Session ID: {session_id}")

                    case "input_audio_buffer.cleared":
                        print("Input Audio Buffer Cleared Message")

                    case "input_audio_buffer.speech_started":
                        audio_start_ms = event.get("audio_start_ms")
                        print(f"Voice activity detection started at {audio_start_ms} [ms]")
                        await self.stop_audio()

                    case "input_audio_buffer.speech_stopped":
                        print("Speech stopped")

                    case "conversation.item.input_audio_transcription.completed":
                        transcript = event.get("transcript")
                        print(f" User:-- {transcript}")

                    case "conversation.item.input_audio_transcription.failed":
                        error_msg = event.get("error")
                        print(f"  Error: {error_msg}")

                    case "response.done":
                        print("Response Done Message")
                        response = event.get("response", {})
                        print(f"  Response Id: {response.get('id')}")
                        if response.get("status_details"):
                            print(f"  Status Details: {json.dumps(response.get('status_details'), indent=2)}")

                    case "response.audio_transcript.done":
                        transcript = event.get("transcript")
                        print(f" AI:-- {transcript}")
                        # Just for debug
                        await self.send_message(json.dumps({
                            "Kind": "Transcription",
                            "Text": transcript
                    }))

                    case "response.audio.delta":
                        if(self.is_raw_audio):
                            delta = event.get("delta")
                            audio_bytes = base64.b64decode(delta)
                            await self.send_message(audio_bytes)
                        else:
                            await self.voicelive_to_acs(event.get("delta"))

                    case "error":
                        print(f"Voice Live Error: {event}")

                    case _:
                        print(f"[VoiceLiveACSHandler] Other: {event_type}")
        except Exception as e:
            print(f"[VoiceLiveACSHandler] Receiver error: {e}")


    async def send_message(self, message: Data):
        try:
            # print(f"Sending to client: {type(message), message[:30]}")
            await self.incoming_websocket.send(message)
        except Exception as e:
            print(f"[VoiceLiveACSHandler] Failed to send message: {e}")

    async def voicelive_to_acs(self, base64_data):
        try:
            data = {
                "Kind": "AudioData",
                "AudioData": {
                    "Data": base64_data
                },
                "StopAudio": None
            }
            await self.send_message(json.dumps(data))
        except Exception as e:
            print(f"[VoiceLiveACSHandler] Error in voicelive_to_acs: {e}")

    async def stop_audio(self):
        stop_audio_data = {
            "Kind": "StopAudio",
            "AudioData": None,
            "StopAudio": {}
        }
        await self.send_message(json.dumps(stop_audio_data))

    async def acs_to_voicelive(self, stream_data):
        try:
            data = json.loads(stream_data)
            kind = data['kind']
            if kind == "AudioData":
                audio_data_section = data.get("audioData", {})
                if not audio_data_section.get("silent", True):
                    audio_data = audio_data_section.get("data")
                    await self.audio_to_voicelive(audio_data)
        except Exception as e:
            print(f"[VoiceLiveACSHandler] Error processing WebSocket message: {e}")
    
    async def web_to_voicelive(self, audio_bytes):
        audio_b64 = base64.b64encode(audio_bytes).decode("ascii")
        await self.audio_to_voicelive(audio_b64)
