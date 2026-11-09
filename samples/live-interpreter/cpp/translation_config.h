// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

#pragma once

#include <speechapi_cxx.h>

namespace live_interpreter
{
inline void ConfigureTranslation(
    Microsoft::CognitiveServices::Speech::Translation::SpeechTranslationConfig& config)
{
    config.AddTargetLanguage("fr");
    config.SetVoiceName("personal-voice");
    config.SetSpeechSynthesisOutputFormat(
        Microsoft::CognitiveServices::Speech::SpeechSynthesisOutputFormat::Riff16Khz16BitMonoPcm);
}
}
