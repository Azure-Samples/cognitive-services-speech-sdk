# C# Console app for .NET Core on Windows

This sample demonstrates various forms of speech recognition, intent recognition, and translation using the Speech SDK for C#.
It runs under .NET Core 2.0 on Windows.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A Windows PC; some sample scenarios require a working microphone.
* [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
* The **.NET Core cross-platform development** workload in Visual Studio.
  You can enable it in **Tools** \> **Get Tools and Features**.

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](../../README.md#get-the-samples)

  > **Note:** make sure to download the complete set of samples, not just this directory.
  > This sample references code in the directory [../../sharedcontent/console](../../sharedcontent/console), which is shared across samples.

* Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file contained within it.
* To tailor the sample to your configuration, use search and replace across the whole solution (for example, via **Edit** \> **Find and Replace** \> **Quick Replace**) to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * `YourDeploymentId` (optional): replace with the deployment ID of your customized model in [CRIS](https://cris.ai).
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingServiceRegion`: replace with your Language Understanding service subscription key.
    * `YourLanguageUnderstandingSubscriptionKey`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.
    * `YourLanguageUnderstandingIntentName1`, `YourLanguageUnderstandingIntentName2`, `YourLanguageUnderstandingIntentName3`: replace with names of intents that your Language Understanding service application recognizes.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

## References

* [.NET Framework on Windows version of this sample](../../dotnet-windows/console)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
