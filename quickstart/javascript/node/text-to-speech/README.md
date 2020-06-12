# Quickstart: Synthesize speech in JavaScript on Node.js.

This sample demonstrates how to synthesize speech with the Speech SDK for JavaScript on Node.js.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A [Node.js](https://nodejs.org) compatible device.

## Prepare the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
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

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
