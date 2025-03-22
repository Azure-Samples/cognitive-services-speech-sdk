# Embedded speech sample in C# for Unity

This sample demonstrates embedded (offline, on-device) speech recognition and synthesis with the Speech SDK for Unity (C#).
You can choose whether to use recognition or synthesis, or both.

For more advanced use cases, see the [C# .NET Core samples](../../dotnetcore/embedded-speech).
It is recommended to try them out before this Unity specific sample.

> **Note:**
> * Embedded speech recognition only supports audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

See the [platform requirements for installing the Speech SDK for Unity](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp&tabs=windows%2Cubuntu%2Cunity%2Cjre%2Cmaven%2Cnodejs%2Cmac%2Cpypi#tabpanel_1_unity).

Requirements specific to this embedded speech sample are as follows.
* Unity **2020.3** or later.
* Supported target devices (at the moment these are the only supported platforms for embedded speech with Unity):
  * PC with Windows 10 or higher.
  * Android ARM-based device with **Android 7.0 (API level 24)** or higher, with a working microphone
    and [enabled for development](https://developer.android.com/studio/debug/dev-options).
    Note that embedded speech synthesis with neural voices is only supported on ARM64.
* One or more embedded speech models. See https://aka.ms/embedded-speech for the latest information on how to obtain models.

## Prepare and run the sample

1. Download the Speech SDK with embedded speech support for Unity.
   * **By downloading the Speech SDK for Unity you acknowledge its license,
     see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
   * The Speech SDK for Unity is packaged as a Unity asset package (`.unitypackage`).
     Download it from https://aka.ms/csspeech/embedded-speech-unitypackage.
   * Also, [download the sample code to your development PC](/README.md#get-the-samples).
1. Open the sample project in Unity.
   * Start Unity. In the launch window, go to **Projects** and select **Open**.
   * Navigate to and select the folder containing this sample.
   * After a while, the Unity Editor window should appear.
     If there is a prompt to "Enter Safe Mode" (as "*The project you are opening contains compilation errors*"
     because the Speech SDK package has not been imported yet), choose **Ignore**.
1. Import the Speech SDK.
   * Open menu **Assets** > **Import Package** > **Custom Package...**.
   * In the file picker, locate and select the `Microsoft.CognitiveServices.Speech.Embedded.[version].unitypackage` file that you downloaded earlier.
   * Ensure that all files from the package are selected and choose **Import**.
     * **Note:** If you have installed Unity only for a specific platform (e.g. Unity CloudBuild for Android),
       you may need to remove other platform (iOS, Mac, WSA) binaries from the Speech SDK Plugins folder
       to avoid possible conflicts with the same library names.
   * If you use the Speech SDK **1.43.0** or newer release, the Unity Editor Console will show errors ending with: "*Unable to resolve reference 'Azure.Core'.*"
     See the next step for how to resolve this.
1. Install dependencies.
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
1. Select the `HelloWorld` sample scene.
   * In the Project window (bottom left by default), navigate to **Assets** > **Scenes** and double-click on the `HelloWorld` scene to open it up.
1. Add embedded speech model files to streaming assets.
   * In the Project window, navigate to **Assets** and select **Create** > **Folder** (right-click on Assets or go to Assets top menu).
     Name the folder **StreamingAssets**. (Make sure the new folder is at the same level as Scenes and Scripts under Assets.)
   * Drag and drop the embedded speech model(s) of your choice to StreamingAssets.
     For example, in order to use the speech recognition en-US model, copy the model folder `en-US` to StreamingAssets so that the model path in the Project window is
     `Assets\StreamingAssets\en-US` (possibly with some model specific subfolders beneath).
1. Review and update the sample code.
   * In the Project window, navigate to **Assets** > **Scripts** and double-click the `HelloWorld` C# script to edit it.
     (This script is used as a component of the canvas object in the HelloWorld scene.)
   * Update the settings marked as configurable (model/voice name and license, also the list of files if Android is the target).
     If either recognition or synthesis is not needed, leave the corresponding name string empty.
1. Build and run the sample.
   * In the Unity Editor, use the **Play** button in the toolbar.
     * For a stand-alone application, use **File** > **Build Settings**.
   * Once the sample app is running, use the app buttons as follows.
     * *Copy model files* : Copies model files as configured in `HelloWorld.cs` to the app data directory on Android.
       This step can be skipped on Windows where files in streaming assets can be used directly.
       * This must be done the very first time the sample app is run and before anything else.
         It is not necessary later unless model files are changed.
     * *Initialize objects* : Initializes the recognizer and/or synthesizer as configured in `HelloWorld.cs`.
       * This can take a moment due to loading of model data, so it is best done as a separate stage in advance before starting recognition or synthesis.
     * *Recognize speech* : Listens to the device default microphone for input and transcribes recognized speech to text in the app window.
       This returns one result - run it again to recognize more.
     * *Synthesize speech* : Reads input from the text entry above this button, and synthesizes speech to the device default speaker.

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
