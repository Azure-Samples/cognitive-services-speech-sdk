# Embedded speech samples in Python

These samples demonstrate embedded (offline, on-device) speech recognition, synthesis and translation using the Speech SDK for Python.

Embedded speech enables offline speech recognition, synthesis, and translation without a network connection to the cloud speech service.
See https://aka.ms/embedded-speech for the latest information on supported features and platforms.

> **Note:**
> * Embedded speech recognition and translation only support audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-python) for details on system requirements and setup.
* Supported operating systems and architectures for embedded speech:
  * Windows - `x64`, `ARM64`.
  * Linux - `x64`, `ARM64`.
  * macOS - `x64`, `ARM64`.
* For speech recognition or translation:
  * A working microphone device (and permission to access it for audio capture) for the microphone-based scenarios. The file-based recognition scenario reads a WAV file instead and does not require a microphone.
  * One or more embedded speech recognition or translation models. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded models.
* For speech synthesis:
  * A working speaker device (and permission to access it for audio playback).
  * One or more embedded speech synthesis voices. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded voices.

## Install the Speech SDK

**By using the Cognitive Services Speech SDK you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

Embedded speech requires the embedded variant of the Speech SDK Python package, which includes the on-device inference runtime. Install it in your Python interpreter, typically by executing the command

```sh
pip install azure-cognitiveservices-speech-embedded
```

in a terminal.

> **Note:** The embedded package (`azure-cognitiveservices-speech-embedded`) is a superset of the cloud-only package (`azure-cognitiveservices-speech`). It exposes the same `azure.cognitiveservices.speech` import namespace and supports all cloud scenarios in addition to embedded ones. Install only one of the two packages.

[Download the sample code to your development PC.](/README.md#get-the-samples)

## Configure the samples

The embedded models and voices are not included with the SDK and must be obtained separately (see https://aka.ms/embedded-speech). To tailor the samples to your configuration, there are two options:

* Edit the default values in [settings.py](settings.py) before running the samples.
* Alternatively set the corresponding environment variables (shown in parentheses) before running the sample application.

Sample settings:

1. `EmbeddedSpeechModelLicense` (`EMBEDDED_SPEECH_MODEL_LICENSE`)
   * Embedded speech model license (text).
   * This applies to embedded speech recognition, synthesis and translation.
   * It is presumed that all your embedded speech models use the same license.
1. `EmbeddedSpeechRecognitionModelPath` (`EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH`)
   * Path to the local embedded speech recognition model(s) on the device file system.
     This may be a single model folder or a top-level folder for several models.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for model files.
1. `EmbeddedSpeechRecognitionModelName` (`EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME`)
   * Name of the embedded speech recognition model to be used for recognition.
   * The model name can be short (see https://aka.ms/speech/sr-languages, e.g. `en-US`) or the full model name provided with your model.
1. `EmbeddedSpeechSynthesisVoicePath` (`EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH`)
   * Path to the local embedded speech synthesis voice(s) on the device file system.
     This may be a single voice folder or a top-level folder for several voices.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for voice files.
1. `EmbeddedSpeechSynthesisVoiceName` (`EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME`)
   * Name of the embedded speech synthesis voice to be used for synthesis.
   * The voice name can be short (see https://aka.ms/speech/tts-languages, e.g. `en-US-JennyNeural`) or the full voice name provided with your voice.
1. `EmbeddedSpeechTranslationModelPath` (`EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH`)
   * Path to the local embedded speech translation model(s) on the device file system.
     This may be a single model folder or a top-level folder for several models.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for model files.
1. `EmbeddedSpeechTranslationModelName` (`EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME`)
   * Name of the embedded speech translation model to be used for translation.
   * The full model name provided with your model must be given.

The keyword-triggered recognition sample additionally requires a keyword recognition model file (`.table`) and the matching keyword phrase. By default it uses `data/keyword_computer.table` with the keyword `Computer`; change `KEYWORD_MODEL_FILE_NAME` and `KEYWORD_PHRASE` in [settings.py](settings.py) to use your own. The default `keyword_computer.table` is not included in the repository; download it from https://aka.ms/embedded-speech-samples-data and place it in the [data](data) folder as described in [data/readme.txt](data/readme.txt). See https://aka.ms/speech/kws for details on creating a keyword model.

The file-based recognition sample reads audio from `data/speech_test.wav` (configurable via `WAV_AUDIO_FILE_NAME` in [settings.py](settings.py)). This file is also not included in the repository; download it from https://aka.ms/embedded-speech-samples-data and place it in the [data](data) folder as described in [data/readme.txt](data/readme.txt).

## Run the samples

To run the app, navigate to the `samples/python/embedded-speech` directory in your local copy of the samples repository.
Start the app with the command

```sh
python3 main.py
```

Depending on your platform, the Python 3 executable might also just be called `python`.

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

To set the configuration with environment variables instead of editing `settings.py`, for example on Linux or macOS:

```sh
export EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH="/path/to/models"
export EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME="en-US"
export EMBEDDED_SPEECH_MODEL_LICENSE="<model license text>"
python3 main.py
```

or on Windows (PowerShell):

```powershell
$env:EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH = "C:\path\to\models"
$env:EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME = "en-US"
$env:EMBEDDED_SPEECH_MODEL_LICENSE = "<model license text>"
python main.py
```

## References

* [Embedded speech documentation](https://aka.ms/embedded-speech)
* [Speech SDK API reference for Python](https://aka.ms/csspeech/pythonref)
