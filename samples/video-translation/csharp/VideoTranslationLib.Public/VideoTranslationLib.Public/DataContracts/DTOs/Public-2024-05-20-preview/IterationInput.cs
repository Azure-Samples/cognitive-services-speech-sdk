//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

public partial class IterationInput
{
    public int? SpeakerCount { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportSubtitleInVideo { get; set; }

    public WebvttFile WebvttFile { get; set; }
}
