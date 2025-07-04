from app.handler.media_handler import handle_media_streaming

async def websocket_acs_handler(ws, voice_service):
    # For ACS client, route to media handler (can extend for ACS specifics)
    pass
