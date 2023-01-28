# Speech-to-text WPF sample for .NET Framework on Windows

This sample demonstrates how to recognize speech with C# in a WPF application under the .NET Framework on Windows.
It demonstrates recognition in the language of your choice, from file or microphone.
It can show side-by-side results of speech recognition using the base model and a custom model that you created using [CRIS](https://cris.ai).

The sample runs on .NET Framework 4.6.1 (or above) on Windows.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC; some sample scenarios require a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* The **.NET desktop development** workload in Visual Studio.
  You can enable it in **Tools** \> **Get Tools and Features**.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

The app displays a graphical user interface (GUI), which shows baseline model recognition results on the left and custom model speech recognition results on the bottom.
Use the **Settings** fly-out on the right to configure input source, recognition type, region and language, and (most importantly) your subscription key.

If you like detailed output from the speech service, use the **Baseline Model Output** and **Custom Model Output** fly-outs at the bottom of the GUI.

### Notes

* If you use the **Save Keys** button, your subscription key will be written to an unencrypted file named `SubscriptionKey.txt` on your disk, inside [.NET Isolated Storage](https://docs.microsoft.com/dotnet/standard/io/isolated-storage).
  Please use the [documentation](https://docs.microsoft.com/dotnet/standard/io/isolated-storage) if you'd like to locate and delete this file.

* If you are using your own `.wav` file as input source, make sure it is in the right format.
  Currently, the only supported `.wav` format is **mono (single-channel), 16 kHz sample rate, 16 bits per sample**.

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
