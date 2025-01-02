//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using System;

public class CreateTranslationAndIterationAndWaitUntilTerminatedBaseOptions : CreateTranslationBaseOptions
{
    [Option('t', "translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }

    [Option('i', "iterationId", Required = true, HelpText = "Specify iteration ID.")]
    public string IterationId { get; set; }

    [Option('w', "webvttFileAzureBlobUrl", Required = false, HelpText = "Specify webvtt file Azure blob URL, it is required from secondary iteration.")]
    public Uri WebvttFileAzureBlobUrl { get; set; }

    [Option('k', "webvttFileKind", Required = false, HelpText = "Specify webvtt file kind: SourceLocaleSubtitle, TargetLocaleSubtitle or MetadataJson, it is required from secondary iteration.")]
    public WebvttFileKind? WebvttFileKind { get; set; }
}

