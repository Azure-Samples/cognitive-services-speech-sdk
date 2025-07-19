import asyncio
import json
import websockets
import base64
import uuid
from websockets.asyncio.client import connect as ws_connect
from websockets.typing import Data


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

class VoiceLiveService:
    def __init__(self, config):
        self.api_key = config["AZURE_VOICE_LIVE_API_KEY"]
        self.endpoint = config["AZURE_VOICE_LIVE_ENDPOINT"]
        self.model = config["VOICE_LIVE_MODEL"]

        self.ws = None
        self.player = AudioPlayerAsync()
        self.send_queue = asyncio.Queue()
        self.send_task = None

    async def connect(self):
        ws_url = (
            self.endpoint.replace("https", "wss") +
            f"/voice-agent/realtime?api-version=2025-05-01-preview"
            f"&x-ms-client-request-id={self._generate_guid()}&model={self.model}&api-key={self.api_key}"
        )
        print(f"[VoiceLiveService] Connecting to {ws_url} ...")
        self.ws = await websockets.connect(ws_url)
        print("[VoiceLiveService] Connected")

        # Send initial session update and response.create
        await self._send_session_update()
        await self._start_response()

        self.send_task = asyncio.create_task(self._sender_loop())
        asyncio.create_task(self._receiver_loop())
        await asyncio.sleep(0)

    async def _send_session_update(self):
        msg = {
            "type": "session.update",
            "session": {
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
        await self._send_json(msg)
        print("[VoiceLiveService] Sent session update")

    async def _start_response(self):
        msg = {"type": "response.create"}
        await self._send_json(msg)
        print("[VoiceLiveService] Started response")

    async def _send_json(self, obj):
        if self.ws:
            await self.ws.send(json.dumps(obj))

    async def send_audio(self, audio_bytes: bytes):
        audio_b64 = base64.b64encode(audio_bytes).decode("ascii")
        msg = {
            "type": "input_audio_buffer.append",
            "audio": audio_b64
        }
        await self.send_queue.put(json.dumps(msg))

    async def _sender_loop(self):
        try:
            while True:
                msg = await self.send_queue.get()
                # print(f"[VoiceLiveService] Sender loop got chunk, length={len(msg)}")
                if self.ws:
                    await self.ws.send(msg)
        except Exception as e:
            print(f"[VoiceLiveService] Sender loop error: {e}")
    
    async def _receiver_loop(self):
        try:
            async for message in self.ws:
                obj = json.loads(message)
                if obj.get("type") == "response.audio.delta":
                    delta = obj.get("delta")
                    if delta:
                        audio_bytes = base64.b64decode(delta)
                        self.player.add_data(audio_bytes)
                else: 
                    print(f"[VoiceLiveService] Received: {message}")
                    # if obj.get("type") == "response.output_item.done":
                    #     await self._send_json({"type": "input_audio_buffer.commit"})
                    #     await self._send_json({"type": "response.create"})
        except Exception as e:
            print(f"[VoiceLiveService] Receiver error: {e}")

    def _generate_guid(self):
        return str(uuid.uuid4())

    async def close(self):
        if self.send_task:
            self.send_task.cancel()
            try:
                await self.send_task
            except asyncio.CancelledError:
                pass
        if self.ws:
            await self.ws.close()
            print("[VoiceLiveService] WS Closed")
        self.player.terminate()
