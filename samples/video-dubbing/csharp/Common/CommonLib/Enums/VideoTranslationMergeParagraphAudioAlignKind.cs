//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Enums;

using System;

public enum VideoTranslationMergeParagraphAudioAlignKind
{
    [Obsolete("Do not use directly - used to discover serializer issues.")]
    None = 0,

    TruncateIfExceed,

    SpeedUpIfExceed,
}
