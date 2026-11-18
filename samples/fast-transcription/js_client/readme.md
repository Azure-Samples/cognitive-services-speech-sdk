# JavaScript Speech Recognition, Synthesis and Translation Sample for Node.js

This sample demonstrates audio transcription using the Fast Transcription API for JavaScript on Node.js. 

## Prerequisites
1. A [Node.js](https://nodejs.org) compatible device.
1. An Azure AI Speech resource in one of the regions where the fast transcription API is available. The supported regions are: Central India, East US, Southeast Asia, and West Europe. For more information and region identifier, see [Speech service regions](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/regions)
1. An audio file (less than 2 hours long and less than 200 MB in size) in one of the formats and codecs supported by the batch transcription API. For more information about supported audio formats, see [Supported audio formats](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/batch-transcription-audio-data?tabs=portal#supported-audio-formats-and-codecs)
1. For additional explanation, see [fast transcription documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/fast-transcription-create)


## Prepare the sample

* Update the following strings in the `main.js` file with your configuration:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with. For example, `westus` or `northeurope`.
   * `YourAudio_FilePath`: The file path for your audio file with speech to be transcribed. See for example a file named `test_call_audio.wav` located in same folder

* Run `npm install` to install any required dependency on your computer.

## Run the sample

```shell
node main.js 
```

## References

* [fast transcription documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/fast-transcription-create)
