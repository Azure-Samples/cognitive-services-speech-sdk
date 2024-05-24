# C# ASP.NET Core MVC

This sample demonstrates various forms of speech recognition, intent recognition, and translation using the Speech SDK for C#.
It runs under .NET 8 or later on Windows or Linux (see the list of [supported Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux)).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* Either one of the following:
  * On Windows:
    * [Microsoft Visual Studio 2019](https://www.visualstudio.com/), Community Edition or higher.
  * On Windows or Linux:
    * [.NET 8](https://dotnet.microsoft.com/download/dotnet/8.0)
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

* If you are using Microsoft Visual Studio 2019 on Windows:
  * Start Microsoft Visual Studio 2019 and select **File** \> **Open** \> **Project/Solution**.
  * Navigate to the folder containing this sample, and select the solution file contained within it.
* To tailor the sample to your configuration, use search and replace across the whole solution (for example, in Visual Studio, via **Edit** \> **Find and Replace** \> **Quick Replace**) to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.

  ```bash
  dotnet build samples/samples.csproj
  ```

## Run the sample

### Using Visual Studio 2019

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

* [.NET Framework on Windows version of this sample](../../dotnet-windows/console)
* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
