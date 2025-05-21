# C# Example to use the Speech Services Batch Transcription API

Speech Services Batch Transcription is exposed through a REST API. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly. For detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription).

## Prerequisites

- **recordingsBlobUris** (Azure Blob Storage SAS URI pointing to audio files) or **recordingsContainerUri** (Azure Blob Storage Container SAS URI that stored audio files). You can upload your local audio or recording files to Azure Blob Storage. Alternatively, you can download example en-US audio files from [sample audio files](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/sampledata/audiofiles) and upload them to your Azure Blob Storage.
- The **locale** of the audio file (e.g., en-US, zh-CN, etc.).

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

1. Ensure dotnet 8.0 is installed.
2. Set needed secret in environment variables.
2. Build the project:
    ```
    dotnet build batchclient/batchclient.csproj
    ```
3. Run the project:
    ```
    dotnet batchclient/bin/Debug/net8.0/BatchTranscriptions.dll
    ```
## Advanced Usage
By default, the sample code uses the `recordingsBlobUris` parameter. You can specify either a single recording file blob URL or multiple URLs, separated by commas.

If you prefer to use the `recordingsContainerUri` parameter, follow these steps:

1. In **batchclient/program.cs**, comment out the following line:
    ```C#
    ContentUrls = _userConfig.recordingsBlobUris,
    ```
2. Uncomment the following line:
    ```C#
    //ContentContainerUrl = _userConfig.contentAzureBlobContainer,
    ```
3. Rebuild the program using the `Azure AI Speech Toolkit: Build the Sample App` command.
4. Run the sample with `Azure AI Speech Toolkit: Run the Sample App`. Ensure that the SAS URI for the container has `Read` and `List` permissions.