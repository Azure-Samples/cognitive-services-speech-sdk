//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.Utility;

using System.Collections.Generic;
using System.Globalization;
using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.VideoTranslation.Enums;

public class VideoTranslationPoolInputArgs
{
    public VideoTranslationPoolInputArgs()
    {
        this.AdditionalHeaders = new Dictionary<string, string>();
        this.AdditionalProperties = new Dictionary<string, string>();
    }

    public string VideoFilePath { get; set; }

    public CultureInfo SourceLocale { get; set; }

    public VideoTranslationVoiceKind? VoiceKind { get; set; }

    public string EnableFeatures { get; set; }

    public string ProfileName { get; set; }

    public bool? WithoutSubtitleInTranslatedVideoFile { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public int? SpeakerCount { get; set; }

    public bool EnableLipSync { get; set; }

    public bool? ExportPersonalVoicePromptAudioMetadata { get; set; }

    public string PersonalVoiceModelName { get; set; }

    public bool? IsAssociatedWithTargetLocale { get; set; }

    public bool? IsNewTargetLocaleCreation { get; set; }

    public VideoTranslationWebvttSourceKind? WebvttSourceKind { get; set; }

    public List<CultureInfo> TargetLocales { get; set; }

    public Dictionary<string, string> AdditionalProperties { get; private set; }

    public Dictionary<string, string> AdditionalHeaders { get; private set; }
}

