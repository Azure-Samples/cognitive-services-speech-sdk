# Speech recognition sample for compressed audio stream on Android

This sample demonstrates how to recognize speech from compressed audio input with Java using the Speech SDK for Android.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone. Make sure that "USB debugging" is enabled on your device.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Create folder `jniLibs` inside `app\src\main`. (`jniLibs` should be at the same level as the `java` folder)
* Create four folders inside `jniLibs` with names `arm64-v8a`, `armeabi-v7a`, `x86`, `x86_64` respectively.
* Create `libgstreamer_android.so` for each Application Binary Interface (ABI) using the information provided in  [Compressed audio input with the Speech SDK on Android](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams-android)
* Place the `libgstreamer_android.so` inside `jniLibs\arm64-v8a` and the other folders respectively. 
* Create a folder named `input` in the root of your phone's local storage. Copy the file [whatstheweatherlike.mp3](./whatstheweatherlike.mp3) into this folder.
* Press Ctrl+F9, or select **Build** \> **Make Project**.

## Run the sample

* Connect your Android device to your development PC.
* Press Shift+F10, or select **Run** \> **Run 'app'**.
* In the deployment target windows that comes up, pick your Android device.
* On your Android device, see the app launch. click on the `BUTTON`. After a few seconds you should see text displaying the speech recognition Result ID (a GUID), followed by `Status:RecognizedSpeech Recognized text:<What's the weather like?>.`

## Troubleshooting

If you don't see the recognized text, troubleshoot using the Android studio `Logcat` Window. The sample code logs exceptions. For example, if you did not place the mp3 file in the correct place, you will see a log line similar to this:
```
4059-4059/com.microsoft.cognitiveservices.speech.samples.compressedinput E/SpeechSDKDemo: unexpected /storage/emulated/0/input/whatstheweatherlike.mp3 (No such file or directory)
```

## Note on Android permission handling

Please note that the Speech SDK requires following permissions, i.e., the INTERNET, READ_EXTERNAL_STORAGE as well as the RECORD_AUDIO permission. As for Android API levels up to 22, requesting the permissions is handled at installation time but from API level 23 users are able to grant/revoke permissions at any time.

Note that this sample targets minSdkVersion 23 or later, so you need to remove the following code if you want to retarget to minSdkVersion 22 or earlier:

```java
// Initialize SpeechSDK and request required permissions.
try {
    // a unique number within the application to allow
    // correlating permission request responses with the request.
    int permissionRequestId = 5;

    // Request permissions needed for speech recognition
    ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET, READ_EXTERNAL_STORAGE}, permissionRequestId);
}
catch (Exception ex) {
    Log.e("SpeechSDK", "could not init sdk, " + ex.toString());
    recognizedTextView.setText("Could not initialize: " + ex.toString());
}
```

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
* [Use codec compressed audio input with the Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-codec-compressed-audio-input-streams?tabs=debian&pivots=programming-language-java)
