//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

// No need expose optional features list to end user.
public class VideoTranslationProfileMetadata
{
    public string Name { get; set; }

    public bool? IsPrivate { get; set; }

    public bool? IsDefault { get; set; }

    public string Description { get; set; }
}
