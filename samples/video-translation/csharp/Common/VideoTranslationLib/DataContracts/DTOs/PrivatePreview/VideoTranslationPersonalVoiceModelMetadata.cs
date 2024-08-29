//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

public class VideoTranslationPersonalVoiceModelMetadata
{
    public string ModelName { get; set; }

    public bool? IsDefault { get; set; }

    public string Description { get; set; }
}
