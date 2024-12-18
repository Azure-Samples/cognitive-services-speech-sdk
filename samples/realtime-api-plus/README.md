# Azure Speech Realtime

This project demonstrates how to use the Azure AI Speech Services to extend the ability of Open AI GPT4o realtime service. The Azure Voice AI provides many conversational voices as well as the ability to customize the voice to match the desired persona.

This project provides the same websocket interface as the Open AI GPT4o realtime service, so it can be used as a drop-in replacement. Customers could use the existing Open AI GPT4o realtime client to connect.

Two modes are supported:

- **Transparency mode**: The service acts as a transparent proxy, forwarding the text, audio and events to the Azure Open AI GPT4o realtime service.
- **GPT4o + Azure Voice AI mode**: The service uses the GT4o model to handle input audio and generate text, then uses the Azure Voice AI to generate the audio response.
- **GPT4o + Azure Avatar mode**: The service uses the GT4o model to handle input audio and generate text, then uses the Azure Avatar service to synthesize the audio and render the talking head.


## Getting Started

You have a few options foe getting started with this project.

### Option 1: Build dockers and run locally

Fill the environment variables in the `docker-compose.yml` file.

```sh
docker compose up
```

## Guidance

### Work with Custom Neural Voice

This project supports CNV to synthesize the voice. To use CNV, you need to create a custom voice model in the Azure Speech service and provide the model ID in the environment variable `CNV_DEPLOYMENT_ID` and `VITE_CNV_VOICE`.

### Security

This project supports [Managed Identity](https://learn.microsoft.com/entra/identity/managed-identities-azure-resources/overview) or key based authentication. If `AZURE_OPENAI_KEY` or `SPEECH_KEY` environment variables are set, the service will use key based authentication for quick testing, otherwise it will use Managed Identity to authenticate with Azure services. We recommend using Managed Identity for production deployments.
