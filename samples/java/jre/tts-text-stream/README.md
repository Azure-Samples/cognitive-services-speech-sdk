# Java Console Sample: TTS Text Stream

This sample demonstrates how to stream text from an LLM (simulated via Ollama) to Azure Text-to-Speech (TTS) using the Speech SDK for Java in a console application.

## Prerequisites

1.  **Java Development Kit (JDK)**: Version 8 or later.
2.  **Maven**: For building the project.
3.  **Azure Subscription**: An Azure subscription key for the Speech Service.
4.  **Local LLM (Ollama)**: This sample is configured to connect to a local Ollama instance running at `http://localhost:11434`. You can change the endpoint in `Main.java`.

## Setup

1.  **Set Environment Variables**:
    Set the following environment variables with your Azure Speech resource keys:
    *   `SPEECH_KEY`: Your subscription key.
    *   `SPEECH_REGION`: Your service region (e.g., `westus`, `eastus`).

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
    Execute the JAR file generated in the `target` directory:
    ```bash
    java -cp target/tts-text-stream-1.0-SNAPSHOT.jar;target/dependency/* com.microsoft.cognitiveservices.speech.samples.ttstextstream.Main
    ```
    (Note: The `pom.xml` might need configuration to copy dependencies to `target/dependency`. If you use `mvn exec:java`, it's easier).

    **Using Maven Exec Plugin**:
    ```bash
    mvn exec:java
    ```

## Functionality

*   **Connects to Azure Speech Service**: Initializes the synthesizer with `Raw16Khz16BitMonoPcm` output format.
*   **Audio Playback**: Uses Java `javax.sound.sampled` API to play the received audio stream in real-time.
*   **LLM Streaming**: Connects to a local Ollama server, sends a prompt, and streams the text response.
*   **Markdown Filtering**: Filters out Markdown syntax from the LLM response before sending it to TTS, ensuring clean speech output.
