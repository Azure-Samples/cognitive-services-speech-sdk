# Scenarios: Language Learning via Azure Pronunciation Assessment with Speech to Text

This sample demonstrates how to use Azure pronunciation assessment SDK for language learning.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select an **Azure AI Service** resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Prerequisites

Before you get started, here's a list of prerequisites:

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A [Node.js](https://nodejs.org) compatible device.

## Prepare the sample

* Update the following strings in the `settings.js` file with your configuration:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with. For example, `westus` or `northeurope`.
  * Your language for speech recognition, if you want to change the default `en-us` (American English).
  * `YourAudioFile.wav`: An audio file with speech to be recognized. The format is 16khz sample rate, mono, 16-bit per sample PCM. See for example a file named `whatstheweatherlike.wav` located in several folders in this repository. Make sure the above language settings matches the language spoken in the WAV file. Note: This sample assumes there is a 44 bit wav header on the file and skips those bytes in the stream.


* Run `npm install` to install any required dependency on your computer.

## Run the sample

```shell
node index.js [pronunciationAssessment|pronunciationAssessmentConfiguredWithJson|pronunciationAssessmentContinue|pronunciationAssessmentFromMicrophone] {filename}
```

## Run the Microphone Streaming sample for node.js

Although the Cognitive Services SDK doesn't yet support mic streaming, its easy to get it working using external packages.
To use the sample provided, follow these steps:
* Download and install [Sox ver 14.4.1](https://sourceforge.net/projects/sox/files/sox/14.4.1/)
* Ensure you have the keys in `settings.js` as outlined above.
* Run `npm i` from the terminal.
* Run `node pronunciationAssessmentFromMicrophone.js`
* Once you see the 'Listening for speech' message, you can speak into the mic
* You should see the final response after you have seen the 'buffer of speech received' message.

## Support

If you have a problem or are missing a feature, please have a look at our [support page](https://docs.microsoft.com/azure/cognitive-services/speech-service/support).

## References

* [Node.js quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-node)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
