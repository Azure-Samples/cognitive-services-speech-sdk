# JavaScript Speech Recongition and Translation Sample for the Web Browser

This sample demonstrates how to recognize speech with the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
See [this article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser) for introductory information on the Speech SDK for JavaScript.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* For intent recognition: an *endpoint* subscription key for the [Language Understanding Intelligent Service (LUIS)](https://www.luis.ai/home), and an application ID.
* A PC or Mac, with a working microphone.
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](../../../README.md#get-the-samples)
* From the [Speech SDK for JavaScript .zip package](https://aka.ms/csspeech/jsbrowserpackage) extract the file
  `microsoft.cognitiveservices.speech.sdk.bundle.js` and place it into the folder that contains this sample.

If you want to host the sample on a web server:

* Edit the `token.php` source:
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Replace the string `YourSubscriptionKey` with your own subscription key.
* Edit the `index.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the token.php resource.
* Deploy all files to your web server.

## Run the sample

* In case you are running the sample from your local computer, open index.html from the location where you have downloaded this sample with a JavaScript capable browser.
  Use the input fields to set your subscription key and service region.
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [JavaScript quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
