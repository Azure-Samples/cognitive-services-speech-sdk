# Java Console app for the Java Run-Time Environment (JRE) on Windows or Linux

This sample demonstrates various forms of speech recognition, intent recognition, speech synthesis, and translation using the Speech SDK for Java on Windows or Linux.

> **Note:**
> the Speech SDK for the JRE currently supports only the Windows x64 platform, and Linux x64 and ARM64 (Ubuntu 16.04, Ubuntu 18.04, Debian 9, RHEL 8, CentOS 8).

## Prerequisites

* A subscription key for the Speech service. [Create a new Azure account, and try Cognitive Services for free.](https://azure.microsoft.com/free/cognitive-services/)
* A PC (Windows x64, Ubuntu 16.04 or 18.04 on x64 or ARM64, RHEL 8 or CentOS 8 on x64) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) some sample scenarios require a working microphone.
* 64-bit JRE/JDK for Java 8.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.[<sup>[1]</sup>](#footnote1)
* On Ubuntu, run the following commands for the installation of required packages:

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

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* To tailor the sample to your environment, use search and replace across the whole project to update the following strings:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * `YourEndpointId` (optional): replace with the endpoint ID of your customized model in [CRIS](https://cris.ai).
  * `YourAudioFile.wav`: replace with a path to a `.wav` file on your disk **(required format: 16 kHz sample rate, 16 bit samples, mono / single-channel)**
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.
    * `YourLanguageUnderstandingIntentName1`, `YourLanguageUnderstandingIntentName2`, `YourLanguageUnderstandingIntentName3`: replace with names of intents that your Language Understanding service application recognizes.
  * The following settings apply to keyword-triggered recognition:
    * `YourKeywordRecognitionModelFile.table`: replace with the location of your keyword recognition model file.
    * `YourKeyword`: replace with the phrase your keyword recognition model triggers on.

* Save the modified files.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
