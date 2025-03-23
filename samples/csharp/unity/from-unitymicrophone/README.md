# Sample: Start speech recognition by using Unity's microphone and streaming audio to the Speech SDK for Unity

This Speech SDK sample demonstrates how to use Unity's microphone and streaming mic audio using PushAudioModeStream in [Unity](https://unity3d.com/).
Using Unity's microphone instead of internal microphone from the Speech SDK can be useful in scenarios where user wants e.g. to save the audio recorded from the microphone for some other purposes. Also this can be usefull in platforms like Android which has limitations in shared microphone accesses. 

> Note:
> The Speech SDK for Unity supports Windows Desktop (x86 and x64) or Universal Windows Platform (x86, x64, ARM/ARM64), Android (x86, ARM32/64), iOS (x64 simulator, ARM64), Mac (x64) and Linux (x64).

## Prerequisites

* This sample is targeted for Unity 2020.3 or later. To install, pick an appropriate option in the [Unity store](https://store.unity.com/).
  > Note:
  > If you are not familiar with Unity, it is recommended to study the [Unity User Manual](https://docs.unity3d.com/Manual/UnityManual.html) before starting your app development.
* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later and with a working microphone.
* A Mac device with Xcode and Unity installed for iOS development.
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.
* Access to your computer's microphone.
* To target Android: an Android device (ARM32/64, x86; API 23: Android 6.0 Marshmallow or higher) [enabled for development](https://developer.android.com/studio/debug/dev-options) with a working microphone.
* To target iOS: an iOS device (ARM64) [enabled for development](https://learn.unity.com/tutorial/building-for-mobile) with a working microphone.

## Download the Speech SDK for Unity and the sample code

* **By downloading the Speech SDK for Unity you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* The Speech SDK for Unity is packaged as a Unity asset package (.unitypackage).
  Download it from [here](https://aka.ms/csspeech/unitypackage).
* Also, [download the sample code to your development PC.](/README.md#get-the-samples)

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
    * NOTE: If you have installed Unity only for specific platform e.g. Unity CloudBuild for Android, be aware that you may need to remove other
      platform (iOS, Mac, WSA) binaries from the Speech SDK Plugins folder to avoid possible conflicts with the same library name.
  * If you use the Speech SDK **1.43.0** or newer release, the Unity Editor Console will show errors ending with: "*Unable to resolve reference 'Azure.Core'.*"
    See the next step for how to resolve this.
* Install dependencies.
  * Starting with the Speech SDK **1.43.0** release, the C# bindings depend on the [Azure.Core](https://www.nuget.org/packages/Azure.Core) NuGet package.
    The following is a suggested method for easy installation:
    * Download the latest **NuGetForUnity** `.unitypackage` from https://github.com/GlitchEnzo/NuGetForUnity.
    * In the Unity Editor, open menu **Assets** > **Import Package** > **Custom Package...**.
    * In the file picker, locate and select the `NuGetForUnity.[version].unitypackage` file that you downloaded earlier.
    * Ensure that all files from the package are selected and choose **Import**.
    * Open menu **NuGet** > **Manage NuGet Packages**. (This only appears after the installation of NuGetForUnity.)
    * Search for **Azure.Core**, select it for installation, and **Install**.
    * Close the Unity Editor. This is necessary in order to clean up any remaining errors and update the project - otherwise the sample will not work.
    * Re-open the sample project in Unity. There should be no errors anymore, and you can proceed normally.
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
