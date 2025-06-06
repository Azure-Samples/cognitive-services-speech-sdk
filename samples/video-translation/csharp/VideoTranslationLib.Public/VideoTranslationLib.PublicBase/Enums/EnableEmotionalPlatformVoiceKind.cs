// <copyright file="EnableEmotionalPlatformVoiceKind.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;

public enum EnableEmotionalPlatformVoiceKind
{
    // Why use enum? Because null is not false, different from other default behavior.
    // Behavior is different
    // Default value: en-US is true, other false
    Auto = 0,

    Enable,

    Disable,
}
