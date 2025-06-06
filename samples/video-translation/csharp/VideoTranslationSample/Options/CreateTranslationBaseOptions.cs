//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.CommonLib;
using System;
using System.Globalization;
using VoiceKind = Cris.Http.DTOs.Public.VideoTranslation.Public20250520.VoiceKind;

public partial class CreateTranslationBaseOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationId)]
    public string TranslationId { get; set; }

    [Option("videoFileAzureBlobUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.VideoFileAzureBlobUrl)]
    public Uri VideoFileAzureBlobUrl { get; set; }

    [Option("audioFileAzureBlobUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.AudioFileAzureBlobUrl)]
    public Uri AudioFileAzureBlobUrl { get; set; }

    [Option("sourceLocale", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SourceLocale)]
    public CultureInfo SourceLocale { get; set; }

    [Option("targetLocale", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.TargetLocale)]
    public CultureInfo TargetLocale { get; set; }

    [Option("voiceKind", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.VoiceKind)]
    public VoiceKind VoiceKind { get; set; }

    [Option("speakerCount", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SpeakerCount)]
    public int? SpeakerCount { get; set; }

    [Option("enableLipSync", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableLipSync)]
    public bool EnableLipSync { get; set; }

    [Option("subtitleMaxCharCountPerSegment", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleMaxCharCountPerSegment)]
    public int? SubtitleMaxCharCountPerSegment { get; set; }

    [Option("exportSubtitleInVideo", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.ExportSubtitleInVideo)]
    public bool ExportSubtitleInVideo { get; set; }

    [Option("translationName", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationName)]
    public string TranslationName { get; set; }

    [Option("translationDescription", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationDescription)]
    public string TranslationDescription { get; set; }
}

