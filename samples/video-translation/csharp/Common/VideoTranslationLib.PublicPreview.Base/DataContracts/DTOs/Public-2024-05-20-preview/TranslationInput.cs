// <copyright file="TranslationInput.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;
using System.Globalization;
using System.Linq;

public class TranslationInput
{
    public CultureInfo SourceLocale { get; set; }

    public CultureInfo TargetLocale { get; set; }

    public VoiceKind VoiceKind { get; set; }

    public int? SpeakerCount { get; set; }

    public bool? EnableLipSync { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportSubtitleInVideo { get; set; }

    public Uri VideoFileUrl { get; set; }

    public string GetVideoFileName()
    {
        if (string.IsNullOrEmpty(this.VideoFileUrl?.OriginalString))
        {
            return string.Empty;
        }

        var videoFileName = this.VideoFileUrl.Segments.Last();
        videoFileName = Uri.UnescapeDataString(videoFileName);
        return videoFileName;
    }
}
