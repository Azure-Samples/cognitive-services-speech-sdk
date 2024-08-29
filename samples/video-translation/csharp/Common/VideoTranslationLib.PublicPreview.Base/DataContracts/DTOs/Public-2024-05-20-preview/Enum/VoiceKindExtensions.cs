// <copyright file="VoiceKindExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using Microsoft.SpeechServices.Common.Client;

public static class VoiceKindExtensions
{
    public static VideoTranslationVoiceKind AsCoreEngineEnum(this VoiceKind voiceKind)
    {
        return voiceKind switch
        {
            VoiceKind.PlatformVoice => VideoTranslationVoiceKind.PlatformVoice,
            VoiceKind.PersonalVoice => VideoTranslationVoiceKind.PersonalVoice,
            _ => VideoTranslationVoiceKind.PlatformVoice,
        };
    }
}
