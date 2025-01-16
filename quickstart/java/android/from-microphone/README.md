# Quickstart: Recognize speech from a microphone in Java on Android

This sample demonstrates how to recognize speech with Java using the Speech SDK for Android.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=dotnet%2Cx-android%2Clinux%2Candroid%2Cwindowsinstall&pivots=programming-language-java) on the SDK documentation page which describes how to build this sample from scratch in Android Studio.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android).

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

## Note on Android permission handling

Please note that the Speech SDK requires two permissions, i.e., the INTERNET as well as the RECORD_AUDIO permission. As for Android API levels up to 22, requesting the permissions is handled at installation time but from API level 23 users are able to grant/revoke permissions at any time.

Note that this quickstart targets minSdkVersion 21, so you need to add the following code if you want to retarget to minSdkVersion 23 or later:

```java
// Initialize SpeechSDK and request required permissions.
try {
    // a unique number within the application to allow
    // correlating permission request responses with the request.
    int permissionRequestId = 5;

    // Request permissions needed for speech recognition
    ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET}, permissionRequestId);
}
catch (Exception ex) {
    Log.e("SpeechSDK", "could not init sdk, " + ex.toString());
    recognizedTextView.setText("Could not initialize: " + ex.toString());
}
```

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
