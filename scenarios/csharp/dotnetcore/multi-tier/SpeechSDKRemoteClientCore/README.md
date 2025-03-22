# SpeechSDKRemoteClientCore

## Overview

The `SpeechSDKRemoteClientCore` is a core component of the Microsoft Azure Speech SDK sample, designed to demonstrate building a middle-tier service that processes audio from an Edge Client device.

## Key Files

- **AudioAcknowledgedEventArgs.cs**: Event arguments for acknowledged audio processing.
- **AudioBuffer.cs**: Abstract class for managing audio data storage and processing.
- **ConnectionFailedException.cs**: Custom exception for connection failures.
- **DataChunk.cs**: Represents a chunk of audio data.
- **IStreamTransport.cs**: Interface for stream transport used for sending and receiving data.
- **PCMAudioBuffer.cs**: Buffer for PCM audio, managing replay until acknowledged.
- **SpeechSDKRemoteClient.cs**: Manages connection and audio transmission to the service.

## Key Features

- **Audio Handling**: Includes classes for managing audio buffers, such as `AudioBuffer` and `PCMAudioBuffer`, to ensure seamless audio transmission and acknowledgment.
- **Transport Agnosticism**: The core components are designed to be independent of the transport implementation, supporting both WebSockets and gRPC as transport layers.
- **Exception Handling**: Provides custom exception types like `ConnectionFailedException` to handle various error conditions gracefully.
- **Event-Driven Architecture**: Utilizes events to notify when audio is acknowledged, data is received, or errors occur.

## Components

- **Audio Acknowledgment**: `AudioAcknowledgedEventArgs` class is used for handling audio acknowledgment events.
- **Data Handling**: `DataChunk` and `DataReceivedEventArgs` classes manage audio data chunks and data reception events.
- **Transport Interface**: `IStreamTransport` interface defines the contract for different transport implementations.
- **Exception Management**: `ConnectionFailedException` provides a structured approach for handling connection failures.

### Detailed Audio Buffering

#### AudioBuffer

The `AudioBuffer` is a fundamental class responsible for managing audio data efficiently. It acts as a circular buffer that stores audio samples temporarily before they are processed or transmitted. Key responsibilities include:

- **Data Storage**: Temporarily holds audio data that is received or yet to be acknowledged.
- **Acknowledgment Handling**: Tracks acknowledged audio parts, removing them once confirmed.
- **Buffer Rewinding**: Rewinds to the last acknowledged point for resending unacknowledged audio, ensuring continuity in case of errors.

#### PCMAudioBuffer

The `PCMAudioBuffer` extends `AudioBuffer` with specific optimizations for PCM audio data:

- **PCM Optimization**: Efficiently handles PCM audio streams, ideal for raw audio data.
- **Robustness in Transmission**: Ensures audio transmission remains robust, even under unstable network conditions.
- **Error Recovery**: Facilitates seamless retransmission by rewinding to acknowledged points, maintaining synchronization and preventing audio loss.

### Importance in the Overall System

The `AudioBuffer` and `PCMAudioBuffer` are crucial for ensuring reliable audio transmission between the client and the middle-tier. They enable the system to handle network issues gracefully, providing a seamless user experience and maintaining synchronization for continuous speech recognition processes.

## Usage

This component is not standalone and is intended to be used as part of the Azure Speech SDK sample project. It serves as the core library shared among different client transport implementations, such as WebSockets and gRPC, for handling audio streaming and communication with the middle-tier service.

## Integration with Other Projects

The `SpeechSDKRemoteClientCore` is designed to work alongside other projects in the Azure Speech SDK sample solution. It provides shared functionality required by client applications to interact with the middle-tier service, which in turn communicates with the Azure Speech Service.

## Getting Started

To use this component, integrate it into your Azure Speech SDK sample project and configure it with the appropriate transport type (WebSocket or gRPC). Ensure that your client application is set up to handle events and exceptions as per your business logic requirements.

## Further Reading

For a deeper understanding of how this component fits into the overall system, refer to the system overview and other related projects in the Azure Speech SDK sample solution.

## License

This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for more details.