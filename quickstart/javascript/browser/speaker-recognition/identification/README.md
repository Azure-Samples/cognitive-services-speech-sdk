# Quickstart: Identify a speaker in JavaScript on a Web Browser.

These samples demonstrate how to identify a speaker with audio sample files using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac.
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](../../../README.md#get-the-samples)

If you want to host the sample on a web server:

* Deploy all files to your web server.
* IMPORTANT: Never add a subscription key to client-side code. For advanced guidance on keeping sensitive subscription info secure, see the [sample using a server-side auth token](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/browser)

## Run the `independent-identification.html` sample

* In case you are running the sample from your local computer, open `independent-identification.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
* Use the input fields to set your `subscription key` and `service region`.
* Press the `Create Profile` button to create a voice profile and enroll using the chosen enrollment file.
* Press the `Identify Speaker` button to identify a speaker using the chosen identification file.
   
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-file?pivots=programming-language-javascript)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
