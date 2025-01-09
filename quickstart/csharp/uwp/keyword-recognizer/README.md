# Quickstart: Recognize keywords in C# under Universal Windows Platform

This sample demonstrates how to recognize keywords with C# under the Universal Windows Platform using the Speech SDK.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=uwp%2Cx-android%2Clinux%2Candroid%2Cwindowsinstall&pivots=programming-language-csharp) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio.

## Prerequisites

* A Windows PC with a working microphone and [Microsoft Visual Studio](https://www.visualstudio.com/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

When you see the `Enable microphone` button, click it. Then select the `Listen for Keyword`. At this point you can say the keyword **"Computer"**. Once the keyword is detected, you can start talking and once you are done you can click the button `Save to File` to save the audio that was just recorded to a file.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-uwp)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
