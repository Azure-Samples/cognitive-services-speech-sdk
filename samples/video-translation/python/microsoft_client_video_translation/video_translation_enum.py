# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from enum import Enum


class VoiceKind(str, Enum):
    PlatformVoice = 'PlatformVoice'
    PersonalVoice = 'PersonalVoice'


class WebvttFileKind(str, Enum):
    SourceLocaleSubtitle = 'SourceLocaleSubtitle'
    TargetLocaleSubtitle = 'TargetLocaleSubtitle'
    MetadataJson = 'MetadataJson'


class EventHubVideoTranslationEventKind(str, Enum):
    Ping = 'Ping'
    TranslationCompletion = 'TranslationCompletion'
    IterationCompletion = 'IterationCompletion'


class EnableEmotionalPlatformVoice(str, Enum):
    Auto = 'Auto'
    Enable = 'Enable'
    Disable = 'Disable'


class InputFileSourceKind(str, Enum):
    AzureStorageBlobManagedIdentity = 'AzureStorageBlobManagedIdentity'

