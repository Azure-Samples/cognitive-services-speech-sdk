//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;

public partial class IterationResult
{
    public Uri TranslatedVideoFileUrl { get; set; }

    public Uri SourceLocaleSubtitleWebvttFileUrl { get; set; }

    public Uri TargetLocaleSubtitleWebvttFileUrl { get; set; }

    public Uri MetadataJsonWebvttFileUrl { get; set; }
}
