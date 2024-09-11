//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using Newtonsoft.Json;

public class Iteration<TIterationInput> : StatefulResourceBase
    where TIterationInput : IterationInput
{
    public TIterationInput Input { get; set; }

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
