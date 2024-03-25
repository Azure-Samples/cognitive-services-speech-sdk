// <copyright file="TextToSpeechJob.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.Tts.BatchSynthesis.Api.TextToSpeech.DTOs;

using System.ComponentModel.DataAnnotations;
using Microsoft.Tts.BatchSynthesis.Api.Common;

public class TextToSpeechJob
{
    public string? Id { get; set; }

    public SynthesisStatus Status { get; set; }

    public DateTime CreatedDateTime { get; set; }

    public DateTime LastActionDateTime { get; set; }

    public required TextToSpeechInputKind InputKind { get; set; }

    [MinLength(1)]
    [MaxLength(10000)]
    public IReadOnlyList<TextToSpeechInput>? Inputs { get; set; }

    public IReadOnlyDictionary<string, Guid> CustomVoices { get; init; } = new Dictionary<string, Guid>();

    public TextToSpeechProperties Properties { get; set; } = new TextToSpeechProperties();

    public SynthesisConfig? SynthesisConfig { get; set; }

    public TextToSpeechOutput? Outputs { get; set; }
}
