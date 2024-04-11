//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public;

using System;

public abstract class StatelessResourceBase
{
    public string Id { get; set; }

    public string DisplayName { get; set; }

    public string Description { get; set; }

    public DateTime CreatedDateTime { get; set; }
}
