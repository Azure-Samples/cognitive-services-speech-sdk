# TTS Avatar real-time API sample for Android

This sample demonstrates usage of Azure text-to-speech avatar real-time API, with Java using the Speech SDK for Android.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run Android Studio](https://developer.android.com/studio/).
* An ARM-based Android device [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working speaker. Make sure that "USB debugging" is enabled on your device.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Open this folder as a project in Android Studio.
* Edit the `MainActivity` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
  * Update below parameters if you want to use the avatar service with different configurations:
    * `avatarCharacter` - The character of the avatar. By default it's lisa, and you can update this value to use a different avatar.
    * `avatarStyle` - The style of the avatar. You can update this value to use a different avatar style. This parameter is optional for custom avatar.
    * `customAvatar` - Set this to `true` if you are using a custom avatar.
    * `ttsVoice` - The voice of the TTS. Here is the [available TTS voices list](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts#supported-languages).
    * `ttsEndpointID` - The deployment ID (also called endpoint ID) of your custom voice. If you are not using a custom voice, please leave it empty.
* Press Ctrl+F9, or select **Build** \> **Make Project**.

## Run the sample

* Connect your Android device to your development PC.
* Press Shift+F10, or select **Run** \> **Run 'app'**.
* In the deployment target windows that comes up, pick your Android device.
* On your Android device, use the buttons in the user interface to run the sample.
  * Touch `Start Session` button to setup video connection with Azure TTS Talking Avatar service. If everything goes well, you should see a live video with an avatar being shown on the screen.
  * Type some text in the text box on top of the screen and touch `Speak` button to send the text to Azure TTS Talking Avatar service. The service will synthesize the text to talking avatar video, and send the video stream back to the browser. The application will play the video stream. You should see the avatar speaking the text you typed with mouth movement, and hear the voice which is synchronized with the mouth movement.
  * You can either continue to type text in the text box and let the avatar speak that text by touching `Speak` button, or touch `Stop Session` button to stop the video connection with Azure TTS Talking Avatar service. If you touch `Stop Session` button, you can touch `Start Session` button to start a new video connection with Azure TTS Talking Avatar service.

## Note on Android permission handling

Please note that the Speech SDK requires two permissions, i.e., the INTERNET permission. As for Android API levels up to 22, requesting the permissions is handled at installation time but from API level 23 users are able to grant/revoke permissions at any time.

Note that this sample targets minSdkVersion 23 or later, so you need to remove the following code if you want to retarget to minSdkVersion 22 or earlier:

```java
// Note: we need to request the permissions
int requestCode = 5; // Unique code for the permission request
ActivityCompat.requestPermissions(MainActivity.this, new String[]{ACCESS_NETWORK_STATE, INTERNET}, requestCode);
```

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
