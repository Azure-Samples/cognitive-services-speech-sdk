# Examples of synthesis with input text stream

The input text stream API is designed to generate audio from text that is being streamed or generated in chunks. A typical scenario is to speak text generated from GPT-like models. Compared to non-text stream APIs, the text stream API significantly reduces TTS latency.

|  | Non text stream | Text Stream |
| ---------- | -------- | ----------- |
| Input Type | Whole GPT response | Each GPT output chunk |
| Latency | High: Time of full GPT response + Time of TTS | Low: Time of few GPT chunks + Time of TTS |

## Setup Instructions

### 1. Open the Project

Open the folder `samples/java/android/tts-text-stream` in Android Studio.

### 2. Configure Credentials

Open `app/src/main/java/com/microsoft/cognitiveservices/speech/samples/ttstextstream/MainActivity.java` and configure the following settings:

**Azure Speech Config:**
```java
private static final String SPEECH_KEY = "YourSubscriptionKey";
private static final String SPEECH_REGION = "YourServiceRegion";
private static final String SPEECH_VOICE = "en-us-Ava:DragonHDLatestNeural";
```

This sample uses the websocket v2 endpoint internally:

```text
wss://{region}.tts.speech.microsoft.com/cognitiveservices/websocket/v2
```

### 3. Build and Run

1. Connect an Android device or start an emulator.
2. Build the project in Android Studio.
3. Run the application.
4. Click **Start Demo** to send text chunks to the TTS input stream.
5. The streamed text is shown in the middle panel, logs are shown at the bottom, audio is played through `AudioTrack`, and the generated WAV file is saved to the temporary directory reported in the runtime logs.

## API overview
### Create text stream request
To use the text stream API, you have to use the websocket V2 endpoint.  
```wss://{region}.tts.speech.microsoft.com/cognitiveservices/websocket/v2```

### Set global properties
Since the input of text stream API is partial text. SSML, which is based on XML, is not supported. And thus properties that set in SSML should be set in a new way.  

For now we only support set voice name and output format.

### Create input text stream
Please specify `speechsdk.SpeechSynthesisRequestInputType.TextStream` when creating the request.

### Send text to stream
For each text chunk, call `request.input_stream.write(text)` to send text to the stream.  
Please do not wait too long(longer than 30s) between creating the request and sending the first text, and also between sending two texts, otherwise the connection may be closed by server and you may got 503 error.

### Close text stream
When you finish sending chunks, call `request.input_stream.close()` to close the stream.

## Sample behavior

This Android sample is self-contained.

- It does not depend on an external LLM service.
- It keeps the Android entrypoint and the text-stream demo logic in a single Java file for easier sample maintenance.
- It streams a built-in list of text chunks to demonstrate the API flow.
- It plays streamed PCM audio in real time through `AudioTrack` and also saves it as a WAV file after the stream completes.
