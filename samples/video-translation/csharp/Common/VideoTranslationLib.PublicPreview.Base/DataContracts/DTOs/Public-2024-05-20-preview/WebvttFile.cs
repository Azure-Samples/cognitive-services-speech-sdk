// <copyright file="WebvttFile.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;

public class WebvttFile
{
    public Uri Url { get; set; }

    public WebvttFileKind Kind { get; set; }

    public bool HasValue()
    {
        return !string.IsNullOrEmpty(this.Url?.OriginalString) && this.Kind != WebvttFileKind.None;
    }
}
