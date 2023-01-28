# Quickstart: Synthesize speech in Java on Android

This sample demonstrates how to synthesize speech with Java using the Speech SDK for Android.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-java-android) on the SDK documentation page which describes how to build this sample from scratch in Android Studio.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run Android Studio.
* [Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device (API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working speaker, or an [Android emulator](https://developer.android.com/studio/run/emulator).

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-java-android).

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the free trial subscription.
* Press Ctrl+F9, or select **Build** \> **Make Project**.

## Run the sample

* Connect your Android device to your development PC, or create an Android emulator.
* Press Shift+F10, or select **Run** \> **Run 'app'**.
* In the deployment target windows that comes up, pick your Android device.

## Note on Android permission handling

Please note that the Speech SDK requires the INTERNET permission. As for Android API levels up to 22, requesting the permissions is handled at installation time but from API level 23 users are able to grant/revoke permissions at any time.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-text-to-speech-java-android)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
