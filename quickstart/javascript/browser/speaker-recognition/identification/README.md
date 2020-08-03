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
* From the [Speech SDK for JavaScript .zip package](https://aka.ms/csspeech/jsbrowserpackage) extract the file
  `microsoft.cognitiveservices.speech.sdk.bundle.js` and place it into the folder that contains this quickstart.

If you want to host the sample on a web server:

* The web server must be secure (HTTPS).
* Edit the `token.php` source:
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
    Note: As a Preview feature, speaker identification is only available for the westus region.
  * Replace the string `YourSubscriptionKey` with your own subscription key.
* Edit the `index.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the token.php resource.
* Deploy all files to your web server.

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
