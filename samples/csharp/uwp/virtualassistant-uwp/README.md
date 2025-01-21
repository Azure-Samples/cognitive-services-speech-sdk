# Quickstart: Using Voice Assistants in C# Universal Windows Platform

This sample demonstrates how to use Voice Assistants to recognize speech and receive activity responses.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A pre-configured bot created using Bot Framework version 4.2 or above. See [here for steps on how to create a bot](https://blog.botframework.com/2018/05/07/build-a-microsoft-bot-framework-bot-with-the-bot-builder-sdk-v4/). The bot would need to subscribe to the new [Direct Line Speech channel](https://docs.microsoft.com/azure/bot-service/bot-service-channel-connect-directlinespeech) to receive voice inputs.
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

When you see the `Enable microphone` button, click it. Enter your bot connection ID, subscription key, and region to the input fields and press the "Config" button. Once configured, you can press `Start` to begin a speech interaction or send a custom activity. You will see the `Conversation history` and `Activity log` updated with each interaction.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/voice-assistants?tabs=jre&pivots=programming-language-csharp)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
