//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;

using Microsoft.SpeechServices.Common.Client;
using System;
using System.Globalization;

public partial class TranslationInput
{
    public CultureInfo SourceLocale { get; set; }

    public CultureInfo TargetLocale { get; set; }

    public VoiceKind VoiceKind { get; set; }

    public bool? AutoCreateFirstIteration { get; set; }

    public int? SpeakerCount { get; set; }

    public int? SubtitleMaxCharCountPerSegment { get; set; }

    public bool? ExportSubtitleInVideo { get; set; }

    public Uri VideoFileUrl { get; set; }

    public Uri AudioFileUrl { get; set; }

    public bool? EnableLipSync { get; set; }

    public InputFileSourceKind? InputFileSourceKind { get; set; }

    public Guid? InputFileAzureStorageBlobManagedIdentityClientId { get; set; }
}
