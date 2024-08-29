//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.CustomVoice;
using Microsoft.SpeechServices.VideoTranslation.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;

public class TranslationCreate : StatelessResourceBase
{
    public Guid? VideoFileId { get; set; }

    public VideoTranslationMergeParagraphAudioAlignKind? AlignKind { get; set; }

    public VideoTranslationVoiceKind? VoiceKind { get; set; }

    public string EnableFeatures { get; set; }

    public string ProfileName { get; set; }

    public int? SpeakerCount { get; set; }

    public bool EnableLipSync { get; set; }

    public int? TargetLocaleSubtitleMaxCharCountPerSegment { get; set; }

    public bool? WithoutSubtitleInTranslatedVideoFile { get; set; }

    public bool? ExportPersonalVoicePromptAudioMetadata { get; set; }

    public bool? IsAssociatedWithTargetLocale { get; set; }

    public bool? IsNewTargetLocaleCreation { get; set; }

    public string PersonalVoiceModelName { get; set; }

    public VideoTranslationWebvttSourceKind? WebvttSourceKind { get; set; }

    public Guid? BaseTargetLocaleId { get; set; }

    // Json serialized speaker consents
    public List<string> Consents { get; set; }

    public Dictionary<CultureInfo, TranslationTargetLocaleCreate> TargetLocales { get; set; }
}
