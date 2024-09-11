//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public class Translation<TIteration, TIterationInput> : StatefulResourceBase
    where TIteration : Iteration<TIterationInput>
    where TIterationInput : IterationInput
{
    public TranslationInput Input { get; set; }

    public Iteration<TIterationInput> LatestIteration { get; set; }

    public Iteration<TIterationInput> LatestSucceededIteration { get; set; }

    public string FailureReason { get; set; }
}
