# JavaScript Speech Recognition, Synthesis and Translation Sample for Node.js

This sample demonstrates how to recognize and synthesis speech with the Speech SDK for JavaScript on Node.js. It is based on the [Microsoft Cognitive Services Speech SDK for JavaScript](https://aka.ms/csspeech/npmpackage).
See [this article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-node) for introductory information on the Speech SDK for JavaScript on Node.js.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* For intent recognition: an *endpoint* subscription key for the [Language Understanding Intelligent Service (LUIS)](https://www.luis.ai/home), and an application ID.
* A [Node.js](https://nodejs.org) compatible device.

## Prepare the sample

* Update the following strings in the `settings.js` file with your configuration:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.

* Run `npm install` to install any required dependency on your computer.

## Run the sample

The sample demonstrates the speech, intent, and translation recognizers, as well as speech synthesizer. You can start them individually by calling:

```shell
node index.js [speech|intent|translate|synthesis] {filename}
```

## References

* [Node.js quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-node)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
