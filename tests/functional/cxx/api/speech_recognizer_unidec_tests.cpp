//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"


TEST_CASE("Offline continuous recognition using file input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(weather.m_inputDataFilename));

    auto config = CurrentSpeechConfig();
    UseOfflineUnidec(config);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks<SpeechRecognizer, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>(recognizer.get(), result);
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition using file input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(weather.m_inputDataFilename));

    auto config = CurrentSpeechConfig();
    UseOfflineUnidec(config);

    SPXTEST_SECTION("single recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            auto result = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->Text));
        }
    }
}

TEST_CASE("Offline continuous recognition using push stream input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(weather.m_inputDataFilename));

    auto config = CurrentSpeechConfig();
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    SPXTEST_SECTION("continuous recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto result = make_shared<RecoPhrases>();

            ConnectCallbacks<SpeechRecognizer, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>(recognizer.get(), result);
            PushData(pushStream.get(), weather.m_inputDataFilename);
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition using push stream input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(weather.m_inputDataFilename));

    auto config = CurrentSpeechConfig();
    UseOfflineUnidec(config);
    auto pushStream = AudioInputStream::CreatePushStream();

    SPXTEST_SECTION("single recognition")
    {
        auto numLoops = 3;

        for (int i = 0; i < numLoops; i++)
        {
            auto audioInput = AudioConfig::FromStreamInput(pushStream);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

            PushData(pushStream.get(), weather.m_inputDataFilename);
            auto result = recognizer->RecognizeOnceAsync().get();

            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->Text));
        }
    }
}

TEST_CASE("Offline continuous recognition using pull stream input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig();
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

            ConnectCallbacks<SpeechRecognizer, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>(recognizer.get(), result);
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();

            INFO(GetText(result->phrases));
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->phrases[0].Text));
        }
    }
}

TEST_CASE("Offline single recognition using pull stream input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);

    auto config = CurrentSpeechConfig();
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
            SPXTEST_REQUIRE(VerifyText(weather.m_utterance, result->Text));
        }
    }
}
