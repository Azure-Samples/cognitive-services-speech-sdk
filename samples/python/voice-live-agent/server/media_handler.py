import asyncio
import json
import logging
from acs_media_streaming_handler import AcsMediaStreamingHandler
from voice_live_service import AzureVoiceLiveService

class MediaHandler:
    """
    Routes streaming audio from different clients to the AI service
    and sends AI-generated audio back to the original client.
    """
    def __init__(self):
        self.ai_service = AzureVoiceLiveService()

    async def handle_acs(self, ws):
        """
        Handles ACS WebSocket client.
        """
        handler = AcsMediaStreamingHandler(ws, self.ai_service)
        await handler.process()

    async def _send_to_ai_and_wait_response(self, audio_bytes):
        """
        Sends audio to AI, waits for full response, returns it.
        Used in HTTP-style audio flows (e.g. Twilio).
        """
        await self.ai_service.create_session()
        results = []

        # Buffer AI's response audio
        async def on_audio(audio: bytes):
            results.append(audio)

        self.ai_service.register_callback(on_audio)
        await self.ai_service.send_audio(audio_bytes)

        # Allow time for AI to respond
        await asyncio.sleep(1)

        await self.ai_service.close()
        return b"".join(results)

    # Future: Audiohook, custom handlers can be added here
