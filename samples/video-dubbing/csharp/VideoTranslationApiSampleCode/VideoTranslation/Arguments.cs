//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib.Attributes;
using Microsoft.SpeechServices.CommonLib.CommandParser;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CommonLib.Extensions;
using Microsoft.SpeechServices.VideoTranslation.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

[Comment("VideoTranslation tool.")]
public class Arguments<TDeploymentEnvironment>
    where TDeploymentEnvironment : System.Enum
{
    [Argument(
        "mode",
        Description = "Specifies the execute modes.",
        Optional = false,
        UsagePlaceholder = "mode",
        RequiredModes = "QueryMetadata,UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,QueryVideoOrAudioFiles,QueryTranslations,QueryVideoOrAudioFile,QueryTranslation,DeleteVideoOrAudioFile,DeleteTranslation,QueryTargetLocales,QueryTargetLocale,UpdateTargetLocaleEdittingWebvttFile,DeleteTargetLocale")]
    private string modeString = string.Empty;

    [Argument(
        "apiVersion",
        Description = "Specifies the api version.",
        Optional = true,
        UsagePlaceholder = "apiVersion",
        OptionalModes = "QueryMetadata,UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,QueryVideoOrAudioFiles,QueryTranslations,QueryVideoOrAudioFile,QueryTranslation,DeleteVideoOrAudioFile,DeleteTranslation,QueryTargetLocales,QueryTargetLocale,DeleteTargetLocale")]
    private string apiVersion = string.Empty;

    [Argument(
        "region",
        Description = "Specifies the environment: eastus",
        Optional = false,
        UsagePlaceholder = "eastus",
        RequiredModes = "QueryMetadata,UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,QueryVideoOrAudioFiles,QueryVideoOrAudioFile,QueryTranslations,QueryTranslation,DeleteVideoOrAudioFile,DeleteTranslation,QueryTargetLocales,QueryTargetLocale,UpdateTargetLocaleEdittingWebvttFile,DeleteTargetLocale")]
    private string regionString = string.Empty;

    [Argument(
        "subscriptionKey",
        Description = "Specifies speech subscription key.",
        Optional = false,
        UsagePlaceholder = "subscriptionKey",
        RequiredModes = "QueryMetadata,UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,QueryVideoOrAudioFiles,QueryVideoOrAudioFile,QueryTranslations,QueryTranslation,DeleteVideoOrAudioFile,DeleteTranslation,QueryTargetLocales,QueryTargetLocale,UpdateTargetLocaleEdittingWebvttFile,DeleteTargetLocale")]
    private string subscriptionKey = string.Empty;

    [Argument(
        "id",
        Description = "Specifies the Guid format id",
        Optional = true,
        UsagePlaceholder = "id",
        RequiredModes = "QueryVideoOrAudioFile,QueryTranslation,DeleteVideoOrAudioFile,DeleteTranslation")]
    private Guid id = Guid.Empty;

    [Argument(
        "sourceLocale",
        Description = "Specifies the source locale: zh-CN/en-US/ru-RU/es-ES/pl-PL",
        Optional = true,
        UsagePlaceholder = "zh-CN/en-US/ru-RU/es-ES/pl-PL",
        RequiredModes = "UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation")]
    private string sourceLocaleString = string.Empty;

    [Argument(
        "targetLocale",
        Description = "Specifies the target locale: zh-CN/en-US/ru-RU/es-ES/pl-PL",
        Optional = true,
        UsagePlaceholder = "zh-CN/en-US/ru-RU/es-ES/pl-PL",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,UpdateTargetLocaleEdittingWebvttFile,DeleteTargetLocale,QueryTargetLocale")]
    private string targetLocaleString = string.Empty;

    [Argument(
        "targetLocales",
        Description = "Specifies the target locales, for example: zh-CN,en-US,ru-RU,es-ES,pl-PL",
        Optional = true,
        UsagePlaceholder = "zh-CN,en-US,ru-RU,es-ES,pl-PL",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string targetLocalesString = string.Empty;

    [Argument(
        "videoOrAudioFileId",
        Description = "Specifies video or audio file ID.",
        Optional = true,
        UsagePlaceholder = "videoOrAudioFileId",
        RequiredModes = "CreateTranslation,UpdateTargetLocaleEdittingWebvttFile,DeleteTargetLocale,QueryTargetLocale")]
    private Guid videoOrAudioFileId = Guid.Empty;

    [Argument(
        "sourceVideoOrAudioFilePath",
        Description = "Specifies path of source video or audio file.",
        Optional = true,
        UsagePlaceholder = "sourceVideoOrAudioFilePath",
        RequiredModes = "UploadVideoOrAudioFile,UploadVideoOrAudioFileIfNotExist,UploadVideoOrAudioFileAndCreateTranslation")]
    private string sourceVideoOrAudioFilePath = string.Empty;

    [Argument(
        "webvttSourceKind",
        Description = "Specifies webvtt source kind: FileUpload(default)/TargetLocale .",
        Optional = true,
        UsagePlaceholder = "sourceLocaleWebvttFilePath",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string webvttSourceKind = string.Empty;

    [Argument(
        "sourceLocaleWebvttFilePath",
        Description = "Specifies file path of source locale webvtt.",
        Optional = true,
        UsagePlaceholder = "sourceLocaleWebvttFilePath",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string sourceLocaleWebvttFilePath = string.Empty;

    [Argument(
        "targetLocaleWebvttFilePath",
        Description = "Specifies file path of source locale webvtt.",
        Optional = true,
        UsagePlaceholder = "targetLocaleWebvttFilePath",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,UpdateTargetLocaleEdittingWebvttFile")]
    private string targetLocaleWebvttFilePath = string.Empty;

    [Argument(
        "voiceKind",
        Description = "Specifies TTS synthesis voice kind.",
        Optional = true,
        UsagePlaceholder = "PlatformVoice/PersonalVoice",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string voiceKindString = string.Empty;

    [Argument(
        "deleteAssociations",
        Description = "Delete the video file and its associated translations.",
        Optional = true,
        OptionalModes = "DeleteVideoOrAudioFile,DeleteTargetLocale")]
    private bool deleteAssociations = false;

    [Argument(
        "reuseExistingVideoOrAudioFile",
        Description = "Whether reuse existing audio.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation")]
    private bool reuseExistingVideoOrAudioFile = false;

    [Argument(
        "withoutSubtitleInTranslatedVideoFile",
        Description = "Whether without subtitle in translated video file.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private bool withoutSubtitleInTranslatedVideoFile = false;

    [Argument(
        "subtitleMaxCharCountPerSegment",
        Description = "Subtitle max char per segment.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private int subtitleMaxCharCountPerSegment = 0;

    [Argument(
        "exportPersonalVoicePromptAudioMetadata",
        Description = "Whether export personal voice prompt audio metadata.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private bool exportPersonalVoicePromptAudioMetadata = false;

    [Argument(
        "personalVoiceModelName",
        Description = "Personal voice model name.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string personalVoiceModelName = string.Empty;

    [Argument(
        "isAssociatedWithTargetLocale",
        Description = "is associated with target locale.",
        Optional = true,
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    public bool isAssociatedWithTargetLocale = false;

    [Argument(
        "additionalHttpHeaders",
        Description = "Specifies additional http headers.",
        Optional = false,
        UsagePlaceholder = "name1=value1,name2=value2",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,QueryTranslation,")]
    private string additionalHttpHeaders = string.Empty;

    [Argument(
        "enableFeatures",
        Description = "Specifies feature list to be enabled, supported features: GptTextReformulation",
        Optional = false,
        UsagePlaceholder = "GptTextReformulation,[Others]",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string enableFeatures = string.Empty;

    [Argument(
        "profileName",
        Description = "Specifies profile to use.",
        Optional = false,
        UsagePlaceholder = "GptTextReformulation,[Others]",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation")]
    private string profileName = string.Empty;

    [Argument(
        "createTranslationAdditionalProperties",
        Description = "Specifies additional properties when create translation.",
        Optional = false,
        UsagePlaceholder = "name1=value1,name2=value2",
        OptionalModes = "UploadVideoOrAudioFileAndCreateTranslation,CreateTranslation,")]
    private string createTranslationAdditionalProperties = string.Empty;

    public string EnableFeatures => this.enableFeatures;

    public string ProfileName => this.profileName;

    public string ApiVersion => this.apiVersion;

    public bool DeleteAssociations => this.deleteAssociations;

    public bool ReuseExistingVideoOrAudioFile => this.reuseExistingVideoOrAudioFile;

    public string SourceVideoOrAudioFilePath => this.sourceVideoOrAudioFilePath;

    public VideoTranslationWebvttSourceKind? TypedWebvttSourceKind =>
        string.IsNullOrWhiteSpace(this.webvttSourceKind) ? null :
            Enum.Parse<VideoTranslationWebvttSourceKind>(this.webvttSourceKind);

    public string SpeechSubscriptionKey => this.subscriptionKey;

    public string SourceLocaleWebvttFilePath => this.sourceLocaleWebvttFilePath;

    public string TargetLocaleWebvttFilePath => this.targetLocaleWebvttFilePath;

    public bool WithoutSubtitleInTranslatedVideoFile => this.withoutSubtitleInTranslatedVideoFile;

    public int? SubtitleMaxCharCountPerSegment => this.subtitleMaxCharCountPerSegment;

    public bool ExportPersonalVoicePromptAudioMetadata => this.exportPersonalVoicePromptAudioMetadata;

    public string PersonalVoiceModelName => this.personalVoiceModelName;

    public bool IsAssociatedWithTargetLocale => this.isAssociatedWithTargetLocale;

    public Guid Id => this.id;

    public Guid VideoOrAudioFileId => this.videoOrAudioFileId;

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

    public IReadOnlyDictionary<string, string> TypedAdditionalHttpHeaders =>
        this.additionalHttpHeaders.ToDictionaryWithDelimeter();

    public IReadOnlyDictionary<string, string> TypedCreateTranslationAdditionalProperties =>
        this.createTranslationAdditionalProperties.ToDictionaryWithDelimeter();

    public IEnumerable<CultureInfo> TypedTargetLocales
    {
        get
        {
            var targetLocales = new List<CultureInfo>();
            if (!string.IsNullOrEmpty(this.targetLocalesString))
            {
                foreach (var targetLocaleString in this.targetLocalesString.Split(","))
                {
                    targetLocales.Add(CultureInfo.CreateSpecificCulture(targetLocaleString));
                }
            }
            else if (!string.IsNullOrEmpty(this.targetLocaleString))
            {
                targetLocales.Add(CultureInfo.CreateSpecificCulture(targetLocaleString));
            }

            return targetLocales;
        }
    }

    public VideoTranslationVoiceKind? VoiceKind
    {
        get
        {
            if (!string.IsNullOrEmpty(this.voiceKindString))
            {
                if (Enum.TryParse<VideoTranslationVoiceKind>(this.voiceKindString, true, out var voiceKind))
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
}
