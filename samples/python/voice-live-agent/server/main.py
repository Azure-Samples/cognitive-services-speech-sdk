import os
import asyncio
import json
import logging
import websockets

from acs_media_streaming_handler import AcsMediaStreamingHandler
from voice_live_service import AzureVoiceLiveService

# Config + setup
CALLBACK_HOST = os.getenv("CALLBACK_HOST", "ws://localhost:8765")
ACS_WS_PATH = "/ws/acs"
VOICE_LIVE_PATH = "/ws/voice-live"
AI_ENDPOINT = os.getenv("AZURE_VOICE_LIVE_ENDPOINT")
AI_KEY = os.getenv("AZURE_VOICE_LIVE_API_KEY")
AI_MODEL = os.getenv("VOICE_LIVE_MODEL", "gpt-4o-mini-realtime-preview")

logging.basicConfig(level=logging.INFO)

async def ws_router(ws, path):
    if path == ACS_WS_PATH:
        logging.info("ACS client connected")
        ai_service = AzureVoiceLiveService(AI_ENDPOINT, AI_KEY, AI_MODEL)
        handler = AcsMediaStreamingHandler(ws, ai_service)
        await handler.process()
    elif path == VOICE_LIVE_PATH:
        # Optional: direct Voice Live testing endpoint
        async for msg in ws:
            logging.info(f"Voice-Live Test received: {msg}")
    else:
        logging.warning(f"Unknown connection path: {path}")
        await ws.close()

async def main():
    port = int(os.getenv("PORT", "8765"))
    async with websockets.serve(ws_router, "0.0.0.0", port):
        logging.info(f"Server listening on ws://0.0.0.0:{port}")
        await asyncio.Future()  # keep alive

if __name__ == "__main__":
    asyncio.run(main())
