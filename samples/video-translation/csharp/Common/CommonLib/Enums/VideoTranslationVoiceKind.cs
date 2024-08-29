//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Microsoft.SpeechServices.Common.Client;

public enum VideoTranslationVoiceKind
{
    [Obsolete("Do not use directly - used to discover serializer issues.")]
    None = 0,

    PlatformVoice,

    PersonalVoice,

    ZeroShot,
}
