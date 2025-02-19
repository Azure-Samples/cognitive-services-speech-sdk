# C# Example to use the Speech Services Batch Transcription API

Speech Services Batch Transcription is exposed through a REST API. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly. For detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription).

## Prerequisites

- **recordingsBlobUris** (Azure Blob Storage SAS URI pointing to audio files) or **recordingsContainerUri** (Azure Blob Storage Container SAS URI that stored audio files). You can upload your local audio or recording files to Azure Blob Storage. Alternatively, you can download example en-US audio files from [sample audio files](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/sampledata/audiofiles) and upload them to your Azure Blob Storage.
- The **locale** of the audio file (e.g., en-US, zh-CN, etc.).

## Run the Sample within VS Code
1. Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger `Azure AI Speech Toolkit: Configure Azure Speech Resources` command from command palette to select speech resource.
4. Trigger `Azure AI Speech Toolkit: Configure and Setup the Sample App` command from command palette to configure and setup the sample. This command only needs to be run once.
5. Trigger `Azure AI Speech Toolkit: Build the Sample App` command from command palette to build the sample.
6. Trigger `Azure AI Speech Toolkit: Run the Sample App` command from command palette to run the sample.

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