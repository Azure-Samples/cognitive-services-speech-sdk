# Embedded speech samples in Java (JRE)

These samples demonstrate embedded and hybrid speech using the Speech SDK for Java.

Embedded speech enables offline (on-device) speech recognition, synthesis, and translation.
Hybrid speech uses cloud speech services by default and embedded speech as a fallback in case cloud connectivity is limited.
See https://aka.ms/embedded-speech for the latest information on supported features and platforms.

> **Note:**
> * Embedded speech recognition and translation only support audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples
> * For Android, see the [Android specific example](../../android/embedded-speech) (of embedded speech recognition and synthesis).

## Prerequisites

See the [platform requirements for installing the Speech SDK](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java).
Requirements specific to embedded speech samples are as follows.
* Supported operating systems and architectures:
  * Windows - `x64`.
  * Linux - `x64`, `ARM64`.
  * macOS - `x64`, `ARM64`.
* Java 8 or newer JDK. Check with `java -version` on the command line.
  * **Note:** Make sure that the Java installation is native to the system architecture (e.g. macOS `ARM64`) and not running through emulation.
* If using Eclipse (or a similar development environment):
  * [Eclipse IDE for Java developers](https://www.eclipse.org/downloads/packages/), version 2021-12 or newer.
* If using the command line:
  * [Maven](https://maven.apache.org/) (`mvn`). On Linux, install from the distribution repositories if available. On macOS, use `brew`.
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
* Edit `Settings.java` before building the sample and update the values of the settings listed below.
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
1. `CloudSpeechServiceRegion` (`CLOUD_SPEECH_SERVICE_REGION`)
   * Cloud speech service region. This is needed with hybrid speech configuration. If not set, only embedded speech will be used.
1. `CloudSpeechRecognitionLanguage` (`CLOUD_SPEECH_RECOGNITION_LANGUAGE`)
   * Cloud speech recognition language in BCP-47 format, case-sensitive. This is needed with hybrid speech configuration. If not set, en-US will be assumed.
1. `CloudSpeechSynthesisLanguage` (`CLOUD_SPEECH_SYNTHESIS_LANGUAGE`)
   * Cloud speech synthesis language in BCP-47 format, case-sensitive. This is needed with hybrid speech configuration. If not set, en-US will be assumed.

### Eclipse

1. Launch Eclipse with an empty workspace.
1. Open menu *File* \> *Import...*.
1. Select *General* \> *Projects from Folder or Archive*.
1. For *Import source*, open *Directory...* and find and select the folder containing this sample.
1. Select *Finish* to import the project.
   * If there is an error "*The import ... cannot be resolved*" or similar, open menu *Project* \> *Clean...* and select *Clean all projects*, then *Clean*.
1. Check the Package Explorer view (open *Windows* \> *Show View* \> *Package Explorer*) and verify that
   * the project shows no errors, and
   * the project's *Maven Dependencies* show `client-sdk-embedded-[version].jar` (may take a moment to load).

### Command-line

Navigate to the folder containing this sample, and run
```sh
mvn clean package
```

## Run the sample

### Eclipse

* To run the sample application, press **Ctrl+F11** or open menu *Run* \> *Run*.
  * If prompted to "*Select a way to run*" select **Java Application**.
* If you didn't update `Settings.java` or want to override built-in settings:
  * Open menu *Run* \> *Run Configurations...* and go to *Environment* tab.
  * Add environment variables. Enter *Name* (like `CLOUD_SPEECH_SERVICE_REGION`) and *Value* (for example, `westus`) for each setting as listed in [build instructions](#build-the-sample). Do not use quotation marks around the values.

The application displays a menu that you can navigate using your keyboard.
Choose the scenarios that you are interested in.

**Note:** On **macOS** there is no access to the microphone from Eclipse by default, and you must use the command-line for speech recognition to work.

**Note:** On **macOS**, if you get a message
"*libMicrosoft.CognitiveServices.Speech.(...).dylib can't be opened because Apple cannot check it for malicious software*" or similar,
follow instructions in https://support.apple.com/HT202491 (see "*If you want to open an app that hasn't been notarized or is from an unidentified developer*") to allow opening the file.

### Command-line

Navigate to the folder containing this sample and run
```sh
java -jar ./target/EmbeddedSpeechSamples-snapshot-jar-with-dependencies.jar
```
(This requires that `mvn clean package` earlier was successful and had no errors.)

If you didn't update `Settings.java` or want to override built-in settings, set environment variables as listed in [build instructions](#build-the-sample) before running the sample application. For example:

**Windows**
```sh
set CLOUD_SPEECH_SERVICE_REGION=westus
```
**Note:** On Windows, do **not** use quotation marks around the values of environment variables (even if there are spaces in them). Otherwise the quotation marks will end up as part of the setting values which will not work.

**Linux, macOS**
```sh
export CLOUD_SPEECH_SERVICE_REGION="westus"
```
**Note:** On Linux and macOS, use quotation marks around the values of environment variables set on the command line as shown in the example. Otherwise strings with spaces (like embedded speech model/voice names) will be cut short.

## Remarks

For a new project from scratch, add [client-sdk-embedded](https://mvnrepository.com/artifact/com.microsoft.cognitiveservices.speech/client-sdk-embedded) (`.jar`) as a dependency.
This package supports cloud, embedded, and hybrid speech.
Do **not** add [client-sdk](https://mvnrepository.com/artifact/com.microsoft.cognitiveservices.speech/client-sdk) in the same project, it supports only cloud speech services.

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
