# Java Console Sample: TTS Text Stream

This sample demonstrates how to push text to Azure Text-to-Speech in small chunks by using the Speech SDK text stream API from a Java console application.

## Prerequisites

1.  **Java Development Kit (JDK)**: Version 8 or later.
2.  **Maven**: For building the project.
3.  **Azure Subscription**: An Azure subscription key for the Speech Service.
4.  **Azure Speech resource**: A Speech resource key and region.

## Setup

1.  **Set Environment Variables**:
    Set the following environment variables with your Azure Speech resource keys:
    *   `SPEECH_KEY`: Your subscription key.
    *   `SPEECH_REGION`: Your service region (e.g., `westus`, `eastus`).
    *   `SPEECH_VOICE` (optional): Voice name. Default is `en-US-AvaMultilingualNeural`.
    *   `OUTPUT_FILE` (optional): Output WAV file path. Default is `target/streaming_output.wav`.

    On Windows (PowerShell):
    ```powershell
    $env:SPEECH_KEY="your-key"
    $env:SPEECH_REGION="your-region"
    ```

    On Linux/macOS:
    ```bash
    export SPEECH_KEY="your-key"
    export SPEECH_REGION="your-region"
    ```

2.  **Build the Project**:
    Navigate to the project directory and run:
    ```bash
    mvn clean package
    ```

3.  **Run the Sample**:
    Execute the fat JAR generated in the `target` directory:
    ```bash
    java -jar target/tts-text-stream-1.0-SNAPSHOT-jar-with-dependencies.jar
    ```
    Using `mvn exec:java` is the simplest way to run the sample.

    **Using Maven Exec Plugin**:
    ```bash
    mvn exec:java
    ```

    If you use an Azure China Speech resource, also set:
    ```powershell
    $env:SPEECH_DOMAIN="tts.speech.azure.cn"
    ```

    Or pass a full websocket endpoint explicitly:
    ```powershell
    $env:SPEECH_ENDPOINT="wss://<region>.<domain>/cognitiveservices/websocket/v2"
    ```

## Functionality

*   **Uses the websocket v2 endpoint** required by the text stream API.
*   **Streams text incrementally** by writing chunked text to `SpeechSynthesisRequestInputType.TextStream`.
*   **Collects streamed PCM audio** from `AudioDataStream` and writes it to a WAV file.
*   **Keeps the sample self-contained** with a built-in chunk list so it can run without an LLM backend.
