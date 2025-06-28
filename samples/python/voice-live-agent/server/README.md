# Voice Live Agent Template (Python + ACS)

Lightweight template to test real-time voice calls using **Azure Communication Services (ACS)** Call Automation + **Azure Voice Live API** — no PSTN number needed. Start locally with `uv run`, deploy later to Azure Web App.

---

## Quick Overview

- **Framework**: Python 3.9+, FastAPI for HTTP & WebSocket  
- **ACS Integration**: Incoming call → media streaming → `/ws` → handled by Python  
- **Voice Live API**: Real-time AI via WebSocket bridge  
- **Client**: Browser using ACS Web Calling SDK  
- **Local Testing**: `uv run uvicorn server.main:app --reload`  
- **Production Deployment**: Same code can be deployed to Azure Web App or Docker

---

## Project Structure
voice-live-agent/
├── server/
│ ├── main.py               # Entrypoint – handles triggers from clients
│ ├── voice_live_service.py # Azure Voice Live API logic (session, messaging)
│ ├── media_handler.py      # Router between streaming clients and AI service
│ ├── acs_media_streaming_handler.py  # Handles ACS WebSocket streaming
│ └── helper.py             # Utility functions for JSON parsing and data extraction
├── client/
│ ├── index.html # Mic capture UI
│ └── main.js # ACS Web Calling SDK client
└── .env # ACS & Voice Live credentials

## Quick Start

### 1. Initialize env & dependencies:

    ```bash
    uv init --app
    uv add fastapi[standard] azure-communication-callautomation azure-identity websockets python-dotenv
    ```

### 2. Create `.env`:

    ```ini
    ACS_CONNECTION_STRING=...
    AZURE_VOICE_LIVE_API_KEY=...
    AZURE_VOICE_LIVE_ENDPOINT=...
    VOICE_LIVE_MODEL=...
    CALLBACK_HOST=http://localhost:8000
    ```

### 3. Run locally:

    ```bash
    uv run uvicorn server.main:app --reload
    ```

### 4. Test in browser: 
   Open `client/index.html` and click **Start Call**.

---

## Deployment

- Azure Web App : TBD