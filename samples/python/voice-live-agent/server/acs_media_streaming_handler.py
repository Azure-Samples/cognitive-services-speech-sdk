import json
import asyncio
from fastapi import WebSocket

class AcsMediaStreamingHandler:
    def __init__(self, websocket: WebSocket, config, ai_service):
        self.ws = websocket
        self.config = config
        self.ai_service = ai_service
        self.active = True

    async def process(self):
        """Core loop to receive from ACS and dispatch to AI service."""
        await self.ws.accept()
        try:
            # Initialize AI session
            await self.ai_service.create_ai_session()
            # Start reading from ACS
            await self._receive_loop()
        except asyncio.CancelledError:
            pass
        except Exception as ex:
            print("Processing error:", ex)
        finally:
            await self.ai_service.close()
            await self.ws.close()
            self.active = False

    async def _receive_loop(self):
        while self.active:
            raw = await self.ws.receive_text()
            packet = json.loads(raw)
            kind = packet.get("kind")

            if kind == "AudioData":
                audio_hex = packet["audioData"]["data"]
                audio_bytes = bytes.fromhex(audio_hex)
                await self.ai_service.send_audio_to_ai(audio_bytes)
            elif kind == "AudioMetadata":
                # Optional: log or use metadata
                print("Input metadata:", packet["audioMetadata"])
            elif kind.lower().startswith("stopaudio"):
                print("Received StopAudio from ACS")
                self.active = False

    async def send_audio_to_acs(self, audio_bytes: bytes):
        """Called by the AI service to send its response into the call."""
        payload = {
            "kind": "AudioData",
            "audioData": {"data": audio_bytes.hex()}
        }
        await self.ws.send_text(json.dumps(payload))

    async def send_control(self, control_json: str):
        """Send control messages like stop commands."""
        await self.ws.send_text(control_json)
