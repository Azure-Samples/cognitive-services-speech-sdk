# Quickstart: Synthesize speech in JavaScript on Node.js.

This sample demonstrates how to synthesize speech with the Speech SDK for JavaScript on Node.js.

## Prerequisites

* A subscription key and region for the Speech service. 
  * Azure subscription - [Create one for free](https://azure.microsoft.com/en-us/free/cognitive-services/)
  * [Create a Speech resource](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices) in the Azure portal.
  * Get the resource key and region. After your Speech resource is deployed, select Go to resource to view and manage keys. or  more information about Cognitive Services resources, see [Get the keys for your resource](https://docs.microsoft.com/en-us/azure/cognitive-services/cognitive-services-apis-create-account?tabs=multiservice%2Cwindows#get-the-keys-for-your-resource)
* A [Node.js](https://nodejs.org) compatible device.

## Prepare the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
See the [accompanying article](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/get-started-text-to-speech?tabs=terminal&pivots=programming-language-javascript) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio 2017.
* Open a command prompt at the quickstart directory, and run `npm install` to install the dependencies of the quickstart.
  This will place the Speech SDK library in the `node_modules` directory.
* Update the `index.js` file with your configuration:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Replace the string `YourAudioFile.wav` with a path you desire.

## Run the sample

Execute `node index.js` from the location where you have downloaded this quickstart.

## References
* [JavaScript article on the SDK documentation site](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/get-started-text-to-speech?tabs=terminal&pivots=programming-language-javascript)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
