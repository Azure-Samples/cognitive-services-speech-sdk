# Intent recognition (standalone) samples in C++

These samples demonstrate a standalone implementation of intent recognition that was previously available only as part of the Speech SDK.
It can be used to extract structured information about intents in text.

> **Note:**
> * The standalone intent recognition is valid only for *offline pattern matching or exact matching intents*.
>   It does not use or depend on cloud services and does not require the Speech SDK to run (but can be used with it).
> * Only models of PatternMatchingModel type are supported.
Cloud-based *Conversational Language Understanding* (CLU) or *Language Understanding* (LUIS) models and services cannot be used.
> * Supported languages are documented in https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=intent-recognizer-pattern-matcher

## Prerequisites

This sample project is for Windows (`x64`, `ARM64`) with [Microsoft Visual Studio 2022 or newer](https://www.visualstudio.com/).

Some examples use the Speech SDK for speech-to-text.
See the [platform requirements for installing the Speech SDK](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-cpp).

## Build the sample

**By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

Examples that use the Speech SDK require audio files for speech recognition input.
See `samples\test_input\readme.txt` for instructions on how to download them. They need to be in place before the sample can be built.

### Visual Studio

* Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
* Navigate to the folder containing this sample, and select the solution file `samples.sln` contained within it.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
  * If you are seeing red squigglies from IntelliSense for Speech SDK APIs, right-click into your editor window and select **Rescan** > **Rescan Solution** to resolve.
  * If there is an error "*Could not find a part of the path*" while Speech SDK packages are installed, copy/move this sample folder to the storage drive root or close to it, so that folder and file paths become shorter.

## Run the sample

### Visual Studio

* Set environment variables that are required by the examples that use the Speech SDK for speech-to-text:
  * Open menu **Debug** \> **samples Debug Properties**.
  * Navigate to **Configuration Properties** \> **Debugging** \> **Environment**.
  * Click to open a pull-down menu, select **Edit...**.
  * Add environment settings (`name=value`) for `SPEECH_SUBSCRIPTION_KEY` and `SPEECH_SERVICE_REGION`.
    For example:
    ```sh
    SPEECH_SERVICE_REGION=westus
    ```
    Notes:
    * Make sure that each `name=value` appears on a separate line.
    * Do not use quotation marks around the values.
    * Environment variables set in this way are only in effect when the solution configuration is **Debug**.
    * The settings are stored in plain text in a new `samples.vcxproj.user` file in the project directory.
* Because the examples are implemented as **Catch2** test cases, it is recommended to enable verbose output:
  * Open menu **Debug** \> **samples Debug Properties**.
  * Navigate to **Configuration Properties** \> **Debugging** \> **Command Arguments**.
  * Add `--durations yes`.
* To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**.
* To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

### Command-line

* Set environment variables `SPEECH_SUBSCRIPTION_KEY` and `SPEECH_SERVICE_REGION` to valid values. For example:
  ```sh
  set SPEECH_SERVICE_REGION=westus
  ```
  **Note:** On Windows, do **not** use quotation marks around the values of environment variables (even if there are spaces in them). Otherwise the quotation marks will end up as part of the setting values which will not work.
* Navigate to the folder that contains the generated executable according to the solution configuration and platform (for example, `x64\Release` under this folder), and run:
  ```sh
  samples.exe --durations yes
  ```

## Remarks

For a new project, refer to `samples\samples.vcxproj` for necessary `AdditionalOptions` and `AdditionalIncludeDirectories`.

In comparison to the Speech SDK IntentRecognizer usage, the standalone implementation has the following main differences:

* There is no support for audio as input, only text. You can run the Speech SDK SpeechRecognizer or similar for speech-to-text before intent recognition.

* Instead of (or in addition to)
  ```
  #include <speechapi_cxx.h>
  ```
  use
  ```
  #include <intentapi_cxx.h>
  ```

* To create an IntentRecognizer instance, instead of
  ```
  auto intentRecognizer = IntentRecognizer::FromConfig(speechConfig);
  ```
  use
  ```
  auto intentRecognizer = IntentRecognizer::FromLanguage(language);
  ```

* To get detailed intent recognition results if needed, instead of
  ```
  auto detailedOutputJson = result->Properties.GetProperty("LanguageUnderstandingSLE_DetailedResult");
  ```
  use
  ```
  auto detailedOutputJson = result->GetDetailedResult();
  ```

* If your project uses both Speech SDK and standalone IntentRecognizer, refer to EntityType and EntityMatchMode enums with either no namespace prefix or the full prefix.
  Otherwise there can be a namespace clash with the Speech SDK (as long as it still comes with intent recognition as part of the SDK).

  For example, instead of
  ```
  Intent::EntityType::Any
  ```
  use either
  ```
  using namespace Microsoft::SpeechSDK::Standalone::Intent;
  EntityType::Any
  ```
  or
  ```
  Microsoft::SpeechSDK::Standalone::Intent::EntityType::Any
  ```

## References

* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)
