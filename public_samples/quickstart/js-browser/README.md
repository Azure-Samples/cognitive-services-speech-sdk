# Quickstart: Recognize speech in JavaScript on a Web Browser.

This sample demonstrates how to recognize speech with the Speech SDK for JavaScript on a Web Browser, like Microsoft Edge, or Chrome.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-javascript) on the SDK documentation page which describes how to build this sample from scratch in your favourite editor.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac, with a working microphone.
* A text editor.
* Optionally, a Web Server that supports hosting PHP scripts.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-javascript).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](../../README.md#get-the-samples)
* In case you are not using a Web Server to host the sample on:
  * Create an empty directory on your computer and copy the JavaScript sample files into that folder.
* Save the modified files.

* In case you want to host the sample on a Web Server:
  * Edit the `index.html` source:
  * Replace the string `authorizationEndpoint` with the full URL where you can access the token.php resource.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Edit the `token.php` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
* Deploy all files to your Web Server.

## Run the sample

* In case you are running the sample from your local computer, open index.html with a JavaScript capable browser.
* In case you are hosting the sample on a Web Server, open a Web Browser and navigate to the full URL where you host the sample.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-jre)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
