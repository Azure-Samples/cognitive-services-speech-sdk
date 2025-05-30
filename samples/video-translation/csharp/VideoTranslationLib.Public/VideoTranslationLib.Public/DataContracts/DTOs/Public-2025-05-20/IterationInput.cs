//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;

using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using System;
using System.Runtime.InteropServices;

public partial class IterationInput
{
    public EnableEmotionalPlatformVoiceKind? EnableEmotionalPlatformVoice { get; set; }

    public int? SpeakerCount { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportSubtitleInVideo { get; set; }

    public WebvttFile WebvttFile { get; set; }

    public Uri TtsCustomLexiconFileUrl { get; set; }

    public Guid? TtsCustomLexiconFileIdInAudioContentCreation { get; set; }

    public bool? EnableOcrCorrectionFromSubtitle { get; set; }

    // This may either slow down video for longer translated text or speed up video for shorter translated text.
    public bool? EnableVideoSpeedAdjustment { get; set; }

    public bool? ExportTargetLocaleAdvancedSubtitleFile { get; set; }

    public string SubtitlePrimaryColor { get; set; }

    public string SubtitleOutlineColor { get; set; }

    public int? SubtitleFontSize { get; set; }
}
