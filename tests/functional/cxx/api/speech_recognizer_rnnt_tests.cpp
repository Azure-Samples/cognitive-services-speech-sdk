//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

TEST_CASE("Offline continuous recognition with RNN-T using file input", "[api][cxx][rnnt]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig();
    UseOfflineRnnt(config);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks(recognizer.get(), result);
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), 10 * WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline continuous recognition with RNN-T using push stream input", "[api][cxx][rnnt]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig();
    UseOfflineRnnt(config);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto pushStream = AudioInputStream::CreatePushStream();
            auto audioInput = AudioConfig::FromStreamInput(pushStream);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks(recognizer.get(), result);
            PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), 10 * WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline continuous recognition with RNN-T using pull stream input", "[api][cxx][rnnt]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig();
    UseOfflineRnnt(config);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            std::shared_ptr<PullAudioInputStream> pullStream;
            auto audioInput = CreateAudioPullSingleChannel(pullStream);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks(recognizer.get(), result);
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), 10 * WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition with RNN-T and offline TTS", "[api][cxx][rnnt][hybrid_tts]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig();
    UseOfflineRnnt(config);
    // Enable also offline TTS
    config->SetProperty("SPEECH-SynthBackend", "offline");
    config->SetProperty("SPEECH-SynthOfflineDataLocation", Config::OfflineVoicePath);

    SPXTEST_SECTION("single recognition and synthesis")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            auto resultReco = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(resultReco->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, resultReco->Text));

            // Run TTS on the recognition result text
            auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

            synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e)
            {
                SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
            };

            synthesizer->SynthesisCompleted += [](const SpeechSynthesisEventArgs& e)
            {
                SPXTEST_REQUIRE(e.Result->Properties.GetProperty("SynthesisFinishedBy") == "offline");
            };

            auto resultSynth = synthesizer->SpeakTextAsync(resultReco->Text).get();
            SPXTEST_REQUIRE(resultSynth->Reason == ResultReason::SynthesizingAudioCompleted);
            SPXTEST_REQUIRE(resultSynth->Properties.GetProperty("SynthesisFinishedBy") == "offline");
        }
    }
}
