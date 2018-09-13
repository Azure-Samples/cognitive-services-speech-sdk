# Java Console app for the Java Run-Time Environment (JRE) on Windows or Linux (Ubuntu 16.04)

This sample demonstrates various forms of speech recognition, intent recognition, and translation using the Speech SDK for Java on Windows or Linux.

> **Note:**
> the Speech SDK for the JRE currently supports only the Windows x64 platform and Linux (Ubuntu 16.04) x64 distribution.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64, Ubuntu 16.04 x64) capable to run Eclipse, some sample scenarios require a working microphone.
* 64-bit JRE/JDK for Java 8.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.
* On Ubuntu 16.04, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install build-essential libssl1.0.0 libcurl3 libasound2 wget
  ```

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](../../README.md#get-the-samples)
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

* Save the modified files.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
