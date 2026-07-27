#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Shared configuration for the embedded speech samples.

To tailor the samples to your configuration, either edit the default values in the
"CONFIGURABLE SETTINGS" section below or set the corresponding environment variables
(shown in the comments) before running the sample application.
"""

import os

import azure.cognitiveservices.speech as speechsdk


# START OF CONFIGURABLE SETTINGS

# Embedded speech model license (text).
# This applies to embedded speech recognition, synthesis and translation.
# It is presumed that all your embedded speech models use the same license.
EmbeddedSpeechModelLicense = "YourEmbeddedSpeechModelLicense"  # or set EMBEDDED_SPEECH_MODEL_LICENSE

# Path to the local embedded speech recognition model(s) on the device file system.
# This may be a single model folder or a top-level folder for several models.
# Use an absolute path or a path relative to the application working folder.
# The path is recursively searched for model files.
# Files belonging to a specific model must be available as normal individual files in a model folder,
# not inside an archive, and they must be readable by the application process.
EmbeddedSpeechRecognitionModelPath = "YourEmbeddedSpeechRecognitionModelPath"  # or set EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH

# Name of the embedded speech recognition model to be used for recognition.
# This can be a short locale (e.g. "en-US") or the full model name provided with your model.
EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"  # or set EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME

# Path to the local embedded speech synthesis voice(s) on the device file system.
# This may be a single voice folder or a top-level folder for several voices.
# Use an absolute path or a path relative to the application working folder.
# The path is recursively searched for voice files.
# Files belonging to a specific voice must be available as normal individual files in a voice folder,
# not inside an archive, and they must be readable by the application process.
EmbeddedSpeechSynthesisVoicePath = "YourEmbeddedSpeechSynthesisVoicePath"  # or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH

# Name of the embedded speech synthesis voice to be used for synthesis.
# This can be a short voice name (e.g. "en-US-JennyNeural") or the full voice name provided with your voice.
EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"  # or set EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME

# Path to the local embedded speech translation model(s) on the device file system.
# This may be a single model folder or a top-level folder for several models.
# Use an absolute path or a path relative to the application working folder.
# The path is recursively searched for model files.
# Files belonging to a specific model must be available as normal individual files in a model folder,
# not inside an archive, and they must be readable by the application process.
EmbeddedSpeechTranslationModelPath = "YourEmbeddedSpeechTranslationModelPath"  # or set EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH

# Name of the embedded speech translation model to be used for translation.
# Use the full model name provided with your model.
EmbeddedSpeechTranslationModelName = "YourEmbeddedSpeechTranslationModelName"  # or set EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME

# END OF CONFIGURABLE SETTINGS

# Note: Hybrid (cloud & embedded) speech configuration is not included in these samples
# because the Speech SDK for Python does not currently expose HybridSpeechConfig. See the
# C++, C# or Java embedded speech samples for hybrid speech examples.


# Embedded speech recognition and translation default input audio format settings.
# In addition, little-endian signed integer samples are required.
EMBEDDED_SPEECH_SAMPLES_PER_SECOND = 16000  # or 8000
EMBEDDED_SPEECH_BITS_PER_SAMPLE = 16        # DO NOT MODIFY; no other format supported
EMBEDDED_SPEECH_CHANNELS = 1                # DO NOT MODIFY; no other format supported

# For more information about keyword recognition and models, see
# https://learn.microsoft.com/azure/ai-services/speech-service/keyword-recognition-overview
KEYWORD_MODEL_FILE_NAME = "data/keyword_computer.table"
KEYWORD_PHRASE = "Computer"

# Input audio file (WAV format) used by the file-based recognition sample.
# The audio must use one of the supported formats; see the note above.
WAV_AUDIO_FILE_NAME = "data/speech_test.wav"


def _get_setting(environment_variable_name, default_value):
    """Get a setting value from an environment variable or fall back to the default."""
    value = os.environ.get(environment_variable_name)
    return value if value else default_value


def _resolve(environment_variable_name, default_value, placeholder):
    """Resolve a setting, treating the placeholder default as 'not set' (None)."""
    value = _get_setting(environment_variable_name, default_value)
    return None if value == placeholder else value


# These are resolved on first use by _verify_settings().
SpeechModelLicense = None
SpeechRecognitionModelPath = None
SpeechRecognitionModelName = None
SpeechSynthesisVoicePath = None
SpeechSynthesisVoiceName = None
SpeechTranslationModelPath = None
SpeechTranslationModelName = None
_verified = False


def _verify_settings():
    """Resolve settings from environment variables or defaults. Returns True if the license is set."""
    global SpeechModelLicense, _verified
    global SpeechRecognitionModelPath, SpeechRecognitionModelName
    global SpeechSynthesisVoicePath, SpeechSynthesisVoiceName
    global SpeechTranslationModelPath, SpeechTranslationModelName

    print("Current working directory: {}".format(os.getcwd()))

    SpeechModelLicense = _get_setting("EMBEDDED_SPEECH_MODEL_LICENSE", EmbeddedSpeechModelLicense)
    if not SpeechModelLicense or SpeechModelLicense == "YourEmbeddedSpeechModelLicense":
        print("## ERROR: The embedded speech model license is not set.")
        _verified = True
        return False

    SpeechRecognitionModelPath = _resolve(
        "EMBEDDED_SPEECH_RECOGNITION_MODEL_PATH", EmbeddedSpeechRecognitionModelPath,
        "YourEmbeddedSpeechRecognitionModelPath")
    SpeechRecognitionModelName = _resolve(
        "EMBEDDED_SPEECH_RECOGNITION_MODEL_NAME", EmbeddedSpeechRecognitionModelName,
        "YourEmbeddedSpeechRecognitionModelName")

    SpeechSynthesisVoicePath = _resolve(
        "EMBEDDED_SPEECH_SYNTHESIS_VOICE_PATH", EmbeddedSpeechSynthesisVoicePath,
        "YourEmbeddedSpeechSynthesisVoicePath")
    SpeechSynthesisVoiceName = _resolve(
        "EMBEDDED_SPEECH_SYNTHESIS_VOICE_NAME", EmbeddedSpeechSynthesisVoiceName,
        "YourEmbeddedSpeechSynthesisVoiceName")

    SpeechTranslationModelPath = _resolve(
        "EMBEDDED_SPEECH_TRANSLATION_MODEL_PATH", EmbeddedSpeechTranslationModelPath,
        "YourEmbeddedSpeechTranslationModelPath")
    SpeechTranslationModelName = _resolve(
        "EMBEDDED_SPEECH_TRANSLATION_MODEL_NAME", EmbeddedSpeechTranslationModelName,
        "YourEmbeddedSpeechTranslationModelName")

    _verified = True
    return True


def _ensure_verified():
    if not _verified:
        return _verify_settings()
    return bool(SpeechModelLicense) and SpeechModelLicense != "YourEmbeddedSpeechModelLicense"


# Utility functions for the main menu.
def has_speech_recognition_model():
    if not _ensure_verified():
        return False
    if not SpeechRecognitionModelPath or not SpeechRecognitionModelName:
        print("## ERROR: No speech recognition model specified.")
        return False
    return True


def has_speech_synthesis_voice():
    if not _ensure_verified():
        return False
    if not SpeechSynthesisVoicePath or not SpeechSynthesisVoiceName:
        print("## ERROR: No speech synthesis voice specified.")
        return False
    return True


def has_speech_translation_model():
    if not _ensure_verified():
        return False
    if not SpeechTranslationModelPath or not SpeechTranslationModelName:
        print("## ERROR: No speech translation model specified.")
        return False
    return True


# Path-only checks for the "list models/voices" scenarios. Listing only needs a
# model/voice path; the specific model or voice name is not required to enumerate
# what is available at that path.
def has_speech_recognition_model_path():
    if not _ensure_verified():
        return False
    if not SpeechRecognitionModelPath:
        print("## ERROR: No speech recognition model path specified.")
        return False
    return True


def has_speech_synthesis_voice_path():
    if not _ensure_verified():
        return False
    if not SpeechSynthesisVoicePath:
        print("## ERROR: No speech synthesis voice path specified.")
        return False
    return True


def has_speech_translation_model_path():
    if not _ensure_verified():
        return False
    if not SpeechTranslationModelPath:
        print("## ERROR: No speech translation model path specified.")
        return False
    return True


def create_embedded_speech_config():
    """
    Creates an instance of an embedded speech config.

    A single config can be used for embedded speech recognition, synthesis and
    translation, depending on which model paths and names are set.

    :returns: a configured :class:`speechsdk.EmbeddedSpeechConfig`, or ``None`` if no
        model path is set or the model license is missing.
    """
    if not _ensure_verified():
        return None

    # Add paths for offline data.
    paths = []
    if SpeechRecognitionModelPath:
        paths.append(SpeechRecognitionModelPath)
    if SpeechSynthesisVoicePath:
        paths.append(SpeechSynthesisVoicePath)
    if SpeechTranslationModelPath:
        paths.append(SpeechTranslationModelPath)

    if not paths:
        print("## ERROR: No model path(s) specified.")
        return None

    # Note, if there is only one path then you can also use EmbeddedSpeechConfig.from_path(path).
    # All paths must be valid directory paths on the file system, otherwise e.g. initialization of
    # embedded speech synthesis will fail.
    config = speechsdk.EmbeddedSpeechConfig.from_paths(paths)

    if SpeechRecognitionModelName:
        # Mandatory configuration for embedded speech recognition.
        config.set_speech_recognition_model(SpeechRecognitionModelName, SpeechModelLicense)

    if SpeechSynthesisVoiceName:
        # Mandatory configuration for embedded speech synthesis.
        config.set_speech_synthesis_voice(SpeechSynthesisVoiceName, SpeechModelLicense)
        if "Neural" in SpeechSynthesisVoiceName:
            # Embedded neural voices only support 24kHz sample rate.
            config.set_speech_synthesis_output_format(
                speechsdk.SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm)

    if SpeechTranslationModelName:
        # Mandatory configuration for embedded speech translation.
        config.set_speech_translation_model(SpeechTranslationModelName, SpeechModelLicense)

    # Disable profanity masking.
    # config.set_profanity(speechsdk.ProfanityOption.Raw)

    return config
