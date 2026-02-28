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

    [Option("iterationId", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.IterationId)]
    public string IterationId { get; set; }

    public string IterationIdOrNew
    {
        get
        {
            if (!string.IsNullOrWhiteSpace(this.IterationId))
            {
                return this.IterationId;
            }

            return Guid.NewGuid().ToString();
        }
    }

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

    [Option("webvttFileKind",
        Required = false,
        Default = WebvttFileKind.MetadataJson,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileKind)]
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

    [Option(
        "adjustWebvttAlignment",
        Required = false,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.AdjustWebvttAlignment)]
    public bool AdjustWebvttAlignment { get; set; }

    [Option(
        "subtitleVerticalMargin",
        Required = false,
        Default = VideoTranslationPublicConst.DefaultValue.SubtitleVerticalMargin,
        HelpText = "Subtitle vertical margin must be between 0 and 200(default 10).")]
    public int SubtitleVerticalMargin { get; set; }

    [Option("adjustBackgroundVolumeMultiplier", Required = false, Default = 0, HelpText = "Adjust the background volume based on the maximum volume with this multiplier factor. The value range is (0, 1], the suggested value is 0.6 if you want to adjust the background volume. If not specified, the original background audio volume will be kept unchanged.")]
    public double AdjustBackgroundVolumeMultiplier { get; set; }

    [Option("pushResultToAzureStorageBlobDirUrl", Required = false, HelpText = "Push result to Azure storage blob dir URL.")]
    public Uri PushResultToAzureStorageBlobDirUrl { get; set; }

    [Option("pushResultToAzureStorageBlobManagedIdentityClientId", Required = false, HelpText = "User-assigned managed identity client ID.")]
    public Guid PushResultToAzureStorageBlobManagedIdentityClientId { get; set; }

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

