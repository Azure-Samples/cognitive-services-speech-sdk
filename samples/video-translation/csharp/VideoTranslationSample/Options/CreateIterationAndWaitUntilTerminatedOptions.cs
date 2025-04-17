//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using System;

[Verb("createIterationAndWaitUntilTerminated", HelpText = "Create iteration.")]
public partial class CreateIterationAndWaitUntilTerminatedOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }

    [Option("iterationId", Required = true, HelpText = "Specify iteration ID.")]
    public string IterationId { get; set; }

    [Option("iterationName", Required = false, HelpText = "Specify iteration name, if not provided, will use iterationId as iterationName")]
    public string IterationName { get; set; }

    [Option("speakerCount", Required = false, HelpText = "Specify speaker count.")]
    public int? SpeakerCount { get; set; }

    [Option("subtitleMaxCharCountPerSegment", Required = false, HelpText = "Specify subtitle max visiable char count per segment.")]
    public int? SubtitleMaxCharCountPerSegment { get; set; }

    [Option("exportSubtitleInVideo", Required = false, HelpText = "Specify speaker count.")]
    public bool? ExportSubtitleInVideo { get; set; }

    [Option("webvttFileAzureBlobUrl", Required = false, HelpText = "Specify webvtt file Azure blob URL, it is required from secondary iteration, please specify webvttFileKind as well.")]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option("webvttFileKind", Required = false, HelpText = "Specify webvtt file kind: SourceLocaleSubtitle, TargetLocaleSubtitle or MetadataJson, it is required from secondary iteration.")]
    public WebvttFileKind WebvttFileKind { get; set; }

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

