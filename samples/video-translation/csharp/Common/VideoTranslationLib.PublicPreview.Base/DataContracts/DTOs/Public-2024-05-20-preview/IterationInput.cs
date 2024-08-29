// <copyright file="IterationInput.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public class IterationInput
{
    public int? SpeakerCount { get; set; }

    public bool? EnableLipSync { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportSubtitleInVideo { get; set; }

    public WebvttFile WebvttFile { get; set; }
}
