# Speech SDK Remote Client gRPC

The **Speech SDK Remote Client gRPC** is a component of the multi-tier architecture designed to facilitate robust audio transmission from client devices to a middle-tier service using gRPC. This project is part of the Microsoft Azure Speech SDK samples, demonstrating how to build a middle-tier service for processing audio with Azure's Speech Service.

## Overview

This project implements a gRPC-based client that communicates with a middle-tier service. The middle-tier service interfaces with the Azure Speech Service to perform speech recognition. The results are sent back to the middle tier for further processing, enabling developers to integrate business logic and ensure robust audio transmission.

## Project Structure

### Key Files

- **SpeechSDKRemoteClientGRPC.csproj**: The project file defining dependencies and target framework (netstandard2.1).
- **GRPCTransport.cs**: Implements the `IStreamTransport<SpeechSDKRemoteResponse>` interface using gRPC, handling audio transmission and message reception.

## Core Functionality

### gRPC Transport

The `GRPCTransport` class is central to this project. It facilitates:

- **Connection State Management**: 
  - Tracks the current connection state through gRPC call status
  - Provides the `State` property to monitor connection status
  - Exposes `IsReadyForSending` to indicate when audio can be transmitted (requires valid audio format and Connected state)
- **Audio Transmission**: 
  - Sends audio data in a streaming fashion using gRPC bidirectional streaming
  - Ensures audio format is properly configured with required parameters (bits per sample, channel count, samples per second)
  - Validates audio format before allowing transmission
- **Message Handling**: Processes incoming messages using Protocol Buffers, handling audio acknowledgments and recognition stopped messages.
- **Error Handling**: Implements robust error handling for gRPC-specific scenarios.

### Connection States

The transport implements the following connection states:

- **Disconnected**: Initial state, when gRPC channel is not established, or when connection is lost
- **Connected**: When gRPC channel is established and ready for streaming
- **Closing**: During graceful connection termination

### Events

The transport implementation exposes several events:

- **MessageReceived**: Triggered when a message is received from the server.
- **Stopped**: Indicates that the connection has stopped, with detailed reason information.
- **AudioAcknowledged**: Signals that an audio segment has been acknowledged by the server.

## System Integration

The **Speech SDK Remote Client gRPC** is part of a larger architecture involving:

- **Edge Devices**: Capture audio and send it to the middle-tier using either gRPC or WebSockets.
- **Middle-Tier Service**: Receives audio, processes it using Azure Speech SDK, and sends results back.
- **Azure Speech Service**: Performs speech recognition, providing transcriptions and other insights.

This project integrates seamlessly with other client models, such as the WebSocket-based client, providing flexibility in transport options and demonstrating different integration strategies with the Azure Speech Service.

## Usage

To use this client:

1. **Configure the Endpoint**: Specify the gRPC endpoint of your middle-tier service.
2. **Choose the Transport Type**: Ensure the transport type matches your middle-tier configuration (gRPC in this case).
3. **Configure Audio Format**: Set the audio format with appropriate parameters before sending data.
4. **Monitor Connection State**: Use the `State` property to track connection status.
5. **Check Transmission Readiness**: Verify `IsReadyForSending` before attempting to send audio.
6. **Send Audio**: Use the client to send audio data, monitoring events for acknowledgment and results.

### Example

```csharp
var endpoint = new Uri("https://your-service-endpoint");
var transport = new GRPCTransport(endpoint);

// Configure audio format
transport.SetAudioFormat(new AudioFormat 
{
    BitsPerSample = 16,
    ChannelCount = 1,
    SamplesPerSecond = 16000
});

// Connect and monitor state
await transport.ConnectAsync();
if (transport.IsReadyForSending)
{
    // Send audio data
    await transport.SendAudioAsync(audioData);
}
```

## Performance Considerations

The gRPC implementation offers several advantages:

- **Protocol Buffers**: Efficient binary serialization
- **HTTP/2**: Multiplexed streams and header compression
- **Bidirectional Streaming**: Efficient for continuous audio transmission
- **Type Safety**: Strongly typed message contracts

## License

This project is licensed under the MIT License. See `LICENSE.md` for more details.

---

This README provides a comprehensive overview of the **Speech SDK Remote Client gRPC** project, detailing its structure, core functionality, and integration within the larger system. For additional details, refer to other README files in the multi-tier directory.
