# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import locale
from datetime import datetime
from dataclasses import dataclass
from urllib3.util import Url
from typing import Optional

from microsoft_client_video_translation.video_translation_enum import (
    WebvttFileKind, VoiceKind, EnableEmotionalPlatformVoice, InputFileSourceKind
)

from microsoft_speech_client_common.client_common_dataclass import (
    StatefulResourceBaseDefinition
)


@dataclass(kw_only=True)
class TranslationInputBaseDefinition:
    speakerCount: Optional[int] = None
    exportSubtitleInVideo: Optional[bool] = None
    subtitleMaxCharCountPerSegment: Optional[int] = None


@dataclass(kw_only=True)
class WebvttFileDefinition:
    url: Url
    kind: WebvttFileKind


@dataclass(kw_only=True)
class TranslationInputDefinition(TranslationInputBaseDefinition):
    # This is optional because the moment after translation created, API has not downloaded video file to server side yet.
    videoFileUrl: Optional[str] = None
    audioFileUrl: Optional[str] = None
    sourceLocale: locale = None
    targetLocale: locale
    voiceKind: VoiceKind
    autoCreateFirstIteration: Optional[bool] = None
    enableLipSync: Optional[bool] = None
    inputFileSourceKind: Optional[InputFileSourceKind] = None
    inputFileAzureStorageBlobManagedIdentityClientId: Optional[str] = None


@dataclass(kw_only=True)
class IterationInputDefinition(TranslationInputBaseDefinition):
    webvttFile: Optional[WebvttFileDefinition] = None
    enableEmotionalPlatformVoice: Optional[EnableEmotionalPlatformVoice] = None
    ttsCustomLexiconFileUrl: Optional[Url] = None
    ttsCustomLexiconFileIdInAudioContentCreation: Optional[str] = None
    enableVideoSpeedAdjustment: Optional[bool] = None
    enableOcrCorrectionFromSubtitle: Optional[bool] = None
    exportTargetLocaleAdvancedSubtitleFile: Optional[bool] = None
    subtitlePrimaryColor: Optional[str] = None
    subtitleOutlineColor: Optional[str] = None
    subtitleFontSize: Optional[int] = None
    subtitleVerticalMargin: Optional[int] = None
    adjustWebvttAlignment: Optional[bool] = None
    use24kPromptAudio: Optional[bool] = None
    exportAdjustedPromptAudioInWebvtt: Optional[bool] = None
    adjustBackgroundVolumeMultiplier: Optional[float] = None
    pushResultToAzureStorageBlobDirUrl: Optional[Url] = None
    pushResultToAzureStorageBlobManagedIdentityClientId: Optional[str] = None


@dataclass(kw_only=True)
class IterationResultDefinition:
    translatedVideoFileUrl: Optional[Url] = None
    sourceLocaleSubtitleWebvttFileUrl: Optional[Url] = None
    targetLocaleSubtitleWebvttFileUrl: Optional[Url] = None
    metadataJsonWebvttFileUrl: Optional[Url] = None


@dataclass(kw_only=True)
class IterationDefinition(StatefulResourceBaseDefinition):
    input: IterationInputDefinition
    result: Optional[IterationResultDefinition] = None
    failureReason: Optional[str] = None


@dataclass(kw_only=True)
class TranslationDefinition(StatefulResourceBaseDefinition):
    input: TranslationInputDefinition
    firstIterationInput: Optional[IterationInputDefinition] = None
    latestIteration: Optional[IterationDefinition] = None
    latestSucceededIteration: Optional[IterationDefinition] = None
    failureReason: Optional[str] = None
    expiresDateTime: Optional[datetime] = None


@dataclass(kw_only=True)
class PagedTranslationDefinition:
    value: list[TranslationDefinition]
    nextLink: Optional[Url] = None


@dataclass(kw_only=True)
class PagedIterationDefinition:
    value: list[IterationDefinition]
    nextLink: Optional[Url] = None
