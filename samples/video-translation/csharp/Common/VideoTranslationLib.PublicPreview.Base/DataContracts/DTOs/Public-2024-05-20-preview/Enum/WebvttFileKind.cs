// <copyright file="WebvttFileKind.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public enum WebvttFileKind
{
    None = 0,

    SourceLocaleSubtitle,

    TargetLocaleSubtitle,

    MetadataJson,
}
