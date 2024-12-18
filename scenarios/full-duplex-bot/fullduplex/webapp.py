import os
from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import logging
import csv

logging.basicConfig(level=logging.INFO)

WEBSOCKET_URL = os.environ.get("WEBSOCKET_URL")

if not WEBSOCKET_URL:
    raise ValueError("WEBSOCKET_URL must be set")

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")

templates = Jinja2Templates(directory="templates")

# read csv
region = os.getenv("SPEECH_REGION", "").lower()
preview_regions = ["eastus", "southeastasia", "westeurope", "westus2"]
with open('voices.csv', 'r') as f:
    f.readline()
    reader = csv.reader(f)
    voice_list ={
        "voices": {
            k: v.strip() for k, v, p in reader if not p or region in preview_regions
        }
    }

print(voice_list)



@app.get("/", response_class=HTMLResponse)
async def read_item(request: Request):
    logging.info(request.headers)
    logging.info(request.url)
    return templates.TemplateResponse(
        name="index.html", context={"ws_url": WEBSOCKET_URL, "request": request}
    )

@app.get("/voices", response_class=JSONResponse)
async def voices(request: Request):
    return JSONResponse(content=voice_list)