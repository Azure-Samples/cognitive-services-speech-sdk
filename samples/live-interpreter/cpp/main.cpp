// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md in the project root.

#include "sample_helpers.h"
#include "translation_config.h"

#include <iostream>
#include <memory>
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Translation;

namespace
{
using live_interpreter::OnEvent;
using live_interpreter::SessionState;

int Run()
{
    const auto key = live_interpreter::RequiredEnvironment("SPEECH_KEY");
    const auto endpoint = live_interpreter::RequiredEnvironment("SPEECH_ENDPOINT");
    const auto input = live_interpreter::RequiredEnvironment("LIVE_INTERPRETER_AUDIO_FILE");
    live_interpreter::ValidateWavFile(input);

    // Pass the universal v2 URL unchanged, including any query parameters.
    auto config = SpeechTranslationConfig::FromEndpoint(endpoint, key);
    live_interpreter::ConfigureTranslation(*config);
    auto language = AutoDetectSourceLanguageConfig::FromOpenRange();
    auto audio = AudioConfig::FromWavFileInput(input);
    auto recognizer = TranslationRecognizer::FromConfig(config, language, audio);
    auto state = std::make_shared<SessionState>();

    recognizer->Recognized.Connect([state](const TranslationRecognitionEventArgs& event) {
        OnEvent(state, [&] {
            if (event.Result->Reason == ResultReason::NoMatch)
            {
                std::cout << "NOMATCH: Speech could not be recognized.\n";
                return;
            }
            if (event.Result->Reason != ResultReason::TranslatedSpeech &&
                event.Result->Reason != ResultReason::RecognizedSpeech)
                throw std::runtime_error("Unexpected recognition result: " +
                    std::to_string(static_cast<int>(event.Result->Reason)));
            const auto detected = event.Result->Properties.GetProperty(
                PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult);
            std::cout << "RECOGNIZED in '" << detected << "': " << event.Result->Text << '\n';
            for (const auto& translation : event.Result->Translations)
                std::cout << "  TRANSLATED into '" << translation.first << "': " << translation.second << '\n';
        });
    });

    recognizer->Synthesizing.Connect([state](const TranslationSynthesisEventArgs& event) {
        OnEvent(state, [&] {
            if (event.Result->Reason == ResultReason::Canceled)
                throw std::runtime_error("Translation synthesis was canceled.");
            const auto& bytes = event.Result->Audio;
            // An empty chunk ends a phrase, not the recognition session.
            if (bytes.empty() || state->failed)
                return;
            const auto output = "translation-" + std::to_string(++state->audioIndex) + ".wav";
            live_interpreter::WriteNewFile(output, bytes);
            std::cout << "AUDIO: " << output << " (" << bytes.size() << " bytes)\n";
        });
    });

    recognizer->Canceled.Connect([state](const TranslationRecognitionCanceledEventArgs& event) {
        OnEvent(state, [&] {
            std::cout << "CANCELED: Reason=" << static_cast<int>(event.Reason) << '\n';
            // EndOfStream is normal for WAV input. All other cancellations fail.
            if (event.Reason != CancellationReason::EndOfStream)
            {
                std::cerr << "CANCELED: ErrorCode=" << static_cast<int>(event.ErrorCode)
                          << " Details=" << event.ErrorDetails << '\n';
                state->failed = true;
            }
            state->done = true;
        });
    });

    recognizer->SessionStopped.Connect([state](const SessionEventArgs&) {
        OnEvent(state, [&] {
            std::cout << "Session stopped.\n";
            state->done = true;
        });
    });

    std::cout << "Start translation to fr...\n";
    recognizer->StartContinuousRecognitionAsync().get();
    {
        std::unique_lock<std::mutex> lock(state->mutex);
        state->changed.wait(lock, [&] { return state->done; });
    }
    // Never call Stop or destroy the recognizer from an SDK callback.
    recognizer->StopContinuousRecognitionAsync().get();
    recognizer->Recognized.DisconnectAll();
    recognizer->Synthesizing.DisconnectAll();
    recognizer->Canceled.DisconnectAll();
    recognizer->SessionStopped.DisconnectAll();
    recognizer.reset();
    std::lock_guard<std::mutex> lock(state->mutex);
    return state->failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
}

int main()
{
    try
    {
        live_interpreter::CompletionDeadline deadline;
        return Run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Live Interpreter failed: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
