# Tests

## Overview

The `Tests` project is a crucial component of the multi-tier architecture sample for utilizing the Microsoft Azure Speech SDK. It focuses on validating the functionality and robustness of audio transmission from an edge client device to a middle-tier service, which then processes the audio using the Azure Speech Service.

This project provides a comprehensive suite of tests to ensure the reliability and accuracy of the system's various components, including different transport protocols and audio processing mechanisms.

## Features

- **Comprehensive Test Coverage**: Includes unit tests for individual components and integration tests for end-to-end scenarios.
- **Support for Multiple Protocols**: Tests both WebSocket and GRPC transport implementations.
- **Audio Buffer Management**: Validates the functionality of audio buffering and acknowledgment mechanisms.
- **Exception Handling**: Ensures that the system gracefully handles errors and edge cases.
- **Extensible Framework**: Allows developers to add custom tests and extend existing ones to cover additional scenarios.

## Project Structure

- **AudioMessageTests.cs**: 
  - **Purpose**: Tests the functionality of audio message handling and processing.
  - **Key Features**: Validates the correct parsing and transmission of audio messages across different transport protocols.

- **EndToEndBase.cs**: 
  - **Purpose**: Serves as the base class for end-to-end tests, providing common setup and utility functions.
  - **Key Features**: Facilitates the orchestration of complex test scenarios involving multiple components.

- **EndToEndGrpc.cs**: 
  - **Purpose**: Contains end-to-end tests specifically for the GRPC transport implementation.
  - **Key Features**: Validates the complete workflow from audio input to recognition result retrieval using GRPC.

- **EndToEndWebSocket.cs**: 
  - **Purpose**: Contains end-to-end tests specifically for the WebSocket transport implementation.
  - **Key Features**: Ensures that the WebSocket-based transmission and communication function as expected.

- **PCMAudioBufferTests.cs**: 
  - **Purpose**: Tests the PCM audio buffer's handling and management mechanisms.
  - **Key Features**: Validates buffer operations such as writing, acknowledgments, and rewinds.

- **SpeechSDKRemoteClientTests.cs**: 
  - **Purpose**: Tests the remote client's integration and interaction with the middle-tier service.
  - **Key Features**: Ensures that the client correctly manages connections and message exchanges.

- **SpeechSDKWrapperTests.cs**: 
  - **Purpose**: Tests the wrapper around the Azure Speech SDK.
  - **Key Features**: Ensures that the wrapper correctly interfaces with the Speech SDK and handles events appropriately.

- **TimeOutTestBase.cs**: 
  - **Purpose**: Provides a base class for tests that involve timeout scenarios.
  - **Key Features**: Facilitates the testing of timeout handling and recovery mechanisms.

- **Tests.csproj**: 
  - **Purpose**: Defines the project structure and dependencies for the test suite.
  - **Key Features**: Lists package references and other project settings required for running the tests.

## Prerequisites

- .NET 8.0 SDK or later
- Azure Speech SDK
- Docker (optional, for running middle-tier services in containers)

## Running Tests

1. Restore the project dependencies:
   ```bash
   dotnet restore
   ```

2. Run the tests using the following command:
   ```bash
   dotnet test
   ```

## Extending the Tests

Developers can extend the test suite by adding new test cases or modifying existing ones to cover additional scenarios and edge cases. The project is structured to be modular and easily extensible.

## License

This project is licensed under the MIT License. See the `LICENSE.md` file for more details.

## Acknowledgements

This project is part of a larger sample demonstrating the use of the [Azure Speech SDK](https://github.com/Azure/azure-sdk-for-net) in a multi-tier architecture. It showcases integration patterns and best practices for robust audio transmission and processing.