//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public partial class Iteration : StatefulResourceBase
{
    public IterationInput Input { get; set; }

    public IterationResult Result { get; set; }

    public string FailureReason { get; set; }
}
