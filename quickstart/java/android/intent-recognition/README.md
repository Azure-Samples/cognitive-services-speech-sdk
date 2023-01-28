# Quickstart: Recognize intent in Java on Android

This sample demonstrates how to recognize intent with Java using the Speech SDK for Android.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=dotnet%2Cx-android%2Clinux%2Candroid%2Cwindowsinstall&pivots=programming-language-java) on the SDK documentation page which describes how to build this sample from scratch in Android Studio.

## Prerequisites

* A LUIS account. You can get one for free through the [LUIS portal](https://www.luis.ai/home).
* A new or existing LUIS app - [create LUIS app](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/intent-recognition#create-a-luis-app-for-intent-recognition)
* A PC (Windows, Linux, Mac) capable to run Android Studio.
* [Android Studio](https://developer.android.com/studio/).
* An ARM32, ARM64, or x86-based Android device (API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-android).

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own LUIS subscription key.
  * Replace the string `YourServiceRegion` with the service region of your LUIS subscription.
  * Replace the string `YourApplicationKey` with your LUIS application ID.
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
