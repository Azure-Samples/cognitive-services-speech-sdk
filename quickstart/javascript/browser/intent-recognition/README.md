# Quickstart: Using the Speech Service to Recognize intent in JavaScript on a Web Browser.

This sample shows how to recognize intent with the Speech Service using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome. It illustrates how the SDK can be used to translate speech in one language to text in a different language.

* See the [overview article](https://docs.microsoft.com/azure/cognitive-services/speech-service/intent-recognition) on the SDK documentation page to learn more about intent recognition using the Speech Service.

## Prerequisites

* A LUIS account. You can get one for free through the [LUIS portal](https://www.luis.ai/home).
* A new or existing LUIS app - [create LUIS app](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/intent-recognition#create-a-luis-app-for-intent-recognition)
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
* To run this sample, you will need to create or use an existing LUIS app - [create LUIS app](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/intent-recognition#create-a-luis-app-for-intent-recognition)
* Use the input fields to set your `LUIS Primary key` and `LUIS Location`.
* From the LUIS app portal, in the Manage menu, copy the App ID from the Application Settings screen into the `LUIS App ID` field.
* Press the `Start Intent Recognition` button to start recognizing speech intents from your microphone.
  
## Running .html samples
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

> Note: On Safari, the sample web page needs to be hosted on a web server; Safari doesn't allow websites loaded from a local file to use the microphone.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
* [Speech SDK Intent Recognition](https://docs.microsoft.com/azure/cognitive-services/speech-service/intent-recognition)
