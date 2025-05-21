# Quickstart: Translate speech in Java on Windows, macOS or Linux

This sample demonstrates how to translate speech with the Speech SDK for Java on Windows, macOS or Linux.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-translation?tabs=script%2Cwindowsinstall&pivots=programming-language-java) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> The Speech SDK for the JRE currently supports the Windows x64 platform, macOS x64 (10.14 or later), macOS M1 arm64 (11.0 or later), and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Eclipse](https://www.eclipse.org),[<sup>[1]</sup>](#footnote1) with a working microphone.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>

### Build the sample

&gt; Note: more detailed step-by-step instructions are [available here](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-translation?tabs=script%2Cwindowsinstall&amp;pivots=programming-language-java).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* Edit the `Main` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourEndpointUrl` with the endpoint URL of your subscription.
    For example, replace with `https://westus.api.cognitive.microsoft.com/` if your service region is `westus`.
* Save the modified file.

### Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
