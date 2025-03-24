# SpeechSDKRemoteClientApp

## Overview

The `SpeechSDKRemoteClientApp` is part of a multi-tier architecture sample that demonstrates how to utilize the Microsoft Azure Speech SDK to perform speech recognition. This application acts as a client that streams audio files to a middle-tier service, which then communicates with the Azure Speech Service for processing using the Speech SDK.

## Key Files

- **AudioStreamSource.cs**: Manages the streaming of audio files to the middle-tier service.
- **IAudioStreamTransport.cs**: Interface defining the transport mechanism for audio streams.
- **Program.cs**: Entry point for the client application; configures transport type, endpoint, and file path.
- **WavFileUtilities.cs**: Utility for handling WAV file operations.

## Features

- **Multiple Transport Protocols**: Supports both WebSockets and GRPC for data transmission to the middle tier.
- **WAV File Processing**: Reads and processes WAV file headers to extract audio format details.
- **Real-time Simulation**: Simulates real-time microphone input by streaming audio in chunks.
- **Event Handling**: Handles events such as message reception and recognition stops with predefined actions.
- **Extensible**: Allows developers to implement custom transport types by extending the provided interfaces.

## Prerequisites

- .NET 8.0 SDK or later
- Azure Speech SDK
- Docker (optional, for running middle-tier services in containers)

## Getting Started

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Azure-Samples/cognitive-services-speech-sdk
   ```

2. Navigate to the project directory:
   ```bash
   cd /git/scenarios/csharp/dotnetcore/multi-tier/SpeechSDKRemoteClientApp
   ```

3. Restore the project dependencies:
   ```bash
   dotnet restore
   ```

### Usage

Run the application with command-line arguments to specify transport type, endpoint, and audio file path.

Example command:

```bash
   dotnet run -- --transport grpc --endpoint https://localhost:5001 --file path/to/audio.wav
```

## Project Structure

- **Program.cs**: 
  - **Purpose**: Serves as the entry point of the application.
  - **Key Features**: 
    - Parses command-line arguments to configure transport type, endpoint, and audio file path.
    - Initiates the audio streaming process using the specified transport protocol.
    - Utilizes `System.CommandLine` for argument parsing and handling.

- **AudioStreamSource.cs**: 
  - **Purpose**: Manages the streaming of audio files to the middle-tier service.
  - **Key Features**: 
    - Implements `IAudioStreamTransport` to define a standard method for sending audio.
    - Reads audio data from a file, processes it in chunks, and sends it to the specified endpoint.
    - Handles real-time simulation by introducing delays between audio chunks to mimic live streaming.
    - Listens for messages and events from the service, such as recognition results and stop signals.

- **IAudioStreamTransport.cs**: 
  - **Purpose**: Defines the interface for audio stream transport mechanisms.
  - **Key Features**: 
    - Provides a method signature for sending audio data to a specified endpoint.
    - Ensures that different transport implementations adhere to a common protocol.

- **WavFileUtilities.cs**: 
  - **Purpose**: Provides utility functions for handling WAV file operations.
  - **Key Features**: 
    - Reads and parses WAV file headers to extract audio format information such as sample rate and channels.
    - Ensures compatibility with various WAV file formats by handling optional chunks and data sections.

- **SpeechSDKRemoteClientApp.csproj**: 
  - **Purpose**: Defines the project structure and dependencies.
  - **Key Features**: 
    - Specifies the target framework (.NET 8.0) and output type (executable).
    - Lists package references, such as `System.CommandLine`, and project references to shared components and transport implementations.

## Extending the Sample

Developers can extend the sample by implementing custom transport types or modifying the event handling logic to suit specific business needs. The project is structured to be modular and easily extensible.

## License

This project is licensed under the MIT License. See the `LICENSE.md` file for more details.

## Acknowledgements

This project uses the [Azure Speech SDK](https://github.com/Azure/azure-sdk-for-net) and demonstrates integration patterns with middle-tier services.