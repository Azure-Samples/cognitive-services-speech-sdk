# SpeechSDKRemoteSharedGRPC

## Overview

The `SpeechSDKRemoteSharedGRPC` project is a key component of a multi-tier architecture designed to demonstrate the integration of Microsoft Azure Speech SDK for processing audio data from edge client devices. This project specifically focuses on shared components used in the GRPC-based communication model between client and service layers.

This project serves as a shared library that provides common functionalities and protocols needed by both the `SpeechSDKRemoteClientGRPC` and `SpeechSDKRemoteServiceGRPC` projects, facilitating seamless communication and processing across different tiers.

## Architecture

The overall architecture of the solution includes:

- **Edge Client**: Sends audio data to the middle-tier service.
- **Middle Tier Service**: Uses Azure Speech SDK to process audio data.
- **Azure Speech SDK**: Interfaces with Azure Speech Services for speech recognition.
- **Azure Speech Services**: Executes the recognition tasks and returns results.

This project ensures that the communication protocol between the client and the middle-tier service is robust, allowing for retransmissions of unacknowledged audio data in case of a failure.

## Components

### Core Files

- **speech.proto**: Defines the GRPC service contract and messages exchanged between the client and service.
- **SpeechSDKRemoteSharedGRPC.csproj**: Project file that manages build configurations and dependencies.

### Generated Files

- **Speech.cs & SpeechGrpc.cs**: Auto-generated files from the `speech.proto`, containing the GRPC server and client stubs.
- **AssemblyInfo.cs**: Contains metadata information about the assembly.

## Features

- **Protocol Buffers**: Utilizes Google's Protocol Buffers for defining and serializing structured data, enabling efficient data exchange over GRPC.
- **Shared Logic**: Provides shared classes and methods that are used across both client and server components, ensuring consistency and reducing redundancy.
- **Resilience**: Implements mechanisms to handle network disruptions, allowing for audio data retransmission.

## System Integration

The `SpeechSDKRemoteSharedGRPC` project fits into the larger system by providing essential shared components that are reused in both client (`SpeechSDKRemoteClientGRPC`) and service (`SpeechSDKRemoteServiceGRPC`) implementations. This helps maintain a clean separation of concerns while promoting code reuse.

For a comprehensive understanding, refer to the [System Overview](../README.md).

## Related Projects

- [SpeechSDKRemoteClientGRPC](../SpeechSDKRemoteClientGRPC/README.md): Handles client-side processing and communication using GRPC.
- [SpeechSDKRemoteServiceGRPC](../SpeechSDKRemoteServiceGRPC/README.md): Implements the middle-tier service for processing audio data using GRPC.
- [SpeechSDKRemoteSharedWebSocket](../SpeechSDKRemoteSharedWebSocket/README.md): Provides similar shared functionalities for WebSocket-based communication.

## Getting Started

### Prerequisites

- [.NET Core SDK](https://dotnet.microsoft.com/download) installed on your machine.
- Familiarity with GRPC and Protocol Buffers.

### Building the Project

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```
2. Navigate to the project directory:
   ```bash
   cd SpeechSDKRemoteSharedGRPC
   ```
3. Build the project:
   ```bash
   dotnet build
   ```

### Usage Example

This shared library is not standalone and is used as part of the larger Azure Speech SDK sample. To utilize it:

1. Reference the `SpeechSDKRemoteSharedGRPC` project in your client or service implementation.
2. Use the GRPC stubs generated from `speech.proto` to implement communication logic.
3. Ensure your client or service handles retransmissions and acknowledgments as defined in the shared logic.

For detailed integration steps with the client and service layers, see the respective README files:
- [Client Integration](../SpeechSDKRemoteClientGRPC/README.md#usage)
- [Service Integration](../SpeechSDKRemoteServiceGRPC/README.md#usage)

## Extending the Library

Developers can extend this shared library by:

- **Custom Protocol Buffers**: Modify `speech.proto` to add new message types or services as needed.
- **Additional Shared Logic**: Implement additional shared utilities or logic to support new features across client and service layers.

## Conclusion

The `SpeechSDKRemoteSharedGRPC` project provides the foundational shared components necessary for implementing a robust multi-tier architecture for speech recognition using Azure's Speech SDK. By facilitating consistent communication protocols and shared logic, it plays a crucial role in the wider solution's design.

## License

This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for more details.