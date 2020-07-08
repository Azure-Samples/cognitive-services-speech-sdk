#  Copyright (c) Microsoft. All rights reserved.
#  See https://aka.ms/csspeech/license201809 for the full license information.

"""
Microsoft Speech SDK for Python
"""

from .speech import *
from . import audio
from . import intent
from . import translation
from . import languageconfig

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

from .audio import AudioConfig
from .languageconfig import (
    AutoDetectSourceLanguageConfig,
    SourceLanguageConfig,
)

# override __module__ for correct docs generation
root_namespace_classes = (
    AudioDataStream,
    AutoDetectSourceLanguageResult,
    CancellationDetails,
    CancellationErrorCode,
    CancellationReason,
    Connection,
    ConnectionEventArgs,
    EventSignal,
    KeywordRecognitionModel,
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
    SpeechSynthesisCancellationDetails,
    SpeechSynthesisEventArgs,
    SpeechSynthesisOutputFormat,
    SpeechSynthesisResult,
    SpeechSynthesisWordBoundaryEventArgs,
    SpeechSynthesizer,
    StreamStatus,
    AudioStreamContainerFormat,
)
for cls in root_namespace_classes:
    cls.__module__ = __name__
__all__ = [cls.__name__ for cls in root_namespace_classes]

