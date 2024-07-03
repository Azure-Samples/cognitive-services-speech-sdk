# Embedded speech sample in Java on Android

This sample demonstrates embedded (offline, on-device) speech recognition and synthesis with the Speech SDK for Android (Java).
You can choose whether to use recognition or synthesis, or both.

For more advanced use cases, see the [Java JRE samples](../../jre/embedded-speech).
It is recommended to try them out before this Android specific sample.

> **Note:**
> * Embedded speech recognition only supports audio in the following format:
>   * single channel
>   * 8000 or 16000 Hz sample rate
>   * 16-bit little-endian signed integer samples

## Prerequisites

See the [platform requirements for installing the Speech SDK](https://learn.microsoft.com/azure/cognitive-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java).

Requirements specific to this embedded speech sample are as follows.
* Android ARM-based device with **Android 7.0 (API level 24)** or higher, with a working microphone and [enabled for development](https://developer.android.com/studio/debug/dev-options).
  Note that embedded speech synthesis with neural voices is only supported on ARM64.
* One or more embedded speech models. See https://aka.ms/embedded-speech for the latest information on how to obtain embedded models.

## Build the sample

**By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

[Download the sample code to your development PC.](/README.md#get-the-samples)

To build:
1. Open the folder containing this sample as a project in [Android Studio](https://developer.android.com/studio/).
1. Review the `MainActivity` source and update the following settings as necessary:
   * `EmbeddedSpeechModelsRootPath`
     * Path to the embedded speech recognition/synthesis models on the Android device file system.
       This may be a single model folder or a top-level folder for several models.
     * Use an absolute path or a path relative to the application working folder.
       The path is recursively searched for model files.
     * Files belonging to a specific model must be present as normal individual files in a model folder,
       not in a package, and they must be readable by the application process.
       The model internal subfolder structure must be intact i.e. as originally delivered.
   * `EmbeddedSpeechRecognitionModelName`
     * Name of the embedded speech recognition model to be used for recognition.
       If recognition is not needed, leave the default value unchanged.
     * The model name can be short (see https://aka.ms/speech/sr-languages, e.g. `en-US`) or full (e.g. `Microsoft Speech Recognizer en-US FP Model V8.1`).
   * `EmbeddedSpeechRecognitionModelKey`
     * Decryption key of the (encrypted) embedded speech recognition model.
       If recognition is not needed, leave the default value unchanged.
   * `EmbeddedSpeechSynthesisVoiceName`
     * Name of the embedded speech synthesis voice to be used for synthesis.
       If synthesis is not needed, leave the default value unchanged.
     * The voice name can be short (see https://aka.ms/speech/tts-languages, e.g. `en-US-AriaNeural`) or full (e.g. `Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)`).
   * `EmbeddedSpeechSynthesisVoiceKey`
     * Decryption key of the (encrypted) embedded speech synthesis voice.
       If synthesis is not needed, leave the default value unchanged.
1. Press **Ctrl+F9** or select **Build** \> **Make Project**.

Note: If the build is successful but Android Studio shows references to Speech SDK symbols in red and displays "*Cannot resolve symbol ...*", delete `.gradle` and `.idea` folders, then rebuild.

## Run the sample

1. Connect your Android device to your development PC.
1. Copy the embedded speech model(s) of your choice to the Android device, under a path as configured in `MainActivity`.
1. In Android Studio, press **Shift+F10** or select **Run** \> **Run 'app'**.
   * If a deployment target window comes up, select your Android device.
1. On the Android device, allow all requested permissions for the app when asked.
   * The sample app depends on the `requestLegacyExternalStorage` attribute for access to files on the external storage.
     This access method will not work if the application specifies a target API level 30 (Android 11) or higher.
     See https://developer.android.com/about/versions/11/privacy/storage for information on alternatives.
1. Use the buttons in the app as follows.
   * *Initialize objects* : Initializes the recognizer and/or synthesizer based on model/voice configuration (name, key) in `MainActivity`.
     * This can take a moment due to loading of model data, so it is best done as a separate stage in advance before starting recognition or synthesis.
   * *Recognize speech* : Listens to the device default microphone for input and transcribes recognized speech to text in the app window. This returns one result - run it again to recognize more.
   * *Synthesize speech* : Reads input from the text entry above this button, and synthesizes speech to the device default speaker.

## Remarks

For a new project from scratch, add [client-sdk-embedded](https://mvnrepository.com/artifact/com.microsoft.cognitiveservices.speech/client-sdk-embedded) (`.aar`) as a dependency.
This package supports cloud, embedded, and hybrid speech.
Do **not** add [client-sdk](https://mvnrepository.com/artifact/com.microsoft.cognitiveservices.speech/client-sdk) in the same project, it supports only cloud speech services.

**Note:** Make sure that `@aar` suffix is used when the dependency is specified in `build.gradle`. For example,
```
dependencies {
    implementation 'com.microsoft.cognitiveservices.speech:client-sdk-embedded:1.35.0@aar'
    ...
```

## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)
