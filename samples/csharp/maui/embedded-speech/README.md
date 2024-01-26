# Embedded speech sample in C# for MAUI

This sample demonstrates embedded (offline, on-device) speech recognition and synthesis with the Speech SDK for .NET MAUI.
You can choose whether to use recognition or synthesis, or both.

For more advanced use cases, see the [C# .NET Core samples](../../dotnetcore/embedded-speech).
It is recommended to try them out before this MAUI specific sample.

> **Note:**
> * Embedded speech recognition only supports audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

* [Supported platforms for MAUI application development](https://learn.microsoft.com/dotnet/maui/supported-platforms?view=net-maui-7.0).
  Embedded speech with MAUI currently supports only the following targets:
  * Windows 10 or higher on x64 or ARM64 hardware.
  * Android ARM-based devices with **Android 7.0 (API level 24)** or higher.
    Embedded speech synthesis with neural voices is only supported on ARM64.
* [Microsoft Visual Studio 2022](https://www.visualstudio.com/) with .NET MAUI workload and [.NET 7.0 or later](https://learn.microsoft.com/dotnet/core/install/windows?tabs=net70).
  See [prerequisites for MAUI](https://learn.microsoft.com/dotnet/maui/get-started/installation?view=net-maui-7.0&tabs=vswin).
* One or more embedded speech models. See https://aka.ms/embedded-speech for the latest information on how to obtain models.

## Prepare the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Navigate to the folder containing this sample, and copy the embedded speech model(s) of your choice to the project resource folder `embedded-speech\Resources\Raw`.
  * All the files of each model must be present as normal individual files in a model folder, not in a package.
  * The model internal subfolder structure must be intact i.e. as originally delivered.
* Start Microsoft Visual Studio 2022 and select **File** \> **Open** \> **Project/Solution**.
* Find and select the solution file in this sample folder.
* Edit the `MainPage.xaml.cs` source file under the main project and update the settings marked as configurable (model name, key, and list of files).
  If either recognition or synthesis is not needed, leave the corresponding default values unchanged.

## Build and run the sample for Windows
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, select **Debug** \> **Start Debugging** or **Start Without Debugging**.
* The application may request the microphone access which you need to allow in order to have the speech to text functionality using microphone as input.
* Once the sample app is running, use the app buttons as follows.
  * *Copy model files* : Copies model files as configured in `MainPage.xaml.cs` to the app data directory on device.
    * The full path is shown in the status field at the bottom of the app window after copying is finished.
    * This must be done the very first time the sample app is run and before anything else. It is not necessary later unless model files are changed.
  * *Initialize objects* : Initializes the recognizer and/or synthesizer as configured in `MainPage.xaml.cs`.
    * This can take a moment due to loading of model data, so it is best done as a separate stage in advance before starting recognition or synthesis.
  * *Recognize speech* : Listens to the device default microphone for input and transcribes recognized speech to text in the app window. This returns one result - run it again to recognize more.
  * *Synthesize speech* : Reads input from the text entry above this button, and synthesizes speech to the device default speaker.

## Build and run the sample for Android
* Set the target machine to Android device or emulator of your choice. Note that MAUI will request to install required Android SDKs before you can run the sample using Android machine target.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, use the **Debug** menu, or click the selected Android device or emulator; see **Android emulators** or **Android local devices** in target device drop down selector.
* The application will request the microphone access which you need to allow in order to have the speech to text functionality using microphone as input.
* Use the sample app buttons as described in instructions for Windows.

## Remarks

For a new project from scratch, install the following Speech SDK packages as needed:
* [Microsoft.CognitiveServices.Speech](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech)
  * mandatory for using the Speech SDK
* [Microsoft.CognitiveServices.Speech.Extension.Embedded.SR](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Embedded.SR)
  * required for embedded speech recognition
* [Microsoft.CognitiveServices.Speech.Extension.Embedded.TTS](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Embedded.TTS)
  * required for embedded speech synthesis
* [Microsoft.CognitiveServices.Speech.Extension.ONNX.Runtime](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.ONNX.Runtime)
  * required by embedded speech recognition and synthesis
* [Microsoft.CognitiveServices.Speech.Extension.Telemetry](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Telemetry)
  * required by embedded speech recognition and synthesis

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
