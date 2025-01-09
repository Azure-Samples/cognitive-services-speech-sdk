# Quickstart: Recognize intent in C++ for Windows

This sample demonstrates how to recognize intent with C++ using the Speech SDK for Windows.

## Prerequisites

* A LUIS account. You can get one for free through the [LUIS portal](https://www.luis.ai/home).
* A Windows PC with a working microphone and [Microsoft Visual Studio](https://www.visualstudio.com/) installed. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-cpp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Edit the `helloworld.cpp` source:
  * Replace the string `YourLanguageUnderstandingSubscriptionKey` with your own LUIS endpoint key.
  * Replace the string `YourLanguageUnderstandingServiceRegion` with the short identifier for the region your LUIS subscription is in.
    For example, replace with `westus` for West US.
  * Replace the string `YourLanguageUnderstandingAppId` The LUIS app ID. You can find it on your app's Settings page in the [LUIS portal](https://www.luis.ai/home).
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**:
  * On a 64-bit Windows installation, choose `x64` as active solution platform.
  * On a 32-bit Windows installation, choose `x86` as active solution platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

> **Note**
> If you are seeing red squigglies from IntelliSense for Speech SDK APIs,
> right-click into your editor window and select **Rescan** > **Rescan Solution** to resolve.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-cpp-windows)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
