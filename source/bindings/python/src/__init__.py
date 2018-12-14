#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

"""
Microsoft Speech SDK
"""

from .speech import *
from .translation import *
from .intent import *
from .audio import *

try:
    from .version import __version__
except ImportError:
    __version__ = '0.0.0'

from .speech_py_impl import (
    CancellationDetails,
    CancellationReason,
    NoMatchDetails,
    NoMatchReason,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    ResultReason,
)

# override __module__ for correct docs generation
for cls in (
    CancellationReason,
    NoMatchReason,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    ResultReason,
):
    cls.__module__ = __name__

for cls in (
    CancellationDetails,
    NoMatchDetails,
):
    cls.__module__ = __name__ + '.speech'


__all__ = [
    'AudioConfig',
    'AudioInputStream',
    'AudioStreamFormat',
    'CancellationDetails',
    'CancellationReason',
    'EventSignal',
    'IntentRecognitionCanceledEventArgs',
    'IntentRecognitionEventArgs',
    'IntentRecognitionResult',
    'IntentRecognizer',
    'LanguageUnderstandingModel',
    'NoMatchDetails',
    'NoMatchReason',
    'OutputFormat',
    'PropertyCollection',
    'PropertyId',
    'PullAudioInputStream',
    'PullAudioInputStreamCallback',
    'PushAudioInputStream',
    'RecognitionEventArgs',
    'RecognitionResult',
    'ResultReason',
    'SessionEventArgs',
    'SpeechConfig',
    'SpeechRecognitionCanceledEventArgs',
    'SpeechRecognitionEventArgs',
    'SpeechRecognitionResult',
    'SpeechRecognizer',
    'SpeechTranslationConfig',
    'TranslationRecognitionCanceledEventArgs',
    'TranslationRecognitionEventArgs',
    'TranslationRecognitionResult',
    'TranslationRecognizer',
    'TranslationSynthesisEventArgs',
    'TranslationSynthesisResult',
    'Recognizer',
]

