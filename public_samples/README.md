# Sample Repository for the Microsoft Cognitive Services Speech SDK

This project hosts the **samples** for the Microsoft Cognitive Services Speech SDK. To find out more about the Microsoft Cognitive Services Speech SDK itself, please visit the [SDK documentation site](https://aka.ms/csspeech).

## News

* Speech SDK Release 1.1.0

  Beginning of November we released version 1.1.0 of the Speech SDK. The release notes can be found [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/releasenotes#speech-service-sdk-110).

## Features

This repository hosts samples that help you to get started with several features of the SDK.
In addition more complex scenarios are included to give you a head-start on using speech technology in your application.

We tested the samples with the latest released version of the SDK (1.1.0) on Windows 10, Ubuntu 16.04 x64, Android devices (API 23: Android 6.0 Marshmallow or higher), and iOS 11.4 devices.

## Getting Started

The [SDK documentation](https://aka.ms/csspeech) has extensive sections about getting started, setting up the SDK, as well as the process to acquire the required subscription keys. You will need subscription keys to run the samples on your machines, you therefore should follow the instructions on these pages before continuing.

## Get the samples

* The easiest way to use these samples without using Git is to download the current version as a [ZIP file](https://github.com/Azure-Samples/cognitive-services-speech-sdk/archive/master.zip).

  * On Windows, before you unzip the archive, right-click it, select **Properties**, and then select **Unblock**.
  * Be sure to unzip the entire archive, and not just individual samples.

* Clone this sample repository using a Git client.

## Build and run the samples

> **Note: the samples make use of the Microsoft Cognitive Services Speech SDK.
> By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**

Please see the description of each individual sample for instructions on how to build and run it.

## Overview of samples

Our quickstarts for various platforms demonstrate how to do one-shot speech recognition from microphone.
If you want to build them from scratch, please follow the articles in the Quickstart section on our [documentation page](https://aka.ms/csspeech).

| Quickstart                                                                                                  | Platform | Description                                                          |
| ---                                                                                                         | ---      | ---                                                                  |
| [Quickstart C++ for Linux](./quickstart/cpp-linux/)                                                         | Linux    | Demonstrates one-shot speech recognition from microphone             |
| [Quickstart C++ for Windows](./quickstart/cpp-windows/)                                                     | Windows  | Demonstrates one-shot speech recognition from microphone             |
| [Quickstart C# .NET for Windows](./quickstart/csharp-dotnet-windows/)                                       | Windows  | Demonstrates one-shot speech recognition from microphone             |
| [Quickstart C# .NET Core (Windows or Linux)](./quickstart/csharp-dotnetcore/)                               | Windows, Linux  | Demonstrates one-shot speech recognition from microphone      |
| [Quickstart for Android](./quickstart/java-android/)                                                        | Android  | Demonstrates one-shot speech recognition from microphone             |
| [Quickstart Java JRE](./quickstart/java-jre/)                                                               | Windows, Linux | Demonstrates one-shot speech recognition from microphone       |
| [Quickstart JavaScript](./quickstart/js-browser/)                                                           | Web | Demonstrates one-shot speech recognition from microphone                  |
| [Quickstart Objective-C](./quickstart/objectivec-ios/)                                                      | iOS | Demonstrates one-shot speech recognition from a file with recorded speech |

The following samples demonstrate additional capabilities of the Speech SDK, such additional modes of speech recognition as well as intent recognition and translation:

| Sample                                                                                                      | Platform | Description                                                          |
| ---                                                                                                         | ---      | ---                                                                  |
| [C++ Console app for Windows](./samples/cpp/windows/console)                                                | Windows  | Demonstrates speech recognition, intent recognition, and translation |
| [C# Console app for .NET Framework on Windows](./samples/csharp/dotnet-windows/console)                     | Windows  | Demonstrates speech recognition, intent recognition, and translation |
| [C# Console app for .NET Core (Windows or Linux)](./samples/csharp/dotnetcore/console)                      | Windows, Linux  | Demonstrates speech recognition, intent recognition, and translation |
| [Java Console app for JRE](./samples/java/jre/console)                                                      | Windows, Linux | Demonstrates speech recognition, intent recognition, and translation |
| [Speech-to-text WPF sample for .NET Framework on Windows](./samples/csharp/dotnet-windows/speechtotext-wpf) | Windows  | Demonstrates speech recognition |
| [Translation WPF sample for .NET Framework on Windows](./samples/csharp/dotnet-windows/translation-wpf)     | Windows  | Demonstrates translation |
| [Speech-to-text UWP sample](./samples/csharp/uwp/speechtotext-uwp)                                          | Windows  | Demonstrates speech recognition |
| [Speech recognition sample for Android](./samples/java/android/sdkdemo)                                     | Android  | Demonstrates speech and intent recognition |
| [Speech recognition and translation sample for the browser, using JavaScript](./samples/js/browser)         | Web  | Demonstrates speech recognition, intent recognition, and translation |

## Resources

- [Microsoft Cognitive Services Speech Service and SDK Documentation](https://aka.ms/csspeech)
