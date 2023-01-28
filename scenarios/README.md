---
page_type: sample
languages:
- cpp
- csharp
- java
- python
name: "Speech Scenarios and Use Cases"
description: "Learn how to use the Speech SDK in certain popular scenarios such as call center and captioning."
products:
- azure
- azure-cognitive-services
---

# Sample Repository for Speech Scenarios

This folder hosts code and input **samples** for popular Speech-to-text scenarios. To find out more about the Microsoft Cognitive Services Speech SDK itself, please visit the [SDK documentation site](https://aka.ms/csspeech).

The current scenarios showcased here are:
* [Captioning](#captioning)
* [Call center transcription and analytics](#call-center-transcription-and-analytics)

## Captioning

Visit the [captioning quickstart](https://learn.microsoft.com/azure/cognitive-services/speech-service/captioning-quickstart) for a detailed guide on how to get started with captioning using the Speech Service.

### Usage and arguments

Connection:

* `--key`: Your Speech resource key. Overrides the SPEECH_KEY environment variable. You must set the environment variable (recommended) or use the `--key` option.
* `--region REGION`: Your Speech resource region. Overrides the SPEECH_REGION environment variable. You must set the environment variable (recommended) or use the `--region` option. Examples: `westus`, `northeurope`

Input:

* `--input FILE`: Input audio from file. The default input is the microphone. 
* `--format FORMAT`: Use compressed audio format. Valid only with `--file`. Valid values are `alaw`, `any`, `flac`, `mp3`, `mulaw`, and `ogg_opus`. The default value is `any`. To use a `wav` file, don't specify the format. This option is not available with the JavaScript captioning sample. For compressed audio files such as MP4, install GStreamer and see [How to use compressed input audio](~/articles/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams.md). 

Language:

* `--language LANG`: Specify a language using one of the corresponding [supported locales](~/articles/cognitive-services/speech-service/language-support.md?tabs=stt-tts). This is used when breaking captions into lines. Default value is `en-US`.

Recognition:

* `--offline`: Output offline results. Overrides `--realTime`. Default output mode is offline.
* `--realTime`: Output real-time results. 

Real-time output includes `Recognizing` event results. The default offline output is `Recognized` event results only. These are always written to the console, never to an output file. The `--quiet` option overrides this. For more information, see [Get speech recognition results](~/articles/cognitive-services/speech-service/get-speech-recognition-results.md).

Accuracy options:

* `--phrases PHRASE1;PHRASE2`: You can specify a list of phrases to be recognized, such as `Contoso;Jessie;Rehaan`. For more information, see [Improve recognition with phrase list](~/articles/cognitive-services/speech-service/improve-accuracy-phrase-list.md).

Output:

* `--help`: Show this help and stop
* `--output FILE`: Output captions to the specified `file`. This flag is required.
* `--srt`: Output captions in SRT (SubRip Text) format. The default format is WebVTT (Web Video Text Tracks). For more information about SRT and WebVTT caption file formats, see [Caption output format](~/articles/cognitive-services/speech-service/captioning-concepts.md#caption-output-format).
* `--maxLineLength LENGTH`: Set the maximum number of characters per line for a caption to LENGTH. Minimum is 20. Default is 37 (30 for Chinese).
* `--lines LINES`: Set the number of lines for a caption to LINES. Minimum is 1. Default is 2.
* `--delay MILLISECONDS`: How many MILLISECONDS to delay the display of each caption, to mimic a real-time experience. This option is only applicable when you use the `realTime` flag. Minimum is 0.0. Default is 1000.
* `--remainTime MILLISECONDS`: How many MILLISECONDS a caption should remain on screen if it is not replaced by another. Minimum is 0.0. Default is 1000.
* `--quiet`: Suppress console output, except errors.
* `--profanity OPTION`: Valid values: raw, remove, mask. For more information, see [Profanity filter](~/articles/cognitive-services/speech-service/display-text-format.md#profanity-filter) concepts.
* `--threshold NUMBER`: Set stable partial result threshold. The default value is `3`. This option is only applicable when you use the `realTime` flag. For more information, see [Get partial results](~/articles/cognitive-services/speech-service/captioning-concepts.md#get-partial-results) concepts.

## Call Center Transcription and Analytics

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