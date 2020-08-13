# Quickstart: Using Voice Assistant in Java on Windows or Linux

This sample demonstrates how to recognize speech for custom voice assistants and receive activity responses.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-voice-assistant-java-jre) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> the Speech SDK for the JRE currently supports only the Windows x64 platform, macOS 10.13 or later, and Linux x64 (Ubuntu 16.04, Ubuntu 18.04, Debian 9, RHEL 8, CentOS 8) and ARM64 (Ubuntu 16.04, Ubuntu 18.04, Debian 9).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64, Ubuntu 16.04/18.04 on x64 or ARM64, RHEL 8 or CentOS 8 on x64) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) with a working microphone.
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

* On RHEL 8 or CentOS 8, run the following commands for the installation of required packages:

  ```sh
  sudo yum update
  sudo yum install alsa-lib java-1.8.0-openjdk-devel openssl
  ```

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM64.</small>

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/voice-assistants&pivots=programming-language-java).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
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
