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
    IntentRecognitionCanceledEventArgs,
    IntentRecognitionEventArgs,
    IntentRecognitionResult,
    NoMatchDetails,
    NoMatchReason,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    RecognitionEventArgs,
    RecognitionResult,
    ResultReason,
    SessionEventArgs,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionEventArgs,
    SpeechRecognitionResult,
    TranslationRecognitionCanceledEventArgs,
    TranslationRecognitionEventArgs,
    TranslationRecognitionResult,
    TranslationSynthesisEventArgs,
    TranslationSynthesisResult,
    SessionEventSignal,
    RecognitionEventSignal,
    SpeechRecognitionEventSignal,
    TranslationTextEventSignal,
    IntentEventSignal,
    SpeechRecognitionCanceledEventSignal,
    TranslationTextCanceledEventSignal,
    IntentCanceledEventSignal,
    TranslationSynthesisEventSignal,
)

__all__ = [
    'AudioConfig',
    'AudioInputStream',
    'AudioStreamFormat',
    'CancellationDetails',
    'CancellationReason',
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
    'SessionEventSignal',
    'RecognitionEventSignal',
    'Recognizer',
    'SpeechRecognitionEventSignal',
    'TranslationTextEventSignal',
    'IntentEventSignal',
    'SpeechRecognitionCanceledEventSignal',
    'TranslationTextCanceledEventSignal',
    'IntentCanceledEventSignal',
    'IntentEventSignal',
    'TranslationSynthesisEventSignal',
]
