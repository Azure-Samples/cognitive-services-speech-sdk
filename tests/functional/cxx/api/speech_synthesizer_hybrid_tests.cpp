//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <memory.h>
#include "test_utils.h"
#include "file_utils.h"
#include "synthesizer_utils.h"
#include "speechapi_cxx_speech_synthesizer.h"
#include "speechapi_cxx_speech_synthesis_result.h"
#include "speechapi_cxx_audio_config.h"

using namespace TTS;

static constexpr char font_token_file[] = "Tokens.xml";

std::shared_ptr<SpeechConfig> OfflineConfig()
{
    auto config = SpeechConfig::FromHost("wss://fake.com");
    config->SetProperty("SPEECH-SynthBackend", "offline");
    config->SetProperty("SPEECH-SynthOfflineDataLocation", GetOfflineVoicePath());
    return config;
}

std::shared_ptr<SpeechConfig> HybridConfig()
{
    auto config = UspSpeechConfig();
    config->SetProperty("SPEECH-SynthBackend", "hybrid");
    config->SetProperty("SPEECH-SynthOfflineDataLocation", GetOfflineVoicePath());
    config->SetProperty("SPEECH-SynthBackendSwitchingPolicy", "auto");
    return config;
}

SPXTEST_CASE_BEGIN("Offline Synthesis", "[api][cxx][hybrid_tts]")
{

    SPXTEST_REQUIRE(exists(GetOfflineVoicePath() + "/" + font_token_file));

    auto synthesizer = SpeechSynthesizer::FromConfig(OfflineConfig(), nullptr);

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e)
    {
        SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
    };

    synthesizer->SynthesisCompleted += [](const SpeechSynthesisEventArgs& e)
    {
        SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
    };

    SPXTEST_SECTION("Normal")
    {
        for (auto& utterance : AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"])
        {
            auto result = synthesizer->SpeakTextAsync(utterance.Text).get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioCompleted);
            auto data = ReadWavFile(DefaultSettingsMap[INPUT_DIR] + "/" + utterance.OfflineSynthesizedFilePath);
#ifdef _WIN32 // the generated audio is not binary equal on different platform.
            SPXTEST_REQUIRE(AreBinaryEqual(result->GetAudioData(), data));
#else
            SPXTEST_REQUIRE(result->GetAudioLength() > data.size() / 2);
#endif
            SPXTEST_REQUIRE(result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
        }
    }

    SPXTEST_SECTION("Synthesis using SSML")
    {
        for (auto& utterance : AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"])
        {
            auto result = synthesizer->SpeakSsmlAsync(utterance.Ssml).get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioCompleted);
            auto data = ReadWavFile(DefaultSettingsMap[INPUT_DIR] + "/" + utterance.OfflineSynthesizedFilePath);
#ifdef _WIN32 // the generated audio is not binary equal on different platform.
            SPXTEST_REQUIRE(AreBinaryEqual(result->GetAudioData(), data));
#else
            SPXTEST_REQUIRE(result->GetAudioLength() > data.size() / 2);
#endif
            SPXTEST_REQUIRE(result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
        }
    }

#ifdef __linux__ // TTS offline engine is only full-tested on linux, as there is no customer using Windows.
    SPXTEST_SECTION("Stop")
    {
        auto longText = AudioUtterancesMap[SYNTHESIS_LONG_UTTERANCE].Utterances["en-US"][0].Text;

        // lengthen the text, to ensure the stopping works.
        longText += longText;
        longText += longText;

        std::future<void> stopFuture;
        bool stopped = false;
        synthesizer->Synthesizing += [&synthesizer, &stopFuture, &stopped](const SpeechSynthesisEventArgs& e) {
            SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
            UNUSED(e);
            if (!stopped)
            {
                stopFuture = synthesizer->StopSpeakingAsync();
            }

            stopped = true;
        };

        auto result = synthesizer->SpeakTextAsync(longText).get();
        SPXTEST_REQUIRE(result->GetAudioLength() > 0);

        synthesizer->Synthesizing.DisconnectAll();
        // ensure stop() in finished.
        stopFuture.get();
        auto result2 = synthesizer->SpeakTextAsync(longText).get();
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
        SPXTEST_REQUIRE(result->GetAudioLength() < result2->GetAudioLength());
    }
#endif

    SPXTEST_SECTION("Invalid location")
    {
        auto config = OfflineConfig();
        config->SetProperty("SPEECH-SynthOfflineDataLocation", "invalid");

        auto invalidSynthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

        auto result = invalidSynthesizer->SpeakTextAsync(AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text).get();
        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
        auto cancellationDetail = SpeechSynthesisCancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellationDetail->ErrorDetails.find("Local TTS engine not initialized") != std::string::npos);
    }
}SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN("Hybrid Synthesis", "[api][cxx][hybrid_tts]")
{
    SPXTEST_REQUIRE(exists(GetOfflineVoicePath() + "/" + font_token_file));

    SPXTEST_SECTION("Normal")
    {
        auto synthesizer = SpeechSynthesizer::FromConfig(HybridConfig(), nullptr);
        for (auto& utterance : AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"])
        {
            auto result = synthesizer->SpeakTextAsync(utterance.Text).get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioCompleted);
            SPXTEST_REQUIRE(result->GetAudioLength() > 0);
        }
    }

    SPXTEST_SECTION("Switching policy")
    {
        auto synthesizer = SpeechSynthesizer::FromConfig(HybridConfig(), nullptr);

        std::string backend;

        synthesizer->Synthesizing += [&backend](const SpeechSynthesisEventArgs& e)
        {
            SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == backend);
        };

        synthesizer->SynthesisCompleted += [&backend](const SpeechSynthesisEventArgs& e)
        {
            SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == backend);
        };

        synthesizer->Properties.SetProperty("SPEECH-SynthBackendSwitchingPolicy", "force_cloud");
        backend = "";
        auto result1 = synthesizer->SpeakTextAsync(AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text).get();
        SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result1->GetAudioLength() > 0);
        SPXTEST_REQUIRE(result1->Properties.GetProperty("SynthesisFinishedBy").empty());

        synthesizer->Properties.SetProperty("SPEECH-SynthBackendSwitchingPolicy", "force_offline");
        backend = "offline";
        auto result2 = synthesizer->SpeakTextAsync(AudioUtterancesMap[SYNTHESIS_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text).get();
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
        SPXTEST_REQUIRE(result2->Properties.GetProperty("SynthesisFinishedBy") == "offline");
        SPXTEST_REQUIRE(!AreBinaryEqual(result1->GetAudioData(), result2->GetAudioData()));
    }
}SPXTEST_CASE_END()
