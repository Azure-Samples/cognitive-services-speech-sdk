//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client;
using Microsoft.SpeechServices.CommonLib;
using System;
using System.Globalization;
using VoiceKind = Cris.Http.DTOs.Public.VideoTranslation.Public20250520.VoiceKind;

public partial class CreateTranslationBaseOptions : BaseOptions
{
    [Option("translationId", Required = false, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationId)]
    public string TranslationId { get; set; }

    public string TranslationIdOrNew
    {
        get
        {
            return string.IsNullOrWhiteSpace(this.TranslationId) ?
                Guid.NewGuid().ToString() : this.TranslationId;
        }
    }

    [Option(
        "autoCreateFirstIteration",
        Required = false,
        HelpText = "Auto create first iteration.")]
    public bool? AutoCreateFirstIteration { get; set; }

    public bool AutoCreateFirstIterationWithDefault()
    {
        // If not specified, the default value is true,
        // which means it will auto create first iteration when creating translation,
        // this is for better performance.
        return this.AutoCreateFirstIteration ?? true;
    }

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

    [Option("inputFileSourceKind", Required = false, HelpText = "Specifies the type of source for the input media file. If set to AzureStorageBlobManagedIdentity, the input media file is expected to be stored in Azure Blob Storage and accessed using a managed identity.")]
    public InputFileSourceKind InputFileSourceKind { get; set; }

    [Option("inputFileAzureStorageBlobManagedIdentityClientId", Required = false, HelpText = "provide the Azure Active Directory client ID of the managed identity for accessing Azure Blob Storage. If this client ID is not specified, the system-assigned managed identity will be used by default.")]
    public Guid InputFileAzureStorageBlobManagedIdentityClientId { get; set; }
}

