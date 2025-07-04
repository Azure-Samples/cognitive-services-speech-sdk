from quart import websocket
from app.handler.web_handler import websocket_web_handler
from app.handler.acs_handler import websocket_acs_handler

def register_websocket_routes(app):
    @app.websocket("/web/ws")
    async def web_ws():
        await websocket_web_handler(websocket, app.config)

    @app.websocket("/acs/ws")
    async def acs_ws():
        await websocket_acs_handler(websocket, app.config)
