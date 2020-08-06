---
page_type: sample
languages:
- cpp
- csharp
- java
- javascript
- nodejs
- objc
- python
- swift
name: "Microsoft Cognitive Services Speech SDK Samples"
description: "Learn how to use the Microsoft Cognitive Services Speech SDK to add speech-enabled features to your apps."
products:
- azure
- azure-cognitive-services
---

# Sample Repository for the Microsoft Cognitive Services Speech SDK

This project hosts the **samples** for the Microsoft Cognitive Services Speech SDK. To find out more about the Microsoft Cognitive Services Speech SDK itself, please visit the [SDK documentation site](https://aka.ms/csspeech).

## News

Please check [here](https://aka.ms/csspeech/whatsnew) for release notes and older releases.

* Speech SDK Release 1.13.0

  July 2020 we released version 1.13.0 of the Speech SDK

* Speech SDK Release 1.12.1

  June 2020 we released version 1.12.1 of the Speech SDK  

* Speech SDK Release 1.12.0

  May 2020 we released version 1.12.0 of the Speech SDK

* Speech SDK Release 1.11.0

  March 2020 we released version 1.11.0 of the Speech SDK

* Speech SDK Release 1.10.0

  February 2020 we released version 1.10.0 of the Speech SDK

* Speech SDK Release 1.9.0

  January 2020 we released version 1.9.0 of the Speech SDK

* Speech SDK Release 1.8.0

  November 2019 we released version 1.8.0 of the Speech SDK

* Speech SDK Release 1.7.0

  September 2019 we released version 1.7.0 of the Speech SDK

* Speech SDK Release 1.6.0

  June 2019 we released version 1.6.0 of the Speech SDK.

* Speech SDK Release 1.5.1

  Middle of May 2019 we released version 1.5.1 of the Speech SDK.

* Speech SDK Release 1.5.0

  Beginning of May 2019 we released version 1.5.0 of the Speech SDK.

* Speech SDK Release 1.4.0

  Beginning of April 2019 we released version 1.4.0 of the Speech SDK.

* Speech SDK Release 1.3.1

  End of February 2019 we released version 1.3.1 of the Speech SDK.

## Features

This repository hosts samples that help you to get started with several features of the SDK.
In addition more complex scenarios are included to give you a head-start on using speech technology in your application.

We tested the samples with the latest released version of the SDK on Windows 10, Linux (on [supported Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux)), Android devices (API 23: Android 6.0 Marshmallow or higher), and iOS 11.4 devices.

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

### Speech recognition quickstarts

The following quickstarts demonstrate how to perform one-shot speech recognition using a microphone.
If you want to build them from scratch, please follow the articles in the Quickstart section on our [documentation page](https://aka.ms/csspeech).

| Quickstart | Platform | Description |
| ---------- | -------- | ----------- |
| [Quickstart C++ for Linux](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/linux/from-microphone) | Linux | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C++ for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/windows/from-microphone) | Windows  | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C++ for macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/macos/from-microphone) | macOS |  | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C# .NET for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnet/from-microphone) | Windows | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C# .NET Core](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnetcore/from-microphone) | Windows, Linux, macOS | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C# UWP for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/uwp/from-microphone) | Windows | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart C# Unity (Windows or Android)](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/unity/from-microphone) | Windows, Android | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart for Android](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/android/from-microphone) | Android  | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart Java JRE](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/jre/from-microphone) | Windows, Linux, macOS | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart JavaScript](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/javascript/browser) | Web | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart Node.js](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/javascript/node) | Node.js | Demonstrates one-shot speech recognition from a file. |
| [Quickstart Python](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/python/from-microphone) | Windows, Linux, macOS | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart Objective-C iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/objectivec/ios/from-microphone) | iOS | Demonstrates one-shot speech recognition from a file with recorded speech. |
| [Quickstart Swift iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/swift/ios/from-microphone) | iOS | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart Objective-C macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/objectivec/macos/from-microphone) | macOS | Demonstrates one-shot speech recognition from a microphone. |
| [Quickstart Swift macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/swift/macos/from-microphone) | macOS | Demonstrates one-shot speech recognition from a microphone. |

### Speech translation quickstarts

The following quickstarts demonstrate how to perform one-shot speech translation using a microphone.
If you want to build them from scratch, please follow the articles in the Quickstart section on our [documentation page](https://aka.ms/csspeech).

| Quickstart | Platform | Description |
| ---------- | -------- | ----------- |
| [Quickstart C++ for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/windows/translate-speech-to-text) | Windows | Demonstrates one-shot speech translation/transcription from a microphone. |
| [Quickstart C# .NET Framework for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnet/translate-speech-to-text) | Windows | Demonstrates one-shot speech translation/transcription from a microphone. |
| [Quickstart C# .NET Core](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnetcore/translate-speech-to-text) | Windows, Linux, macOS | Demonstrates one-shot speech translation/transcription from a microphone. |
| [Quickstart C# UWP for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/uwp/translate-speech-to-text) | Windows | Demonstrates one-shot speech translation/transcription from a microphone. |
| [Quickstart Java JRE](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/jre/translate-speech-to-text) | Windows, Linux, macOS | Demonstrates one-shot speech translation/transcription from a microphone. |

### Speech synthesis quickstarts

The following quickstarts demonstrate how to perform one-shot speech synthesis to a speaker.
If you want to build them from scratch, please follow the articles in the Quickstart section on our [documentation page](https://aka.ms/csspeech).

| Quickstart | Platform | Description |
| ---------- | -------- | ----------- |
| [Quickstart C++ for Linux](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/linux/text-to-speech) | Linux | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart C++ for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/windows/text-to-speech) | Windows  | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart C++ for macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/cpp/macos/text-to-speech) | macOS | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart C# .NET for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnet/text-to-speech) | Windows | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart C# UWP for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/uwp/text-to-speech) | Windows | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart C# .NET Core](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/dotnetcore/text-to-speech) | Windows, Linux | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart for C# Unity (Windows or Android)](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/unity/text-to-speech) | Windows, Android | Demonstrates one-shot speech synthesis to a synthesis result and then rendering to the default speaker. |
| [Quickstart for Android](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/android/text-to-speech) | Android | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart Java JRE](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/jre/text-to-speech) | Windows, Linux, macOS | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart Python](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/python/text-to-speech) | Windows, Linux, macOS | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart Objective-C iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/objectivec/ios/text-to-speech) | iOS | Demonstrates one-shot speech synthesis to a synthesis result and then rendering to the default speaker. |
| [Quickstart Swift iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/swift/ios/text-to-speech) | iOS | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart Objective-C macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/objectivec/macos/text-to-speech) | macOS | Demonstrates one-shot speech synthesis to the default speaker. |
| [Quickstart Swift macOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/swift/macos/text-to-speech) | macOS | Demonstrates one-shot speech synthesis to the default speaker. |

