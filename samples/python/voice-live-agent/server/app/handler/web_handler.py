import asyncio
from quart import websocket
from app.handler.media_handler import handle_media_streaming
from app.service.voice_live_service import VoiceLiveService

async def websocket_web_handler(ws, config):
    """
    Handle WebSocket connection from the browser client.
    Receives audio chunks from client, forwards to voice live API,
    and sends back audio response over the same WebSocket.
    """
    try:
        voice_service = VoiceLiveService(config)
        await voice_service.connect()
        producer  = asyncio.create_task(handle_media_streaming(ws, voice_service))
        await asyncio.gather(producer)
    except Exception as e:
        print(f"[media_handler:web] Exception: {e}")
    finally:
        await ws.close(1000)
        await voice_service.close()
