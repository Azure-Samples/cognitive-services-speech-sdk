
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <map>
#include <queue>
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

TEST_CASE("Pronunciation assessment configuration", "[api][cxx]")
{
    SPXTEST_SECTION("normal")
    {
        std::shared_ptr<PronunciationAssessmentConfig> pronConfig = PronunciationAssessmentConfig::Create("reference");
        auto jsonString = pronConfig->ToJson();
        auto j = nlohmann::json::parse(jsonString);
        SPXTEST_REQUIRE(j["referenceText"] == "reference");
        SPXTEST_REQUIRE(j["gradingSystem"] == "FivePoint");
        SPXTEST_REQUIRE(j["granularity"] == "Phoneme");
        SPXTEST_REQUIRE(j["dimension"] == "Comprehensive");
        SPXTEST_REQUIRE(!j.contains("scenarioId"));

        pronConfig = PronunciationAssessmentConfig::Create("reference",
                                                           PronunciationAssessmentGradingSystem::HundredMark,
                                                           PronunciationAssessmentGranularity::Word, true, "id");
        pronConfig->SetReferenceText("new reference");
        jsonString = pronConfig->ToJson();
        j = nlohmann::json::parse(jsonString);
        SPXTEST_REQUIRE(j["referenceText"] == "new reference");
        SPXTEST_REQUIRE(j["gradingSystem"] == "HundredMark");
        SPXTEST_REQUIRE(j["granularity"] == "Word");
        SPXTEST_REQUIRE(j["dimension"] == "Comprehensive");
        SPXTEST_REQUIRE(j["enableMiscue"] == true);
        SPXTEST_REQUIRE(j["scenarioId"] == "id");
    }

    SPXTEST_SECTION("from json")
    {
        std::string jsonString = R"( {"dimension": "Comprehensive", "enableMiscue": false, "key": "value"} )";
        auto pronConfig = PronunciationAssessmentConfig::CreateFromJson(jsonString);
        SPXTEST_REQUIRE(nlohmann::json::parse(pronConfig->ToJson()) == nlohmann::json::parse(jsonString));
    }

    SPXTEST_SECTION("invalid parameters")
    {
        REQUIRE_THROWS(PronunciationAssessmentConfig::Create("reference", static_cast<PronunciationAssessmentGradingSystem>(8)));
        REQUIRE_THROWS(
            PronunciationAssessmentConfig::Create("reference", PronunciationAssessmentGradingSystem::HundredMark,
                static_cast<PronunciationAssessmentGranularity>(8)));
        REQUIRE_THROWS(PronunciationAssessmentConfig::CreateFromJson("invalid json"));
    }
}

void TestPronunciationAssessment(const string& audio, const string& locale, bool withReferenceText, PronunciationAssessmentGranularity granularity, bool enableMiscue)
{
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(audio));
    auto pronunciationAssessmentConfig = PronunciationAssessmentConfig::Create(
        withReferenceText ? AudioUtterancesMap[audio].Utterances[locale][0].Text : "",
        PronunciationAssessmentGradingSystem::HundredMark, granularity, enableMiscue);
    auto speechConfig = CurrentSpeechConfigForPronunciationAssessment();
    speechConfig->SetSpeechRecognitionLanguage(locale);
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    pronunciationAssessmentConfig->ApplyTo(recognizer);

    SPXTEST_REQUIRE(!recognizer->Properties.GetProperty(PropertyId::PronunciationAssessment_Params).empty());

    auto result = recognizer->RecognizeOnceAsync().get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
    auto pronResult = PronunciationAssessmentResult::FromResult(result);
    if (withReferenceText && !enableMiscue)
    {
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[audio].Utterances[locale][0].Text));
    }

    SPXTEST_REQUIRE(pronResult->PronunciationScore > 0);
    SPXTEST_REQUIRE(pronResult->AccuracyScore > 0);
    SPXTEST_REQUIRE(pronResult->FluencyScore > 0);
    SPXTEST_REQUIRE(pronResult->CompletenessScore > 0);

    auto jsonString = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    auto j = nlohmann::json::parse(jsonString);
    auto x = j["NBest"][0]["PronunciationAssessment"]["PronScore"];
    SPXTEST_REQUIRE(pronResult->PronunciationScore == j["NBest"][0]["PronunciationAssessment"]["PronScore"]);
    auto word = j["NBest"][0]["Words"][0];
    if (granularity == PronunciationAssessmentGranularity::FullText)
    {
        SPXTEST_REQUIRE(!word.contains("PronunciationAssessment"));
    }
    else
    {
        SPXTEST_REQUIRE(word.contains("PronunciationAssessment"));
    }
}

TEST_CASE("Pronunciation assessment", "[api][cxx]") {
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(PRONUNCIATION_ASSESSMENT_BAD_PRONUNCIATION)));

    SPXTEST_SECTION("with reference text")
    {
        TestPronunciationAssessment(PRONUNCIATION_ASSESSMENT_BAD_PRONUNCIATION, "en-US", true, PronunciationAssessmentGranularity::Phoneme, false);
    }

    SPXTEST_SECTION("zh-cn")
    {
        TestPronunciationAssessment(PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE, "zh-CN", true, PronunciationAssessmentGranularity::Phoneme, false);
    }

    SPXTEST_SECTION("without reference text")
    {
        TestPronunciationAssessment(PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE, "zh-CN", false, PronunciationAssessmentGranularity::Phoneme, false);
    }

    SPXTEST_SECTION("enable miscue")
    {
        TestPronunciationAssessment(PRONUNCIATION_ASSESSMENT_BAD_PRONUNCIATION, "en-US", true, PronunciationAssessmentGranularity::Phoneme, true);
    }

    SPXTEST_SECTION("granularity FullText")
    {
        TestPronunciationAssessment(PRONUNCIATION_ASSESSMENT_BAD_PRONUNCIATION, "en-US", true, PronunciationAssessmentGranularity::FullText, false);
    }
}
