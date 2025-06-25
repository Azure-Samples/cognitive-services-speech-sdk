import os
import asyncio
import websockets
import json
import base64
import logging

class AzureVoiceLiveService:
    def __init__(self):
        self.endpoint = os.getenv("AZURE_VOICE_LIVE_ENDPOINT")
        self.api_key = os.getenv("AZURE_VOICE_LIVE_API_KEY")
        self.model = os.getenv("VOICE_LIVE_MODEL", "gpt-4o-mini-realtime-preview")
        self.ws = None
        self.receive_task = None
        self.callback = None  # set via register_callback()

    async def create_session(self):
        url = (
            self.endpoint.replace("https://", "wss://") +
            f"/voice-agent/realtime?api-version=2025-05-01-preview&model={self.model}&api-key={self.api_key}"
        )
        self.ws = await websockets.connect(url)
        logging.info("Voice Live WebSocket connected")
        self.receive_task = asyncio.create_task(self._receive_loop())
        await self._send_session_update()
        await self._send_start_response()

    async def _send_session_update(self):
        payload = {
            "type": "session.update",
            "session": {
                "turn_detection": {
                    "type": "azure_semantic_vad",
                    "threshold": 0.3,
                    "prefix_padding_ms": 200,
                    "silence_duration_ms": 200
                },
                "input_audio_noise_reduction": {"type": "azure_deep_noise_suppression"},
                "input_audio_echo_cancellation": {"type": "server_echo_cancellation"},
                "voice": {"name": "en-US-Aria:DragonHDLatestNeural", "temperature": 0.8}
            }
        }
        await self.ws.send(json.dumps(payload))

    async def _send_start_response(self):
        await self.ws.send(json.dumps({"type": "response.create"}))

    async def _receive_loop(self):
        async for msg in self.ws:
            data = json.loads(msg)
            t = data.get("type")
            if t == "response.audio.delta":
                audio_bytes = base64.b64decode(data["delta"])
                if self.callback:
                    await self.callback(audio_bytes)
            elif t == "input_audio_buffer.speech_started":
                # interruption/barge-in handling
                await self.ws.send(json.dumps({"type": "input_audio_buffer.stop"}))

    async def send_audio(self, audio_bytes: bytes):
        payload = {
            "type": "input_audio_buffer.append",
            "audio": base64.b64encode(audio_bytes).decode()
        }
        await self.ws.send(json.dumps(payload))

    def register_callback(self, callback):
        self.callback = callback  # media handler will set this

    async def close(self):
        if self.receive_task:
            self.receive_task.cancel()
        if self.ws:
            await self.ws.close()
