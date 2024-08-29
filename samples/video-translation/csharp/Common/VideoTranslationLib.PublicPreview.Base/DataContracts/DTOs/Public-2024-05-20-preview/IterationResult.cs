// <copyright file="IterationResult.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;

public class IterationResult
{
    public Uri TranslatedVideoFileUrl { get; set; }

    public Uri SourceLocaleSubtitleWebvttFileUrl { get; set; }

    public Uri TargetLocaleSubtitleWebvttFileUrl { get; set; }

    public Uri MetadataJsonWebvttFileUrl { get; set; }
}
