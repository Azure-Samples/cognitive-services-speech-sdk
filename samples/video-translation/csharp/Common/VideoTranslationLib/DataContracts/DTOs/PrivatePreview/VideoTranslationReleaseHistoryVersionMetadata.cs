//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using System;

public class VideoTranslationReleaseHistoryVersionMetadata
{
    public string Version { get; set; }

    public DateTime Date { get; set; }

    public string Description { get; set; }
}
