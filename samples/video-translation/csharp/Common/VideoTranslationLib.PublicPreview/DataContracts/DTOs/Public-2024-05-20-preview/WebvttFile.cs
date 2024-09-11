//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;

public class WebvttFile
{
    public Uri Url { get; set; }

    public WebvttFileKind Kind { get; set; }

    public bool HasValue()
    {
        return !string.IsNullOrEmpty(this.Url?.OriginalString) && this.Kind != WebvttFileKind.None;
    }
}
