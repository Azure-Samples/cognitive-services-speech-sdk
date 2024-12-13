# Full duplex demo based on Azure SR, TTS and AOAI

## Getting Started

> Note: most browsers only support localhost or https for the microphone. So please ensure you are using https or localhost to access the webpage.

You have a few options foe getting started with this project.

### Option 1: Build dockers and run locally

Fill the environment variables in the `docker-compose.yml` file.

```sh
docker compose up
```

### Option 1: Deploy to Azure Container APP

1. Build the docker and push it to the Azure Container Registry.
   - e.g., `docker build . -t xx.azurecr.io/full-duplex-bot --progress=plain --push`
2. Create a new Azure OAI resource and Azure Speech resource.
3. You can use Azure services e.g., Azure Container APP or Azure APP Service to deploy the docker image.
   1. We will use Azure Container APP for this demo.
   2. Create a new Azure Container APP Environment.
   3. Deploy the first Container APP to host the webpage. An ingress is also needed so you can access the webpage via https.
   4. Deploy the second Container APP to host the WebSocket server.
      1. Override the default command to run the WebSocket server. `python3, ws_server.py`
      2. Configure the scale rules so the replicas can be scaled up and down based on the traffic.
      3. Set the following env variables:
         - `SPEECH_REGION`
         - `SPEECH_RESOURCE_ID`
         - `AZURE_OPENAI_ENDPOINT`
         - `AZURE_CLIENT_ID` (optional)
      4. Choose SR provider (optional). We support Azure real-time SR and Azure fast transcription as the SR provider. If you want to use Azure fast transcription, you need to set the following env variable:
         - `SPEECH_RECOGNIZER_PROVIDER` to `azure-fast-transcription`

   5. Get the ingress DNS of the WebSocket server, and set it in the webpage.
      1. `WEBSOCKET_URL`