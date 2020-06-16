# Quickstart: Using virtual assistant (Preview) in C# Universal Windows Platform

This sample demonstrates how to recognize speech for custom voice-first virtual assistants (Preview) and receive activity responses.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-virtual-assistant-csharp-uwp) on the SDK documentation page which describes how to build a similar app from scratch in Visual Studio 2017.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A pre-configured bot created using Bot Framework version 4.2 or above. See [here for steps on how to create a bot](https://blog.botframework.com/2018/05/07/build-a-microsoft-bot-framework-bot-with-the-bot-builder-sdk-v4/). The bot would need to subscribe to the new [Direct Line Speech channel](https://docs.microsoft.com/azure/bot-service/bot-service-channel-connect-directlinespeech) to receive voice inputs.
    > **Note:**
    > In preview, the Direct Line Speech channel currently supports only the `westus2` region.
* A Windows PC with Windows 10 Fall Creators Update (10.0; Build 16299) or later, with a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* The **Universal Windows Platform development** workload in Visual Studio.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

When you see the `Enable microphone` button, click it. Enter your bot connection ID, subscription key, and region to the input fields and press the "Config" button. Once configured, you can press `Start` to begin a speech interaction or send a custom activity. You will see the `Conversation history` and `Activity log` updated with each interaction.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-virtual-assistant-java-jre)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
