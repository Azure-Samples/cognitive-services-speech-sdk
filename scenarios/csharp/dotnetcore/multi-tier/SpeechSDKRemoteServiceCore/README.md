# Speech SDK Remote Service Core

This component provides the core functionality for the Speech SDK Remote Service, implementing a wrapper around Microsoft's Cognitive Services Speech SDK for continuous speech recognition with streaming audio support.

## Overview

The SpeechSDKRemoteServiceCore provides:
- Configuration management for Speech Service credentials and endpoints
- Transport-agnostic response handling
- Continuous speech recognition with streaming audio support
- Event-based recognition status updates

## Key Components

### IConfigurationService
Defines the minimum parameters needed to connect to the Speech Service:
- Speech Service Endpoint
- Speech Service Region
- Speech Service Key

### IResponseTransport
A transport-agnostic interface for sending responses back to the client, handling:
- Audio acknowledgment for processed segments
- Recognition stop notifications with reason and error details

### SpeechSDKWrapper
The main wrapper around Microsoft's Speech SDK that provides:
- Continuous speech recognition
- Push audio stream input handling
- Comprehensive event system for recognition status
- Error handling and cancellation management

## Events

The SpeechSDKWrapper exposes the following events:
- SDKRecognizing: Fired during ongoing recognition
- SDKRecognized: Fired when recognition is complete
- SDKSessionStarted/Stopped: Session lifecycle events
- SDKCanceled: Recognition cancellation with reason
- SDKSpeechStartDetected/EndDetected: Speech boundary detection

## Usage

```csharp
// Create configuration
var config = new BasicConfigurationService 
{
    SpeechServiceEndpoint = new Uri("your_endpoint"),
    SpeechServiceRegion = "your_region",
    SpeechServiceKey = "your_key"
};

// Initialize wrapper with config and transport
var wrapper = new SpeechSDKWrapper(
    speechConfig,
    audioFormat,
    responseTransport);

// Start recognition
await wrapper.StartRecognitionAsync();

// Send audio data
wrapper.OnAudioReceived(audioData);

// Stop recognition
await wrapper.StopRecognition();
```

## Error Handling

The service handles various recognition stop scenarios:
- User cancellation
- End of stream
- Error conditions with detailed error codes
- Unknown scenarios

## Dependencies

- Microsoft.CognitiveServices.Speech SDK
- System.Threading.Tasks for async operations
