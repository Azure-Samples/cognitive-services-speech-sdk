//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;
using System.Globalization;
using System.Linq;

public partial class TranslationInput
{
    public CultureInfo SourceLocale { get; set; }

    public CultureInfo TargetLocale { get; set; }

    public VoiceKind VoiceKind { get; set; }

    public int? SpeakerCount { get; set; }

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
