# Quickstart: Using voice assistant in Java on Windows or Linux (Ubuntu 16.04, Ubuntu 18.04, Debian 9)

This sample demonstrates how to recognize speech for custom voice assistant and receive activity responses.
See the [accompanying article](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> the Speech SDK for the JRE currently supports only the Windows x64 platform, macOS 10.13 or later, and Linux x64 and ARM64 (Ubuntu 16.04, Ubuntu 18.04, Debian 9).

## Prerequisites

* A subscription key for the Speech service any one of the supported [Voice Assistant Azure regions](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/regions#voice-assistants). See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64, Ubuntu 16.04/18.04 on x64 or ARM64) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) with a working microphone.
* 64-bit JRE/JDK for Java 8.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.[<sup>[1]</sup>](#footnote1)
* A pre-configured bot created using Bot Framework version 4.2 or above. See [here for steps on how to create a bot](https://blog.botframework.com/2018/05/07/build-a-microsoft-bot-framework-bot-with-the-bot-builder-sdk-v4/). The bot would need to subscribe to the new "Direct Line Speech" channel to receive voice inputs.
* On Ubuntu 16.04 or 18.04, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl1.0.0 libasound2
  ```
* On Debian 9, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl1.0.2 libasound2
  ```

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM64.</small>

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](../../../README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* Edit the `Main` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    > **Note:** In preview, the Direct Line Speech channel currently supports only the `westus2` region.
  * Replace the string `YourChannelSecret` with the secret created when you subscribed your bot to the Direct Line Speech channel.    
* Save the modified file.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
