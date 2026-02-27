# Examples to synthesis with input text stream

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
private static final String SPEECH_SUBSCRIPTION_KEY = "YourSubscriptionKey";
private static final String SPEECH_REGION = "YourServiceRegion";
```

**LLM Configuration (Optional):**
If you have a local LLM or OpenAI endpoint you wish to test with:
```java
private static final String OPENAI_ENDPOINT = "http://Your-LLM-Server-IP:11434/v1";
private static final String OPENAI_KEY = "YourOpenAIKey";
```

### 3. Build and Run

1. Connect an Android device or start an emulator.
2. Build the project in Android Studio.
3. Run the application.
4. Click **Start Stream** to connect to the backend LLM and hear the streaming TTS output.

## API overview
### Create text stream request
To use the text stream API, you have to use the websocket V2 endpoint.  
```wss://{region}.tts.speech.microsoft.com/cognitiveservices/websocket/v2```

### Set global properties
Since the input of text stream API is parital text. SSML, which is based on XML, is not supported. And thus properties that set in SSML should be set in a new way.  

For now we only support set voice name and output format.

### Create input text stream
Please specify `speechsdk.SpeechSynthesisRequestInputType.TextStream` when creating the request.

### Send text to stream
For each text that generated from GPT, call `request.input_stream.write(text)` to send text to the stream.  
Please do not wait too long(longer than 30s) between creating the request and sending the first text, and also between sending two texts, otherwise the connection may be closed by server and you may got 503 error.

### Close text stream
When GPT finished the output, call `request.input_stream.close()` to close the stream.

## About MarkdownStreamFilter
This sample integrates a `MarkdownStreamFilter` helper class to clean up LLM output for better speech synthesis results.

- **Purpose**: Removes visual Markdown formatting (like `**bold**`, `[links](url)`, ````code blocks````) that should not be read aloud.
- **Working Principle**: It processes text chunks in real-time using a lightweight state machine. It handles markers split across network chunks and **processes characters with zero latency**. This ensures the TTS engine receives text immediately while filtering out unwanted symbols.
