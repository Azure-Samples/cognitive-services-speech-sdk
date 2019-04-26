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
    CancellationErrorCode,
    CancellationReason,
    NoMatchDetails,
    NoMatchReason,
    OutputFormat,
    ProfanityOption,
    PropertyCollection,
    PropertyId,
    ResultReason,
    ServicePropertyChannel,
)

# override __module__ for correct docs generation
for cls in (
    AudioConfig,
    CancellationDetails,
    CancellationErrorCode,
    CancellationReason,
    Connection,
    ConnectionEventArgs,
    EventSignal,
    NoMatchDetails,
    NoMatchReason,
    OutputFormat,
    PhraseListGrammar,
    ProfanityOption,
    PropertyCollection,
    PropertyId,
    RecognitionEventArgs,
    RecognitionResult,
    Recognizer,
    ResultFuture,
    ResultReason,
    ServicePropertyChannel,
    SessionEventArgs,
    SpeechConfig,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionEventArgs,
    SpeechRecognitionResult,
    SpeechRecognizer,
):
    cls.__module__ = __name__


__all__ = [
    'CancellationDetails',
    'CancellationReason',
    'Connection',
    'ConnectionEventArgs',
    'EventSignal',
    'NoMatchDetails',
    'NoMatchReason',
    'OutputFormat',
    'PhraseListGrammar',
    'ProfanityOption',
    'PropertyCollection',
    'PropertyId',
    'RecognitionEventArgs',
    'RecognitionResult',
    'Recognizer',
    'ResultReason',
    'ServicePropertyChannel',
    'SessionEventArgs',
    'SpeechConfig',
    'SpeechRecognitionCanceledEventArgs',
    'SpeechRecognitionEventArgs',
    'SpeechRecognitionResult',
    'SpeechRecognizer',
]

