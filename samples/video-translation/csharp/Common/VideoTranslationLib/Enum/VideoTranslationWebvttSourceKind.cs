//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.Enums;

public enum VideoTranslationWebvttSourceKind
{
    None = 0,

    // In this kind, NOT associate with target locale with current translation.
    FileUpload,

    // 1. In this kind, will associate with target local with current translation.
    // 2. If no user editting file, translate without webvtt file instead of response error.
    TargetLocale,
}
