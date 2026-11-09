# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license.

import os
from pathlib import Path
import sys
import threading

import azure.cognitiveservices.speech as speechsdk


def required_environment(name):
    value = os.environ.get(name)
    if not value:
        raise ValueError(f"Set {name} before running this sample.")
    return value


def create_translation_config(speech_key, endpoint):
    config = speechsdk.translation.SpeechTranslationConfig(
        subscription=speech_key, endpoint=endpoint
    )
    config.add_target_language("fr")
    config.voice_name = "personal-voice"
    config.set_speech_synthesis_output_format(
        speechsdk.SpeechSynthesisOutputFormat.Riff16Khz16BitMonoPcm
    )
    return config


def write_audio(path, audio):
    with open(path, "xb") as output:
        output.write(audio)


def main():
    speech_key = required_environment("SPEECH_KEY")
    endpoint = required_environment("SPEECH_ENDPOINT")
    audio_file = required_environment("LIVE_INTERPRETER_AUDIO_FILE")
    if not Path(audio_file).is_file():
        raise ValueError(f"WAV input file not found: {audio_file}")

    translation_config = create_translation_config(speech_key, endpoint)
    # An empty configuration enables open-range language detection in Python.
    auto_detect_config = (
        speechsdk.languageconfig.AutoDetectSourceLanguageConfig()
    )
    audio_config = speechsdk.audio.AudioConfig(filename=audio_file)
    recognizer = speechsdk.translation.TranslationRecognizer(
        translation_config=translation_config,
        auto_detect_source_language_config=auto_detect_config,
        audio_config=audio_config,
    )

    stopped = threading.Event()
    state_lock = threading.Lock()
    failures = []
    audio_index = 0

    def log_result(label, result):
        language = result.properties.get(
            speechsdk.PropertyId
            .SpeechServiceConnection_AutoDetectSourceLanguageResult
        )
        print(f"{label} in '{language}': {result.text}")
        for language, translation in result.translations.items():
            print(f"    TRANSLATED into '{language}': {translation}")

    def recognizing(event):
        log_result("RECOGNIZING", event.result)

    def recognized(event):
        if event.result.reason == speechsdk.ResultReason.TranslatedSpeech:
            log_result("RECOGNIZED", event.result)
        elif event.result.reason == speechsdk.ResultReason.NoMatch:
            print("NOMATCH: Speech couldn't be recognized.")

    def synthesizing(event):
        nonlocal audio_index
        audio = event.result.audio
        if not audio:
            return
        with state_lock:
            audio_index += 1
            output_file = f"translation-{audio_index}.wav"
            try:
                write_audio(output_file, audio)
                print(f"Wrote {output_file}: {len(audio)} bytes")
            except OSError as error:
                failures.append(f"Couldn't write {output_file}: {error}")
                stopped.set()

    def canceled(event):
        print(f"CANCELED: Reason={event.reason}")
        if event.reason == speechsdk.CancellationReason.Error:
            with state_lock:
                failures.append(
                    f"ErrorCode={event.error_code}, "
                    f"ErrorDetails={event.error_details}"
                )
        stopped.set()

    recognizer.recognizing.connect(recognizing)
    recognizer.recognized.connect(recognized)
    recognizer.synthesizing.connect(synthesizing)
    recognizer.canceled.connect(canceled)
    recognizer.session_stopped.connect(lambda event: stopped.set())

    print("Start translation...")
    recognizer.start_continuous_recognition_async().get()
    try:
        if not stopped.wait(timeout=300):
            raise TimeoutError("Translation timed out after five minutes.")
    finally:
        recognizer.stop_continuous_recognition_async().get()

    with state_lock:
        if failures:
            raise RuntimeError(failures[0])


if __name__ == "__main__":
    try:
        main()
    except (ValueError, RuntimeError, OSError) as error:
        print(f"Live Interpreter failed: {error}", file=sys.stderr)
        sys.exit(1)
