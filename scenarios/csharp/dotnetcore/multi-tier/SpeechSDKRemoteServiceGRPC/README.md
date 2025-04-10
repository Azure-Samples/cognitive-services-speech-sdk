# SpeechSDKRemoteServiceGRPC

## Overview

The `SpeechSDKRemoteServiceGRPC` project is part of a multi-tier architecture designed to demonstrate the use of the Microsoft Azure Speech SDK. This sample illustrates how to build a middle-tier service that processes audio data from an edge client device using the Azure Speech SDK through a GRPC transport layer.

This project is one of several in a suite designed to show different methods of communication and processing, with counterparts including `SpeechSDKRemoteServiceWebSocket` for WebSocket-based transport.

## Architecture

The architecture consists of several key components:

- **Edge Client Device**: Sends audio data.
- **Middle Tier Service**: Processes audio data using Azure Speech SDK.
- **Azure Speech SDK**: Provides speech recognition capabilities.
- **Azure Speech Service**: Performs the actual speech recognition.

A robust protocol ensures that in the event of a link failure, the client can resend unacknowledged audio data to maintain continuity.

## Components

### Key Files

- **`Program.cs`**: 
  - **Purpose**: Configures and runs the GRPC service.
  - **Key Features**: 
    - Sets up command-line options for configuring Azure region, key, and service endpoint.
    - Creates and runs a web application configured to use GRPC.
    - Utilizes `System.CommandLine` for parsing command-line arguments.

- **`SpeechSDKRemoteService.cs`**: 
  - **Purpose**: Implements core logic for interfacing with the Azure Speech SDK and handling audio data.
  - **Key Features**: 
    - Defines the GRPC service `ProcessRemoteSpeechAudio` method to process incoming audio streams.
    - Sets up event handlers for speech recognition events such as start, stop, and text recognition.
    - Utilizes a `SpeechSDKWrapper` for managing sessions and sending recognition results back to the client.

- **`GRPCResponseTransport.cs`**: 
  - **Purpose**: Handles GRPC transport specifics for sending and receiving messages.
  - **Key Features**: 
    - Implements methods to acknowledge audio data and communicate recognition stops back to the client.
    - Utilizes `IServerStreamWriter` for GRPC response streaming.

### Configuration

Configuration files include:

- `appsettings.json` and `appsettings.Development.json`: Contain configuration settings for the service, such as endpoint URLs, authentication keys, and other settings needed by the Azure Speech SDK.

## Usage

1. **Build the Project**: Ensure that all dependencies are resolved and build the project using .NET Core CLI or Visual Studio.
   
2. **Run the Service**: Execute the `SpeechSDKRemoteServiceGRPC` application to start the GRPC service. You can provide the following command-line arguments:
   - `--region`: Specifies the Azure region for the Speech service.
   - `--key`: Specifies the Azure key for the Speech service.
   - `--endpoint`: (Optional) Specifies the endpoint for the Speech service. Defaults to `http://localhost:5000`.

3. **Client Interaction**: Deploy a client that sends audio data over GRPC to this service. The service processes the audio using the Speech SDK and returns results.

## Deployment

The service can be packaged and deployed in a Docker container, allowing for scalable deployment options in cloud environments.

## Conclusion

The `SpeechSDKRemoteServiceGRPC` project provides a robust framework for integrating Azure Speech capabilities into a multi-tier architecture using GRPC. Its design ensures reliable communication and processing, making it suitable for applications requiring scalable and fault-tolerant speech recognition solutions.

## License

This project is licensed under the MIT License. See the `LICENSE.md` file for more details.