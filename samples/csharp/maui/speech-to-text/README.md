# Sample: Speech to text using the CognitiveServices Speech SDK for MAUI

This Speech SDK sample demonstrates how to use speech to text functionalities with Microsoft CognitiveServices Speech SDK for MAUI.

> Note:
> The Speech SDK for MAUI supports Windows (x86, x64, ARM/ARM64), Android (x86, x64 and ARM32/64), iOS (x64/ARM64 simulators and ARM64 device) and Maccatalyst (x64, ARM64) targets.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* [Supported platforms for MAUI application development](https://learn.microsoft.com/dotnet/maui/supported-platforms)
* [Microsoft Visual Studio 2022](https://www.visualstudio.com/), Community Edition or higher is preferred.
* [.NET 8.0 or later](https://learn.microsoft.com/dotnet/core/install/windows).
* [Prerequisites for MAUI](https://learn.microsoft.com/dotnet/maui/get-started/installation)

## Prepare the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2022 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample (under csharp\maui\speech-to-text), and select the solution file contained within it.
* Project requires to install `Microsoft.CognitiveServices.Speech` nuget package. Install it from under `Tools -> Nuget Package Manager -> Manage Nuget Packages For Solution` and install the latest version of Speech SDK for the project.
* Edit the `MainPage.xaml.cs` source under the main project:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.

## Build and run the sample for Windows
* Set the target machine for your solution to **Windows Machine** on the top drop down box of Visual Studio windows:
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, press F5 or click **Windows Machine** button next to drop down target machine selector.
* The application will request the microphone access which you need to allow in order to use the speech to text functionality using microphone as input.
* Press the `Recognize Speech` button and say aloud some speech phrase, e.g. `What's the weather like?`. The successful outcome 
  is that speech will be transcribed to text in the application screen.

## Build and run the sample for Android
* Set the target machine to Android device or emulator of your choice. Note that MAUI will request to install required Android SDKs before you can run the sample using Android machine target.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, press F5 or click the selected Android device or emulator, see **Android emulators** or **Android local devices** in target device drop down selector.
* The application will request the microphone access which you need to allow in order to use the speech to text functionality using microphone as input.
* Press the `Recognize Speech` button and say aloud some speech phrase, e.g. `What's the weather like?`. The successful outcome 
  is that speech will be transcribed to text in the application screen.

## Build and run the sample for iOS
* Set the target machine to iOS device or emulator of your choice. Note that MAUI will need access to MAC device to use required Xcode tools before you can run the sample using iOS machine target. See the [Pair to Mac for iOS development](https://learn.microsoft.com/dotnet/maui/ios/pair-to-mac)
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* To run the sample app, press F5 or click the selected iOS device or emulator, see **iOS simulators** or **iOS Remote or Local devices** in target device drop down selector.
* The application will request the microphone access which you need to allow in order to use the speech to text functionality using microphone as input.
* Press the `Recognize Speech` button and say aloud some speech phrase, e.g. `What's the weather like?`. The successful outcome 
  is that speech will be transcribed to text in the application screen.

## Build and run the sample for Maccatalyst
* Read the instructions here on how to build MAUI application on Mac [Build a Mac Catalyst app with .NET CLI](https://learn.microsoft.com/dotnet/maui/mac-catalyst/cli)
* In order to run the sample on Mac, you need to ensure your Mac has been installed with required workloads for Android and Mac.
  dotnet command will prompt with error and information if some of those workloads are missing.
* The application will request the microphone access which you need to allow in order to use the speech to text functionality using microphone as input.
* Press the `Recognize Speech` button and say aloud some speech phrase, e.g. `What's the weather like?`. The successful outcome 
  is that speech will be transcribed to text in the application screen.

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
