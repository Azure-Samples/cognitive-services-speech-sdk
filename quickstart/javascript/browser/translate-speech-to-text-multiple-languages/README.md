# Quickstart: Using the Speech Service to Translate Speech to multiple languages in JavaScript on a Web Browser.

This sample shows how to translate speech into multiple languages with the Speech Service using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome. It illustrates how the SDK can be used to translate speech in one language to text in a different language.

* See the [overview article](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-translation) on the SDK documentation page to learn more about translation using the Speech Service.

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
* Edit the `index.html` source:
  * Replace the value for the variable `authorizationEndpoint` with the full URL where you can access the token.php resource.
* Deploy all files to your web server.

## Run the `index.html` sample

* In case you are running the sample from your local computer, open `index.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
* Use the input fields to set your `subscription key` and `service region`.
* Select the source language from the `Source Language` dropdown.
* Select the two target output languages from the `Target Language` dropdowns.
* Press the `Start recognition` button to start recognizing speech from your microphone.
  
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

> Note: On Safari, the sample web page needs to be hosted on a web server; Safari doesn't allow websites loaded from a local file to use the microphone.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
* [Speech SDK Speech Translation](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-translation)
