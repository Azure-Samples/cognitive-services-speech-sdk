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

    auto getRecognizer = []()
    {
        auto config = GetDynamicGrammarTestsConfig();
        auto audioConfig = AudioConfig::FromWavFileInput(dgiWreckANiceBeach.m_inputDataFilename);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);
        return recognizer;
    };
    
    auto recoTextMatches = [](auto getRecognizer, auto modifyPhraseList, const std::string& utterance)
    {
        auto triedOnce = false;

        while (true)
        {
            auto recognizer = getRecognizer();

            auto phraselist = PhraseListGrammar::FromRecognizer(recognizer);
            SPXTEST_REQUIRE(phraselist != nullptr);

            modifyPhraseList(phraselist);

            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);

            SPXTEST_REQUIRE(!result->Text.empty());

            if (utterance == result->Text || !triedOnce)
            {
                SPXTEST_REQUIRE(utterance == result->Text);
                break;
            }

            SPX_TRACE_INFO("Utterance failed to match,'%s' != '%s' trying again. %s(%d)", utterance.c_str(), result->Text.c_str(), __FILE__, __LINE__);
            triedOnce = true;
        }
    };

    SPXTEST_SECTION("PhraseListGrammar")
    {
        auto beachHint = "Wreck a nice beach"; // DGI currently doesn't strip away punctuation
        auto correctRecoText = dgiWreckANiceBeach.m_utterance;
        auto defaultRecoText = "Recognize speech.";
        auto unrelatedPhrase1 = "This is a test of the emergency broadcast system.";
        auto unrelatedPhrase2 = "This is not the right transcript.";

        SPXTEST_GIVEN("phrase list grammar used")
        { 
           
            SPXTEST_WHEN("no phrases added")
            {
                recoTextMatches(
                    getRecognizer,
                    [](auto phraselist) 
                    {
                       if(phraselist){}
                    },
                    defaultRecoText);
            }

            SPXTEST_WHEN("unrelated phrase added")
            {
                recoTextMatches(
                    getRecognizer,
                    [unrelatedPhrase1](auto phraselist)
                    {
                        phraselist->AddPhrase(unrelatedPhrase1);
                    },
                    defaultRecoText);
            }

            SPXTEST_WHEN("multiple unrelated phrases added")
            {
                recoTextMatches(
                    getRecognizer,
                    [unrelatedPhrase1, unrelatedPhrase2](auto phraselist)
                    {
                        phraselist->AddPhrase(unrelatedPhrase1);
                        phraselist->AddPhrase(unrelatedPhrase2);
                    },
                    defaultRecoText);
            }

            SPXTEST_WHEN("correct phrase added, but then later cleared")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint](auto phraselist)
                    {
                        phraselist->AddPhrase(beachHint);
                        phraselist->Clear();
                    },
                    defaultRecoText);
            }

            SPXTEST_WHEN("correct phrase added")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint](auto phraselist)
                    {
                        phraselist->AddPhrase(beachHint);
                    },
                    correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, after unrelated phrase")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint, unrelatedPhrase1](auto phraselist)
                    {
                        phraselist->AddPhrase(unrelatedPhrase1);
                        phraselist->AddPhrase(beachHint);
                    },
                    correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, after multiple unrelated phrases")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint, unrelatedPhrase1, unrelatedPhrase2](auto phraselist)
                    {
                        phraselist->AddPhrase(unrelatedPhrase1);
                        phraselist->AddPhrase(unrelatedPhrase2);
                        phraselist->AddPhrase(beachHint);
                    },
                    correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added before unrelated phrase")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint, unrelatedPhrase1](auto phraselist)
                    {
                        phraselist->AddPhrase(beachHint);
                        phraselist->AddPhrase(unrelatedPhrase1);
                    },
                    correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, before multiple unrelated phrases")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint, unrelatedPhrase1, unrelatedPhrase2](auto phraselist)
                    {
                        phraselist->AddPhrase(beachHint);
                        phraselist->AddPhrase(unrelatedPhrase1);
                        phraselist->AddPhrase(unrelatedPhrase2);
                    },
                    correctRecoText);
            }

            SPXTEST_WHEN("correct phrase added, between multiple unrelated phrases")
            {
                recoTextMatches(
                    getRecognizer,
                    [beachHint, unrelatedPhrase1, unrelatedPhrase2](auto phraselist)
                    {
                        phraselist->AddPhrase(unrelatedPhrase1);
                        phraselist->AddPhrase(beachHint);
                        phraselist->AddPhrase(unrelatedPhrase2);
                    },
                    correctRecoText);
            }
        }
    }
}
