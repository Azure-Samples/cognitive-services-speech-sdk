// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

#include "translation_config.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;

int main()
{
    try
    {
        auto config = SpeechTranslationConfig::FromEndpoint(
            "wss://example.invalid/stt/speech/universal/v2?setfeature=llm");
        config->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Riff8Khz16BitMonoPcm);
        live_interpreter::ConfigureTranslation(*config);
        if (config->GetSpeechSynthesisOutputFormat() != "riff-16khz-16bit-mono-pcm")
            throw std::runtime_error("Translation configuration must explicitly request riff-16khz-16bit-mono-pcm.");
        if (config->GetVoiceName() != "personal-voice" ||
            config->GetTargetLanguages() != std::vector<std::string>{"fr"})
            throw std::runtime_error("Translation target or voice changed.");
        std::cout << "SDK output format: " << config->GetSpeechSynthesisOutputFormat() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
