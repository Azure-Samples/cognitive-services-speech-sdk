# C# Console app for .NET Core (Windows or Linux)

This sample demonstrates how to use Azure pronunciation assessment SDK and REST API for language learning.
It runs under .NET 8.0 or later on Windows or Linux (see the list of [supported Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux)).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac); some sample scenarios require a working microphone. See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp) for details on system requirements and setup.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

  > **Note:** make sure to download the complete set of samples, not just this directory.
  > This sample references code in the directory [../../sharedcontent/console](../../sharedcontent/console), which is shared across samples.

* If you are using Microsoft Visual Studio on Windows:
  * Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
  * Navigate to the folder containing this sample, and select the solution file contained within it.
* To tailor the sample to your configuration, use search and replace across the whole solution (for example, in Visual Studio, via **Edit** \> **Find and Replace** \> **Quick Replace**) to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * `YourEndpointId` (optional): replace with the endpoint ID of your customized model in [CRIS](https://cris.ai).
* If you are using Microsoft Visual Studio on Windows, press <kbd>Ctrl+Shift+B</kbd>, or select **Build** \> **Build Solution**.
* If you are using the .NET Core CLI, run the following command from the directory that contains this sample:

  ```bash
  dotnet build samples/samples.csproj
  ```

## Run the sample

### Using Visual Studio

To debug the app and then run it, press <kbd>F5</kbd> or use **Debug** \> **Start Debugging**. To run the app without debugging, press <kbd>Ctrl+F5</kbd> or use **Debug** \> **Start Without Debugging**.

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

### Using the .NET Core CLI

Run the following command below from the directory that contains this sample.
(We assume you performed a Debug build earlier)

```bash
cd samples/bin/Debug/net8.0
dotnet samples.dll
```

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

## References

* [.NET Framework on Windows version of this sample](../../dotnet-windows/language-learning)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
