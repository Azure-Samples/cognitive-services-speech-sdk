# Speech SDK Remote Service WebSocket

This project is part of the multi-tier architecture for demonstrating Microsoft Azure Speech SDK capabilities. The `SpeechSDKRemoteServiceWebSocket` component serves as a middle-tier service that processes audio from edge client devices, routes it to the Azure Speech SDK, and handles the communication with the Azure Speech Service using WebSocket. The service is containerized for ease of deployment and scalability.

## Overview

### Purpose
The core purpose of this project is to facilitate robust audio transmission from client devices to a middle-tier service where it is processed using Azure Speech SDK. The service then communicates with Azure Speech Service for speech recognition and sends back the results to the client.

### Key Features
- **WebSocket Communication**: Utilizes WebSocket for efficient, bidirectional communication between the client and the middle-tier service.
- **Azure Speech SDK Integration**: Leverages Azure's powerful speech recognition capabilities.
- **Audio Acknowledgment**: Ensures that unacknowledged audio can be retransmitted if the connection is interrupted.
- **Sample Tests**: Includes a battery of tests and infrastructure to ensure the reliability of the service.
- **Containerized Deployment**: Uses Docker for easy deployment and scalability.

## File Structure

- **`Program.cs`**: Entry point of the application, configures and runs the web application.
- **`SpeechSDKRemoteService.cs`**: Handles WebSocket connections and integrates with the Speech SDK.
- **`WebSocketResponseTransport.cs`**: Manages the response transport mechanisms over WebSocket.
- **`appsettings.json` & `appsettings.Development.json`**: Configuration files for different environments.
- **`Dockerfile`**: Contains instructions to build a Docker image for the service.

## Getting Started

### Prerequisites
- [.NET 8.0 SDK](https://dotnet.microsoft.com/download/dotnet/8.0)
- [Docker](https://www.docker.com/get-started)
- An Azure subscription with access to the Speech SDK.

### Building and Running the Service

1. **Configure the Service**: Update `appsettings.json` with the necessary Azure Speech Service credentials and configurations.
   
2. **Run the Service**: Use the following command-line arguments when starting the service:
   - `--region`: Specifies the Azure region for the Speech service.
   - `--key`: Specifies the Azure key for the Speech service.
   - `--endpoint`: (Optional) Specifies the endpoint for the Speech service. Defaults to `http://localhost:5000`.

3. **Build the Docker Image**:
   ```bash
   docker build -t speech-sdk-remote-service-websocket .
   ```

4. **Run the Docker Container**:
   ```bash
   docker run -p 5001:5001 speech-sdk-remote-service-websocket
   ```

5. **Access the Service**: The service listens on port `5001` for incoming WebSocket connections.

### Integration with Client

This service is designed to interact with client applications that stream audio data. The client applications should be configured to use WebSocket as the transport type and direct their streams to this service's endpoint.

## Extending the Service

### Adding Business Logic

Business logic that processes the speech recognition results can be added in `SpeechSDKRemoteService.cs`. Hooks are provided for integrating custom logic based on session events and recognized speech.

### Customizing Protocols

Developers can customize the message protocols by modifying `WebSocketResponseTransport.cs` and associated utilities for message serialization and deserialization.

## Conclusion

The `SpeechSDKRemoteServiceWebSocket` project is a robust middle-tier solution for handling speech recognition tasks using Azure services. With its modular architecture and Docker-based deployment, it can be easily integrated and scaled in various edge computing scenarios.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

---

This README provides a comprehensive guide to understanding, deploying, and extending the Speech SDK Remote Service WebSocket component. For further information, please refer to the additional documentation and sample projects in the solution.