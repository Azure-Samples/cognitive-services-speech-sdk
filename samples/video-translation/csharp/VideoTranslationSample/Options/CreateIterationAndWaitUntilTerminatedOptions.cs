//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;
using System;

[Verb("createIterationAndWaitUntilTerminated", HelpText = "Create iteration.")]
public partial class CreateIterationAndWaitUntilTerminatedOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationId)]
    public string TranslationId { get; set; }

    [Option("iterationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.IterationId)]
    public string IterationId { get; set; }

    [Option("iterationName", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.IterationName)]
    public string IterationName { get; set; }

    [Option("speakerCount", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SpeakerCount)]
    public int? SpeakerCount { get; set; }

    [Option("subtitleMaxCharCountPerSegment", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleMaxCharCountPerSegment)]
    public int? SubtitleMaxCharCountPerSegment { get; set; }

    [Option("exportSubtitleInVideo", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.ExportSubtitleInVideo)]
    public bool ExportSubtitleInVideo { get; set; }

    [Option("exportTargetLocaleAdvancedSubtitleFile", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.ExportTargetLocaleAdvancedSubtitleFile)]
    public bool ExportTargetLocaleAdvancedSubtitleFile { get; set; }

    [Option("webvttFileAzureBlobUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileAzureBlobUrl)]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option("webvttFileKind", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileKind)]
    public WebvttFileKind WebvttFileKind { get; set; }

    [Option("ttsCustomLexiconFileUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TtsCustomLexiconFileUrl)]
    public Uri TtsCustomLexiconFileUrl { get; set; }

    [Option("ttsCustomLexiconFileIdInAudioContentCreation", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TtsCustomLexiconFileIdInAudioContentCreation)]
    public Guid TtsCustomLexiconFileIdInAudioContentCreation { get; set; }

    [Option("enableVideoSpeedAdjustment", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableVideoSpeedAdjustment)]
    public bool EnableVideoSpeedAdjustment { get; set; }

    [Option("enableOcrCorrectionFromSubtitle", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableOcrCorrectionFromSubtitle)]
    public bool EnableOcrCorrectionFromSubtitle { get; set; }

    [Option("subtitlePrimaryRgbaColor", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitlePrimaryRgbaColor)]
    public string SubtitlePrimaryRgbaColor { get; set; }

    [Option("subtitleOutlineRgbaColor", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleOutlineRgbaColor)]
    public string SubtitleOutlineRgbaColor { get; set; }

    [Option("subtitleFontSize", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleFontSize)]
    public int SubtitleFontSize { get; set; }

    [Option("enableEmotionalPlatformVoice", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableEmotionalPlatformVoice)]
    public EnableEmotionalPlatformVoiceKind EnableEmotionalPlatformVoice { get; set; }

    public WebvttFile WebvttFile
    {
        get
        {
            if (string.IsNullOrWhiteSpace(this.WebvttFileAzureBlobUrl?.OriginalString))
            {
                return null;
            }

            if (this.WebvttFileKind == WebvttFileKind.None)
            {
                throw new ArgumentException($"Please provide {nameof(this.WebvttFileKind)} argument");
            }

            return new WebvttFile()
            {
                Kind = this.WebvttFileKind,
                Url = this.WebvttFileAzureBlobUrl,
            };
        }
    }
}

