# Quickstart: Create a Voice Assistant with the Speech SDK in C# UWP

This quickstart will connect to a previously authored and configured bot to enable a voice assistant experience from the client application.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-voice-assistant-csharp-uwp) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A previously created bot configured with the [Direct Line Speech channel](https://docs.microsoft.com/azure/bot-service/bot-service-channel-connect-directlinespeech)
* A Windows PC with a working microphone and [Microsoft Visual Studio](https://www.visualstudio.com/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Edit the `MainPage.xaml.cs` source:
  * Replace the string `YourChannelSecret` with your own channel secret id.
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

When you see the `Enable microphone` button, click it. Then select the `Listen` button and start speaking, your audio and a sample activity will be sent to the service.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-voice-assistant-csharp-uwp)
* [Speech SDK API reference for C#](https://docs.microsoft.com/dotnet/api/microsoft.cognitiveservices.speech.dialog?view=azure-dotnet)
