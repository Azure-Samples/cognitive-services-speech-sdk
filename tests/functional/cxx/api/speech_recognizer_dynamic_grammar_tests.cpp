//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>
#include <thread>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "speechapi_cxx.h"

using namespace Microsoft::CognitiveServices::Speech;

std::shared_ptr<SpeechConfig> GetDynamicGrammarTestsConfig()
{
    return !Config::Endpoint.empty()
        ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech)
        : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
}

TEST_CASE("Dynamic Grammar Basics", "[api][cxx][dgi]")
{
    dgiWreckANiceBeach.UpdateFullFilename(Config::InputDir);
    REQUIRE(exists(dgiWreckANiceBeach.m_inputDataFilename));

    auto config = GetDynamicGrammarTestsConfig();
    auto audioConfig = AudioConfig::FromWavFileInput(dgiWreckANiceBeach.m_inputDataFilename);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

    auto recoTextMatches = [](auto recognizer, const std::string& utterance)
    {
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);

        SPXTEST_REQUIRE(!result->Text.empty());
        SPXTEST_REQUIRE(utterance == result->Text);
    };

    SPXTEST_SECTION("PhraseListGrammar")
    {
        auto beachHint = "Wreck a nice beach"; // DGI currently doesn't strip away punctuation
        auto correctRecoText = dgiWreckANiceBeach.m_utterance;
        auto defaultRecoText = "Recognize speech.";
        auto unrelatedPhrase1 = "This is a test of the emergency broadcast system.";
        auto unrelatedPhrase2 = "This is not the right transcript.";

        SPXTEST_GIVEN("phrase list grammar NOT used")
        {
            recoTextMatches(recognizer, defaultRecoText);
        }

        SPXTEST_GIVEN("phrase list grammar used")
        {
            auto phraselist = PhraseListGrammar::FromRecognizer(recognizer);
            SPXTEST_REQUIRE(phraselist != nullptr);

            SPXTEST_WHEN("no phrases added")
            {
                recoTextMatches(recognizer, defaultRecoText);
            }

            SPXTEST_WHEN("unrelated phrase added")
            {
                phraselist->AddPhrase(unrelatedPhrase1);
                recoTextMatches(recognizer, defaultRecoText);
            }

            SPXTEST_WHEN("multiple unrelated phrases added")
            {
                phraselist->AddPhrase(unrelatedPhrase1);
                phraselist->AddPhrase(unrelatedPhrase2);
                recoTextMatches(recognizer, defaultRecoText);
            }

            SPXTEST_WHEN("correct phrase added, but then later cleared")
            {
                phraselist->AddPhrase(beachHint);
                phraselist->Clear();
                recoTextMatches(recognizer, defaultRecoText);
            }

            SPXTEST_WHEN("correct phrase added")
            {
                phraselist->AddPhrase(beachHint);
                recoTextMatches(recognizer, correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, after unrelated phrase")
            {
                phraselist->AddPhrase(unrelatedPhrase1);
                phraselist->AddPhrase(beachHint);
                recoTextMatches(recognizer, correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, after multiple unrelated phrases")
            {
                phraselist->AddPhrase(unrelatedPhrase1);
                phraselist->AddPhrase(unrelatedPhrase2);
                phraselist->AddPhrase(beachHint);
                recoTextMatches(recognizer, correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added before unrelated phrase")
            {
                phraselist->AddPhrase(beachHint);
                phraselist->AddPhrase(unrelatedPhrase1);
                recoTextMatches(recognizer, correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, before multiple unrelated phrases")
            {
                phraselist->AddPhrase(beachHint);
                phraselist->AddPhrase(unrelatedPhrase1);
                phraselist->AddPhrase(unrelatedPhrase2);
                recoTextMatches(recognizer, correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, between multiple unrelated phrases")
            {
                phraselist->AddPhrase(unrelatedPhrase1);
                phraselist->AddPhrase(beachHint);
                phraselist->AddPhrase(unrelatedPhrase2);
                recoTextMatches(recognizer, correctRecoText);
            }
        }
    }
}
