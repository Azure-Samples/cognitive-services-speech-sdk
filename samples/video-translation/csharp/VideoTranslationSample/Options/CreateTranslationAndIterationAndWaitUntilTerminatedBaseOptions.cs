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
    [Option("iterationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.IterationId)]
    public string IterationId { get; set; }

    [Option("webvttFileAzureBlobUrl", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileAzureBlobUrl)]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option("webvttFileKind", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.WebvttFileKind)]
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
}

