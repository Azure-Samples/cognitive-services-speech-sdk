// <copyright file="Iteration.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using Newtonsoft.Json;

public class Iteration : StatefulResourceBase
{
    public IterationInput Input { get; set; }

    public IterationResult Result { get; set; }

    public string? FailureReason { get; set; }

    // This is different from the content editin concept in billing, billing is based on first iteration or not.
    [JsonIgnore]
    public bool IsContentEditing
    {
        get
        {
            return this.Input?.WebvttFile?.HasValue() ?? false;
        }
    }
}
