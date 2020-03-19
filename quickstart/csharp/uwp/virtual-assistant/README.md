# Quickstart: Create a voice-first virtual assistant with the Speech SDK in C# UWP

This quickstart will connect to a previously authored and configured bot to enable a voice-first virtual assistant experience from the client application.
See the [accompanying article](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/quickstart-voice-assistant-csharp-uwp) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio.

## Prerequisites

* An Azure subscription key for the Speech Services in the `westus2` region. Create this subscription on the [Azure portal](https://portal.azure.com/).
* A previously created bot configured with the [Direct Line Speech channel](https://docs.microsoft.com/azure/bot-service/bot-service-channel-connect-directlinespeech)
    > **Note:**
    > In preview, the Direct Line Speech channel currently supports only the `westus2` region.

    > **Note:**
    > The 30-day trial for the standard pricing tier described in [Try Speech Services for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started) is restricted to westus (not westus2) and is thus not compatible with Direct Line Speech. Free and standard tier westus2 subscriptions are compatible.

* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later, with a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* The **Universal Windows Platform development** workload in Visual Studio.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
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

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-virtual-assistant-csharp-uwp)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
