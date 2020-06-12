# Sample: Start speech recognition by using Unity's microphone and streaming audio to the Speech SDK for Unity

This Speech SDK sample demonstrates how to use Unity's microphone and streaming mic audio using PushAudioModeStream in [Unity](https://unity3d.com/).
Using Unity's microphone instead of internal microphone from the Speech SDK can be useful in scenarios where user wants e.g. to save the audio recorded from the microphone for some other purposes. Also this can be usefull in platforms like Android which has limitations in shared microphone accesses. 

> Note:
> The Speech SDK for Unity supports Windows Desktop (x86 and x64) or Universal Windows Platform (x86, x64, ARM/ARM64), Android (x86, ARM32/64) and iOS (x64 simulator, ARM64).
> Speech SDK support has been built and verified for Windows and Android using Windows PC development environment for Unity and for iOS using Mac development environment for Unity.

## Prerequisites

* This sample is targeted for Unity 2018.3 or later. To install, pick an appropriate option in the [Unity store](https://store.unity.com/).
  > Note:
  > [UWP ARM64 support was only added in Unity 2019.1](https://blogs.unity3d.com/2019/04/16/introducing-unity-2019-1/#universal)
  > If you are not familiar with Unity, it is recommended to study the [Unity User Manual](https://docs.unity3d.com/Manual/UnityManual.html) before starting your app development.
* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later and with a working microphone.
* A Mac device with Xcode and Unity installed for iOS development.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* For Windows ARM64 support, install the [optional build tools, and Windows 10 SDK for ARM/ARM64](https://blogs.windows.com/buildingapps/2018/11/15/official-support-for-windows-10-on-arm-development/)
* Access to your computer's microphone.
* To target Android: an Android device (ARM32/64, x86; API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.
* To target iOS: an iOS device (ARM64) [enabled for development](https://learn.unity.com/tutorial/building-for-mobile) with a working microphone.

## Download the Speech SDK for Unity and the sample code

* **By downloading the Speech SDK for Unity you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* The Speech SDK for Unity is packaged as a Unity asset package (.unitypackage).
  Download it from [here](https://aka.ms/csspeech/unitypackage).
* Also, [download the sample code to your development PC.](../../README.md#get-the-samples)

## Prepare the sample

* Open the sample project in Unity:
  * Start Unity. In the launch window, under the **Projects** tab select **Open**.
  * Select the folder which contains this sample.
  * After a bit of time, the Unity Editor window should pop up.
    > Note: do not worry about the error  `The type or namespace name
    > 'CognitiveServices' does not exist in the namespace 'Microsoft' (are you
    > missing an assembly reference?)` after opening the sample. This error
    > gets resolved in the next step by importing the Speech SDK.
* Import the Speech SDK:
  * Import the Speech SDK by selecting **Assets** > **Import Package** > **Custom Package**.
  * In the file picker, select the Speech SDK .unitypackage file that you downloaded before.
  * Ensure that all files are selected and click **Import**.
* Select the `HelloWorld` sample scene:
  * In the Project Window (bottom left by default), navigate to **Assets** > **Scenes** and double-click on the `HelloWorld` scene to open it up.
* Update the sample code with your subscription data:
  * In the Project Window, navigate to **Assets** > **Scripts** and double-click the `HelloWorld` C# script to edit it.
    Note: you can configure which code editor will be launched under **Edit** > **Preferences**, for details see [here](https://docs.unity3d.com/Manual/Preferences.html).
  * Do the following edits:
    * Replace the string `YourSubscriptionKey` with your own subscription key.
    * Replace the string `YourServiceRegion` with the service region of your subscription. For example, replace with `westus` if you are using the 30-day free trial subscription.
    * Save the file.

> Note:
> the HelloWorld C# script is used as a component of the canvas object in the HelloWorld scene.

## Run the sample

* Press the **Play** button in the Unity Editor toolbar (below the menu bar).
  * In the Unity Editor's Game Window, click the `Start` button and say aloud some speech phrase, e.g. `What's the weather like?`. Expected result is that speech phrase get transcribed and transcribed text is shown in the UI. Speech transcription will continue until `Stop` button is pressed.
   * Check also the [Console Window](https://docs.unity3d.com/Manual/Console.html) for debug messages.
* Click the **Play** button again to stop running the app.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-unity)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
