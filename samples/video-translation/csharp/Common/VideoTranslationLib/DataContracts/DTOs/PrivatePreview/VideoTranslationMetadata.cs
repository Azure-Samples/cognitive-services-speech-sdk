//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;
using Microsoft.SpeechServices.DataContracts;
using System;
using System.Collections.Generic;
using System.Globalization;

public class VideoTranslationMetadata : ResponseBase
{
    public Dictionary<CultureInfo, VideoTranslationSourceLocaleMetadata> SupportedSourceLocales { get; set; }

    public Dictionary<CultureInfo, VideoTranslationTargetLocaleMetadata> SupportedTargetLocales { get; set; }

    // Not use enum VideoTranslationFeatureMetadata as key to avoid client breaking when server add new enum value.
    public List<VideoTranslationFeatureMetadata> SupportedFeatures { get; set; }

    public List<VideoTranslationPersonalVoiceModelMetadata> SupportedPersonalVoiceModels { get; set; }

    public List<VideoTranslationReleaseHistoryVersionMetadata> ReleaseHistoryVersions { get; set; }

    public List<VideoTranslationProfileMetadata> SubscriptionProfiles { get; set; }
}
