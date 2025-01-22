# C# Example to use the Speech Services Batch Transcription API

Speech Services Batch Transcription is exposed through a REST API. The samples here do **NOT** require the installation of the Cognitive Service Speech SDK, but use the REST API directly. For detailed explanation see the [batch transcription documentation](https://docs.microsoft.com/azure/cognitive-services/speech-service/batch-transcription).

## Run the Sample within VS Code
1. Install "Azure AI Speech Toolkit" extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select speech resource.
4. Trigger "Azure AI Speech Toolkit: Configure and Setup the Sample App" command from command palette to configure and setup the sample. This command only needs to be run once.
5. Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
6. Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.

    This command will run the sample as a task in terminal. Interactively input SAS URI pointing to an audio file stored in Azure Blob Storage. (You can upload your local audio or recording files to your Azure blob storage. Of course, if you don't have audio, you can download this en-US audio file as an example: [Call1_separated_16k_health_insurance.wav](https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav) and upload it to your Azure blob storage.)

## Prerequisites
- A SAS URI pointing to an audio file stored in Azure Blob Storage.
- The locale (e.g. en-US, zh-CN, etc.) of the audio file.
