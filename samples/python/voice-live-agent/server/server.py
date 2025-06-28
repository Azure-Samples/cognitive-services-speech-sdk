from dotenv import load_dotenv
import os

from quart import Quart
from quart_cors import cors

from app.service.voice_live_service import VoiceLiveService
from websocket import register_websocket_routes

load_dotenv()

app = cors(Quart(__name__), allow_origin="*")  # Allow all for dev
app.config["AZURE_VOICE_LIVE_API_KEY"] = os.getenv("AZURE_VOICE_LIVE_API_KEY")
app.config["AZURE_VOICE_LIVE_ENDPOINT"] = os.getenv("AZURE_VOICE_LIVE_ENDPOINT")
app.config["VOICE_LIVE_MODEL"] = os.getenv("VOICE_LIVE_MODEL")
app.config["SYSTEM_PROMPT"] = os.getenv("SYSTEM_PROMPT")

register_websocket_routes(app)

@app.route("/")
async def index():
    return await app.send_static_file("index.html")

if __name__ == "__main__":
    app.run(debug=True, port=8000)
