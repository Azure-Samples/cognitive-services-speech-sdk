# Quickstart: Recognize speakers in C# (.Net Framework Windows) - Speech Service

In this quickstart, you'll learn how to do speaker recognition, including creating a voice profile, enrollment training and speaker verification and identification with C# under the .NET Framework (version 4.6.1 or above) using the Speech SDK for Windows.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=dotnet%2Cx-android%2Clinux%2Candroid%2Cwindowsinstall&pivots=programming-language-csharp) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio 2017.

**Note:** Microsoft limits access to speaker recognition. You can apply for access through the [Azure Cognitive Services speaker recognition limited access review](https://aka.ms/azure-speaker-recognition). For more information, see [Limited access for speaker recognition](https://docs.microsoft.com/legal/cognitive-services/speech-service/speaker-recognition/limited-access-speaker-recognition).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC.
* [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.
* The **.NET desktop development** workload in Visual Studio.
  You can enable it in **Tools** \> **Get Tools and Features**.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Edit the `Program.cs` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key. Use the `Speech` resource in Azure (not the `Speech Recognition` resource).
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    Make sure the region in your Azure resource matches the region you put into the sample, otherwise you'll get a 401 unauthorized access error.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/Speech-Service/quickstarts/multi-device-conversation?pivots=programming-language-csharp)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
