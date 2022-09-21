# Quickstart: Recognize speech in JavaScript on a Web Browser.

These samples demonstrate how to recognize speech using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.

## Prerequisites

* A subscription key and region for the Speech service [Create one for free](https://azure.microsoft.com/en-us/free/cognitive-services/)
* [Create a Speech resource](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices) in the Azure portal.
* Get the resource key and region. After your Speech resource is deployed, select Go to resource to view and manage keys. or  more information about Cognitive Services resources, see [Get the keys for your resource](https://docs.microsoft.com/en-us/azure/cognitive-services/cognitive-services-apis-create-account?tabs=multiservice%2Cwindows#get-the-keys-for-your-resource)
* A PC or Mac, with a working speaker.
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

If you want to host the sample on a web server:

* Deploy all files to your web server.
* IMPORTANT: Never add a subscription key to client-side code. For advanced guidance on keeping sensitive subscription info secure, see the [sample using a server-side auth token](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/browser)

## Run the `index.html` sample

* In case you are running the sample from your local computer, open `index.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
  * Use the input fields to set your `subscription key` and `service region`.
  * Input a text to be synthesized in the `Text` field and press the `Start synthesis` button to start synthesizing speech.
 
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)