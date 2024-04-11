//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;

using System;
using System.Globalization;

// For query target locale list.
public class VideoFileTargetLocaleBrief : StatefulResourceBase
{
    public CultureInfo SourceLocale { get; set; }

    public CultureInfo TargetLocale { get; set; }

    public Guid VideoFileId { get; set; }

    public Guid? LatestTranslationId { get; set; }

    public VideoFileTargetLocaleBriefPortalProperties PortalProperties { get; set; }
}
