# Embedded speech sample in C# for Unity

This sample demonstrates embedded speech using the Speech SDK for Unity (C#).
Embedded speech recognition is used as an example. For other use cases (embedded speech synthesis, hybrid speech recognition and synthesis), see the [C# .NET Core samples](../../dotnetcore/embedded-speech).
It is recommended to try them out before this Unity specific sample.

Embedded speech enables offline (on-device) speech recognition and synthesis.
Hybrid speech uses cloud speech services by default and embedded speech as a fallback in case cloud connectivity is limited.

> **Note:**
> * Embedded speech is in [preview](https://aka.ms/embedded-speech) and details in this document and samples are subject to change.
> * Embedded speech recognition only supports audio in the following format:
>   * single channel
>   * 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

See the [platform requirements for installing the Speech SDK for Unity](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp&tabs=windows%2Cubuntu%2Cunity%2Cjre%2Cmaven%2Cnodejs%2Cmac%2Cpypi#tabpanel_1_unity).

Requirements specific to this embedded speech sample are as follows.
* Unity **2020.3** or later.
* Supported target devices (at the moment these are the only supported platforms for embedded speech with Unity):
  * PC with Windows 10 or higher.
  * Android ARM-based device with **Android 7.0 (API level 24)** or higher, with a working microphone and [enabled for development](https://developer.android.com/studio/debug/dev-options).
* One or more embedded speech recognition models. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded models.

## Prepare and run the sample

1. Download the Speech SDK with embedded speech support for Unity.
   * **By downloading the Speech SDK for Unity you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
   * The Speech SDK for Unity is packaged as a Unity asset package (`.unitypackage`).
     Download it from https://aka.ms/csspeech/embedded-speech-unitypackage.
   * Also, [download the sample code to your development PC](/README.md#get-the-samples).
1. Open the sample project in Unity.
   * Start Unity. In the launch window, go to **Projects** and select **Open**.
   * Navigate to and select the folder containing this sample.
   * After a while, the Unity Editor window should appear. If there is a prompt to "Enter Safe Mode" (as "*The project you are opening contains compilation errors*" because the Speech SDK package has not been imported yet), choose **Ignore**.
1. Import the Speech SDK.
   * Open menu **Assets** > **Import Package** > **Custom Package...**.
   * In the file picker, locate and select the `Microsoft.CognitiveServices.Speech.Embedded.[version].unitypackage` file that you downloaded earlier.
   * Ensure that all files from the package are selected and choose **Import**.
     * **Note:** If you have installed Unity only for a specific platform (e.g. Unity CloudBuild for Android),
       you may need to remove other platform (iOS, Mac, WSA) binaries from the Speech SDK Plugins folder
       to avoid possible conflicts with the same library names.
1. Select the `HelloWorld` sample scene.
   * In the Project window (bottom left by default), navigate to **Assets** > **Scenes** and double-click on the `HelloWorld` scene to open it up.
1. Add embedded speech recognition model files to streaming assets.
   * In the Project window, navigate to **Assets** and select **Create** > **Folder** (right-click on Assets or go to Assets top menu). Name the folder **StreamingAssets**. (Make sure the new folder is at the same level as Scenes and Scripts under Assets.)
   * Drag and drop the embedded speech recognition model(s) of your choice to StreamingAssets. For example, in order to use an en-US model, copy the model folder `en-US` to StreamingAssets so that the model path in the Project window is `Assets\StreamingAssets\en-US` (possibly with some model specific subfolders beneath).
1. Review and update the sample code.
   * In the Project window, navigate to **Assets** > **Scripts** and double-click the `HelloWorld` C# script to edit it. (This script is used as a component of the canvas object in the HelloWorld scene.)
   * Check and adjust the values of `modelName` and `modelKey` (also `modelFiles` if Android is the target) as necessary.
     * The model name can be short (see https://aka.ms/speech/sr-languages, e.g. `en-US`) or full (e.g. `Microsoft Speech Recognizer en-US FP Model V8`).
1. Build and run the sample.
   * In the Unity Editor, use the **Play** button in the toolbar.
     * For a stand-alone application, use **File** > **Build Settings**.
   * In the sample application window, click the button to recognize speech from your microphone.

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
