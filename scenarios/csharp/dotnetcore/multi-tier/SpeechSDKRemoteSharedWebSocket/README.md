# Speech SDK Remote Shared WebSocket

The **Speech SDK Remote Shared WebSocket** project is a crucial component of a multi-tier architecture designed to demonstrate the capabilities of the Microsoft Azure Speech SDK. This shared library serves as the backbone for handling common functionalities across different client and service implementations that communicate via WebSockets. It is an integral part of an audio processing pipeline that facilitates reliable transfer and processing of audio data from edge devices to the Azure Speech Service.

## Overview

### Purpose
The primary goal of this project is to provide robust and reusable components that support the transmission of audio data using WebSockets. It ensures seamless communication between client devices and a middle-tier service, which then interacts with the Azure Speech SDK for speech recognition tasks.

### Key Features
- **Message Handling**: Provides interfaces and utilities for message serialization and deserialization, ensuring efficient communication.
- **Audio Management**: Includes classes for handling audio data, including format specification and acknowledgment.
- **Event Handling**: Supports event-driven programming through various message types and events to facilitate smooth operation across the communication pipeline.
- **Shared Utilities**: Offers common utilities and components that can be reused in different parts of the multi-tier architecture.

## Project Structure

### Key Files
- **AudioAcknowledgedMessage.cs**: Handles messages related to the acknowledgment of audio data.
- **AudioFormatMessage.cs**: Manages audio format specifications for seamless data processing.
- **AudioMessage.cs**: Represents audio data being transmitted.
- **DisplayText.cs**: Manages text display messages, facilitating UI updates or logs.
- **IMessage.cs**: Defines the interface for messages, allowing for extensibility and custom implementations.
- **JsonMessage.cs**: Provides JSON parsing and serialization for messages.
- **MessageUtility.cs**: Contains utility methods for message operations.
- **RecognitionStoppedMessage.cs**: Manages messages indicating recognition has stopped, enabling appropriate follow-up actions.

## System Integration

The **Speech SDK Remote Shared WebSocket** is designed to work in conjunction with:

- **Edge Devices**: Devices that capture and send audio data using the WebSocket protocol.
- **Middle-Tier Services**: Services that process audio data using Azure's Speech SDK and handle communication with Azure Speech Services.
- **Azure Speech Service**: Provides speech recognition capabilities, converting audio into text and other actionable insights.

For a comprehensive understanding, refer to the [System Overview](../README.md).

## Related Projects

- [SpeechSDKRemoteClientWebSocket](../SpeechSDKRemoteClientWebSocket/README.md): Handles client-side processing and communication using WebSockets.
- [SpeechSDKRemoteServiceWebSocket](../SpeechSDKRemoteServiceWebSocket/README.md): Implements the middle-tier service for processing audio data using WebSockets.
- [SpeechSDKRemoteSharedGRPC](../SpeechSDKRemoteSharedGRPC/README.md): Provides similar shared functionalities for GRPC-based communication.

## Usage

To incorporate this shared library in your project:

1. **Reference the Project**: Ensure your client or service implementation references the `SpeechSDKRemoteSharedWebSocket` project.
2. **Utilize Message Types**: Use the provided message classes and interfaces to handle communication efficiently.
3. **Extend Functionality**: Implement custom logic by extending the provided interfaces and classes to fit specific use cases.

This project is designed to be flexible and extensible, allowing developers to tailor it to their specific needs while maintaining a robust foundation for WebSocket-based communication.

## Extending the Library

Developers can extend this library by:

- **Custom Message Types**: Add new message types by implementing the `IMessage` interface.
- **Enhanced Utilities**: Expand `MessageUtility` and other utility classes to include additional functionalities.

## License

This project is licensed under the MIT License. See `LICENSE.md` for more details.

---

This README provides a comprehensive overview of the **Speech SDK Remote Shared WebSocket** project, detailing its purpose, structure, and role within the larger system. For additional details, refer to the README files of other components in the multi-tier solution.