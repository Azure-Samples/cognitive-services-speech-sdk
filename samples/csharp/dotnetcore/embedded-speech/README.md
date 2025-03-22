# Embedded speech samples in C# (.NET Core)

These samples demonstrate embedded and hybrid speech using the Speech SDK for C# (.NET Core).

Embedded speech enables offline (on-device) speech recognition, synthesis, and translation.
Hybrid speech uses cloud speech services by default and embedded speech as a fallback in case cloud connectivity is limited.
See https://aka.ms/embedded-speech for the latest information on supported features and platforms.

> **Note:**
> * Embedded speech recognition and translation only support audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

See the [platform requirements for installing the Speech SDK](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-csharp).
Requirements specific to embedded speech samples are as follows.
* Supported operating systems and architectures:
  * Windows - `x64`, `ARM64`.
  * Linux - `x64`, `ARM64`.
  * macOS - `x64`, `ARM64`.
* If using Visual Studio (Windows, macOS):
  * [Microsoft Visual Studio 2022 or newer](https://www.visualstudio.com/) with **.NET 8.0**.
* If using the command line:
  * [.NET 8.0](https://learn.microsoft.com/dotnet/core/install/) (dotnet)
* For speech recognition or translation:
  * A working microphone device (and permission to access it for audio capture).
  * One or more embedded speech recognition or translation models. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded models.
* For speech synthesis:
  * A working speaker device (and permission to access it for audio playback).
  * One or more embedded speech synthesis voices. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded voices.
* For hybrid speech recognition or synthesis:
  * A subscription key for the cloud speech service. [Try the speech service for free](https://azure.microsoft.com/free/cognitive-services/).

## Build the sample

**By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

[Download the sample code to your development PC.](/README.md#get-the-samples)

Download example data (a keyword recognition model, speech recognition input files) used in this sample project from
https://aka.ms/embedded-speech-samples-data (zip file).
Unzip the package under this sample folder. The result should be a `data` subfolder with the following files:
```
data\keyword_computer.table
data\performance_test.wav
data\speech_test.raw
data\speech_test.wav
```

To tailor the sample to your configuration, there are two options:
* Edit `Settings.cs` before building the sample and update the values of the settings listed below.
* Alternatively set corresponding environment variables (shown in parentheses in the list) before running the sample application. See details in [how to run the sample](#run-the-sample).

Sample settings:
1. `EmbeddedSpeechModelLicense` (`EMBEDDED_SPEECH_MODEL_LICENSE`)
   * Embedded speech model license (text).
   * This applies to embedded speech recognition, synthesis and translation.
   * It is presumed that all the customer's embedded speech models use the same license.
1. `EmbeddedSpeechRecognitionModelPath` (`EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH`)
   * Path to the local embedded speech recognition model(s) on the device file system.
     This may be a single model folder or a top-level folder for several models.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for model files.
   * Files belonging to a specific model must be present as normal individual files in a model folder,
     not inside an archive, and they must be readable by the application process.
     The model internal subfolder structure must be intact i.e. as originally delivered.
1. `EmbeddedSpeechRecognitionModelName` (`EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME`)
   * Name of the embedded speech recognition model to be used for recognition.
   * The model name can be short (see https://aka.ms/speech/sr-languages, e.g. `en-US`) or full (e.g. `Microsoft Speech Recognizer en-US FP Model V8`).
1. `EmbeddedSpeechSynthesisVoicePath` (`EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH`)
   * Path to the local embedded speech synthesis voice(s) on the device file system.
     This may be a single voice folder or a top-level folder for several voices.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for voice files.
   * Files belonging to a specific voice must be present as normal individual files in a voice folder,
     not inside an archive, and they must be readable by the application process.
     The voice internal subfolder structure must be intact i.e. as originally delivered.
1. `EmbeddedSpeechSynthesisVoiceName` (`EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME`)
   * Name of the embedded speech synthesis voice to be used for synthesis.
   * The voice name can be short (see https://aka.ms/speech/tts-languages, e.g. `en-US-JennyNeural`) or full (e.g. `Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)`).
1. `EmbeddedSpeechTranslationModelPath` (`EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH`)
   * Path to the local embedded speech translation model(s) on the device file system.
     This may be a single model folder or a top-level folder for several models.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for model files.
   * Files belonging to a specific model must be present as normal individual files in a model folder,
     not inside an archive, and they must be readable by the application process.
     The model internal subfolder structure must be intact i.e. as originally delivered.
1. `EmbeddedSpeechTranslationModelName` (`EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME`)
   * Name of the embedded speech translation model to be used for translation.
   * The full model name must be given (e.g. `Microsoft Speech Translator Many-to-English Model V2`).
1. `CloudSpeechSubscriptionKey` (`CLOUD_SPEECH_SUBSCRIPTION_KEY`)
   * Cloud speech service subscription key. This is needed with hybrid speech configuration. If not set, only embedded speech will be used.
1. `CloudSpeechServiceEndpoint` (`CLOUD_SPEECH_SERVICE_ENDPOINT`)
   * Cloud speech service endpoint. This is needed with hybrid speech configuration. If not set, only embedded speech will be used.
1. `CloudSpeechRecognitionLanguage` (`CLOUD_SPEECH_RECOGNITION_LANGUAGE`)
   * Cloud speech recognition language in BCP-47 format, case-sensitive. This is needed with hybrid speech configuration. If not set, en-US will be assumed.
1. `CloudSpeechSynthesisLanguage` (`CLOUD_SPEECH_SYNTHESIS_LANGUAGE`)
   * Cloud speech synthesis language in BCP-47 format, case-sensitive. This is needed with hybrid speech configuration. If not set, en-US will be assumed.

### Visual Studio

* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file `samples.sln` contained within it.
* Set the active solution configuration to the desired value under **Build** \> **Configuration Manager**. Use **AnyCPU** as the platform.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
  * If you are seeing red squigglies from IntelliSense for Speech SDK APIs, right-click into your editor window and select **Rescan** > **Rescan Solution** to resolve.
  * If there is an error "*Could not find a part of the path*" while Speech SDK packages are installed, copy/move this sample folder to the storage drive root or close to it, so that folder and file paths become shorter.

### Command-line

Navigate to the folder containing this sample, and run
```sh
dotnet build samples/samples.csproj
```
to build the sample for **Debug** configuration (default), or
```sh
dotnet build --configuration=Release samples/samples.csproj
```
for **Release** configuration.

## Run the sample

### Visual Studio

* To debug the sample application and then run it, press F5 or use **Debug** \> **Start Debugging**.
* To run the application without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
* If you didn't update `Settings.cs` or want to override built-in settings:
  * Open menu **Debug** \> **samples Debug Properties**, go to **Environment variables**.
  * Enter *Name* (like `CLOUD_SPEECH_SERVICE_ENDPOINT`) and *Value* (for example, `https://westcentralus.api.cognitive.microsoft.com/`) for each setting as listed in [build instructions](#build-the-sample). Do not use quotation marks around the values.

**Note:** On **macOS** there is no access to the microphone from Visual Studio by default, and you must use the command-line for speech recognition to work.

**Note:** On **macOS**, if you get a message
"*libMicrosoft.CognitiveServices.Speech.(...).dylib can't be opened because Apple cannot check it for malicious software*" or similar,
follow instructions in https://support.apple.com/HT202491 (see "*If you want to open an app that hasn't been notarized or is from an unidentified developer*") to allow opening the file.

The application displays a menu that you can navigate using your keyboard.
Choose the scenarios that you are interested in.

### Command-line

Navigate to the folder containing this sample and run
```sh
dotnet samples/bin/Debug/net8.0/samples.dll
```
or
```sh
dotnet samples/bin/Release/net8.0/samples.dll
```
according to the build configuration that was used.

If you didn't update `Settings.cs` or want to override built-in settings, set environment variables as listed in [build instructions](#build-the-sample) before running the sample application. For example:

**Windows**
```sh
set CLOUD_SPEECH_SERVICE_ENDPOINT=https://westcentralus.api.cognitive.microsoft.com/
```
**Note:** On Windows, do **not** use quotation marks around the values of environment variables (even if there are spaces in them). Otherwise the quotation marks will end up as part of the setting values which will not work.

**Linux, macOS**
```sh
export CLOUD_SPEECH_SERVICE_ENDPOINT="https://westcentralus.api.cognitive.microsoft.com/"
```
**Note:** On Linux and macOS, use quotation marks around the values of environment variables set on the command line as shown in the example. Otherwise strings with spaces (like embedded speech model/voice names) will be cut short.

## Remarks

For a new project from scratch, install the following Speech SDK packages as needed:
* [Microsoft.CognitiveServices.Speech](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech)
  * mandatory for using the Speech SDK
* [Microsoft.CognitiveServices.Speech.Extension.Embedded.SR](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Embedded.SR)
  * required for embedded speech recognition and translation
* [Microsoft.CognitiveServices.Speech.Extension.Embedded.TTS](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Embedded.TTS)
  * required for embedded speech synthesis
* [Microsoft.CognitiveServices.Speech.Extension.ONNX.Runtime](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.ONNX.Runtime)
  * required by embedded speech
* [Microsoft.CognitiveServices.Speech.Extension.Telemetry](https://www.nuget.org/packages/Microsoft.CognitiveServices.Speech.Extension.Telemetry)
  * required by embedded speech

## References

* [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)
