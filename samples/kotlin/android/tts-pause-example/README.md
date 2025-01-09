# Speech synthesis sample for Android

This sample demonstrates how to synthesize speech with Kotlin using the Speech SDK for Android with the possibility of pausing the audio stream.

> **Note:**
> this sample is *not* for the Speech Devices SDK and the Roobo device.
> If you are looking for further information on these, visit the [Speech Devices SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk) page.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working speaker. Make sure that "USB debugging" is enabled on your device.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
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
* On your Android device, use the buttons in the user interface to run the sample.$

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)