//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using System;

public class TranslationTargetLocale : TranslationTargetLocaleBase
{
    public Uri InputWebVttFileUrl { get; set; }

    public Uri OutputVideoSubtitleWebVttFileUrl { get; set; }

    public Uri OutputMetadataJsonWebVttFileUrl { get; set; }

    public Uri OutputVideoFileUrl { get; set; }

    public Uri Output24k16bitRiffAudioFileUrl { get; set; }
}
