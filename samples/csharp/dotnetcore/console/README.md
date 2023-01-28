# C# Console app for .NET Core (Windows or Linux)

This sample demonstrates various forms of speech recognition, intent recognition, and translation using the Speech SDK for C#.
It runs under .NET Core 3.1 or later on Windows or Linux (see the list of [supported Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux)).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64 or a supported Linux distribution); some sample scenarios require a working microphone.
* Either one of the following:
  * On Windows:
    * [Microsoft Visual Studio 2017](https://www.visualstudio.com/), Community Edition or higher.
    * The **.NET Core cross-platform development** workload in Visual Studio.
      You can enable it in **Tools** \> **Get Tools and Features**.
  * On Windows or Linux:
    * [.NET Core 3.1](https://dotnet.microsoft.com/download/dotnet/3.1)
    * On Windows you also need the [Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019](https://support.microsoft.com/help/2977003/the-latest-supported-visual-c-downloads) for your platform.
* On Ubuntu or Debian, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl-dev libasound2
  ```

  * On **Ubuntu 22.04 LTS** it is also required to download and install the latest **libssl1.1** package e.g. from http://security.ubuntu.com/ubuntu/pool/main/o/openssl/.

* On RHEL or CentOS, run the following commands for the installation of required packages:

  ```sh
  sudo yum update
  sudo yum install alsa-lib dotnet-sdk-3.1 openssl
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)

  > **Note:** make sure to download the complete set of samples, not just this directory.
  > This sample references code in the directory [../../sharedcontent/console](../../sharedcontent/console), which is shared across samples.

* If you are using Microsoft Visual Studio 2017 on Windows:
  * Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
  * Navigate to the folder containing this sample, and select the solution file contained within it.
* To tailor the sample to your configuration, use search and replace across the whole solution (for example, in Visual Studio, via **Edit** \> **Find and Replace** \> **Quick Replace**) to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * `YourEndpointId` (optional): replace with the endpoint ID of your customized model in [CRIS](https://cris.ai).
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.
    * `YourLanguageUnderstandingIntentName1`, `YourLanguageUnderstandingIntentName2`, `YourLanguageUnderstandingIntentName3`: replace with names of intents that your Language Understanding service application recognizes.
  * The following settings apply to keyword-triggered recognition:
    * `YourKeywordRecognitionModelFile.table`: replace with the location of your keyword recognition model file.
    * `YourKeyword`: replace with the phrase your keyword recognition model triggers on.
* If you are using Microsoft Visual Studio 2017 on Windows, press <kbd>Ctrl+Shift+B</kbd>, or select **Build** \> **Build Solution**.
* If you are using the .NET Core CLI, run the following command from the directory that contains this sample:

  ```bash
  dotnet build samples/samples.csproj
  ```

## Run the sample

### Using Visual Studio 2017

To debug the app and then run it, press <kbd>F5</kbd> or use **Debug** \> **Start Debugging**. To run the app without debugging, press <kbd>Ctrl+F5</kbd> or use **Debug** \> **Start Without Debugging**.

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

### Using the .NET Core CLI

Run the following command below from the directory that contains this sample.
(We assume you performed a Debug build earlier)

```bash
cd samples/bin/Debug/netcoreapp3.1
dotnet samples.dll
```

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

## References

* [.NET Framework on Windows version of this sample](../../dotnet-windows/console)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
