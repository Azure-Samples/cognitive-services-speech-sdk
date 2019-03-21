#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

"""
Microsoft Speech SDK for Python
"""

from .speech import *
from . import audio
from . import intent
from . import translation

try:
    from .version import __version__
except ImportError:
    __version__ = '0.0.0'

from .speech_py_impl import (
    CancellationDetails,
    CancellationReason,
    CancellationErrorCode,
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
    CancellationErrorCode,
    NoMatchReason,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    ResultReason,
    AudioConfig,
    CancellationDetails,
    EventSignal,
    NoMatchDetails,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    RecognitionEventArgs,
    RecognitionResult,
    Recognizer,
    Connection,
    ConnectionEventArgs,
    PhraseListGrammar,
    ResultFuture,
    ResultReason,
    SessionEventArgs,
    SpeechConfig,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionEventArgs,
    SpeechRecognitionResult,
    SpeechRecognizer,
    CancellationDetails,
    NoMatchDetails,
):
    cls.__module__ = __name__


__all__ = [
    'CancellationDetails',
    'CancellationReason',
    'EventSignal',
    'NoMatchDetails',
    'NoMatchReason',
    'OutputFormat',
    'PropertyCollection',
    'PropertyId',
    'RecognitionEventArgs',
    'RecognitionResult',
    'ResultReason',
    'SessionEventArgs',
    'SpeechConfig',
    'SpeechRecognitionCanceledEventArgs',
    'SpeechRecognitionEventArgs',
    'SpeechRecognitionResult',
    'SpeechRecognizer',
    'Recognizer',
    'Connection',
    'ConnectionEventArgs',
    'PhraseListGrammar',
]

