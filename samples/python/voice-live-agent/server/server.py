import asyncio
from dotenv import load_dotenv
import os
import logging

from quart import Quart, request, websocket

from app.handler.acs_event_handler import AcsEventHandler
from app.handler.acs_media_handler import ACSMediaHandler

load_dotenv()

app = Quart(__name__)
app.config["AZURE_VOICE_LIVE_API_KEY"] = os.getenv("AZURE_VOICE_LIVE_API_KEY", "")
app.config["AZURE_VOICE_LIVE_ENDPOINT"] = os.getenv("AZURE_VOICE_LIVE_ENDPOINT")
app.config["VOICE_LIVE_MODEL"] = os.getenv("VOICE_LIVE_MODEL", "gpt-4o-mini")
app.config["ACS_CONNECTION_STRING"] = os.getenv("ACS_CONNECTION_STRING")
app.config["ACS_DEV_TUNNEL"] = os.getenv("ACS_DEV_TUNNEL", "")
app.config["AZURE_USER_ASSIGNED_IDENTITY_CLIENT_ID"] = os.getenv("AZURE_USER_ASSIGNED_IDENTITY_CLIENT_ID", "")

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(levelname)s: %(message)s")

acs_handler = AcsEventHandler(app.config)

@app.route("/acs/incomingcall", methods=["POST"])
async def incoming_call_handler():
    events = await request.get_json()
    host_url = request.host_url.replace("http://", "https://", 1).rstrip("/")
    return await acs_handler.process_incoming_call(events, host_url, app.config)

@app.route('/acs/callbacks/<contextId>', methods=['POST'])
async def acs_event_callbacks(contextId):
    raw_events = await request.get_json()
    return await acs_handler.process_callback_events(contextId, raw_events, app.config)

@app.websocket('/acs/ws')
async def acs_ws():
    print("Incoming ACS WebSocket connection")
    handler = ACSMediaHandler(app.config)
    await handler.init_incoming_websocket(websocket, is_raw_audio=False)
    asyncio.create_task(handler.connect())  # async but non-blocking
    try:
        while True:
            msg = await websocket.receive()
            await handler.acs_to_voicelive(msg)
    except Exception as e:
        print(f"[ACS WS] closed: {e}")

@app.websocket('/web/ws')
async def web_ws():
    print("Incoming Web WebSocket connection")
    handler = ACSMediaHandler(app.config)
    await handler.init_incoming_websocket(websocket, is_raw_audio=True)
    asyncio.create_task(handler.connect())
    try:
        while True:
            msg = await websocket.receive()
            await handler.web_to_voicelive(msg)
    except Exception as e:
        print(f"[Web WS] closed: {e}")

@app.route("/")
async def index():
    return await app.send_static_file('index.html')

if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=8000)
