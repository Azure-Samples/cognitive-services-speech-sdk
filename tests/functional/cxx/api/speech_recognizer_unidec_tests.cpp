//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

TEST_CASE("Offline continuous recognition using file input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

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

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

// Returns future_status::timeout on Linux-arm64.
// Bug 2003392 Offline Unidec continuous reco from file fails on Linux-arm64
TEST_CASE("Offline continuous recognition using file input fails", "[.][api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_3X)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numUtterances = 3u;
        auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_3X));
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
        auto result = make_shared<RecoPhrases>();

        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();

        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();

        INFO(GetText(result->phrases));
        SPXTEST_REQUIRE(result->phrases.size() == numUtterances);
        for (auto i = 0u; i < result->phrases.size(); i++)
        {
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_3X].Utterances["en-US"][0].Text, result->phrases[i].Text));
        }
    }
}

TEST_CASE("Offline single recognition using file input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

    SPXTEST_SECTION("single recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            auto result = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->Text));
        }
    }
}

TEST_CASE("Offline continuous recognition using push stream input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

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

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

// Returns future_status::timeout or empty phrase list on the second round.
// Bug 2003412 Offline Unidec continuous reco from push stream fails
TEST_CASE("Offline continuous recognition using push stream input fails", "[.][api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 2;

        for (int i = 0; i < numLoops; i++)
        {
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks(recognizer.get(), result);
            PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition using push stream input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();

    SPXTEST_SECTION("single recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromStreamInput(pushStream);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            auto result = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->Text));
        }
    }
}

TEST_CASE("Offline continuous recognition using pull stream input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

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

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition using pull stream input", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);

    SPXTEST_SECTION("single recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            std::shared_ptr<PullAudioInputStream> pullStream;
            auto audioInput = CreateAudioPullSingleChannel(pullStream);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            auto result = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text, result->Text));
        }
    }
}

TEST_CASE("Offline recognition with KWS", "[api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");

    SPXTEST_SECTION("detect and recognize twice")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1)));

        for (int i = 0; i < 2; i++)
        {
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
            auto result = make_shared<RecoPhrases>();
            ConnectCallbacks(recognizer.get(), result);
            PushData(pushStream.get(), ROOT_RELATIVE_PATH(COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1));

            recognizer->StartKeywordRecognitionAsync(model).get();
            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopKeywordRecognitionAsync().get();

            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}

// Generates SIGSEGV on Linux-arm64.
// Bug 2003421 Offline Unidec recognition with KWS generates SIGSEGV on Linux-arm64
TEST_CASE("Offline recognition with KWS fails", "[.][api][cxx][unidec]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig(SpxGetTestTrafficType(__FILE__, __LINE__));
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
    auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");

    SPXTEST_SECTION("detect and recognize twice")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2X)));
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2X));

        for (int i = 0; i < 2; i++)
        {
            auto result = make_shared<RecoPhrases>();
            ConnectCallbacks(recognizer.get(), result);

            recognizer->StartKeywordRecognitionAsync(model).get();
            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopKeywordRecognitionAsync().get();

            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(MatchText(AudioUtterancesMap[COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_2X].Utterances["en-US"][0].Text, result->phrases[0].Text));
        }
    }
}
