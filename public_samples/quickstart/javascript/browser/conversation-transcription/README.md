# Quickstart: Transcribe a conversation in JavaScript on a Web Browser.

This sample demonstrates how to transcribe a conversation from a file using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
* See the [overview article](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription) on the SDK documentation page to learn more about Conversation Transcription (Preview).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac.
* User voice samples [See User voice samples for Conversation Transcription](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription#expected-inputs)
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* From the [Speech SDK for JavaScript .zip package](https://aka.ms/csspeech/jsbrowserpackage) extract the file
  `microsoft.cognitiveservices.speech.sdk.bundle.js` and place it into the folder that contains this quickstart.

If you want to host the sample on a web server:

* The web server must be secure (HTTPS).
* The web server must support PHP with curl enabled.
* Edit the `token.php` source:
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Replace the string `YourSubscriptionKey` with your own subscription key.
* Edit the `index.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the token.php resource.
  * Add voice signature data for all participants
* Deploy all files to your web server.

## Run the `index.html` sample

* In case you are running the sample from your local computer, open `conversation-transcription.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
* [To create a conversation and attach a Transcriber](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription):
  * Use the input fields to set your `subscription key` and `service region`.
  * Press the `create conversation` button to create a new conversation and begin transcription.
* Transcription results and speaker IDs will appear in the incoming messages box.
* Press the `leave conversation` button to end transcription and leave the conversation. 
   
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
* [Speech SDK Conversation Transcription (Preview)](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription)
* [Asynchronous conversation transcription (Preview)](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription#asynchronous)
