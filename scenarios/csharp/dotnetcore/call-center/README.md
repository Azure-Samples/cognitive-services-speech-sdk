# Scenarios: Post-call transcription and analytics with C# for .NET Core on Windows

This sample demonstrates how to use post-call transcription and analytics with C# .NET 6.0 on Windows.

## Run the Sample within VS Code
- Install "Azure AI Speech Toolkit" extension in VS Code.
- Download this sample from sample gallery to local machine.
- Trigger "Azure AI Speech Toolkit: Configure Azure Speech Resources" command from command palette to select an **Azure AI services** resource.
- Trigger "Azure AI Speech Toolkit: Build the Sample App" command from command palette to build the sample.
- Trigger "Azure AI Speech Toolkit: Run the Sample App" command from command palette to run the sample.

"Azure AI Speech Toolkit" extension will use the same selected Azure AI service for `speechKey` / `languageKey` / `speechRegion` / `languageEndpoint` arguments. If you prefer to run with separate Speech Services, you can open a terminal and execute commands like this:
```
dotnet run --languageKey YourResourceKey --languageEndpoint YourResourceEndpoint --speechKey YourResourceKey --speechRegion YourResourceRegion --input "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav" --stereo  --output summary.json
```
Check more argument usage in below section.

## Run the Sample without VS Code

Visit the [call center transcription quickstart](https://learn.microsoft.com/azure/cognitive-services/speech-service/call-center-quickstart) for a detailed guide on how to get started transcribing call recordings using the Speech and Language Services.

### Usage and arguments

Connection:

* `--speechKey KEY`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource key. Required for audio transcriptions with the `--input` from URL option.
* `--speechRegion REGION`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices"  title="Create a Speech resource"  target="_blank">Speech</a> resource region. Required for audio transcriptions with the `--input` from URL option. Examples: `eastus`, `northeurope`

* `--languageKey KEY`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesTextAnalytics"  title="Create a Language resource"  target="_blank">Language</a> resource key. Required.
* `--languageEndpoint ENDPOINT`: Your <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesAllInOne" title="Create a Cognitive Services resource"  target="_blank">Cognitive Services</a> or <a href="https://portal.azure.com/#create/Microsoft.CognitiveServicesTextAnalytics"  title="Create a Language resource"  target="_blank">Language</a> resource endpoint. Required. Example: `https://YourResourceName.cognitiveservices.azure.com`

Input:

* `--input URL`: Input audio from URL. You must set either the `--input` or `--jsonInput` option.
* `--jsonInput FILE`: Input an existing batch transcription JSON result from FILE. With this option, you only need a Language resource to process a transcription that you already have. With this option, you don't need an audio file or a Speech resource. Overrides `--input`. You must set either the `--input` or `--jsonInput` option.
* `--stereo`: Indicates that the audio via ```input URL` should be in stereo format. If stereo isn't specified, then mono 16khz 16 bit PCM wav files are assumed. Diarization of mono files is used to separate multiple speakers. Diarization of stereo files isn't supported, since 2-channel stereo files should already have one speaker per channel.
* `--certificate`: The PEM certificate file. Required for C++.

Language:

* `--language LANGUAGE`: The language to use for sentiment analysis and conversation analysis. This value should be a two-letter ISO 639-1 code. The default value is `en`.
* `--locale LOCALE`: The locale to use for batch transcription of audio. The default value is `en-US`.

Output:

* `--help`: Show the usage help and stop
* `--output FILE`: Output the transcription, sentiment, conversation PII, and conversation summaries in JSON format to a text file. For more information, see [output examples](../../../call-center-quickstart.md#check-results).