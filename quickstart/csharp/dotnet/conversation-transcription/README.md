# Quickstart: Transcribe conversations in C# under .NET Framework for Windows

This sample demonstrates how to transcribe conversations with C# under the .NET Framework (version 4.6.1 or above) using the Speech SDK for Windows.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-dotnet-windows) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with a working microphone and [Microsoft Visual Studio](https://www.visualstudio.com/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Edit the `Program.cs` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key. Use the `Speech` resource in Azure (not the `Speech Recognition` resource).
  * Replace the endpoint URL `https://YourServiceRegion.api.cognitive.microsoft.com` with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
    Make sure the endpoint in your code matches the one in your Azure resource, otherwise you'll get a 401 unauthorized access error.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-dotnet-windows)
* [How to article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-conversation-transcription)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
