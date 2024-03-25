// <copyright file="TextToSpeechOutput.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.Tts.BatchSynthesis.Api.TextToSpeech.DTOs;

public class TextToSpeechOutput
{
    public string Result { get; init; }

    public string Summary { get; init; }
}

public class TextToSpeechBillingDetails
{
    public long NeuralCharacters { get; init; }

    public long CustomNeuralCharacters { get; init; }

    public long AoaiCharacters { get; init; }

    public long AoaiHDCharacters { get; init; }

    public long PersonalVoiceCharacters { get; init; }
}