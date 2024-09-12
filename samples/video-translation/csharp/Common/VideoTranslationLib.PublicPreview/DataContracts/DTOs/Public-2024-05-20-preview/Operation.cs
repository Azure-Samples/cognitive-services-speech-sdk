//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System.ComponentModel.DataAnnotations;

public class Operation
{
    [Required]
    [RegularExpression(@"^[a-zA-Z0-9][a-zA-Z0-9._-]{1,62}[a-zA-Z0-9]$")]
    public string Id { get; set; }

    public OperationStatus Status { get; set; }
}
