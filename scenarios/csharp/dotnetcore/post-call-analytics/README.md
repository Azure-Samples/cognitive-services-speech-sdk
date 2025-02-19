# Post-call analytics with Fast Transcription API and Open AI
This sample application transcribes an audio recording using <a href="https://learn.microsoft.com/azure/ai-services/speech-service/fast-transcription-create" title="Fast Transcription API" target="_blank">Fast Transcription API</a> the summarizes the conversation transcription with <a href="https://azure.microsoft.com/products/ai-services/openai-service" title="Azure OpenAI SDK" target="_blank">Azure OpenAI</a> SDK.

## Run the Sample within VS Code
- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- Download this sample from sample gallery to local machine.
- Trigger `Azure AI Speech Toolkit: Configure Azure Speech Resources` command from command palette to select an **Azure AI services** resource.
- Trigger `Azure AI Speech Toolkit: Build the Sample App` command from command palette to build the sample.
- Trigger `Azure AI Speech Toolkit: Run the Sample App` command from command palette to run the sample.

## Prerequisites
- .NET 8.0 SDK or higher

## Usage

`Usage : post-call-analytics <inputAudio> [options]`

Arguments:
* ``<inputAudio>``: Path to the audio file. Required.

Options:
* `--speechKey KEY`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource key. The value can also be set via SPEECH_KEY environment variable. Required.
* `--speechRegion REGION`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource region. The value can also be set via SPEECH_REGION environment variable. Examples: `eastus`, `northeurope`. Required.
* `--openAiKey KEY`: Your <a href="https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesOpenAI" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> resource key. The value can also be set via AOAI_KEY environment variable. Optional.
* `--openAiEndpoint ENDPOINT`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> resource endpoint. Example: `https://YourResourceName.openai.azure.com`. The value can also be set via AOAI_ENDPOINT environment variable. Optional.
* `--openAiDeploymentName OPENAIDEPLOYMENTNAME`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> deployment name.  The value can also be set via AOAI_DEPLOYMENT_NAME environment variable. Example: my-gpt-4o-mini. Optional.
* `--help`: Show the usage help and stop
