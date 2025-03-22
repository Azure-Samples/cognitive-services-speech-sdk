# Speech SDK Remote Client WebSocket

The **Speech SDK Remote Client WebSocket** is a component of the multi-tier architecture designed to facilitate robust audio transmission from client devices to a middle-tier service using WebSockets. This project is part of the Microsoft Azure Speech SDK samples, demonstrating how to build a middle-tier service for processing audio with Azure's Speech Service.

## Overview

This project specifically implements a WebSocket-based client that communicates with a middle-tier service. The middle-tier service, in turn, interfaces with the Azure Speech Service to perform speech recognition. The results are sent back to the middle tier for further processing, enabling developers to integrate business logic and ensure robust audio transmission.

## Project Structure

### Key Files

- **SpeechSDKRemoteClientWebSocket.csproj**: The project file defining dependencies and target framework (netstandard2.1).
- **WebsocketTransport.cs**: Implements the `IStreamTransport<IMessage>` interface using WebSockets, handling audio transmission and message reception.

## Core Functionality

### WebSocket Transport

The `WebSocketTransport` class is central to this project. It facilitates:

- **Connection State Management**: 
  - Tracks the current connection state (Disconnected, Connecting, Connected, Closing)
  - Provides the `State` property to monitor connection status
  - Exposes `IsReadyForSending` to indicate when audio can be transmitted (requires valid audio format and Connected state)
- **Audio Transmission**: 
  - Sends audio data in a streaming fashion
  - Ensures audio format is properly configured with required parameters (bits per sample, channel count, samples per second)
  - Validates audio format before allowing transmission
- **Message Handling**: Processes incoming messages, triggering events for audio acknowledgment and handling recognition stopped messages.
- **Error Handling**: Implements retry logic and error handling to maintain robust connections.

### Connection States

The transport implements the following connection states:

- **Disconnected**: Initial state or when connection is lost/closed
- **Connecting**: During connection establishment
- **Connected**: When WebSocket connection is established and ready
- **Closing**: During graceful connection termination

### Events

The transport implementation exposes several events:

- **MessageReceived**: Triggered when a message is received from the server.
- **Stopped**: Indicates that the connection has stopped, allowing for handling of stop conditions.
- **AudioAcknowledged**: Signals that an audio segment has been acknowledged by the server.

## System Integration

The **Speech SDK Remote Client WebSocket** is part of a larger architecture involving:

- **Edge Devices**: Capture audio and send it to the middle-tier using either WebSockets or gRPC.
- **Middle-Tier Service**: Receives audio, processes it using Azure Speech SDK, and sends results back.
- **Azure Speech Service**: Performs speech recognition, providing transcriptions and other insights.

This project integrates seamlessly with other client models, such as the gRPC-based client, providing flexibility in transport options and demonstrating different integration strategies with the Azure Speech Service.

## Usage

To use this client:

1. **Configure the Endpoint**: Specify the WebSocket endpoint of your middle-tier service.
2. **Choose the Transport Type**: Ensure the transport type matches your middle-tier configuration (WebSocket in this case).
3. **Configure Audio Format**: Set the audio format with appropriate parameters before sending data.
4. **Monitor Connection State**: Use the `State` property to track connection status.
5. **Check Transmission Readiness**: Verify `IsReadyForSending` before attempting to send audio.
6. **Send Audio**: Use the client to send audio data, monitoring events for acknowledgment and results.

Developers can extend this implementation to capture audio directly from microphones or other sources, integrating business logic as needed.

## License

This project is licensed under the MIT License. See `LICENSE.md` for more details.

---

This README provides a comprehensive overview of the **Speech SDK Remote Client WebSocket** project, detailing its structure, core functionality, and integration within the larger system. For additional details, refer to other README files in the multi-tier directory.
