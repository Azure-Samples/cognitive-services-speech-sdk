# Post-call analytics with Fast Transcription API and Open AI

This sample application transcribes an audio recording using <a href="https://learn.microsoft.com/azure/ai-services/speech-service/fast-transcription-create" title="Fast Transcription API" target="_blank">Fast Transcription API</a> the summarizes the conversation transcription with <a href="https://azure.microsoft.com/products/ai-services/openai-service" title="Azure OpenAI SDK" target="_blank">Azure OpenAI</a> SDK.

## Prerequisites
- .NET 6.0 SDK or higher

## Command-line Options

* `--speechKey KEY`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource key. Required.
* `--speechRegion REGION`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource region. Examples: `eastus`, `northeurope` Required.

* `--openAiKey KEY`: Your <a href="https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesOpenAI" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> resource key. Required.
* `--openAiEndpoint ENDPOINT`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> resource endpoint. Example: `https://YourResourceName.openai.azure.com` Required.
* `--openAiDeploymentName OPENAIDEPLOYMENTNAME`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create an Azure OpenAI resource" target="_blank">Azure OpenAI</a> deployment name. Example: my-gpt-4o-mini Required.

* `--inputAudio FILEPATH`: File path to audio. Required.

* `--help`: Show the usage help and stop
