// <copyright file="TextToSpeechProperties.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.Tts.BatchSynthesis.Api.TextToSpeech.DTOs;

using System.ComponentModel.DataAnnotations;

public class TextToSpeechProperties
{
    // User provided properties
    [Range(1, 744)]
    public int TimeToLiveInHours { get; set; } = 744;

    public string? OutputFormat { get; set; } = "riff-24khz-16bit-mono-pcm";

    public bool? ConcatenateResult { get; set; } = false;

    public bool? DecompressOutputFiles { get; set; } = false;

    public bool? WordBoundaryEnabled { get; set; } = false;

    public bool? SentenceBoundaryEnabled { get; set; } = false;

    public Uri DestinationContainerUrl { get; set; }

    public string DestinationPath { get; set; }

    // Readonly properties
    public long? SizeInBytes { get; init; }

    public int? SucceededAudioCount { get; init; }

    public int? FailedAudioCount { get; init; }

    public long? DurationInMilliseconds { get; init; }

    public TextToSpeechBillingDetails? BillingDetails { get; init; }

    public SynthesisError? Error { get; init; }
}
