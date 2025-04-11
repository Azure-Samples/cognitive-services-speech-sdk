# Quickstart: Recognize speech in JavaScript on a Web Browser.

These samples demonstrate how to recognize speech using the Speech SDK for JavaScript on a web browser, like Microsoft Edge, or Chrome.

## Run the Sample within VS Code
- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- Download this sample from sample gallery to local machine.
- Trigger `Azure AI Speech Toolkit: Configure Azure Speech Resources` command from command palette to select speech resource.
- Trigger `Azure AI Speech Toolkit: Configure and Setup the Sample App` command from command palette to configure environment by install `http-server` globally. This command only needs to be run once.
- Trigger `Azure AI Speech Toolkit: Run the Sample App` command from command palette to run the sample. This will open the `index.html` file in your default browser.
- Use value of `SPEECH_RESOURCE_KEY` and `SERVICE_REGION` in `./.env/.env/dev` file to set in the input fields `Subscription` and `Region`. Click `Start Recognition` button to start recognizing speech.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC or Mac, with a working speaker.
* A text editor.
* Optionally, a web server that supports hosting PHP scripts.

## Build the sample

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

If you want to host the sample on a web server:

* Deploy all files to your web server.
* IMPORTANT: Never add a subscription key to client-side code. For advanced guidance on keeping sensitive subscription info secure, see the [sample using a server-side auth token](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/browser)

## Run the `index.html` sample

* In case you are running the sample from your local computer, open `index.html` from the location where you have downloaded this quickstart with a JavaScript capable browser.
  * Use the input fields to set your `subscription key` and `service region`.
  * Press the `Start recognition` button to start recognizing speech.
 
* In case you are hosting the sample on a web server, open a web browser and navigate to the full URL where you host the sample.

## References

* [Speech SDK API reference for JavaScript](https://aka.ms/csspeech/javascriptref)