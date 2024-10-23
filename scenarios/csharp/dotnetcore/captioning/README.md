# Real-Time and Offline Speech Recognition Captioning Tool

This project is a captioning tool that utilizes the Microsoft Azure Cognitive Services Speech SDK to perform real-time and offline speech recognition and output captions in WebVTT or SubRip Text (SRT) format. 

## Features
- Real-time and offline caption generation.
- Support for both microphone and audio file inputs.
- Support for compressed audio formats like MP3, FLAC, and OGG.
- Configurable output formats (WebVTT or SRT) and customization options such as maximum line length and number of lines per caption.
- Ability to customize delay and remain time for captions.
- Option to include custom phrases to improve speech recognition accuracy.

## Run the Sample within VS Code
1. Install "Azure AI Speech Toolkit" extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select speech resource.
4. Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
5. Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.

## Prerequisites
- .NET 6.0 SDK
- Microsoft Azure Cognitive Services Speech SDK
  - Install using NuGet: 
    ```
    dotnet add package Microsoft.CognitiveServices.Speech
    ```
- GStreamer for handling compressed audio inputs. Instructions are [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams).

## Installation
1. Clone the repository:
2. Navigate to the project directory:
3. Install the required NuGet package:
4. (Optional) Install GStreamer for handling compressed audio files.

## Usage
To run the program, use the following command:

### Command-line options
- **--language**: Specify the language for captioning (default: `en-US`).
- **--input**: Specify the input audio file. If not provided, the default input will be the microphone.
- **--format**: Specify the compressed audio format (`alaw`, `flac`, `mp3`, etc.). Only valid with the `--input` option.
- **--offline**: Use offline mode for caption generation (default mode).
- **--realTime**: Use real-time caption generation mode.
- **--output**: Specify the output file for captions.
- **--srt**: Output captions in SubRip Text (SRT) format (default is WebVTT).
- **--maxLineLength**: Set the maximum number of characters per line (default: 37).
- **--lines**: Set the number of lines for a caption (default: 2).
- **--delay**: Delay the appearance of each caption by a certain number of milliseconds (default: 1000ms).
- **--remainTime**: Set how long a caption should remain on the screen (default: 1000ms).
- **--profanity**: Set the profanity option (`raw`, `remove`, `mask`, default: `mask`).
- **--phrases**: Specify custom phrases to improve recognition accuracy.

### Example Usage
1. Real-time captioning with microphone input.
2. Offline captioning with an audio file input.