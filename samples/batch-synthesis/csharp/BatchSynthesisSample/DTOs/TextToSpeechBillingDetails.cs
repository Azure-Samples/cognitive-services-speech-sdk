// <copyright file="TextToSpeechBillingDetails.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.Tts.BatchSynthesis.Api.TextToSpeech.DTOs;

using System.Text.Json.Serialization;

public class TextToSpeechBillingDetails
{
    public long NeuralCharacters { get; init; }

    public long CustomNeuralCharacters { get; init; }

    public long AoaiCharacters { get; init; }

    public long AoaiHDCharacters { get; init; }

    public long PersonalVoiceCharacters { get; init; }
}
