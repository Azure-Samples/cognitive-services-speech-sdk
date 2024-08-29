//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using System;
using System.Collections.Generic;
using System.Globalization;

public class Translation : TranslationBase
{
    public Uri InputWebVttFileUrl { get; set; }

    public Uri OutputVideoSubtitleWebVttFileUrl { get; set; }

    public Uri ReportFileUrl { get; set; }

    public Uri IntermediateZipFileUrl { get; set; }

    public Uri CacheZipFileUrl { get; set; }

    public IReadOnlyDictionary<CultureInfo, TranslationTargetLocale> TargetLocales { get; set; }
}
