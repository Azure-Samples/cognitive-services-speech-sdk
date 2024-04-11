//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.Enums;

public enum VideoTranslationWebVttFilePlainTextKind
{
    None = 0,

    // User can upload webvtt with plain text format, the content of plain text is source locale.
    SourceLocalePlainText,

    // User can upload webvtt with plain text format, the content of plain text is target locale.
    TargetLocalePlainText,
}
