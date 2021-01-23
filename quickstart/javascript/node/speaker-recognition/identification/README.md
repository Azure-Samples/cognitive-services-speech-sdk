# Quickstart: Identify a speaker in JavaScript on Node.js.

This sample demonstrates how to identify a speaker with the Speech SDK for JavaScript on Node.js.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-node) on the SDK documentation page which describes how to build this sample from scratch in your favourite editor.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A [Node.js](https://nodejs.org) compatible device.

## Prepare the sample

* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open a command prompt at the quickstart directory, and run `npm install` to install the dependencies of the quickstart.
  This will place the Speech SDK library in the `node_modules` directory.
* Update the `independent-identification.js` file with your configuration:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
    Note: As a Preview feature, Speaker Identification is only available for the westus region.

## Run the sample

Execute `node independent-identification.js` from the location where you have downloaded this quickstart.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-node)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
