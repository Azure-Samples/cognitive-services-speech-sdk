# Quickstart: Using Virtual assistant in Java on Windows or Linux (Ubuntu 16.04/18.04)

This sample demonstrates how to translate speech with the Speech SDK for Java on Windows or Linux (Ubuntu 16.04)
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-translate-speech-java-jre) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64, Ubuntu 16.04/18.04 x64) capable to run Eclipse, with a working microphone.
* 64-bit JRE/JDK for Java 8.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.
* A pre-configured bot created using Bot Framework version 4.2 or above. See [here for steps on how to create a bot](https://blog.botframework.com/2018/05/07/build-a-microsoft-bot-framework-bot-with-the-bot-builder-sdk-v4/). The bot would need to subscribe to the new "Direct Line Speech" channel to receive voice inputs.

On Ubuntu 16.04, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install build-essential libssl1.0.0 libasound2 wget
  ```

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-virtual-assistant-java-jre).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](../../../README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* Edit the `Main` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For now this will only be supported in "westus2".
  * Replace the string `YourChannelSecret` with the secret created when you subscribed your bot to the Direct Line Speech channel.    
* Save the modified file.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-virtual-assistant-java-jre)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
