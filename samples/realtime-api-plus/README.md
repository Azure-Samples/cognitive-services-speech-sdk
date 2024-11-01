# Azure Speech Realtime

This project demonstrates how to use the Azure AI Speech Services to extend the ability of Open AI GPT4o realtime service. The Azure Voice AI provides many conversational voices as well as the ability to customize the voice to match the desired persona.

This project provides the same websocket interface as the Open AI GPT4o realtime service, so it can be used as a drop-in replacement. Customers could use the existing Open AI GPT4o realtime client to connect.

Two modes are supported:

- **Transparency mode**: The service acts as a transparent proxy, forwarding the text, audio and events to the Azure Open AI GPT4o realtime service.
- **GPT4o + Azure Voice AI mode**: The service uses the GT4o model to handle input audio and generate text, then uses the Azure Voice AI to generate the audio response.
