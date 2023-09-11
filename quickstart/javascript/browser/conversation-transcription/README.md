# Quickstart: Transcribe a conversation in JavaScript on a Web Browser.

This sample demonstrates how to transcribe a conversation from a file using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
* See the [overview article](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription) on the SDK documentation page to learn more about Conversation Transcription (Preview).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac.
* A wav file.
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

If you want to host the sample on a web server:

* Deploy all files to your web server.
* IMPORTANT: Never add a subscription key to client-side code. For advanced guidance on keeping sensitive subscription info secure, see the [sample using a server-side auth token](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/browser)

## Run the `index.html` sample

* In case you are running the sample from your local computer, open `conversation-transcription.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
* Use the input fields to set your `subscription key` and `service region`.
* Press the `Start transcribing conversation` button to begin transcription.
* Transcription results and speaker IDs will appear in the incoming messages box.
* Press the `Stop transcribing conversation` button to end transcription. 
   
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
* [Speech SDK Conversation Transcription (Preview)](https://docs.microsoft.com/azure/cognitive-services/speech-service/conversation-transcription)