### Voice assistants quickstarts

The following quickstarts demonstrate how to create a custom voice assistant.
The applications will connect to a previously authored bot configured to use the Direct Line Speech channel, send a voice request, and return a voice response activity (if configured).
If you want to build these quickstarts from scratch, please follow the articles in the Quickstart section on our [documentation page](https://aka.ms/csspeech).

| Quickstart | Platform | Description |
| ---------- | -------- | ----------- |
| [Quickstart Java JRE](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/java/jre/virtual-assistant) | Windows, Linux, macOS | Demonstrates speech recognition through the DialogServiceConnector and receiving activity responses. |
| [Quickstart C# UWP for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/quickstart/csharp/uwp/virtual-assistant) | Windows | Demonstrates speech recognition through the DialogServiceConnector and receiving activity responses. |

### Samples

The following samples demonstrate additional capabilities of the Speech SDK, such as additional modes of speech recognition as well as intent recognition and translation. Voice Assistant samples can be found in a [separate GitHub repo](https://github.com/Azure-Samples/Cognitive-Services-Voice-Assistant).

| Sample                                                                                                      | Platform | Description                                                          |
| ---                                                                                                         | ---      | ---                                                                  |
| [C++ Console app for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/cpp/windows/console)                                                | Windows  | Demonstrates speech recognition, speech synthesis, intent recognition, conversation transcription and translation |
| [C++ Speech Recognition from MP3/Opus file (Linux only)](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/cpp/linux/compressed-audio-input)        | Linux    | Demonstrates speech recognition from an MP3/Opus file |
| [C# Console app for .NET Framework on Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnet-windows/console)                     | Windows  | Demonstrates speech recognition, speech synthesis, intent recognition, and translation |
| [C# Console app for .NET Core (Windows or Linux)](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnetcore/console)                      | Windows, Linux, macOS  | Demonstrates speech recognition, speech synthesis, intent recognition, and translation |
| [Java Console app for JRE](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/java/jre/console)                                                      | Windows, Linux, macOS | Demonstrates speech recognition, speech synthesis, intent recognition, and translation |
| [Python Console app](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/python/console)                                                              | Windows, Linux, macOS  | Demonstrates speech recognition, speech synthesis, intent recognition, and translation |
| [Speech-to-text WPF sample for .NET Framework on Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnet-windows/speechtotext-wpf) | Windows  | Demonstrates speech recognition |
| [Translation WPF sample for .NET Framework on Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/dotnet-windows/translation-wpf)     | Windows  | Demonstrates translation |
| [Speech-to-text UWP sample](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/uwp/speechtotext-uwp)                                          | Windows  | Demonstrates speech recognition |
| [Text-to-speech UWP sample](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/uwp/texttospeech-uwp)                                          | Windows  | Demonstrates speech synthesis |
| [Speech recognition sample for Android](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/java/android/sdkdemo/)                                     | Android  | Demonstrates speech and intent recognition |
| [Speech recognition and translation sample for the browser, using JavaScript](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/browser/)         | Web      | Demonstrates speech recognition, intent recognition, and translation |
| [Speech recognition and translation sample using JavaScript and Node.js](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/js/node)                 | Node.js  | Demonstrates speech recognition, intent recognition, and translation |
| [Speech recognition sample for iOS using a connection object](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/objective-c/ios)  | iOS      | Demonstrates speech recognition |
| [Extended speech recognition sample for iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/objective-c/ios)           | iOS      | Demonstrates speech recognition using streams etc.|
| [Speech synthesis sample for iOS](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/objective-c/ios)           | iOS      | Demonstrates speech synthesis using streams etc.|
| [C# UWP DialogServiceConnector sample for Windows](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/uwp/virtualassistant-uwp/) | Windows | Demonstrates speech recognition through the DialogServiceConnector and receiving activity responses. |
| [C# Unity sample for Windows or Android](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/unity/speechrecognizer/) | Windows, Android |  Demonstrates speech recognition, intent recognition, and translation for Unity |
| [C# Unity SpeechBotConnector sample for Windows or Android](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/unity/VirtualAssistantPreview/) | Windows, Android | Demonstrates speech recognition through the SpeechBotConnector and receiving activity responses. |

Samples for using the Speech Service REST API (no Speech SDK installation required):

| Sample                                  | Description |
| ---                                     | --- |
| [Batch transcription](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/batch/)  | Demonstrates usage of batch transcription from different programming languages |

## Sample data for Custom Speech
- [Sample data for Custom Speech](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/sampledata/customspeech)

## Resources

- [Microsoft Cognitive Services Speech Service and SDK Documentation](https://aka.ms/csspeech)
