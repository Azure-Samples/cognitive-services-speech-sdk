// <copyright file="Translation.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public class Translation<TIteration> : StatefulResourceBase
    where TIteration : Iteration
{
    public TranslationInput Input { get; set; }

    public Iteration LatestIteration { get; set; }

    public Iteration LatestSucceededIteration { get; set; }

    public string FailureReason { get; set; }
}
