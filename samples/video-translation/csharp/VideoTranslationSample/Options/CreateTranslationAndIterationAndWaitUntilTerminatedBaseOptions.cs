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

public class CreateTranslationAndIterationAndWaitUntilTerminatedBaseOptions : CreateTranslationBaseOptions
{
    [Option("webvttFileAzureBlobUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileAzureBlobUrl)]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option(
        "webvttFileKind",
        Required = false,
        Default = WebvttFileKind.MetadataJson,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileKind)]
    public WebvttFileKind WebvttFileKind { get; set; }

    [Option("ttsCustomLexiconFileUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TtsCustomLexiconFileUrl)]
    public Uri TtsCustomLexiconFileUrl { get; set; }

    // If use Guid?, not support by error: is defined with a bad format
    [Option("ttsCustomLexiconFileIdInAudioContentCreation", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TtsCustomLexiconFileIdInAudioContentCreation)]
    public Guid TtsCustomLexiconFileIdInAudioContentCreation { get; set; }

    [Option("enableVideoSpeedAdjustment", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableVideoSpeedAdjustment)]
    public bool EnableVideoSpeedAdjustment { get; set; }

    [Option("enableOcrCorrectionFromSubtitle", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableOcrCorrectionFromSubtitle)]
    public bool EnableOcrCorrectionFromSubtitle { get; set; }

    [Option("exportTargetLocaleAdvancedSubtitleFile", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.ExportTargetLocaleAdvancedSubtitleFile)]
    public bool ExportTargetLocaleAdvancedSubtitleFile { get; set; }

    [Option("subtitlePrimaryRgbaColor", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitlePrimaryRgbaColor)]
    public string SubtitlePrimaryRgbaColor { get; set; }

    [Option("subtitleOutlineRgbaColor", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleOutlineRgbaColor)]
    public string SubtitleOutlineRgbaColor { get; set; }

    [Option("subtitleFontSize", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.SubtitleFontSize)]
    public int SubtitleFontSize { get; set; }

    [Option("enableEmotionalPlatformVoice", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.EnableEmotionalPlatformVoice)]
    public EnableEmotionalPlatformVoiceKind EnableEmotionalPlatformVoice { get; set; }

    // Not use bool for this --adjustWebvttAlignment argument because:
    //      from API version 2026-03-01, the default value is true if this parameter is not provided,
    //      but if set default value to true for this bool type parameter,
    //      this command line parser doesn't support specify false,
    //      it will always set this parameter to true even user specify
    //      "--adjustWebvttAlignment false" in command line, which is not expected.
    // When use this bool? type for --adjustWebvttAlignment argument, it can specify parameter like this:
    //      "--adjustWebvttAlignment" :which means null for API request, the same behavior as true.
    //      "--adjustWebvttAlignment true" :which means true for API request.
    //      "--adjustWebvttAlignment false" :which means false for API request.
    [Option(
        "adjustWebvttAlignment",
        Required = false,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.AdjustWebvttAlignment)]
    public bool? AdjustWebvttAlignment { get; set; }

    public bool? AdjustWebvttAlignmentApiParameter(string apiVersion)
    {
        if (string.Equals(
                CommonPublicConst.ApiVersions.ApiVersion20240520Preview,
                apiVersion,
                StringComparison.OrdinalIgnoreCase) ||
            string.Equals(
                CommonPublicConst.ApiVersions.ApiVersion20250520,
                apiVersion,
                StringComparison.OrdinalIgnoreCase))
        {
            // For API version 2025-05-20 and previous, always keep the parameter null, since the parameter is not released in public doc for those versions.
            // This is for backward compatiable for user who keep using old API version with latest client tool.
            return null;
        }
        else
        {
            // For API version 2025-03-01 and later, the default value is true if this parameter is not provided.
            return this.AdjustWebvttAlignment;
        }
    }

    [Option(
        "use24kPromptAudio",
        Default = false,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.AdjustWebvttAlignment)]
    public bool Use24kPromptAudio { get; set; }

    [Option(
        "subtitleVerticalMargin",
        Required = false,
        Default = VideoTranslationPublicConst.DefaultValue.SubtitleVerticalMargin,
    HelpText = "Subtitle vertical margin must be between 0 and 200(default 10).")]
    public int SubtitleVerticalMargin { get; set; }

    [Option(
        "adjustBackgroundVolumeMultiplier",
        Required = false,
        Default = 0,
        HelpText = VideoTranslationPublicConst.ArgumentDescription.AdjustBackgroundVolumeMultiplier)]
    public double AdjustBackgroundVolumeMultiplier { get; set; }

    [Option(
        "pushResultToAzureStorageBlobDirUrl",
        Required = false,
        HelpText = "Push result to Azure storage blob dir URL.")]
    public Uri PushResultToAzureStorageBlobDirUrl { get; set; }

    [Option(
        "pushResultToAzureStorageBlobManagedIdentityClientId",
        Required = false,
        HelpText = "User-assigned managed identity client ID.")]
    public Guid PushResultToAzureStorageBlobManagedIdentityClientId { get; set; }
}

