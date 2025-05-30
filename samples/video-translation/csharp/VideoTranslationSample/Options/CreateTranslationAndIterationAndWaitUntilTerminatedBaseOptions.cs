//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;
using System;

public class CreateTranslationAndIterationAndWaitUntilTerminatedBaseOptions : CreateTranslationBaseOptions
{
    [Option("iterationId", Required = true, HelpText = "Specify iteration ID.")]
    public string IterationId { get; set; }

    [Option("webvttFileAzureBlobUrl", Required = false, HelpText = "Specify webvtt file Azure blob URL, it is required from secondary iteration.")]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option("webvttFileKind", Required = false, HelpText = "Specify webvtt file kind: SourceLocaleSubtitle, TargetLocaleSubtitle or MetadataJson, it is required from secondary iteration.")]
    public WebvttFileKind WebvttFileKind { get; set; }

    [Option("ttsCustomLexiconFileUrl", Required = false, HelpText = "Specify whether TTS custom custom lexicon url.")]
    public Uri TtsCustomLexiconFileUrl { get; set; }

    // If use Guid?, not support by error: is defined with a bad format
    [Option("ttsCustomLexiconFileIdInAudioContentCreation", Required = false, HelpText = "Specify whether TTS custom custom lexicon file ID in Audio Content Creation.")]
    public Guid TtsCustomLexiconFileIdInAudioContentCreation { get; set; }

    [Option("enableVideoSpeedAdjustment", Required = false, HelpText = "Specify whether enable video speed adjustment.")]
    public bool EnableVideoSpeedAdjustment { get; set; }

    [Option("enableOcrCorrectionFromSubtitle", Required = false, HelpText = "Specify whether enable OCR correction from subtitle.")]
    public bool EnableOcrCorrectionFromSubtitle { get; set; }

    [Option("exportTargetLocaleAdvancedSubtitleFile", Required = false, HelpText = "Specify whether export target locale advanced subtitle file with format.")]
    public bool ExportTargetLocaleAdvancedSubtitleFile { get; set; }

    [Option("subtitlePrimaryRgbaColor", Required = false, HelpText = "Specify subtitle primary RGBA color.")]
    public string SubtitlePrimaryRgbaColor { get; set; }

    [Option("subtitleOutlineRgbaColor", Required = false, HelpText = "Specify subtitle outline RGBA color.")]
    public string SubtitleOutlineRgbaColor { get; set; }

    [Option("subtitleFontSize", Required = false, HelpText = "Specify subtitile font size between 5 ~ 30.")]
    public int SubtitleFontSize { get; set; }

    [Option("enableEmotionalPlatformVoice", Required = false, HelpText = "Specify emotional platform voice kind: Auto/Enable/Disable")]
    public EnableEmotionalPlatformVoiceKind EnableEmotionalPlatformVoice { get; set; }
}

