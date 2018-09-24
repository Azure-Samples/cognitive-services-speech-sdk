# Speech recognition sample for Android

This sample demonstrates how to recognize speech and intents with Java using the Speech SDK for Android.

> **Note:**
> this sample is *not* for the Speech Devices SDK and the Roobo device.
> If you are looking for further information on these, visit the [Speech Devices SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk) page.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run Android Studio.
* Version 3.1 of [Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device (API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](../../README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
  * If you'd like to use intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs), also do the following edits:
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * Replace the string `YourLanguageUnderstandingAppId` with the ID of a Language Understanding service application that you want to recognize intents with.
    * `YourLanguageUnderstandingIntentName1`, `YourLanguageUnderstandingIntentName2`, `YourLanguageUnderstandingIntentName3`: replace with names of intents that your Language Understanding service application recognizes.
* Press Ctrl+F9, or select **Build** \> **Make Project**.

## Run the sample

* Connect your Android device to your development PC.
* Press Shift+F10, or select **Run** \> **Run 'app'**.
* In the deployment target windows that comes up, pick your Android device.
* On your Android device, use the buttons in the user interface to pick the different sample scenarios.

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
