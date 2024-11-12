# Post-call analytics with Fast Transcription API and Open AI
This sample application transcribes an audio recording using <a href="https://learn.microsoft.com/azure/ai-services/speech-service/fast-transcription-create" title="Fast Transcription API" target="_blank">Fast Transcription API</a> the summarizes the conversation transcription with <a href="https://azure.microsoft.com/products/ai-services/openai-service" title="Azure OpenAI SDK" target="_blank">Azure OpenAI</a> SDK.

## Run the Sample within VS Code
1. Install "Azure AI Speech Toolkit" extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select speech resource.
4. Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
5. Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.

## Prerequisites
- .NET 8.0 SDK or higher

## Command-line Options

* `--speechKey KEY`: the key of your Azure AI Service resource. (Azure AI Speech Toolkit: Run the Sample App will set environment variable automatically, or you can set it manually.)
* `--speechRegion REGION`: the region of your Azure AI Service resource, e.g. westus2. (Azure AI Speech Toolkit: Run the Sample App will set environment variable automatically, or you can set it manually.)

* `--openAiKey KEY`: the key of your Azure AI Service resource. (Azure AI Speech Toolkit: Run the Sample App will set environment variable automatically, or you can set it manually.)
* `--openAiEndpoint ENDPOINT`: the Open AI Endpoint of your Azure AI Service resource. (Azure AI Speech Toolkit: Run the Sample App will set environment variable automatically, or you can set it manually.)
* `--openAiDeploymentName OPENAIDEPLOYMENTNAME`: the Azure OpenAI resource deployment name of your Azure AI Service resource. (Azure AI Speech Toolkit: Run the Sample App will set environment variable automatically, or you can set it manually.) Default value: gpt-4

* `--inputAudio FILEPATH`: File path to audio. Required.

* `--help`: Show the usage help and stop
