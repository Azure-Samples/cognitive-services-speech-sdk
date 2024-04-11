//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using Microsoft.SpeechServices.Cris.Http.DTOs.Public;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;
using Microsoft.SpeechServices.DataContracts;
using System;
using System.Globalization;

// For query target locale list.
public class VideoFileTargetLocale : VideoFileTargetLocaleBrief
{
    public Uri EditingMetadataJsonWebvttFileUri { get; set; }
}
