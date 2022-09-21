# Quickstart: Keyword Recognition in Java on Android

This sample demonstrates how to recognize keywords and stream audio from the result with Java using the Speech SDK for Android.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android) on the SDK documentation page which describes how to build this sample from scratch in Android Studio.

## Prerequisites

* A subscription key and region for the Speech service [Create one for free](https://azure.microsoft.com/en-us/free/cognitive-services/)
* [Create a Speech resource](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices) in the Azure portal.
* Get the resource key and region. After your Speech resource is deployed, select Go to resource to view and manage keys. or  more information about Cognitive Services resources, see [Get the keys for your resource](https://docs.microsoft.com/en-us/azure/cognitive-services/cognitive-services-apis-create-account?tabs=multiservice%2Cwindows#get-the-keys-for-your-resource)
* A PC (Windows, Linux, Mac) capable to run Android Studio.
* [Android Studio](https://developer.android.com/studio/).
* Android device or emulator (API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android).

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Press Ctrl+F9, or select **Build** \> **Make Project**.

## Run the sample

* Connect your Android device to your development PC.
* Press Shift+F10, or select **Run** \> **Run 'app'**.
* In the deployment target windows that comes up, pick your Android device.

## Note on Android permission handling

Please note that this Speech SDK sample requires the RECORD_AUDIO permissions. As for Android API levels up to 22, requesting the permissions is handled at installation time but from API level 23 users are able to grant/revoke permissions at any time.

Note that this quickstart targets minSdkVersion 23.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
