// <copyright file="SynthesisConfig.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

public class SynthesisConfig
{
    public required string Voice { get; init; }

    public string? SpeakerProfileId { get; init; }

    public string? Role { get; init; }

    public string? Style { get; init; }

    public string? StyleDegree { get; init; }

    public string? Rate { get; init; }

    public string? Pitch { get; init; }

    public string? Volume { get; init; }

    public BackgroundAudioConfig? BackgroundAudio { get; init; }
}

public class BackgroundAudioConfig
{
    public required Uri Src { get; init; }

    public long? Fadein { get; init; }

    public long? Fadeout { get; init; }

    public double? Volume { get; init; }
}