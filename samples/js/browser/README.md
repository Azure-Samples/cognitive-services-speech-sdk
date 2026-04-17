# JavaScript Speech Recognition, Synthesis, and Translation Sample for the Web Browser

This sample demonstrates how to recognize, synthesize, and translate speech with the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.
See [this article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser) for introductory information on the Speech SDK for JavaScript.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac, with a working speaker.
* A text editor.
* Ensure you have [Node.js](https://nodejs.org/en/download/) installed.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

If you want to host the sample on a web server, the web server must be able to run Node. Follow these steps:

* Edit the `.env` source:
  * Replace the string `paste-your-speech-region-here` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * Replace the string `paste-your-speech-key-here` with your own subscription key.
* Deploy all files to your web server.

## How to run the sample

* Clone this repo, then change directory to the project root and run `npm install` to install dependencies.
* Add your Azure Speech key and region to the `.env` file, replacing the placeholder text.
* To run the Express server, run `npm run server`.

* In case you are running the sample from your local computer, open `public/index.html` or `public/synthesis.html` from the location where you have downloaded this sample with a JavaScript capable browser.
  Use the input fields to set your subscription key and service region.
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [JavaScript quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-js-browser)
* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)
