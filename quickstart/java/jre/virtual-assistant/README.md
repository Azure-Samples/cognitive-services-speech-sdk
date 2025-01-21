# Quickstart: Using Voice Assistant in Java on Windows, macOS or Linux

This sample demonstrates how to recognize speech for custom voice assistants and receive activity responses.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Eclipse](https://www.eclipse.org),[<sup>[1]</sup>](#footnote1) with a working microphone.
* A pre-configured bot created using Bot Framework version 4.2 or above. See [here for steps on how to create a bot](https://blog.botframework.com/2018/05/07/build-a-microsoft-bot-framework-bot-with-the-bot-builder-sdk-v4/). The bot would need to subscribe to the new "Direct Line Speech" channel to receive voice inputs.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/voice-assistants?pivots=programming-language-java&tabs=jre).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* Edit the `Main` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.   
* Save the modified file.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre)
* [Speech SDK API reference for Java](https://docs.microsoft.com/java/api/com.microsoft.cognitiveservices.speech.dialog?view=azure-java-stable)
