//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using System;
using System.Globalization;
using VoiceKind = Cris.Http.DTOs.Public.VideoTranslation.Public20250520.VoiceKind;

public partial class CreateTranslationBaseOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }

    [Option("videoFileAzureBlobUrl", Required = false, HelpText = "Specify video file Azure blob URL.")]
    public Uri VideoFileAzureBlobUrl { get; set; }

    [Option("audioFileAzureBlobUrl", Required = false, HelpText = "Specify audio file Azure blob URL.")]
    public Uri AudioFileAzureBlobUrl { get; set; }

    [Option("sourceLocale", Required = false, HelpText = "Specify source locale of the video.")]
    public CultureInfo SourceLocale { get; set; }

    [Option("targetLocale", Required = true, HelpText = "Specify target locale of the video.")]
    public CultureInfo TargetLocale { get; set; }

    [Option("voiceKind", Required = true, HelpText = "Specify voice kind: PlatformVoice or PersonalVoice.")]
    public VoiceKind VoiceKind { get; set; }

    [Option("speakerCount", Required = false, HelpText = "Specify speaker count.")]
    public int? SpeakerCount { get; set; }

    [Option("enableLipSync", Required = false, HelpText = "Specify whether enable lip sync.")]
    public bool EnableLipSync { get; set; }

    [Option("subtitleMaxCharCountPerSegment", Required = false, HelpText = "Specify subtitle max visiable char count per segment.")]
    public int? SubtitleMaxCharCountPerSegment { get; set; }

    [Option("exportSubtitleInVideo", Required = false, HelpText = "Specify speaker count.")]
    public bool ExportSubtitleInVideo { get; set; }

    [Option("translationName", Required = false, HelpText = "Specify translation name.")]
    public string TranslationName { get; set; }

    [Option("translationDescription", Required = false, HelpText = "Specify translation description.")]
    public string TranslationDescription { get; set; }
}

