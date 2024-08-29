//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.VideoTranslation.Enums;
using System;
using System.Collections.Generic;

public class TranslationBase : StatefulResourceBase
{
    public Guid VideoFileId { get; set; }

    public VideoTranslationMergeParagraphAudioAlignKind? AudioAlignKind { get; set; }

    public VideoTranslationVoiceKind? VoiceKind { get; init; }

    public string EnableFeatures { get; set; }

    public string ProfileName { get; set; }

    public int? SpeakerCount { get; set; }

    public bool? EnableLipSync { get; set; }

    public bool? WithoutSubtitleInTranslatedVideoFile { get; set; }

    public int? TargetLocaleSubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportPersonalVoicePromptAudioMetadata { get; set; }

    public bool? IsAssociatedWithTargetLocale { get; set; }

    public string PersonalVoiceModelName { get; set; }

    public VideoTranslationWebvttSourceKind? WebvttSourceKind { get; set; }

    public IReadOnlyDictionary<string, string> AdditionalProperties { get; set; }
}
