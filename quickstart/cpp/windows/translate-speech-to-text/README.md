# Quickstart: Translate speech in C++ for Windows

This sample demonstrates how to translate speech with C++ using the Speech SDK for Windows.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-translate-speech-cpp-windows) on the SDK documentation page which describes how to build this sample from scratch in Visual Studio 2017.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC with a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* The **Desktop development with C++** workload in Visual Studio and the **NuGet package manager** component in Visual Studio.
  You can enable both in **Tools** \> **Get Tools and Features**, under the **Workloads** and **Individual components** tabs, respectively.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](../../../README.md#get-the-samples)
* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* Edit the `helloworld.cpp` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
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

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-translate-speech-cpp-windows)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
