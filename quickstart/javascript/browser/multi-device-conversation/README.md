# Quickstart: Recognize speech in JavaScript on a Web Browser.

These samples demonstrate how to recognize speech, and how to create or join a multi-device conversation with real-time transcriptions and translations, using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
* See the [overview article](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation) on the SDK documentation page to learn more about Multi-device Conversation (Preview).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac, with a working microphone.
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
* Edit the  `multi-device-conversation.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the token.php resource.
* Deploy all files to your web server.
  
## Run the `multi-device-conversation.html` sample

* In case you are running the sample from your local computer, open `multi-device-conversation.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
* [To create a conversation](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation#how-it-works):
  * Click the `create a conversation` radio button.
  * Use the input fields to set your `subscription key` and `service region`.
  * Use the `name` input field to set your display name.
  * Use the `language` input field to set a [language code](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation#language-support)
  * Press the `create conversation` button to create a new conversation as the host.
  * If successful, a five-letter conversation code will appear in the `conversation code` input field. Optionally, to join your new conversation, open `multi-device-conversation.html` in a new browser tab or window and follow the `Join a conversation` steps below.
* [To join a conversation](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation#how-it-works):
  * Click the `join a conversation` radio button.
  * Use the `conversation code` input field to set the five-letter conversation code. 
  * Use the `name` input field to set your display name.
  * Use the `language` input field to set a [language code](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation#language-support)
  * Press the `join conversation` button to join an existing conversation as a participant.
* Press the `start speaking` button to start continuous speech recognition (dependent upon language support).
* Press the `stop speaking` button to stop continuous speech recognition.
* Press the `send instant message` button to send an instant message.
* Real-time transcription and translations will appear in the incoming messages box along with notifications of participants joining or leaving the conversation.
* Press the `leave conversation` button to leave the conversation. 
 
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

> Note: On Safari, the sample web page needs to be hosted on a web server; Safari doesn't allow websites loaded from a local file to use the microphone.

## References

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
* [Speech SDK Multi-device Conversation (Preview)](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation)
* [Language support for Multi-device conversation (Preview)](https://docs.microsoft.com/azure/cognitive-services/speech-service/multi-device-conversation#language-support)
