# Translation WPF sample for .NET Framework on Windows

This sample demonstrates how to translate speech with C# in a WPF application under the .NET Framework on Windows.
Multiple languages to translate into can be selected; a language can be separately selected for voice output.

The sample runs on .NET Framework 4.6.1 (or above) on Windows.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with a working microphone and [Microsoft Visual Studio](https://www.visualstudio.com/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

The app displays a graphical user interface (GUI).

* Use the **Enter Subscription Key** text field in the upper right to configure your subscription key.
* In the **Region and Language** fly-out on the left, configure the region that is associated with your subscription.
* In the same fly-out, pick your target language and a language for voice output.
* To start translating, select **Start Translation**.
* The app will then continuously translate utterance from your microphone.
* To stop translating, select **Stop Translation**.

> **Note:**
> if you use the **Save Key** button, your subscription key will be written to an unencrypted file named `SubscriptionKey.txt` on your disk, inside [.NET Isolated Storage](https://docs.microsoft.com/dotnet/standard/io/isolated-storage).
> Please use the [documentation](https://docs.microsoft.com/dotnet/standard/io/isolated-storage) if you'd like to locate and delete this file.

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
