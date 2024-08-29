//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview;

using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib.Attributes;
using Microsoft.SpeechServices.CommonLib.CommandParser;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using System;
using System.Collections.Generic;
using System.Globalization;

[Comment("VideoTranslation tool.")]
public class Arguments<TDeploymentEnvironment>
    where TDeploymentEnvironment : System.Enum
{
    [Argument(
        "mode",
        Description = "Specifies the execute modes.",
        Optional = false,
        UsagePlaceholder = "mode",
        RequiredModes = "CreateTranslation,QueryTranslations,QueryTranslation,DeleteTranslation,CreateIteration,QueryIterations,QueryIteration,CreateConsent,QueryConsents,QueryConsent,DeleteConsent,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string modeString = string.Empty;

    [Argument(
        "apiVersion",
        Description = "Specifies the api version: 2024-05-20-preview or 2024-07-30-preview",
        Optional = true,
        UsagePlaceholder = "apiVersion",
        RequiredModes = "CreateTranslation,QueryTranslations,QueryTranslation,DeleteTranslation,CreateIteration,QueryIterations,QueryIteration,CreateConsent,QueryConsents,QueryConsent,DeleteConsent,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string apiVersion = string.Empty;

    [Argument(
        "region",
        Description = "Specifies the environment: eastus",
        Optional = false,
        UsagePlaceholder = "eastus",
        RequiredModes = "CreateTranslation,QueryTranslations,QueryTranslation,DeleteTranslation,CreateIteration,QueryIterations,QueryIteration,CreateConsent,QueryConsents,QueryConsent,DeleteConsent,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string regionString = string.Empty;

    [Argument(
        "subscriptionKey",
        Description = "Specifies speech subscription key.",
        Optional = false,
        UsagePlaceholder = "subscriptionKey",
        RequiredModes = "CreateTranslation,QueryTranslations,QueryTranslation,DeleteTranslation,CreateIteration,QueryIterations,QueryIteration,CreateConsent,QueryConsents,QueryConsent,DeleteConsent,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string subscriptionKey = string.Empty;

    [Argument(
        "translationId",
        Description = "Specifies the translation resource ID",
        Optional = true,
        UsagePlaceholder = "translationId",
        RequiredModes = "CreateTranslation,QueryTranslation,DeleteTranslation,CreateIteration,QueryIteration,CreateTranslationAndIterationAndWaitUntilTerminated,QueryIterations")]
    private string translationId = string.Empty;

    [Argument(
        "iterationId",
        Description = "Specifies the iteration resource ID",
        Optional = true,
        UsagePlaceholder = "iterationId",
        RequiredModes = "CreateIteration,QueryIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string iterationId = string.Empty;

    [Argument(
        "sourceLocale",
        Description = "Specifies the source locale e.g. en-US, zh-CN",
        Optional = true,
        UsagePlaceholder = "en-US",
        RequiredModes = "CreateTranslation,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string sourceLocaleString = string.Empty;

    [Argument(
        "targetLocale",
        Description = "Specifies the target locale e.g. en-US, zh-CN",
        Optional = true,
        UsagePlaceholder = "zh-CN",
        OptionalModes = "CreateTranslation,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string targetLocaleString = string.Empty;

    [Argument(
        "videoFileAzureBlobUrl",
        Description = "Specifies path of input video file azure blob URL.",
        Optional = true,
        UsagePlaceholder = "videoFileAzureBlobUrl",
        RequiredModes = "CreateTranslation,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string videoFileAzureBlobUrl = string.Empty;

    [Argument(
        "webvttFileAzureBlobUrl",
        Description = "Specifies path of input webvtt file azure blob URL.",
        Optional = true,
        UsagePlaceholder = "webvttFileAzureBlobUrl",
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string webvttFileAzureBlobUrl = string.Empty;

    [Argument(
        "webvttFileKind",
        Description = "Specifies webvtt file kind: MetadataJson(default), SourceLocaleSubtitle or TargetLocaleSubtitle.",
        Optional = true,
        UsagePlaceholder = "MetadataJson(default)/SourceLocaleSubtitle/TargetLocaleSubtitle",
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string webvttFileKind = string.Empty;

    [Argument(
        "voiceKind",
        Description = "Specifies TTS synthesis voice kind: PlatformVoice or PersonalVoice",
        Optional = true,
        UsagePlaceholder = "PlatformVoice(default)/PersonalVoice",
        RequiredModes = "CreateTranslation,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private string voiceKindString = string.Empty;

    [Argument(
        "subtitleMaxCharCountPerSegment",
        Description = "Subtitle max char per segment.",
        Optional = true,
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private int subtitleMaxCharCountPerSegment = 0;

    [Argument(
        "speakerCount",
        Description = "Max speaker count.",
        Optional = true,
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private int speakerCount = 0;

    [Argument(
        "enableLipSync",
        Description = "Whether enable lip sync.",
        Optional = true,
        UsagePlaceholder = "enableLipSync",
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private bool enableLipSync = false;

    [Argument(
        "exportSubtitleInVideo",
        Description = "Whether export subtitle in video.",
        Optional = true,
        UsagePlaceholder = "exportSubtitleInVideo",
        OptionalModes = "CreateIteration,CreateTranslationAndIterationAndWaitUntilTerminated")]
    private bool exportSubtitleInVideo = false;

    public Mode Mode
    {
        get
        {
            if (!Enum.TryParse<Mode>(this.modeString, true, out var mode))
            {
                throw new ArgumentException($"Invalid mode arguments.");
            }

            return mode;
        }
    }

    public string ApiVersion => this.apiVersion;

    public TDeploymentEnvironment Environment
    {
        get
        {
            if (string.IsNullOrEmpty(this.regionString))
            {
                throw new ArgumentNullException(nameof(this.regionString));
            }

            return DeploymentEnvironmentAttribute.ParseFromRegionIdentifier<TDeploymentEnvironment>(this.regionString);
        }
    }

    public string SpeechSubscriptionKey => this.subscriptionKey;

    public string TranslationId => this.translationId;

    public string IterationId => this.iterationId;

    public CultureInfo TypedSourceLocale
    {
        get
        {
            if (string.IsNullOrEmpty(this.sourceLocaleString))
            {
                return null;
            }

            return CultureInfo.CreateSpecificCulture(this.sourceLocaleString);
        }
    }

    public CultureInfo TypedTargetLocale
    {
        get
        {
            if (string.IsNullOrEmpty(this.targetLocaleString))
            {
                return null;
            }

            return CultureInfo.CreateSpecificCulture(this.targetLocaleString);
        }
    }

    public Uri TypedVideoFileAzureBlobUrl => string.IsNullOrWhiteSpace(this.videoFileAzureBlobUrl) ?
        null : new Uri(this.videoFileAzureBlobUrl);

    public Uri TypedWebvttFileAzureBlobUrl => string.IsNullOrWhiteSpace(this.webvttFileAzureBlobUrl) ?
        null : new Uri(this.webvttFileAzureBlobUrl);

    public WebvttFileKind? TypedWebvttFileKind => string.IsNullOrWhiteSpace(this.webvttFileKind) ?
            null : Enum.Parse<WebvttFileKind>(this.webvttFileKind);

    public VoiceKind? TypedVoiceKind
    {
        get
        {
            if (!string.IsNullOrEmpty(this.voiceKindString))
            {
                if (Enum.TryParse<VoiceKind>(this.voiceKindString, true, out var voiceKind))
                {
                    return voiceKind;
                }
                else
                {
                    throw new NotSupportedException(this.voiceKindString);
                }
            }

            return null;
        }
    }

    public int? SubtitleMaxCharCountPerSegment => this.subtitleMaxCharCountPerSegment == 0 ? null : this.subtitleMaxCharCountPerSegment;

    public int? SpeakerCount => this.speakerCount == 0 ? null : this.speakerCount;

    public bool? EnableLipSync => this.enableLipSync ? true : null;

    public bool? ExportSubtitleInVideo => this.exportSubtitleInVideo ? true : null;
}
