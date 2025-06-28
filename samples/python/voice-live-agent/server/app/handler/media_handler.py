import asyncio

async def handle_media_streaming(ws, voice_service):
    """
    Core function to receive audio data from client websocket,
    forward it to VoiceLiveService.
    """
    print("[MediaHandle] Start audio streaming")
    try:
        while True:
            msg = await ws.receive()

            if isinstance(msg, bytes):
                # Audio bytes from client
                await voice_service.send_audio(msg)
            else:
                # Text or control message from client
                # Extend here if needed
                pass

    except asyncio.CancelledError:
        pass
    except Exception as e:
        print(f"[media_handler:web] Error receiving from client websocket: {e}")