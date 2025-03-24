//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <speechapi_cxx.h> // from Speech SDK
#include <intentapi_cxx.h> // from this project

#define CATCH_CONFIG_WINDOWS_CRTDBG
#include "catch2/catch_amalgamated.hpp"

#include "samples.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::SpeechSDK::Standalone::Intent;


std::string SubscriptionKey;
std::string ServiceRegion;

const std::string GetEnvVariable(const char* envVariableName)
{
#pragma warning(suppress : 4996) // getenv
    auto value = getenv(envVariableName);
    return value ? value : "";
}

struct test_init {
    test_init() {
        SubscriptionKey = GetEnvVariable("SPEECH_SUBSCRIPTION_KEY");
        ServiceRegion = GetEnvVariable("SPEECH_SERVICE_REGION");

        if (SubscriptionKey.empty() || ServiceRegion.empty())
        {
            std::cerr << "Cannot read the speech subscription key and/or service region.\n";
            std::cerr << "Set environment variables SPEECH_SUBSCRIPTION_KEY and SPEECH_SERVICE_REGION to valid values.\n";
            std::exit(1);
        }
    }
} test_init_instance;

std::shared_ptr<SpeechConfig> SpeechConfigForIntentTests()
{
    auto speechConfig = SpeechConfig::FromSubscription(SubscriptionKey, ServiceRegion);
    return speechConfig;
}


TEST_CASE("IntentRecognizer::PatternMatching::Basics", "[en][speech]")
{
    SECTION("Intent Recognition works with a simple phrase without intentId")
    {
        REQUIRE(exists(INTENT_UTTERANCE2));

        auto speechConfig = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE2);
        auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        WHEN("There is no entity specified")
        {
            std::string phrase = "open microsoft word"; // capitalization does not matter
            intentRecognizer->AddIntent(phrase); // intentId will be the same as the phrase
            const auto& intentId = phrase;

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, intentId);
        }

        WHEN("There is an entity specified")
        {
            std::string phrase = "open {appName}";
            intentRecognizer->AddIntent(phrase);
            const auto& intentId = phrase;

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, intentId);
            RequireEntity(intentResult, "appName", "microsoft word");
        }
    }

    SECTION("Intent Recognition works with 1 entity")
    {
        REQUIRE(exists(INTENT_UTTERANCE2));

        auto speechConfig = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE2);
        auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        WHEN("There is an intent with 1 phrase")
        {
            intentRecognizer->AddIntent("Open {appName}", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("There is an intent with 1 phrase and punctuation")
        {
            intentRecognizer->AddIntent("Open {appName}'?", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("There is an intent with 1 phrase and extra spaces")
        {
            intentRecognizer->AddIntent("Open {appName}    ", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("There is an intent with 1 phrase and symbols")
        {
            intentRecognizer->AddIntent("Open!@#$%^&*_=+`~;\"/<>' {appName}", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("There is an intent with more than one phrase")
        {
            intentRecognizer->AddIntent("Start {appName}", "open");
            intentRecognizer->AddIntent("Open {appName}", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("There is a greedy entity that will only capture 1 word at the end of a pattern that has punctuation after it")
        {
            intentRecognizer->AddIntent("Turn on the {objectName}.", "HomeAutomation.TurnOn");

            auto intentResult = intentRecognizer->RecognizeOnceAsync("Turn on the lamp.").get();
            RequireIntentId(intentResult, "HomeAutomation.TurnOn");
            RequireEntity(intentResult, "objectName", "lamp");
        }

        WHEN("Recognize through text")
        {
            intentRecognizer->AddIntent("Open {appName}", "open");

            auto intentResult = intentRecognizer->RecognizeOnceAsync("Open microsoft word").get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        WHEN("Priority is given to exact match")
        {
            intentRecognizer->AddIntent("Open {appName}", "open");
            intentRecognizer->AddIntent("open microsoft word", "openWord");

            auto intentResult = intentRecognizer->RecognizeOnceAsync("Open microsoft word").get();
            RequireIntentId(intentResult, "openWord");
        }

        WHEN("There is an intent with an unmatched '{'")
        {
            REQUIRE_THROWS_AS(intentRecognizer->AddIntent("Open {appName", "open"), std::invalid_argument);
        }

        WHEN("There is an intent with an unmatched '}'")
        {
            REQUIRE_THROWS_AS(intentRecognizer->AddIntent("Open {appName", "open"), std::invalid_argument);
        }

        WHEN("There is an entity with an apostrophe")
        {
            intentRecognizer->AddIntent("{don't} open {any}", "Don'tOpen");

            auto intentResult = intentRecognizer->RecognizeOnceAsync("don't Open the pod bay doors").get();
            RequireIntentId(intentResult, "Don'tOpen");
            RequireEntity(intentResult, "any", "the pod bay doors");
            RequireEntity(intentResult, "don't", "don't");
        }
    }

    SECTION("Intent Recognition works with 2 entities")
    {
        REQUIRE(exists(INTENT_UTTERANCE3));

        auto speechConfig = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE3);
        auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        WHEN("There is an intent with more than one entity")
        {
            intentRecognizer->AddIntent("Open {appName} on the {location}", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
            RequireEntity(intentResult, "location", "left");
        }
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Detailed results", "[en][speech]")
{
    SECTION("Intent Recognition contains detailed output")
    {
        REQUIRE(exists(INTENT_UTTERANCE3));

        auto speechConfig = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE3);
        auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        WHEN("There are multiple patterns that would match")
        {
            intentRecognizer->AddIntent("Open {appName} on the {location}", "open");
            intentRecognizer->AddIntent("Open {appName}", "open");

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "open");
            RequireEntity(intentResult, "appName", "microsoft word");
            RequireEntity(intentResult, "location", "left");
            RequireAlternateIntentId(intentResult, "open");
            RequireAlternateCount(intentResult, 2);
        }
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Pattern matching model", "[en][speech]")
{
    SECTION("Pattern Matching Model can be used to configure intent recognizer")
    {
        REQUIRE(exists(INTENT_UTTERANCE2));

        auto speechConfig = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE2);
        auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        SECTION("Init through model Id")
        {
            std::string modelId = "MyTestModel";
            auto model = PatternMatchingModel::FromModelId(modelId);
            std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

            model->Intents.push_back({ {"Open {appName}"}, "Open" });
            models.push_back(model);
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model != nullptr);

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "Open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        SECTION("Adding Entities through model")
        {
            std::string modelId = "MyTestModel";
            auto model = PatternMatchingModel::FromModelId(modelId);
            std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

            model->Intents.push_back({ {"Open {appName}"}, "Open" });
            models.push_back(model);
            model->Entities.push_back({ "appName" , EntityType::Any, EntityMatchMode::Basic, {} });
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model != nullptr);

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "Open");
            RequireEntity(intentResult, "appName", "microsoft word");
        }

        SECTION("Priority given to exact matches")
        {
            std::string modelId = "MyTestModel";
            auto model = PatternMatchingModel::FromModelId(modelId);
            std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

            model->Intents.push_back({ {"Open {appName}"}, "Open" });
            model->Intents.push_back({ {"Open microsoft word"}, "Open2" });
            models.push_back(model);
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model != nullptr);

            auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
            auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
            RequireIntentId(intentResult, "Open2");
        }

        WHEN("There is a strict list before an any")
        {
            std::string modelId = "MyTestModel";
            auto model = PatternMatchingModel::FromModelId(modelId);
            std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

            model->Intents.push_back({ {"{Invoke.Actions} {ControlName.Any}"}, "Click" });
            models.push_back(model);
            model->Entities.push_back({ "Invoke.Actions" , EntityType::List, EntityMatchMode::Strict, {"check", "choose", "uncheck", "unchoose", "toggle", "expand", "collapse", "type in", "dictate in", "move"} });
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model != nullptr);

            auto intentResult = intentRecognizer->RecognizeOnceAsync("check change brightness automatically").get();
            RequireIntentId(intentResult, "Click");
            RequireEntity(intentResult, "Invoke.Actions", "check");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically");
        }

        WHEN("Models are replaced on ApplyLanguageModels")
        {
            auto model = PatternMatchingModel::FromModelId("MyTestModel");
            auto model2 = PatternMatchingModel::FromModelId("MyTestModel2");
            std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

            model->Intents.push_back({ {"{Invoke.Actions} {ControlName.Any}"}, "Click" });
            models.push_back(model);
            model->Entities.push_back({ "Invoke.Actions" , EntityType::List, EntityMatchMode::Strict, {"check", "choose", "uncheck", "unchoose", "toggle", "expand", "collapse", "type in", "dictate in", "move"} });
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model != nullptr);

            auto intentResult = intentRecognizer->RecognizeOnceAsync("check change brightness automatically").get();
            RequireIntentId(intentResult, "Click");
            RequireEntity(intentResult, "Invoke.Actions", "check");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically");
            models.clear();
            models.push_back(model2);

            model2->Intents.push_back({ {"This is my {thing}"}, "This" });
            intentRecognizer->ApplyLanguageModels(models);
            CHECK(model2 != nullptr);

            intentResult = intentRecognizer->RecognizeOnceAsync("This is my life").get();
            RequireIntentId(intentResult, "This");
            RequireEntity(intentResult, "thing", "life");

            intentResult = intentRecognizer->RecognizeOnceAsync("check change brightness automatically").get();
            RequireIntentId(intentResult, "");
        }

        SECTION("Add intents from a single model")
        {
            std::string modelId = "MyTestModel";
            auto model = PatternMatchingModel::FromModelId(modelId);

            model->Intents.push_back({ {"Open {appName}"}, "Open" });

            // Add-methods used in this section are only provided for backwards compatibility.
            // With a PatternMatchingModel type, all intents from the model are added in any case; the intent name and id arguments are ignored.
            // It is recommended to use ApplyLanguageModels instead.

            std::string inputText = "Open Microsoft Word.";

            WHEN("Add intent, specify an intent name")
            {
                std::shared_ptr<LanguageUnderstandingModel> luModel = model;
                intentRecognizer->AddIntent(luModel, "someIntentName");

                auto intentResult = intentRecognizer->RecognizeOnceAsync(inputText).get();
                RequireIntentId(intentResult, "Open");
                RequireEntity(intentResult, "appName", "microsoft word");
            }

            WHEN("Add intent, specify an intent name and id")
            {
                std::shared_ptr<LanguageUnderstandingModel> luModel = model;
                intentRecognizer->AddIntent(luModel, "someIntentName", "someIntentId");

                auto intentResult = intentRecognizer->RecognizeOnceAsync(inputText).get();
                RequireIntentId(intentResult, "Open");
                RequireEntity(intentResult, "appName", "microsoft word");
            }

            WHEN("Add all intents")
            {
                std::shared_ptr<LanguageUnderstandingModel> luModel = model;
                intentRecognizer->AddAllIntents(luModel);

                auto intentResult = intentRecognizer->RecognizeOnceAsync(inputText).get();
                RequireIntentId(intentResult, "Open");
                RequireEntity(intentResult, "appName", "microsoft word");
            }

            WHEN("Add all intents, specify an intent id")
            {
                std::shared_ptr<LanguageUnderstandingModel> luModel = model;
                intentRecognizer->AddAllIntents(luModel, "someIntentId");

                auto intentResult = intentRecognizer->RecognizeOnceAsync(inputText).get();
                RequireIntentId(intentResult, "Open");
                RequireEntity(intentResult, "appName", "microsoft word");
            }
        }
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Prebuilt integer entities", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    SECTION("Prebuilt number text")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open One hundred and thirty eight.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "138");
    }

    SECTION("Prebuilt number digit")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open 1,234,567.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "1234567");
    }

    SECTION("Prebuilt number negative")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("open negative one million two hundred thirty four thousand five hundred sixty seven").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "-1234567");
        intentResult = intentRecognizer->RecognizeOnceAsync("open minus one million two hundred thirty four thousand five hundred sixty seven").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "-1234567");
    }

    SECTION("Prebuilt number zero")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open zero.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "0");
    }

    SECTION("Prebuilt number ordinal")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open 1st.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "1");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open first.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "1");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open 5th.").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "number", "5");
    }

    SECTION("Prebuilt number text patterns with words containing number words")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number}"}, "Click" });
        model->Intents.push_back({ {"Click {control.any}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click done").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "control.any", "done");
    }

    SECTION("Prebuilt number text patterns with two numbers back to back")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number} {number2}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        model->Entities.push_back({ "number2" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click three fifty-five").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "3");
        RequireEntity(intentResult, "number2", "55");
    }

    SECTION("Prebuilt number text patterns and digits")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click 1000 and one").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "1001");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click -1000 and one").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "-1001");
    }

    SECTION("Prebuilt number extensive test")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click twelve hundred 2").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "1202");
    }

    SECTION("Prebuilt number text with non-number text", "[en]")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {number}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open words in nine beside ten times.").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "number");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open ten words in").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "number");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open ten words in nine").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "number");
    }

    SECTION("Prebuilt number mass test", "[en]")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click two zero zero").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "200");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click two zero four").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "204");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click twelve hundred and 2").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "1202");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click five thousand two hundred").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "5200");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click five hundred thousand").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "500000");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click five thousand two hundred five").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "5205");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click one twenty three").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "123");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click nineteen eighty five").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "1985");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click twenty twenty one").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "2021");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click ten thousand fifty one").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "10051");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click one two three").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "123");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click 2nd").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "2");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click too").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "2");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click to").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "2");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click for").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "4");
        intentResult = intentRecognizer->RecognizeOnceAsync("Click fore").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "4");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ListEntity", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    SECTION("ListEntity strict mode")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {appName}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "appName", EntityType::List, EntityMatchMode::Strict, {"microsoft word", "visual studio"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open Microsoft word").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "appName", "microsoft word");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open visual studio").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "appName", "visual studio");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open my good man Batman").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "appName");
    }

    SECTION("ListEntity fuzzy mode")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Open {appName}"}, "Open" });
        models.push_back(model);
        model->Entities.push_back({ "appName", EntityType::List, EntityMatchMode::Fuzzy, {"microsoft word", "visual studio"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open Microsoft word").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "appName", "microsoft word");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open visual studio").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "appName", "visual studio");
        intentResult = intentRecognizer->RecognizeOnceAsync("Open sesame").get();
        RequireIntentId(intentResult, "Open");
        RequireEntity(intentResult, "appName", "sesame");
    }

    SECTION("Strict lists don't match intents if not in the list")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {list}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "list" , EntityType::List, EntityMatchMode::Strict, {"open", "close"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click maximize").get();
        RequireIntentId(intentResult, "");
    }

    SECTION("List entities using instance id's and greed mechanics")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        // Click left control up arrow
        model->Intents.push_back({ {"press {any} {list}"}, "press" });
        model->Intents.push_back({ {"{list} {any}"}, "press" });

        model->Intents.push_back({ {"press {list} {any}"}, "press" });
        model->Intents.push_back({ {"press {any} {list} {any2}"}, "press" });
        model->Intents.push_back({ {"press {list:one} {list:two} {any}"}, "press" });
        model->Intents.push_back({ {"press {list:one} {any} {list:two}"}, "press" });
        model->Intents.push_back({ {"press {any} {list:one} {list:two}"}, "press" });
        model->Intents.push_back({ {"press {list:one} {list:two} {list:three}"}, "press" });
        models.push_back(model);
        model->Entities.push_back({ "list" , EntityType::List,   EntityMatchMode::Strict, {"left shift", "left control", "left arrow", "right shift"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("press shift control left arrow").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list", "left arrow");
        RequireEntity(intentResult, "any", "shift control");

        intentResult = intentRecognizer->RecognizeOnceAsync("press left control shift tab").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list", "left control");
        RequireEntity(intentResult, "any", "shift tab");

        intentResult = intentRecognizer->RecognizeOnceAsync("press control right shift tab").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list", "right shift");
        RequireEntity(intentResult, "any", "control");
        RequireEntity(intentResult, "any2", "tab");

        intentResult = intentRecognizer->RecognizeOnceAsync("press left control left shift tab").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list:one", "left control");
        RequireEntity(intentResult, "list:two", "left shift");
        RequireEntity(intentResult, "any", "tab");

        intentResult = intentRecognizer->RecognizeOnceAsync("press left shift control left arrow").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list:one", "left shift");
        RequireEntity(intentResult, "list:two", "left arrow");
        RequireEntity(intentResult, "any", "control");

        intentResult = intentRecognizer->RecognizeOnceAsync("press left shift left control left arrow").get();
        RequireIntentId(intentResult, "press");
        RequireEntity(intentResult, "list:one", "left shift");
        RequireEntity(intentResult, "list:two", "left control");
        RequireEntity(intentResult, "list:three", "left arrow");

        intentResult = intentRecognizer->RecognizeOnceAsync("left shift left arrow").get();
        RequireEntity(intentResult, "list", "left shift");
        RequireEntity(intentResult, "any", "left arrow");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Entity priority", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    SECTION("Prebuilt integer takes priority over list and any")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {number}"}, "Click" });
        model->Intents.push_back({ {"Click {any}"}, "Click" });
        model->Intents.push_back({ {"Click {list}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        model->Entities.push_back({ "list" , EntityType::List, EntityMatchMode::Strict, {"two zero three", "close"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click two zero three").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "number", "203");
    }

    SECTION("List takes priority over any")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Click {any}"}, "Click" });
        model->Intents.push_back({ {"Click {list}"}, "Click" });
        models.push_back(model);
        model->Entities.push_back({ "list" , EntityType::List, EntityMatchMode::Strict, {"open", "close"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Click close").get();
        RequireIntentId(intentResult, "Click");
        RequireEntity(intentResult, "list", "close");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Model from JSON file", "[en]")
{
    SECTION("Pattern Matching Model from JSON can be used to configure intent recognizer")
    {
        REQUIRE(exists(INTENT_JSON_FILE));

        auto intentRecognizer = IntentRecognizer::FromLanguage();

        auto model = PatternMatchingModel::FromJSONFile(INTENT_JSON_FILE);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("turn the pot on").get();
        RequireIntentId(intentResult, "Turn on");
        RequireEntity(intentResult, "on", "on");
        // Make sure the list entity got there.
        intentResult = intentRecognizer->RecognizeOnceAsync("brew my entry1").get();
        RequireIntentId(intentResult, "Brew");
        RequireEntity(intentResult, "myListEntity", "entry1");
        // Make sure the list entity synonyms got there.
        intentResult = intentRecognizer->RecognizeOnceAsync("brew my synonym1").get();
        RequireIntentId(intentResult, "Brew");
        RequireEntity(intentResult, "myListEntity", "synonym1");
        // Make sure the number entity got there.
        intentResult = intentRecognizer->RecognizeOnceAsync("make me five cups").get();
        RequireIntentId(intentResult, "Brew");
        RequireEntity(intentResult, "number", "5");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Optional patterns", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    std::string modelId = "MyTestModel";

    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"Click [ on the | by my ] {number}"}, "Click" });
    model->Intents.push_back({ {"test [ nestopt0 | nestopt1 | {optEntity} ] this"}, "test" });
    // This is to check if an optional word is included in the following entity. "on" inside of "one".
    model->Intents.push_back({ {"Click [on | o] {number}", "Click [on] [the] {ControlName.Any}", "Click {number} [ne | e]"}, "Click" });
    model->Intents.push_back({ {"Go to parking [{parkingLevel}]"}, "PARK" });
    model->Intents.push_back({ {"[Computer] listen up"}, "KeywordTest" });
    model->Intents.push_back({ {"Start [on] {appName}"}, "AnyNextToOptional1" });
    model->Intents.push_back({ {"Begin {appName} [ve]"}, "AnyNextToOptional2" });
    // Optionals with enties in front of them.
    model->Intents.push_back({ {"{Maximize}", "{Maximize} [the] {Application.Any}", "{Maximize} [the] [{Optional.Application.Words}]"},  "Task.Maximize" });
    // Two optionals together.
    model->Intents.push_back({ {"this is [my] [pattern]"}, "test1" });
    models.push_back(model);
    model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
    model->Entities.push_back({ "Maximize" , EntityType::List, EntityMatchMode::Strict, {"Maximize", "Maximise"} });
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    auto intentResult = intentRecognizer->RecognizeOnceAsync("Click two zero three").get();
    RequireIntentId(intentResult, "Click");
    RequireEntity(intentResult, "number", "203");
    intentResult = intentRecognizer->RecognizeOnceAsync("Click on the two zero three").get();
    RequireIntentId(intentResult, "Click");
    RequireEntity(intentResult, "number", "203");
    intentResult = intentRecognizer->RecognizeOnceAsync("Click by my two zero three").get();
    RequireIntentId(intentResult, "Click");
    RequireEntity(intentResult, "number", "203");
    intentResult = intentRecognizer->RecognizeOnceAsync("test something this").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "optEntity", "something");
    intentResult = intentRecognizer->RecognizeOnceAsync("test this").get();
    RequireIntentId(intentResult, "test");
    RequireNoEntity(intentResult, "optEntity");
    intentResult = intentRecognizer->RecognizeOnceAsync("Click one").get();
    RequireIntentId(intentResult, "Click");
    RequireEntity(intentResult, "number", "1");
    intentResult = intentRecognizer->RecognizeOnceAsync("Click on one").get();
    RequireIntentId(intentResult, "Click");
    RequireEntity(intentResult, "number", "1");
    intentResult = intentRecognizer->RecognizeOnceAsync("Go to parking.").get();
    RequireIntentId(intentResult, "PARK");
    intentResult = intentRecognizer->RecognizeOnceAsync("Computer listen up").get();
    RequireIntentId(intentResult, "KeywordTest");
    intentResult = intentRecognizer->RecognizeOnceAsync("listen up").get();
    RequireIntentId(intentResult, "KeywordTest");
    intentResult = intentRecognizer->RecognizeOnceAsync("this is my pattern").get();
    RequireIntentId(intentResult, "test1");
    intentResult = intentRecognizer->RecognizeOnceAsync("this is pattern").get();
    RequireIntentId(intentResult, "test1");
    intentResult = intentRecognizer->RecognizeOnceAsync("this is my").get();
    RequireIntentId(intentResult, "test1");
    intentResult = intentRecognizer->RecognizeOnceAsync("this is").get();
    RequireIntentId(intentResult, "test1");
    intentResult = intentRecognizer->RecognizeOnceAsync("Start onedrive").get();
    RequireIntentId(intentResult, "AnyNextToOptional1");
    RequireEntity(intentResult, "appName", "onedrive");
    intentResult = intentRecognizer->RecognizeOnceAsync("Start on onedrive").get();
    RequireIntentId(intentResult, "AnyNextToOptional1");
    RequireEntity(intentResult, "appName", "onedrive");
    intentResult = intentRecognizer->RecognizeOnceAsync("Begin onedrive").get();
    RequireIntentId(intentResult, "AnyNextToOptional2");
    RequireEntity(intentResult, "appName", "onedrive");
    intentResult = intentRecognizer->RecognizeOnceAsync("Begin onedrive ve").get();
    RequireIntentId(intentResult, "AnyNextToOptional2");
    RequireEntity(intentResult, "appName", "onedrive");
}

TEST_CASE("IntentRecognizer::PatternMatching::Voice commands", "[en]")
{
    SECTION("Voice command tests")
    {
        auto intentRecognizer = IntentRecognizer::FromLanguage();

        std::string modelId = "MyTestModel";

        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        // System Level Intents
        model->Intents.push_back({ {"Increase [the] volume"},  "Task.VolumeUp" });
        model->Intents.push_back({ {"Decrease [the] volume"},  "Task.VolumeDown" });
        model->Intents.push_back({ {"Mute [the] [audio|volume]"},  "Task.VolumeMute" });
        model->Intents.push_back({ {"Unmute [the] [audio|volume]"},  "Task.VolumeUnmute" });
        // We are having both the UK and US english version so as to avoid any SR locale handling issues.
        model->Intents.push_back({ {"{Maximize}", "{Maximize} [the] {Application.Any}", "{Maximize} [the] [{Optional.Application.Words}]"},  "Task.Maximize" });
        // We are having both the UK and US english version so as to avoid any SR locale handling issues.
        model->Intents.push_back({ {"{Minimize}", "{Minimize} [the] {Application.Any}", "{Minimize} [the] {Optional.Application.Words}"},  "Task.Minimize" });
        model->Intents.push_back({ {"Restore", "Restore [the] {Application.Any}", "Restore [the] {Optional.Application.Words}" },  "Task.Restore" });
        model->Intents.push_back({ {"Close [the] {Application.Any}", "Close [the] {Optional.Application.Words}", "Close"},  "Task.Close" });
        model->Intents.push_back({ {"Minimize all [the] windows", "Minimise all [the] windows", "[Go to|Show] desktop", "go home"},  "Task.ShowDesktop" });
        model->Intents.push_back({ {"List [all the|all] windows", "show [all the|all] windows", "List [all the|all] apps", "show [all the|all] apps", "switch to [all the|all] apps", "show [the] task switcher"},  "Task.ListAllWindows" });
        model->Intents.push_back({ {"snap [the] {Optional.Application.Words} to [the] {Move.Direction}", "snap [the] {Application.Any} to [the] {Move.Direction}", "snap {Application.Any}"},  "Task.Move" });
        model->Intents.push_back({ {"Rotate screen [to {Orientation} | to {.any}]"},  "Task.RotateScreen" });
        model->Intents.push_back({ {"{OnScreenKeyboard.State} [on screen|onscreen] keyboard"},  "Task.OnScreenKeyboard" });
        model->Intents.push_back({ {"show [touch] keyboard"},  "Task.TouchKeyboard.Show" });
        model->Intents.push_back({ {"hide [touch] keyboard"},  "Task.TouchKeyboard.Hide" });
        model->Intents.push_back({ {"[take|capture] screenshot"},  "Task.Screenshot" });

        // System level short cuts
        model->Intents.push_back({ {"open start", "open [the] start menu"},  "Task.Open.StartMenu" });
        model->Intents.push_back({ {"open search", "open [the] search menu"},  "Task.Open.SearchMenu" });
        model->Intents.push_back({ {"open voice access settings"},  "Task.Open.VoiceAccessSettings" });

        // Application launching / window events
        model->Intents.push_back({ {"Open [the] {Application.Any}", "Open"},  "Task.Open" });
        model->Intents.push_back({ {"Start [the] {Application.Any}", "start"},  "Task.Start" });
        model->Intents.push_back({ {"Switch to [the] {Application.Any}", "switch to", "Show [the] {Application.Any}", "Show"},  "Task.SwitchApp" });

        // Voice Access Intents
        model->Intents.push_back({ {"Close voice access", "Stop voice access", "turn off voice access"},  "Task.VoiceAccess.Close" });
        model->Intents.push_back({ {"turn off microphone"},  "Task.VoiceAccess.Off" });
        model->Intents.push_back({ {"Unmute", "listen to me", "Voice access wake up", "Voice wake up"},  "Task.VoiceAccess.On" });
        model->Intents.push_back({ {"mute", "voice stop", "Voice sleep", "Voice access sleep"},  "Task.VoiceAccess.Sleep" });
        model->Intents.push_back({ {"view commands", "Show all commands", "What can I say"},  "Task.ShowAllCommands" });
        model->Intents.push_back({ {"open [voice access] guide"},  "Task.OpenTutorial" });
        model->Intents.push_back({ {"open [voice access] help"},  "Task.Open.VoiceAccessHelp" });

        // Overlay intents
        model->Intents.push_back({ {"Hide Number", "Hide Numbers"},  "Task.Overlay.HideNumbers" });
        model->Intents.push_back({ {"Mouse grid", "show grid", "show {Grid} {Overlay.Scope}", "show {Grid} {Optional.Words} {Overlay.Application.Any}"},  "Task.Grid.Show" });
        model->Intents.push_back({ {"show numbers", "show number", "show {Numbers} {Overlay.Scope}", "show {Numbers} {Optional.Words} {Overlay.Application.Any}"},  "Task.Overlay.ShowNumbers" });
        model->Intents.push_back({ {"[mouse] {Click} [on] {builtin.number}", "[mouse] {Move.Direction} {Click} [on] {builtin.number}", "[mouse] {Tuple} {Click} [on] {builtin.number}", "[mouse] {Click} [on] [the] {ControlName.Any}", "[mouse] {Move.Direction} {Click} [on] [the] {ControlName.Any}", "[mouse] {Tuple} {Click} [on] [the] {ControlName.Any}", "[mouse] {Move.Direction} {Click}", "[mouse] {Tuple} {Click}", "[mouse] {Click}", "[mouse] {Move.Direction} {Click} {Optional.ControlNames}", "[mouse] {Tuple} {Click} {Optional.ControlNames}", "[mouse] {Click} {Optional.ControlNames}"},  "Task.Mouse.Click" });
        model->Intents.push_back({ {"Mark {builtin.number}", "Mark"},  "Task.Grid.Mark" });
        model->Intents.push_back({ {"Drag", "Drag here"},  "Task.Grid.Drag" });
        // Adding additional patterns for hide grid as even after adding phraselist support it still is not able to recognize
        model->Intents.push_back({ {"Hide grid", "Hydrid", "Height grid", "Hyde grid", "close grid"},  "Task.Grid.Hide" });
        model->Intents.push_back({ {"[mouse] grid {builtin.number}"},  "Task.Grid.DrillDown" });
        // Keeping it as commented for future reference.
        //model->Intents.push_back({ {"Show labels", "Show labels {Overlay.Scope}", "Show labels on {Overlay.Application.Any}"},  "Task.Overlay.ShowLabels" });
        //model->Intents.push_back({ {"Hide labels"},  "Task.Overlay.HideLabels" });
        // Invoking standard controls
        model->Intents.push_back({ {"Focus [on|the] {Focus.Name}", "Move to {Focus.Name}", "[Focus|move to]"},  "Task.Focus" });
        model->Intents.push_back({ {"{Invoke.Actions} {ControlName.Any}" },  "Task.InvokeControl" });
        model->Intents.push_back({ {"move {Slider.Optional} {Move.Direction}", "move {SliderName.Any} {Move.Direction}", "move {Articles} {Slider.Optional} to {Move.Direction}",
                 "move {Articles} {SliderName.Any} to {Move.Direction}", "move {Articles} {Slider.Optional} {Move.Direction}", "move {Articles} {SliderName.Any} {Move.Direction}", "move {Slider.Optional} to {Move.Direction}", "move {SliderName.Any} to {Move.Direction}", "move {Slider.Optional} {Move.Direction} {builtin.number} times", "move {SliderName.Any} {Move.Direction} {builtin.number} times", "move {Articles} {Slider.Optional} to {Move.Direction} {builtin.number} times", "move {Articles} {SliderName.Any} to {Move.Direction} {builtin.number} times", "move {Articles} {Slider.Optional} {Move.Direction} {builtin.number} times", "move {Articles} {SliderName.Any} {Move.Direction} {builtin.number} times", "move {Slider.Optional} to {Move.Direction} {builtin.number} times", "move {SliderName.Any} to {Move.Direction} {builtin.number} times", "move {Slider.Optional} to {Move.Direction} by {builtin.number} times", "move {SliderName.Any} to {Move.Direction} by {builtin.number} times", "move {Articles} {Slider.Optional} {Move.Direction} by {builtin.number} times", "move {Articles} {SliderName.Any} {Move.Direction} by {builtin.number} times", "move {Articles} {Slider.Optional} to {Move.Direction} by {builtin.number} times", "move {Articles} {SliderName.Any} to {Move.Direction} by {builtin.number} times"},  "Task.MoveSlider" });
        model->Intents.push_back({ {"Scroll {Move.Direction}", "Scroll {Move.Direction} {builtin.number} lines", "Scroll {Move.Direction} {builtin.number} lines", "Scroll {Move.State}", "{Page.Up}", "{Page.Down}", "{Page.Start}", "{Page.End}", "scroll {builtin.number} lines {Move.Direction}", "Scroll {.any}", "scroll"},  "Task.Scroll" });
        model->Intents.push_back({ {"start scrolling", "start scrolling {Move.Direction}", "continue scrolling", "continue scrolling {Move.Direction}"},  "Task.Scroll.StartScrolling" });
        model->Intents.push_back({ {"stop scrolling"},  "Task.Scroll.StopScrolling" });

        // Dictation intents
        model->Intents.push_back({ {"{Uppercase} {ControlName.Any}", "{Uppercase} [the] {Dictation.Scope}", "{Uppercase} this {Dictation.Scope}", "{Uppercase}  [the] {builtin.ordinal} {Dictation.Scope}", "{Uppercase} [the] {Direction} {builtin.number} {Dictation.Scope}", "{Uppercase} [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Uppercase" });
        model->Intents.push_back({ {"Select {ControlName.Any}", "Select [the] {Dictation.Scope}", "Select this {Dictation.Scope}", "Select [the] {builtin.ordinal} {Dictation.Scope}", "Select [the] {Direction} {builtin.number} {Dictation.Scope}", "Select [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Select" });
        model->Intents.push_back({ {"Spell {ControlName.Any}", "Spell [the] {Dictation.Scope}", "Spell this {Dictation.Scope}", "Spell [the] {builtin.ordinal} {Dictation.Scope}", "Spell [the] {Direction} {builtin.number} {Dictation.Scope}", "Spell [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Spell" });
        model->Intents.push_back({ {"Correct {ControlName.Any}", "Correct [the] {Dictation.Scope}", "Correct this {Dictation.Scope}", "Correct [the] {builtin.ordinal} {Dictation.Scope}", "Correct [the] {Direction} {builtin.number} {Dictation.Scope}", "Correct [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Spell" });
        model->Intents.push_back({ {"Bold {ControlName.Any}", "Bold [the] {Dictation.Scope}", "Bold this {Dictation.Scope}", "Bold [the] {builtin.ordinal} {Dictation.Scope}", "Bold [the] {Direction} {builtin.number} {Dictation.Scope}", "Bold [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Bold" });
        // We are having both the UK and US english version so as to avoid any SR locale handling issues.
        model->Intents.push_back({ {"{Italicize} {ControlName.Any}", "{Italicize} [the] {Dictation.Scope}", "{Italicize} this {Dictation.Scope}", "{Italicize}  [the] {builtin.ordinal} {Dictation.Scope}", "{Italicize} [the] {Direction} {builtin.number} {Dictation.Scope}", "{Italicize} [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Italicize" });
        model->Intents.push_back({ {"{Underline} {ControlName.Any}", "{Underline} [the] {Dictation.Scope}", "{Underline} this {Dictation.Scope}", "{Underline}  [the] {builtin.ordinal} {Dictation.Scope}", "{Underline} [the] {Direction} {builtin.number} {Dictation.Scope}", "{Underline} [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Underline" });
        // We are having both the UK and US english version so as to avoid any SR locale handling issues.
        model->Intents.push_back({ {"{Capitalize} {ControlName.Any}", "{Capitalize} [the] {Dictation.Scope}", "{Capitalize} this {Dictation.Scope}", "{Capitalize} [the] {builtin.ordinal} {Dictation.Scope}", "{Capitalize} [the] {Direction} {builtin.number} {Dictation.Scope}", "{Capitalize} [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Capitalize" });
        model->Intents.push_back({ {"{Lowercase} {ControlName.Any}", "{Lowercase} [the] {Dictation.Scope}", "{Lowercase} this {Dictation.Scope}", "{Lowercase}  [the] {builtin.ordinal} {Dictation.Scope}", "{Lowercase} [the] {Direction} {builtin.number} {Dictation.Scope}", "{Lowercase}  [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Lowercase" });
        model->Intents.push_back({ {"Copy {ControlName.Any}", "Copy [the] {Dictation.Scope}", "Copy this {Dictation.Scope}", "Copy  [the] {builtin.ordinal} {Dictation.Scope}", "Copy [the] {Direction} {builtin.number} {Dictation.Scope}", "Copy [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Copy" });
        model->Intents.push_back({ {"Cut {ControlName.Any}", "Cut [the] {Dictation.Scope}", "Cut this {Dictation.Scope}", "Cut [the] {builtin.ordinal} {Dictation.Scope}", "Cut [the] {Direction} {builtin.number} {Dictation.Scope}", "Cut [the] {Direction} {Dictation.Scope}"},  "Task.Dictation.Cut" });
        model->Intents.push_back({ {"Delete {ControlName.Any}", "Delete [the] {Dictation.Scope}", "Delete this {Dictation.Scope}", "Delete [the] {builtin.ordinal} {Dictation.Scope}", "Delete [the] {Direction} {builtin.number} {Dictation.Scope}", "Delete [the] {Direction} {Dictation.Scope}", "scratch [the] {Dictation.Scope}"},  "Task.Dictation.Delete" });
        model->Intents.push_back({ {"type {Dictation.Text.Any}"},  "Task.Dictation.Type" });
        model->Intents.push_back({ {"Move to {Direction} of {Dictation.Scope}", "Go to {Direction} of {Dictation.Scope}", "Move {Move.Direction} {builtin.number} times", "Go {Move.Direction} {builtin.number} times", "Move {Move.Direction} {builtin.number}", "Go {Move.Direction} {builtin.number}", "Move {Move.Direction}", "Go {Move.Direction}", "Move {Move.Direction} {builtin.number} time", "Go {Move.Direction} {builtin.number} time", "Move {Move.Direction} {builtin.number} {Dictation.Scope}", "Go {Move.Direction} {builtin.number} {Dictation.Scope}", "Move {Move.Direction} by {builtin.number} {Dictation.Scope}", "Go {Move.Direction} by {builtin.number} {Dictation.Scope}", "Move to the {Direction} of {Dictation.Scope}", "Move to the {Direction} of the {Dictation.Scope}", "Go to the {Direction} of {Dictation.Scope}", "Go to the {Direction} of the {Dictation.Scope}", "Move {Move.Direction} by {builtin.number} times", "Go {Move.Direction} by {builtin.number} times", "Move to {Move.Direction} by {builtin.number} times", "Go to {Move.Direction} by {builtin.number} times", "Move {Move.Direction} by {Dictation.Scope}", "Go {Move.Direction} by  {Dictation.Scope}", "Move {Move.Direction} by a {Dictation.Scope}", "Go {Move.Direction} by a {Dictation.Scope}", "Move to {Direction} of the {Dictation.Scope}", "Go to {Direction} of the {Dictation.Scope}", "Move {Move.Direction} by {builtin.number}", "Go {Move.Direction} by {builtin.number}", "Go {Direction} {ControlName.Any}", "Move {Direction} {ControlName.Any}"},  "Task.Dictation.Goto" });
        // Commenting these as these are no longer supported. Keeping them here for future use
        //model->Intents.push_back({ {"Stop dictation"},  "Task.Dictation.Stop" });
        //model->Intents.push_back({ {"Start dictation"},  "Task.Dictation.Start" });

        // Common Intents
        model->Intents.push_back({ {"undo [that]"},  "Task.Undo" });
        model->Intents.push_back({ {"redo", "redo that"},  "Task.Redo" });
        model->Intents.push_back({ {"fast", "faster"},  "Task.Fast" });
        model->Intents.push_back({ {"{Move.Direction}"},  "Task.MoveDirection" });
        model->Intents.push_back({ {"slow", "slower"},  "Task.Slow" });
        model->Intents.push_back({ {"go to {Any.Entity}", "goto {Any.Entity}"},  "Task.Common.GoTo" });
        model->Intents.push_back({ {"stop that", "stop this", "stop"},  "Task.Stop" });
        model->Intents.push_back({ {"hide", "conceal"},  "Task.Hide" });
        model->Intents.push_back({ {"dismiss"},  "Task.Dismiss" });
        model->Intents.push_back({ {"cancel"},  "Task.Cancel" });

        // Mouse and Key board intents
        model->Intents.push_back({ {"stop [the] mouse movement", "stop movement", "stop moving", "stop {Mouse.Movement.Type} mouse"},  "Task.Mouse.StopMoving" });
        model->Intents.push_back({ {"{Mouse.Movement.Type} {Mouse} {Move.Direction} {builtin.number}", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} {builtin.number}", "{Mouse.Movement.Type} {Move.State}", "{Mouse.Movement.Type} {Mouse} {Move.State}", "{Mouse.Movement.Type} {Mouse} {Move.Direction} {Move.State}", "{Mouse.Movement.Type} {Mouse} to {Move.Direction} {Move.State}", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} {Move.State}", "{Mouse.Movement.Type} {Mouse} {Move.Direction}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} {Move.State}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} {builtin.number}", "{Mouse.Movement.Type} {Mouse} to {Move.Direction} {builtin.number}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} by {Move.State}", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} by {builtin.number}", "{Mouse.Movement.Type} {Mouse} to {Move.Direction} by {builtin.number}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} by {builtin.number}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} by {builtin.number} times", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} by {builtin.number} times", "{Mouse.Movement.Type} {Mouse} to {Move.Direction} by {builtin.number} times", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} by {builtin.number} times", "{Mouse.Movement.Type} the {Mouse} {Move.Direction} {builtin.number} times", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction} {builtin.number} times", "{Mouse.Movement.Type} {Mouse} to {Move.Direction} {builtin.number} times", "{Mouse.Movement.Type} the {Mouse} {Move.State}", "{Mouse.Movement.Type} the {Mouse} to {Move.Direction}", "{Mouse.Movement.Type} {Mouse} to {Move.Direction}", "{Mouse.Movement.Type} the {Mouse} {Move.Direction}", "{Mouse.Movement.Type} {Mouse} {.any}", "{Mouse.Movement.Type} {Mouse}"},  "Task.Mouse.Movements" });
        model->Intents.push_back({ {"hold {Special.Keyboard.Keys}", "hold {Keyboard.Keys}", "hold {HoldDown} {Special.Keyboard.Keys}", "hold {HoldDown} {Keyboard.Keys}", "press and hold {Special.Keyboard.Keys}", "press and hold {Keyboard.Keys}", "hold"}, "Task.Hold" });
        model->Intents.push_back({ {"press {Special.Keyboard.Keys}", "press {Keyboard.Keys}", "press"}, "Task.Press" });
        model->Intents.push_back({ {"release {Special.Keyboard.Keys}", "release {Keyboard.Keys}", "release", "release keys"},  "Task.Release" });
        model->Intents.push_back({ {"Paste"},  "Task.Paste" });
        model->Intents.push_back({ {"Save"},  "Task.Save" });
        model->Intents.push_back({ {"Select all", "Select document"},  "Task.SelectAll" });
        model->Intents.push_back({ {"Undo"},  "Task.Undo" });
        model->Intents.push_back({ {"unselect that", "clear selection"},  "Task.UnselectThat" });
        model->Intents.push_back({ {"Copy"},  "Task.Copy" });
        model->Intents.push_back({ {"Cut"},  "Task.Cut" });

        // Recognition mode intents
        model->Intents.push_back({ {"[switch to] default mode"},  "Task.Default.Mode" });
        model->Intents.push_back({ {"[switch to] command mode"},  "Task.Command.Mode" });
        model->Intents.push_back({ {"[switch to] dictation mode"},  "Task.Dictation.Mode" });

        // Browser Intents
        model->Intents.push_back({ {"Search [on] Bing for {Search.Phrase}"},  "Task.WebSearch.BingSearch" });
        model->Intents.push_back({ {"Search [on] Google for {Search.Phrase}"},  "Task.WebSearch.GoogleSearch" });
        model->Intents.push_back({ {"Search [on] YouTube for {Search.Phrase}"},  "Task.WebSearch.YouTubeSearch" });
        model->Intents.push_back({ {"Go to [the] address bar"},  "Task.Browser.AddressBar" });
        model->Intents.push_back({ {"Bookmark [this] page", "Add [to] favorites"},  "Task.Browser.BookmarkPage" });
        model->Intents.push_back({ {"Exit browser"},  "Task.Browser.CloseBrowser" });
        model->Intents.push_back({ {"Exit developer tools"},  "Task.Browser.CloseDeveloperTools" });
        model->Intents.push_back({ {"Exit search box", "close find box"},  "Task.Browser.CloseSearchBox" });
        model->Intents.push_back({ {"show downloads", "open downloads"},  "Task.Browser.Downloads" });
        model->Intents.push_back({ {"Duplicate this tab", "Duplicate tab"},  "Task.Browser.DuplicateTab" });
        model->Intents.push_back({ {"Exit context menu"},  "Task.Browser.ExitContextMenu" });
        model->Intents.push_back({ {"Exit full screen"},  "Task.Browser.ExitFullScreen" });
        model->Intents.push_back({ {"Find {FindWord} on page", "Find {FindWord} on this page", "Find {FindWord}"},  "Task.Browser.FindOnPage" });
        model->Intents.push_back({ {"Full screen", "Activate full screen mode"},  "Task.Browser.FullScreen" });
        model->Intents.push_back({ {"Go back"},  "Task.Browser.GoBack" });
        model->Intents.push_back({ {"Go forward"},  "Task.Browser.GoForward" });
        model->Intents.push_back({ {"Go to home page"},  "Task.Browser.GoHome" });
        model->Intents.push_back({ {"Show history"},  "Task.Browser.History" });
        model->Intents.push_back({ {"Move [the|this] tab to new window"},  "Task.Browser.MoveTabToNewWindow" });
        model->Intents.push_back({ {"Mute [this|the] tab"},  "Task.Browser.MuteTab" });
        model->Intents.push_back({ {"[Open] [a] new private window", "[Open] [a] new incognito window"},  "Task.Browser.NewPrivateWindow" });
        model->Intents.push_back({ {"New tab", "Create [a] new tab", "open [a] new tab", "add [a] new tab"},  "Task.Browser.NewTab" });
        model->Intents.push_back({ {"Open [a] new window"},  "Task.Browser.NewWindow" });
        model->Intents.push_back({ {"Next word"},  "Task.Browser.NextWord" });
        model->Intents.push_back({ {"Go to {Tab Locator} tab", "Move to {Tab Locator} tab", "Switch to {Tab Locator} tab"},  "Task.Browser.GoToNthTab" });
        model->Intents.push_back({ {"Move {Tab Locator} tab to {Move.Direction}"},  "Task.Browser.RearrangeTabs" });
        model->Intents.push_back({ {"Close {Tab Locator} tabs", "Close {Tab Locator} tab", "Close tabs to the {Tab Locator}", "close tab"},  "Task.Browser.CloseTab" });
        model->Intents.push_back({ {"Show bookmarks page", "open favorites page", "open bookmarks page"},  "Task.Browser.OpenBookMarkPage" });
        model->Intents.push_back({ {"Show developer tools"},  "Task.Browser.OpenDeveloperTools" });
        model->Intents.push_back({ {"Open {Open.Link} in [a] new tab"},  "Task.Browser.OpenInNewTab" });
        model->Intents.push_back({ {"Show search box"},  "Task.Browser.OpenSearchBox" });
        model->Intents.push_back({ {"Pin [this|the] tab"},  "Task.Browser.PinTab" });
        model->Intents.push_back({ {"Show previously closed tab", "Open previously closed tab"},  "Task.Browser.PreviouslyClosedTab" });
        model->Intents.push_back({ {"Previous word"},  "Task.Browser.PreviousWord" });
        model->Intents.push_back({ {"Refresh [this|the] page"},  "Task.Browser.RefreshPage" });
        model->Intents.push_back({ {"Unmute [this|the] tab"},  "Task.Browser.UnmuteTab" });
        model->Intents.push_back({ {"Unpin [this|the] tab"},  "Task.Browser.UnpinTab" });
        model->Intents.push_back({ {"Zoom in"},  "Task.Browser.ZoomIn" });
        model->Intents.push_back({ {"Zoom out"},  "Task.Browser.ZoomOut" });
        model->Intents.push_back({ {"Show {ContextMenu} on {Context.Menu.Region}", "Show context menu"},  "Task.Browser.ShowContextMenu" });
        // None intent.
        // Mapping to built in number is required for grid and number overlay use cases.
        // So if its  number it will try to parse and set the entity
        model->Intents.push_back({ {"{builtin.number}"},  "None" });

        // Entities
        model->Entities.push_back({ "builtin.number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        model->Entities.push_back({ "builtin.ordinal" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });

        //list entities
        model->Entities.push_back({ "Dictation.Scope" , EntityType::List, EntityMatchMode::Strict, {"that", "page", "character", "word", "line", "paragraph", "page", "pages", "characters", "words", "lines", "paragraphs", "pages", "selection"} });
        model->Entities.push_back({ "Direction" , EntityType::List, EntityMatchMode::Strict, {"next", "previous", "start", "beginning", "end", "last", "after", "before"} });
        model->Entities.push_back({ "Maximize" , EntityType::List, EntityMatchMode::Strict, {"Maximize", "Maximise"} });
        model->Entities.push_back({ "Minimize" , EntityType::List, EntityMatchMode::Strict, {"Minimize", "Minimise", "Hide", "Remove"} });
        model->Entities.push_back({ "Italicize" , EntityType::List, EntityMatchMode::Strict, {"Italicize", "Italicise"} });
        model->Entities.push_back({ "Capitalize" , EntityType::List, EntityMatchMode::Strict, {"Capitalize", "Capitalise"} });
        model->Entities.push_back({ "Uppercase" , EntityType::List, EntityMatchMode::Strict, {"Uppercase", "Upper case", "all caps"} });
        model->Entities.push_back({ "Lowercase" , EntityType::List, EntityMatchMode::Strict, {"Lowercase", "Lower case", "no caps"} });
        model->Entities.push_back({ "Underline" , EntityType::List, EntityMatchMode::Strict, {"Underline", "Under line"} });
        model->Entities.push_back({ "Click" , EntityType::List, EntityMatchMode::Strict, {"Click", "Tap"} });
        model->Entities.push_back({ "Optional.ControlNames" , EntityType::List, EntityMatchMode::Strict, {"here"} });
        model->Entities.push_back({ "Mouse.Movement.Type" , EntityType::List, EntityMatchMode::Strict, {"copy drag", "drag", "move", "copy dragging", "dragging", "moving"} });
        model->Entities.push_back({ "Orientation" , EntityType::List, EntityMatchMode::Strict, {"landscape", "landscape flipped", "portrait", "portrait flipped"} });
        model->Entities.push_back({ "Tuple" , EntityType::List, EntityMatchMode::Strict, {"double", "triple"} });
        model->Entities.push_back({ "Numbers" , EntityType::List, EntityMatchMode::Strict, {"numbers", "number"} });
        model->Entities.push_back({ "HoldDown" , EntityType::List, EntityMatchMode::Strict, {"down"} });
        model->Entities.push_back({ "ContextMenu" , EntityType::List, EntityMatchMode::Strict, {"context menu"} });
        model->Entities.push_back({ "Grid" , EntityType::List, EntityMatchMode::Strict, {"grid"} });
        model->Entities.push_back({ "Slider.Optional" , EntityType::List, EntityMatchMode::Strict, {"slider"} });
        model->Entities.push_back({ "Page.Up" , EntityType::List, EntityMatchMode::Strict, {"page up"} });
        model->Entities.push_back({ "Page.Down" , EntityType::List, EntityMatchMode::Strict, {"page down"} });
        model->Entities.push_back({ "Page.Start" , EntityType::List, EntityMatchMode::Strict, {"document start", "go to page start", "go to start of the document", "go to the start of document", "go to start of the document", "go to start of document", "page start", "start of the document", "start of document", "go to top", "go to beginning", "move to top", "move to beginning", "go to top of document", "go to beginning of document", "move to top of document", "move to beginning of document", "scroll to top", "go to the beginning of document", "go to the beginning of the document", "scroll to the top", "go to beginning of the document", "move to start of the document", "move to the start of document", "move to start of the document", "move to start of document", "move to beginning of the document", "move to the beginning of document", "move to beginning of the document", "move to beginning of document"} });
        model->Entities.push_back({ "Page.End" , EntityType::List, EntityMatchMode::Strict, {"go to page end", "go to the end of the document", "go to end of the document", "go to the end of document", "go to end of document", "page end", "document end", "end of the document", "end of document", "go to bottom", "go to end", "move to bottom of document", "move to end of the document" , "move to end of document", "go to bottom of document", "move to bottom", "move to end", "scroll to bottom", "scroll to the bottom"} });
        model->Entities.push_back({ "Overlay.Scope" , EntityType::List, EntityMatchMode::Strict, {"everywhere", "here", "window"} });
        model->Entities.push_back({ "Optional.Application.Words" , EntityType::List, EntityMatchMode::Strict, {"this window", "browser", "this", "window", "it"} });
        model->Entities.push_back({ "Optional.Words" , EntityType::List, EntityMatchMode::Strict, {"on", "to"} });
        model->Entities.push_back({ "Articles" , EntityType::List, EntityMatchMode::Strict, {"the"} });
        model->Entities.push_back({ "Mouse" , EntityType::List, EntityMatchMode::Strict, {"mouse"} });
        model->Entities.push_back({ "OnScreenKeyboard.State" , EntityType::List, EntityMatchMode::Strict, {"minimize", "close", "hide", "show", "open"} });
        model->Entities.push_back({ "Move.State" , EntityType::List, EntityMatchMode::Strict, {"fast", "slow", "faster", "slower"} });
        model->Entities.push_back({ "Move.Direction" , EntityType::List, EntityMatchMode::Strict, {"bottom left", "bottom right", "down", "left", "middle", "right", "top left", "top right", "up", "backward", "forward", "back"} });
        model->Entities.push_back({ "Context.Menu.Region" , EntityType::List, EntityMatchMode::Strict, {"tab", "window"} });
        model->Entities.push_back({ "Invoke.Actions" , EntityType::List, EntityMatchMode::Strict, {"check", "choose", "uncheck", "unchoose", "toggle", "expand", "collapse", "flip"} });
        model->Entities.push_back({ "Special.Keyboard.Keys" , EntityType::List, EntityMatchMode::Strict, {"exclamation point", "at sign", "pound sign", "number sign", "dollar sign", "percent sign", "and sign", "open  parenthesis", "open parens", "left parenthesis", "left parens", "close parenthesis", "close parens", "right parenthesis", "right parens", "minus sign", "open brackets", "open square brackets", "left bracket", "left square bracket", "close brackets", "close square brackets", "right bracket", "right square bracket", "close bracket", "grave accent", "full stop", "forward slash", "back space", "space bar", "caps lock", "cap lock", "print screen", "scroll lock", "page up", "page down", "right arrow", "left arrow", "down arrow", "up arrow", "num lock", "number lock", "f one", "f 1", "function one",  "function 1", "f two", "f 2", "function two", "function 2", "f three", "f 3", "function three", "function 3", "f four", " f 4", "function four", "function 4", "f five", "f 5", "function five", "function 5", "f six", "f 6", "function six", "function 6", "f seven", "f 7", "function seven", "function 7", "f eight", " f 8", "function eight", "function 8", "f nine", "f 9", "function nine", "function 10", "f ten", "f 10", "function eleven", "function 11", "f eleven", "f 11", "function twelve", "function 12", "f twelve", "f 12"} });
        model->Entities.push_back({ "Press.Keys" , EntityType::List, EntityMatchMode::Strict, {"enter", "delete", "tab", "backspace"} });


        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        SECTION("Task.Mouse.Click")
        {
            auto intentResult = intentRecognizer->RecognizeOnceAsync("left click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("right click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Left click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Right click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Double click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse left click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse double click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("left click on restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("double click restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse left click on restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse double click restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse click restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse left click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse double click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mouse left click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            intentResult = intentRecognizer->RecognizeOnceAsync("mouse double click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            RequireEntity(intentResult, "Tuple", "double");
        }

        SECTION("NewPrivateWindow")
        {
            auto intentResult = intentRecognizer->RecognizeOnceAsync("Open a new private window").get();
            RequireIntentId(intentResult, "Task.Browser.NewPrivateWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open a new incognito window").get();
            RequireIntentId(intentResult, "Task.Browser.NewPrivateWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("New private window").get();
            RequireIntentId(intentResult, "Task.Browser.NewPrivateWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("New incognito window").get();
            RequireIntentId(intentResult, "Task.Browser.NewPrivateWindow");
        }

        SECTION("None number")
        {
            auto intentResult = intentRecognizer->RecognizeOnceAsync("2345").get();
            RequireIntentId(intentResult, "None");
            RequireEntity(intentResult, "builtin.number", "2345");
            intentResult = intentRecognizer->RecognizeOnceAsync("Random text").get();
            RequireIntentId(intentResult, "");
            intentResult = intentRecognizer->RecognizeOnceAsync("One thirty").get();
            RequireIntentId(intentResult, "None");
            RequireEntity(intentResult, "builtin.number", "130");
            intentResult = intentRecognizer->RecognizeOnceAsync("Five").get();
            RequireIntentId(intentResult, "None");
            RequireEntity(intentResult, "builtin.number", "5");
            intentResult = intentRecognizer->RecognizeOnceAsync("1").get();
            RequireIntentId(intentResult, "None");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("23").get();
            RequireIntentId(intentResult, "None");
            RequireEntity(intentResult, "builtin.number", "23");
        }

        SECTION("Tests should pass")
        {
            auto intentResult = intentRecognizer->RecognizeOnceAsync("minimize this").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize edge").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize window").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize this").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize notepad").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Application.Any", "notepad");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize paint").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Application.Any", "paint");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore this").get();
            RequireIntentId(intentResult, "Task.Restore");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore notepad").get();
            RequireIntentId(intentResult, "Task.Restore");
            RequireEntity(intentResult, "Application.Any", "notepad");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore paint").get();
            RequireIntentId(intentResult, "Task.Restore");
            RequireEntity(intentResult, "Application.Any", "paint");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to file explorer").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "file explorer");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to wordpad").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("close this window").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "this window");
            intentResult = intentRecognizer->RecognizeOnceAsync("close file explorer").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Application.Any", "file explorer");
            intentResult = intentRecognizer->RecognizeOnceAsync("close paint").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Application.Any", "paint");
            intentResult = intentRecognizer->RecognizeOnceAsync("list apps").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("list all windows").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("show all windows").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("show windows").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("show desktop").get();
            RequireIntentId(intentResult, "Task.ShowDesktop");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize all windows").get();
            RequireIntentId(intentResult, "Task.ShowDesktop");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap notepad to left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "notepad");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap file explorer to right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "file explorer");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap paint to top left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "paint");
            RequireEntity(intentResult, "Move.Direction", "top left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap calculator to top right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "calculator");
            RequireEntity(intentResult, "Move.Direction", "top right");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap file explorer to bottom left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "file explorer");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap paint to bottom right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "paint");
            RequireEntity(intentResult, "Move.Direction", "bottom right");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to top left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "top left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to top right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "top right");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to bottom left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap this to bottom right").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Move.Direction", "bottom right");
            intentResult = intentRecognizer->RecognizeOnceAsync("take screenshot").get();
            RequireIntentId(intentResult, "Task.Screenshot");
            intentResult = intentRecognizer->RecognizeOnceAsync("capture screenshot").get();
            RequireIntentId(intentResult, "Task.Screenshot");
            intentResult = intentRecognizer->RecognizeOnceAsync("show keyboard").get();
            RequireIntentId(intentResult, "Task.TouchKeyboard.Show");
            intentResult = intentRecognizer->RecognizeOnceAsync("open on screen keyboard").get();
            RequireIntentId(intentResult, "Task.OnScreenKeyboard");
            RequireEntity(intentResult, "OnScreenKeyboard.State", "open");
            intentResult = intentRecognizer->RecognizeOnceAsync("close on screen keyboard").get();
            RequireIntentId(intentResult, "Task.OnScreenKeyboard");
            RequireEntity(intentResult, "OnScreenKeyboard.State", "close");
            intentResult = intentRecognizer->RecognizeOnceAsync("hide keyboard").get();
            RequireIntentId(intentResult, "Task.TouchKeyboard.Hide");
            intentResult = intentRecognizer->RecognizeOnceAsync("press control").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "control");
            intentResult = intentRecognizer->RecognizeOnceAsync("press shift").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("press a").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "a");
            intentResult = intentRecognizer->RecognizeOnceAsync("press control shift").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "control shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("press windows tab").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "windows tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("press control plus a").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "control plus a");
            intentResult = intentRecognizer->RecognizeOnceAsync("press shift control right").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "shift control right");
            intentResult = intentRecognizer->RecognizeOnceAsync("press windows plus control plus tab").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "windows plus control plus tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("press alt tab two times").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "alt tab two times");
            intentResult = intentRecognizer->RecognizeOnceAsync("press shift control tab five times").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "shift control tab five times");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold control").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "control");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold shift").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold a").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "a");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold control shift").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "control shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold windows tab").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "windows tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold control plus a").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "control plus a");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold shift control right").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "shift control right");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold windows plus control plus tab").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "windows plus control plus tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("release control").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "control");
            intentResult = intentRecognizer->RecognizeOnceAsync("release shift").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("release a").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "a");
            intentResult = intentRecognizer->RecognizeOnceAsync("release control shift").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "control shift");
            intentResult = intentRecognizer->RecognizeOnceAsync("release windows tab").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "windows tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("release control plus a").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "control plus a");
            intentResult = intentRecognizer->RecognizeOnceAsync("release shift control right").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "shift control right");
            intentResult = intentRecognizer->RecognizeOnceAsync("release windows plus control plus tab").get();
            RequireIntentId(intentResult, "Task.Release");
            RequireEntity(intentResult, "Keyboard.Keys", "windows plus control plus tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Create new tab.").get();
            RequireIntentId(intentResult, "Task.Browser.NewTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("New tab").get();
            RequireIntentId(intentResult, "Task.Browser.NewTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open new window").get();
            RequireIntentId(intentResult, "Task.Browser.NewWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open a new window").get();
            RequireIntentId(intentResult, "Task.Browser.NewWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close this tab").get();
            RequireIntentId(intentResult, "Task.Browser.CloseTab");
            RequireEntity(intentResult, "Tab Locator", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close tabs to the right").get();
            RequireIntentId(intentResult, "Task.Browser.CloseTab");
            RequireEntity(intentResult, "Tab Locator", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close other tabs").get();
            RequireIntentId(intentResult, "Task.Browser.CloseTab");
            RequireEntity(intentResult, "Tab Locator", "other");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close third tab").get();
            RequireIntentId(intentResult, "Task.Browser.CloseTab");
            RequireEntity(intentResult, "Tab Locator", "third");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open previously closed tab").get();
            RequireIntentId(intentResult, "Task.Browser.PreviouslyClosedTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to first tab").get();
            RequireIntentId(intentResult, "Task.Browser.GoToNthTab");
            RequireEntity(intentResult, "Tab Locator", "first");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move this tab to left.").get();
            RequireIntentId(intentResult, "Task.Browser.RearrangeTabs");
            RequireEntity(intentResult, "Tab Locator", "this");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move third tab to left.").get();
            RequireIntentId(intentResult, "Task.Browser.RearrangeTabs");
            RequireEntity(intentResult, "Tab Locator", "third");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move this tab to right.").get();
            RequireIntentId(intentResult, "Task.Browser.RearrangeTabs");
            RequireEntity(intentResult, "Tab Locator", "this");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move first tab to right.").get();
            RequireIntentId(intentResult, "Task.Browser.RearrangeTabs");
            RequireEntity(intentResult, "Tab Locator", "first");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move this tab to new window.").get();
            RequireIntentId(intentResult, "Task.Browser.MoveTabToNewWindow");
            intentResult = intentRecognizer->RecognizeOnceAsync("Find Italian on page").get();
            RequireIntentId(intentResult, "Task.Browser.FindOnPage");
            RequireEntity(intentResult, "FindWord", "italian");
            intentResult = intentRecognizer->RecognizeOnceAsync("next word").get();
            RequireIntentId(intentResult, "Task.Browser.NextWord");
            intentResult = intentRecognizer->RecognizeOnceAsync("previous word").get();
            RequireIntentId(intentResult, "Task.Browser.PreviousWord");
            intentResult = intentRecognizer->RecognizeOnceAsync("close find box").get();
            RequireIntentId(intentResult, "Task.Browser.CloseSearchBox");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to address bar").get();
            RequireIntentId(intentResult, "Task.Browser.AddressBar");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go back").get();
            RequireIntentId(intentResult, "Task.Browser.GoBack");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go forward").get();
            RequireIntentId(intentResult, "Task.Browser.GoForward");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to home page").get();
            RequireIntentId(intentResult, "Task.Browser.GoHome");
            intentResult = intentRecognizer->RecognizeOnceAsync("Pin this tab").get();
            RequireIntentId(intentResult, "Task.Browser.PinTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("unpin this tab").get();
            RequireIntentId(intentResult, "Task.Browser.UnpinTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bookmark page").get();
            RequireIntentId(intentResult, "Task.Browser.BookmarkPage");
            intentResult = intentRecognizer->RecognizeOnceAsync("mute the tab.").get();
            RequireIntentId(intentResult, "Task.Browser.MuteTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("unmute the tab.").get();
            RequireIntentId(intentResult, "Task.Browser.UnmuteTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Add to favorites.").get();
            RequireIntentId(intentResult, "Task.Browser.BookmarkPage");
            intentResult = intentRecognizer->RecognizeOnceAsync("Duplicate this tab.").get();
            RequireIntentId(intentResult, "Task.Browser.DuplicateTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Refresh this page.").get();
            RequireIntentId(intentResult, "Task.Browser.RefreshPage");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open guide").get();
            RequireIntentId(intentResult, "Task.OpenTutorial");
            intentResult = intentRecognizer->RecognizeOnceAsync("expand background").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "expand");
            RequireEntity(intentResult, "ControlName.Any", "background");
            intentResult = intentRecognizer->RecognizeOnceAsync("open background").get();
            RequireIntentId(intentResult, "Task.Open");
            RequireEntity(intentResult, "Application.Any", "background");
            intentResult = intentRecognizer->RecognizeOnceAsync("collapse background").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "collapse");
            RequireEntity(intentResult, "ControlName.Any", "background");
            intentResult = intentRecognizer->RecognizeOnceAsync("check allow all notifications").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "check");
            RequireEntity(intentResult, "ControlName.Any", "allow all notifications");
            intentResult = intentRecognizer->RecognizeOnceAsync("uncheck allow all notifications").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "uncheck");
            RequireEntity(intentResult, "ControlName.Any", "allow all notifications");
            intentResult = intentRecognizer->RecognizeOnceAsync("choose allow all notifications").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "choose");
            RequireEntity(intentResult, "ControlName.Any", "allow all notifications");
            intentResult = intentRecognizer->RecognizeOnceAsync("check change brightness automatically").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "check");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically");
            intentResult = intentRecognizer->RecognizeOnceAsync("uncheck change brightness automatically").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "uncheck");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically");
            intentResult = intentRecognizer->RecognizeOnceAsync("unchoose change brightness automatically").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "unchoose");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically");
            intentResult = intentRecognizer->RecognizeOnceAsync("toggle change brightness automatically when lighting changes").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "toggle");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically when lighting changes");
            intentResult = intentRecognizer->RecognizeOnceAsync("move slider right 5 times").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move brightness down").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "SliderName.Any", "brightness");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("move volume down 10 times").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "builtin.number", "10");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "SliderName.Any", "volume");
            intentResult = intentRecognizer->RecognizeOnceAsync("move brightness up 20 times").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "builtin.number", "20");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "SliderName.Any", "brightness");
            intentResult = intentRecognizer->RecognizeOnceAsync("show apps").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to apps").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("focus on search").get();
            RequireIntentId(intentResult, "Task.Focus");
            RequireEntity(intentResult, "Focus.Name", "search");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll up").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.Direction", "up");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll down").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll left").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll right").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("stop scrolling").get();
            RequireIntentId(intentResult, "Task.Scroll.StopScrolling");
            intentResult = intentRecognizer->RecognizeOnceAsync("start scrolling").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            intentResult = intentRecognizer->RecognizeOnceAsync("start scrolling right").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("start scrolling left").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("start scrolling up").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            RequireEntity(intentResult, "Move.Direction", "up");
            intentResult = intentRecognizer->RecognizeOnceAsync("start scrolling down").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll up five lines").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "up");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll down five lines").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll slower").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.State", "slower");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll faster").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Move.State", "faster");
            intentResult = intentRecognizer->RecognizeOnceAsync("page up").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Up", "page up");
            intentResult = intentRecognizer->RecognizeOnceAsync("page down").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Down", "page down");
            intentResult = intentRecognizer->RecognizeOnceAsync("document start").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "document start");
            intentResult = intentRecognizer->RecognizeOnceAsync("document end").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "document end");
            intentResult = intentRecognizer->RecognizeOnceAsync("Activate full screen mode").get();
            RequireIntentId(intentResult, "Task.Browser.FullScreen");
            intentResult = intentRecognizer->RecognizeOnceAsync("Exit full screen").get();
            RequireIntentId(intentResult, "Task.Browser.ExitFullScreen");
            intentResult = intentRecognizer->RecognizeOnceAsync("turn off microphone").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Off");
            intentResult = intentRecognizer->RecognizeOnceAsync("mute").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Sleep");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unmute").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.On");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open downloads").get();
            RequireIntentId(intentResult, "Task.Browser.Downloads");
            intentResult = intentRecognizer->RecognizeOnceAsync("show developer tools").get();
            RequireIntentId(intentResult, "Task.Browser.OpenDeveloperTools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Exit developer tools").get();
            RequireIntentId(intentResult, "Task.Browser.CloseDeveloperTools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show context menu").get();
            RequireIntentId(intentResult, "Task.Browser.ShowContextMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("open bookmarks page").get();
            RequireIntentId(intentResult, "Task.Browser.OpenBookMarkPage");
            intentResult = intentRecognizer->RecognizeOnceAsync("open favorites page").get();
            RequireIntentId(intentResult, "Task.Browser.OpenBookMarkPage");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open images in a new tab").get();
            RequireIntentId(intentResult, "Task.Browser.OpenInNewTab");
            RequireEntity(intentResult, "Open.Link", "images");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show context menu on tab").get();
            RequireIntentId(intentResult, "Task.Browser.ShowContextMenu");
            RequireEntity(intentResult, "Context.Menu.Region", "tab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show context menu on window").get();
            RequireIntentId(intentResult, "Task.Browser.ShowContextMenu");
            RequireEntity(intentResult, "Context.Menu.Region", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search on google for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.GoogleSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search on youtube for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.YouTubeSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search on bing for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.BingSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("middle click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "middle");
            intentResult = intentRecognizer->RecognizeOnceAsync("rotate screen to portrait").get();
            RequireIntentId(intentResult, "Task.RotateScreen");
            RequireEntity(intentResult, "Orientation", "portrait");
            intentResult = intentRecognizer->RecognizeOnceAsync("rotate screen to landscape").get();
            RequireIntentId(intentResult, "Task.RotateScreen");
            RequireEntity(intentResult, "Orientation", "landscape");
            intentResult = intentRecognizer->RecognizeOnceAsync("rotate screen to portrait flipped").get();
            RequireIntentId(intentResult, "Task.RotateScreen");
            RequireEntity(intentResult, "Orientation", "portrait flipped");
            intentResult = intentRecognizer->RecognizeOnceAsync("rotate screen to landscape flipped").get();
            RequireIntentId(intentResult, "Task.RotateScreen");
            RequireEntity(intentResult, "Orientation", "landscape flipped");
            intentResult = intentRecognizer->RecognizeOnceAsync("Exit context menu").get();
            RequireIntentId(intentResult, "Task.Browser.ExitContextMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("turn off voice access").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Close");
            intentResult = intentRecognizer->RecognizeOnceAsync("voice stop").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Sleep");
            intentResult = intentRecognizer->RecognizeOnceAsync("double click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("triple click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Tuple", "triple");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to wordpad").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("open wordpad").get();
            RequireIntentId(intentResult, "Task.Open");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("close grid").get();
            RequireIntentId(intentResult, "Task.Grid.Hide");
            intentResult = intentRecognizer->RecognizeOnceAsync("drag").get();
            RequireIntentId(intentResult, "Task.Grid.Drag");
            intentResult = intentRecognizer->RecognizeOnceAsync("hide grid").get();
            RequireIntentId(intentResult, "Task.Grid.Hide");
            intentResult = intentRecognizer->RecognizeOnceAsync("select that").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("select word").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("select this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("select previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("select next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("select document").get();
            RequireIntentId(intentResult, "Task.SelectAll");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold that").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold word").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("View commands").get();
            RequireIntentId(intentResult, "Task.ShowAllCommands");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unselect that").get();
            RequireIntentId(intentResult, "Task.UnselectThat");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize that").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize word").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("close window").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline that").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline this page").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline word").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline next 6 characters").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "builtin.number", "6");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "characters");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline previous 8 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "builtin.number", "8");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move mouse faster").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.State", "faster");
            intentResult = intentRecognizer->RecognizeOnceAsync("Drag mouse faster").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "drag");
            RequireEntity(intentResult, "Move.State", "faster");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy drag mouse faster").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "copy drag");
            RequireEntity(intentResult, "Move.State", "faster");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move mouse top right ten").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top right");
            RequireEntity(intentResult, "builtin.number", "10");
            intentResult = intentRecognizer->RecognizeOnceAsync("Drag mouse bottom left nineteen").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "drag");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            RequireEntity(intentResult, "builtin.number", "19");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy drag mouse down six").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "copy drag");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "6");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move mouse top right fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top right");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("Drag mouse bottom left slow").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "drag");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            RequireEntity(intentResult, "Move.State", "slow");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy drag mouse down fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "copy drag");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move mouse top right").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Drag mouse bottom left").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "drag");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy drag mouse down").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "copy drag");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of page").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of page").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of selection").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "selection");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of selection").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "selection");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to end of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to end of line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to end of page").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of page").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to end of paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to end of selection").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "selection");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of selection").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "selection");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move up 2 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "2");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go up 9 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "9");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move down 11 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "11");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go down 15 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "15");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move right 5 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "5");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go right 8 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "8");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move left 11 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "11");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left 19 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "19");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move up one line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go up 3 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move down one line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go down 3 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move up one paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go up 3 paragraphs").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "paragraphs");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move down one paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go down 3 paragraphs").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "paragraphs");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move down one page").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "page");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go down 3 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move up one paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go up 3 paragraphs").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "up");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "paragraphs");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move left one character").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "character");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left 3 character").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "character");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move right one character").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "character");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go right 3 character").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "character");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move left one word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left 3 word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move right one word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go right 3 word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move left one line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left 3 line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move right one line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go right 3 line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "right");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize").get();
            RequireIntentId(intentResult, "Task.Minimize");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize").get();
            RequireIntentId(intentResult, "Task.Maximize");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click on restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show numbers here").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            RequireEntity(intentResult, "Overlay.Scope", "here");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show numbers everywhere").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            RequireEntity(intentResult, "Overlay.Scope", "everywhere");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show numbers on mail").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            RequireEntity(intentResult, "Overlay.Application.Any", "mail");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show numbers").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy that").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete that").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut that").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy this line").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut this word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut 9th word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "builtin.ordinal", "9");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete 11th paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "builtin.ordinal", "11");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("close this").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy line").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut previous 3 words").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "builtin.number", "3");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete next 11 paragraphs").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "11");
            RequireEntity(intentResult, "Dictation.Scope", "paragraphs");
            intentResult = intentRecognizer->RecognizeOnceAsync("Grid 9").get();
            RequireIntentId(intentResult, "Task.Grid.DrillDown");
            RequireEntity(intentResult, "builtin.number", "9");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mouse grid 12").get();
            RequireIntentId(intentResult, "Task.Grid.DrillDown");
            RequireEntity(intentResult, "builtin.number", "12");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open voice access guide").get();
            RequireIntentId(intentResult, "Task.OpenTutorial");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show all commands").get();
            RequireIntentId(intentResult, "Task.ShowAllCommands");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize that").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize word").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("close this window").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "this window");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase that").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase that").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cancel").get();
            RequireIntentId(intentResult, "Task.Cancel");
            intentResult = intentRecognizer->RecognizeOnceAsync("Dismiss").get();
            RequireIntentId(intentResult, "Task.Dismiss");
            intentResult = intentRecognizer->RecognizeOnceAsync("Stop").get();
            RequireIntentId(intentResult, "Task.Stop");
            intentResult = intentRecognizer->RecognizeOnceAsync("stop moving mouse").get();
            RequireIntentId(intentResult, "Task.Mouse.StopMoving");
            intentResult = intentRecognizer->RecognizeOnceAsync("stop dragging mouse").get();
            RequireIntentId(intentResult, "Task.Mouse.StopMoving");
            intentResult = intentRecognizer->RecognizeOnceAsync("stop copy dragging mouse").get();
            RequireIntentId(intentResult, "Task.Mouse.StopMoving");
            intentResult = intentRecognizer->RecognizeOnceAsync("default mode").get();
            RequireIntentId(intentResult, "Task.Default.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("command mode").get();
            RequireIntentId(intentResult, "Task.Command.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("dictation mode").get();
            RequireIntentId(intentResult, "Task.Dictation.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("Type what ever is said").get();
            RequireIntentId(intentResult, "Task.Dictation.Type");
            RequireEntity(intentResult, "Dictation.Text.Any", "what ever is said");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bold microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete microsoft technical documentation").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical documentation");
            intentResult = intentRecognizer->RecognizeOnceAsync("open start").get();
            RequireIntentId(intentResult, "Task.Open.StartMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("open start menu").get();
            RequireIntentId(intentResult, "Task.Open.StartMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("open search").get();
            RequireIntentId(intentResult, "Task.Open.SearchMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("open search menu").get();
            RequireIntentId(intentResult, "Task.Open.SearchMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("Voice access wake up").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.On");
            intentResult = intentRecognizer->RecognizeOnceAsync("open voice access settings").get();
            RequireIntentId(intentResult, "Task.Open.VoiceAccessSettings");
            intentResult = intentRecognizer->RecognizeOnceAsync("open voice access help").get();
            RequireIntentId(intentResult, "Task.Open.VoiceAccessHelp");
            intentResult = intentRecognizer->RecognizeOnceAsync("open help").get();
            RequireIntentId(intentResult, "Task.Open.VoiceAccessHelp");
            intentResult = intentRecognizer->RecognizeOnceAsync("go forward 2 words").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "forward");
            RequireEntity(intentResult, "builtin.number", "2");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("go backward 13 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "backward");
            RequireEntity(intentResult, "builtin.number", "13");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("move forward 2 words").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "forward");
            RequireEntity(intentResult, "builtin.number", "2");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("move backward 13 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "backward");
            RequireEntity(intentResult, "builtin.number", "13");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("press shift control left").get();
            RequireIntentId(intentResult, "Task.Press");
            RequireEntity(intentResult, "Keyboard.Keys", "shift control left");
            intentResult = intentRecognizer->RecognizeOnceAsync("select last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("capitalize last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("copy last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("cut last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("delete last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("lowercase last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("upper case last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("lower case last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("under line last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("select previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("select next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("bold next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("capitalize previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("capitalize next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("copy previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("copy next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("cut previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("cut next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("delete previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("delete next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("italicize next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("lowercase previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("lowercase next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("underline next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("uppercase next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("slow").get();
            RequireIntentId(intentResult, "Task.Slow");
            intentResult = intentRecognizer->RecognizeOnceAsync("slower").get();
            RequireIntentId(intentResult, "Task.Slow");
            intentResult = intentRecognizer->RecognizeOnceAsync("fast").get();
            RequireIntentId(intentResult, "Task.Fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("faster").get();
            RequireIntentId(intentResult, "Task.Fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to American multinational technology company").get();
            RequireIntentId(intentResult, "Task.Common.GoTo");
            RequireEntity(intentResult, "Any.Entity", "american multinational technology company");
            intentResult = intentRecognizer->RecognizeOnceAsync("goto American multinational technology company").get();
            RequireIntentId(intentResult, "Task.Common.GoTo");
            RequireEntity(intentResult, "Any.Entity", "american multinational technology company");
            intentResult = intentRecognizer->RecognizeOnceAsync("up").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "up");
            intentResult = intentRecognizer->RecognizeOnceAsync("down").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("top right").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "top right");
            intentResult = intentRecognizer->RecognizeOnceAsync("top left").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "top left");
            intentResult = intentRecognizer->RecognizeOnceAsync("bottom left").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "bottom left");
            intentResult = intentRecognizer->RecognizeOnceAsync("bottom right").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "bottom right");
            intentResult = intentRecognizer->RecognizeOnceAsync("left").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("right").get();
            RequireIntentId(intentResult, "Task.MoveDirection");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll five lines up").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "up");
            intentResult = intentRecognizer->RecognizeOnceAsync("scroll five lines down").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "down");
            intentResult = intentRecognizer->RecognizeOnceAsync("continue scrolling").get();
            RequireIntentId(intentResult, "Task.Scroll.StartScrolling");
            intentResult = intentRecognizer->RecognizeOnceAsync("start of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "start of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("end of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "end of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("focus the search").get();
            RequireIntentId(intentResult, "Task.Focus");
            RequireEntity(intentResult, "Focus.Name", "search");
            intentResult = intentRecognizer->RecognizeOnceAsync("list all apps").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore window").get();
            RequireIntentId(intentResult, "Task.Restore");
            intentResult = intentRecognizer->RecognizeOnceAsync("left tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("right tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore this window").get();
            RequireIntentId(intentResult, "Task.Restore");
            intentResult = intentRecognizer->RecognizeOnceAsync("left tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("right tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("middle tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Move.Direction", "middle");
            intentResult = intentRecognizer->RecognizeOnceAsync("double tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("triple tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Tuple", "triple");
            intentResult = intentRecognizer->RecognizeOnceAsync("tap on restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("tap restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide").get();
            RequireIntentId(intentResult, "Task.Hide");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide this").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide edge").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide window").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Remove").get();
            RequireIntentId(intentResult, "Task.Minimize");
            intentResult = intentRecognizer->RecognizeOnceAsync("Remove this").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("Remove edge").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Remove window").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Minimise").get();
            RequireIntentId(intentResult, "Task.Minimize");
            intentResult = intentRecognizer->RecognizeOnceAsync("Minimise this").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "this");
            intentResult = intentRecognizer->RecognizeOnceAsync("Minimise edge").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Minimise window").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("list windows").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show Task Switcher").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to desktop").get();
            RequireIntentId(intentResult, "Task.ShowDesktop");
            intentResult = intentRecognizer->RecognizeOnceAsync("show").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            intentResult = intentRecognizer->RecognizeOnceAsync("show notepad").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "notepad");
            intentResult = intentRecognizer->RecognizeOnceAsync("show paint").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "paint");
            intentResult = intentRecognizer->RecognizeOnceAsync("show edge").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to search").get();
            RequireIntentId(intentResult, "Task.Focus");
            RequireEntity(intentResult, "Focus.Name", "search");
            intentResult = intentRecognizer->RecognizeOnceAsync("move slider left").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Stop voice access").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Close");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close voice access").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Close");
            intentResult = intentRecognizer->RecognizeOnceAsync("listen to me").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.On");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to the start of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to the start of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to the start of the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to the start of the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to left by 5 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "5");
            intentResult = intentRecognizer->RecognizeOnceAsync("move left by 5 times").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "5");
            intentResult = intentRecognizer->RecognizeOnceAsync("move down by a paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "down");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("move forward by a word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "forward");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("move left by 3 characters").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "Dictation.Scope", "characters");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to beginning of line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "beginning");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to end of the word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to the end of the word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to the end of word").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "end");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to start of the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go to start of the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "start");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left 4").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "4");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go left by 4").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "4");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse top left fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top left");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to top left fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top left");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to top left fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top left");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse top left fast").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "top left");
            RequireEntity(intentResult, "Move.State", "fast");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse left 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to left 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to left 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse left by 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to left by 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to left by 3").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse left by 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to left by 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to left by 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse left 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to left 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to left 3 times").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            RequireEntity(intentResult, "builtin.number", "3");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move the mouse faster").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.State", "faster");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse to left").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("move mouse to left").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the mouse left").get();
            RequireIntentId(intentResult, "Task.Mouse.Movements");
            RequireEntity(intentResult, "Mouse.Movement.Type", "move");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the slider to right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the brightness to right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "SliderName.Any", "brightness");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the slider right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the brightness right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "SliderName.Any", "brightness");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move slider to right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the slider to right 5 times").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the slider to right by 5 times").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move the brightness right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "SliderName.Any", "brightness");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("move slider to right").get();
            RequireIntentId(intentResult, "Task.MoveSlider");
            RequireEntity(intentResult, "Slider.Optional", "slider");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mark").get();
            RequireIntentId(intentResult, "Task.Grid.Mark");
            intentResult = intentRecognizer->RecognizeOnceAsync("start of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "start of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to start of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to start of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to the start of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to the start of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to start of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to start of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to the end of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to the end of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to end of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to end of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to end of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to end of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to the end of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to the end of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("end of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "end of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move after cats are cute").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "after");
            RequireEntity(intentResult, "ControlName.Any", "cats are cute");
            intentResult = intentRecognizer->RecognizeOnceAsync("move before cats are cute").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Direction", "before");
            RequireEntity(intentResult, "ControlName.Any", "cats are cute");
            intentResult = intentRecognizer->RecognizeOnceAsync("focus search").get();
            RequireIntentId(intentResult, "Task.Focus");
            RequireEntity(intentResult, "Focus.Name", "search");
            intentResult = intentRecognizer->RecognizeOnceAsync("flip change brightness automatically when lighting changes").get();
            RequireIntentId(intentResult, "Task.InvokeControl");
            RequireEntity(intentResult, "Invoke.Actions", "flip");
            RequireEntity(intentResult, "ControlName.Any", "change brightness automatically when lighting changes");
            intentResult = intentRecognizer->RecognizeOnceAsync("stop moving").get();
            RequireIntentId(intentResult, "Task.Mouse.StopMoving");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold down control").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "control");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold down exclamation point").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Special.Keyboard.Keys", "exclamation point");
            intentResult = intentRecognizer->RecognizeOnceAsync("hold down a").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "a");
            intentResult = intentRecognizer->RecognizeOnceAsync("press and hold control").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "control");
            intentResult = intentRecognizer->RecognizeOnceAsync("press and hold exclamation point").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Special.Keyboard.Keys", "exclamation point");
            intentResult = intentRecognizer->RecognizeOnceAsync("press and hold a").get();
            RequireIntentId(intentResult, "Task.Hold");
            RequireEntity(intentResult, "Keyboard.Keys", "a");
            intentResult = intentRecognizer->RecognizeOnceAsync("show grid").get();
            RequireIntentId(intentResult, "Task.Grid.Show");
            intentResult = intentRecognizer->RecognizeOnceAsync("show grid here").get();
            RequireIntentId(intentResult, "Task.Grid.Show");
            RequireEntity(intentResult, "Overlay.Scope", "here");
            intentResult = intentRecognizer->RecognizeOnceAsync("show grid everywhere").get();
            RequireIntentId(intentResult, "Task.Grid.Show");
            RequireEntity(intentResult, "Overlay.Scope", "everywhere");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show grid on mail").get();
            RequireIntentId(intentResult, "Task.Grid.Show");
            RequireEntity(intentResult, "Overlay.Application.Any", "mail");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mouse grid").get();
            RequireIntentId(intentResult, "Task.Grid.Show");
            intentResult = intentRecognizer->RecognizeOnceAsync("add new tab.").get();
            RequireIntentId(intentResult, "Task.Browser.NewTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("open new tab").get();
            RequireIntentId(intentResult, "Task.Browser.NewTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("Switch to first tab").get();
            RequireIntentId(intentResult, "Task.Browser.GoToNthTab");
            RequireEntity(intentResult, "Tab Locator", "first");
            intentResult = intentRecognizer->RecognizeOnceAsync("Move to first tab").get();
            RequireIntentId(intentResult, "Task.Browser.GoToNthTab");
            RequireEntity(intentResult, "Tab Locator", "first");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy").get();
            RequireIntentId(intentResult, "Task.Copy");
            intentResult = intentRecognizer->RecognizeOnceAsync("select all").get();
            RequireIntentId(intentResult, "Task.SelectAll");
            intentResult = intentRecognizer->RecognizeOnceAsync("Find Italian on this page").get();
            RequireIntentId(intentResult, "Task.Browser.FindOnPage");
            RequireEntity(intentResult, "FindWord", "italian");
            intentResult = intentRecognizer->RecognizeOnceAsync("Find Italian").get();
            RequireIntentId(intentResult, "Task.Browser.FindOnPage");
            RequireEntity(intentResult, "FindWord", "italian");
            intentResult = intentRecognizer->RecognizeOnceAsync("Go home").get();
            RequireIntentId(intentResult, "Task.ShowDesktop");
            intentResult = intentRecognizer->RecognizeOnceAsync("Duplicate tab.").get();
            RequireIntentId(intentResult, "Task.Browser.DuplicateTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("select all").get();
            RequireIntentId(intentResult, "Task.SelectAll");
            intentResult = intentRecognizer->RecognizeOnceAsync("Scratch that").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("clear selection").get();
            RequireIntentId(intentResult, "Task.UnselectThat");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps microsoft technical report").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "ControlName.Any", "microsoft technical report");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps that").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("upper case last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("All caps next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("tap 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps that").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("lower case last four words").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "last");
            RequireEntity(intentResult, "builtin.number", "4");
            RequireEntity(intentResult, "Dictation.Scope", "words");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps previous word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("No caps next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("move back 13 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "back");
            RequireEntity(intentResult, "builtin.number", "13");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("go back 13 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Goto");
            RequireEntity(intentResult, "Move.Direction", "back");
            RequireEntity(intentResult, "builtin.number", "13");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show numbers on taskbar").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            RequireEntity(intentResult, "Overlay.Application.Any", "taskbar");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mark 1").get();
            RequireIntentId(intentResult, "Task.Grid.Mark");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click 23").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "23");
            intentResult = intentRecognizer->RecognizeOnceAsync("double click 23").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "23");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("right click 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("tap 23").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "23");
            intentResult = intentRecognizer->RecognizeOnceAsync("show all apps").get();
            RequireIntentId(intentResult, "Task.ListAllWindows");
            intentResult = intentRecognizer->RecognizeOnceAsync("Voice wake up").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.On");
            intentResult = intentRecognizer->RecognizeOnceAsync("voice sleep").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Sleep");
            intentResult = intentRecognizer->RecognizeOnceAsync("Voice access sleep").get();
            RequireIntentId(intentResult, "Task.VoiceAccess.Sleep");
            intentResult = intentRecognizer->RecognizeOnceAsync("What can I say").get();
            RequireIntentId(intentResult, "Task.ShowAllCommands");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to top").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to top");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to beginning").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to beginning");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to top").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to top");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to beginning").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to beginning");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to top of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to top of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to beginning of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to beginning of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to top of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to top of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to beginning of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to beginning of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to bottom").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to bottom");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to end").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to end");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to bottom").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "move to bottom");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to end").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "move to end");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to bottom of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to bottom of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to end of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "go to end of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to bottom of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "move to bottom of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to end of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "move to end of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            intentResult = intentRecognizer->RecognizeOnceAsync("Tap").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close tab").get();
            RequireIntentId(intentResult, "Task.Browser.CloseTab");
            intentResult = intentRecognizer->RecognizeOnceAsync("close").get();
            RequireIntentId(intentResult, "Task.Close");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut").get();
            RequireIntentId(intentResult, "Task.Cut");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize it").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "it");
            intentResult = intentRecognizer->RecognizeOnceAsync("close it").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "it");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize it").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Optional.Application.Words", "it");
            intentResult = intentRecognizer->RecognizeOnceAsync("restore it").get();
            RequireIntentId(intentResult, "Task.Restore");
            RequireEntity(intentResult, "Optional.Application.Words", "it");
            intentResult = intentRecognizer->RecognizeOnceAsync("undo").get();
            RequireIntentId(intentResult, "Task.Undo");
            intentResult = intentRecognizer->RecognizeOnceAsync("undo that").get();
            RequireIntentId(intentResult, "Task.Undo");
            intentResult = intentRecognizer->RecognizeOnceAsync("redo").get();
            RequireIntentId(intentResult, "Task.Redo");
            intentResult = intentRecognizer->RecognizeOnceAsync("redo that").get();
            RequireIntentId(intentResult, "Task.Redo");
            intentResult = intentRecognizer->RecognizeOnceAsync("Stop that").get();
            RequireIntentId(intentResult, "Task.Stop");
            intentResult = intentRecognizer->RecognizeOnceAsync("Stop this").get();
            RequireIntentId(intentResult, "Task.Stop");
            intentResult = intentRecognizer->RecognizeOnceAsync("open the wordpad").get();
            RequireIntentId(intentResult, "Task.Open");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("start the wordpad").get();
            RequireIntentId(intentResult, "Task.Start");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to the wordpad").get();
            RequireIntentId(intentResult, "Task.SwitchApp");
            RequireEntity(intentResult, "Application.Any", "wordpad");
            intentResult = intentRecognizer->RecognizeOnceAsync("open the start menu").get();
            RequireIntentId(intentResult, "Task.Open.StartMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("open the search menu").get();
            RequireIntentId(intentResult, "Task.Open.SearchMenu");
            intentResult = intentRecognizer->RecognizeOnceAsync("Increase volume").get();
            RequireIntentId(intentResult, "Task.VolumeUp");
            intentResult = intentRecognizer->RecognizeOnceAsync("Increase the volume").get();
            RequireIntentId(intentResult, "Task.VolumeUp");
            intentResult = intentRecognizer->RecognizeOnceAsync("Decrease volume").get();
            RequireIntentId(intentResult, "Task.VolumeDown");
            intentResult = intentRecognizer->RecognizeOnceAsync("Decrease the volume").get();
            RequireIntentId(intentResult, "Task.VolumeDown");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mute volume").get();
            RequireIntentId(intentResult, "Task.VolumeMute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mute the volume").get();
            RequireIntentId(intentResult, "Task.VolumeMute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mute audio").get();
            RequireIntentId(intentResult, "Task.VolumeMute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Mute the audio").get();
            RequireIntentId(intentResult, "Task.VolumeMute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unmute volume").get();
            RequireIntentId(intentResult, "Task.VolumeUnmute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unmute the volume").get();
            RequireIntentId(intentResult, "Task.VolumeUnmute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unmute audio").get();
            RequireIntentId(intentResult, "Task.VolumeUnmute");
            intentResult = intentRecognizer->RecognizeOnceAsync("Unmute the audio").get();
            RequireIntentId(intentResult, "Task.VolumeUnmute");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize the edge").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("maximize the window").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("copy tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("cut tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("delete tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Italicize tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase tools").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "ControlName.Any", "tools");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search google for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.GoogleSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search youtube for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.YouTubeSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("Search bing for java tutorials").get();
            RequireIntentId(intentResult, "Task.WebSearch.BingSearch");
            RequireEntity(intentResult, "Search.Phrase", "java tutorials");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to dictation mode").get();
            RequireIntentId(intentResult, "Task.Dictation.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("Open images in new tab").get();
            RequireIntentId(intentResult, "Task.Browser.OpenInNewTab");
            RequireEntity(intentResult, "Open.Link", "images");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to default mode").get();
            RequireIntentId(intentResult, "Task.Default.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("switch to command mode").get();
            RequireIntentId(intentResult, "Task.Command.Mode");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Copy the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Copy");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Lowercase the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Lowercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Capitalize the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Capitalize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Cut the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Cut");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Delete the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("scratch the word").get();
            RequireIntentId(intentResult, "Task.Dictation.Delete");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Underline the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Underline");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Italicize the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Italicize the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Italicize the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Italicize the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Italicize");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Uppercase the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Uppercase");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Select the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Select");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bold the line").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bold the next 5 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bold the 1st line").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "builtin.ordinal", "1");
            RequireEntity(intentResult, "Dictation.Scope", "line");
            intentResult = intentRecognizer->RecognizeOnceAsync("Bold the next word").get();
            RequireIntentId(intentResult, "Task.Dictation.Bold");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("left click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("double click on 1").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize the edge").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("minimize the window").get();
            RequireIntentId(intentResult, "Task.Minimize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Maximize the edge").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Maximize the window").get();
            RequireIntentId(intentResult, "Task.Maximize");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Restore the edge").get();
            RequireIntentId(intentResult, "Task.Restore");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Restore the window").get();
            RequireIntentId(intentResult, "Task.Restore");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close the edge").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Application.Any", "edge");
            intentResult = intentRecognizer->RecognizeOnceAsync("Close the window").get();
            RequireIntentId(intentResult, "Task.Close");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            intentResult = intentRecognizer->RecognizeOnceAsync("show touch keyboard").get();
            RequireIntentId(intentResult, "Task.TouchKeyboard.Show");
            intentResult = intentRecognizer->RecognizeOnceAsync("hide touch keyboard").get();
            RequireIntentId(intentResult, "Task.TouchKeyboard.Hide");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click here").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "Optional.ControlNames", "here");
            intentResult = intentRecognizer->RecognizeOnceAsync("Click on the restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            intentResult = intentRecognizer->RecognizeOnceAsync("double click on the restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Tuple", "double");
            intentResult = intentRecognizer->RecognizeOnceAsync("right click on the restaurants near me").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "ControlName.Any", "restaurants near me");
            RequireEntity(intentResult, "Move.Direction", "right");
            intentResult = intentRecognizer->RecognizeOnceAsync("drag here").get();
            RequireIntentId(intentResult, "Task.Grid.Drag");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap notepad to the left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "notepad");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap the notepad to the left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "notepad");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap the notepad to left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Application.Any", "notepad");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap window to the left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap the window to the left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("snap the window to left").get();
            RequireIntentId(intentResult, "Task.Move");
            RequireEntity(intentResult, "Optional.Application.Words", "window");
            RequireEntity(intentResult, "Move.Direction", "left");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show number here").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            RequireEntity(intentResult, "Overlay.Scope", "here");
            intentResult = intentRecognizer->RecognizeOnceAsync("Show number").get();
            RequireIntentId(intentResult, "Task.Overlay.ShowNumbers");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide number").get();
            RequireIntentId(intentResult, "Task.Overlay.HideNumbers");
            intentResult = intentRecognizer->RecognizeOnceAsync("Hide numbers").get();
            RequireIntentId(intentResult, "Task.Overlay.HideNumbers");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to start of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to start of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to the start of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to the start of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to start of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to start of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to beginning of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to beginning of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to the beginning of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to the beginning of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("move to beginning of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "move to beginning of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to the beginning of document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to the beginning of document");
            intentResult = intentRecognizer->RecognizeOnceAsync("go to beginning of the document").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "go to beginning of the document");
            intentResult = intentRecognizer->RecognizeOnceAsync("Scroll to top").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "scroll to top");
            intentResult = intentRecognizer->RecognizeOnceAsync("Scroll to the top").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.Start", "scroll to the top");
            intentResult = intentRecognizer->RecognizeOnceAsync("Scroll to bottom").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "scroll to bottom");
            intentResult = intentRecognizer->RecognizeOnceAsync("Scroll to the bottom").get();
            RequireIntentId(intentResult, "Task.Scroll");
            RequireEntity(intentResult, "Page.End", "scroll to the bottom");
            intentResult = intentRecognizer->RecognizeOnceAsync("spell that").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("spell word").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("spell this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("spell previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("spell next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("correct that").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "that");
            intentResult = intentRecognizer->RecognizeOnceAsync("correct word").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "word");
            intentResult = intentRecognizer->RecognizeOnceAsync("correct this paragraph").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "Dictation.Scope", "paragraph");
            intentResult = intentRecognizer->RecognizeOnceAsync("correct previous 5 pages").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "builtin.number", "5");
            RequireEntity(intentResult, "Direction", "previous");
            RequireEntity(intentResult, "Dictation.Scope", "pages");
            intentResult = intentRecognizer->RecognizeOnceAsync("correct next 66 lines").get();
            RequireIntentId(intentResult, "Task.Dictation.Spell");
            RequireEntity(intentResult, "builtin.number", "66");
            RequireEntity(intentResult, "Direction", "next");
            RequireEntity(intentResult, "Dictation.Scope", "lines");
            intentResult = intentRecognizer->RecognizeOnceAsync("click one").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
            intentResult = intentRecognizer->RecognizeOnceAsync("click on one").get();
            RequireIntentId(intentResult, "Task.Mouse.Click");
            RequireEntity(intentResult, "builtin.number", "1");
        }
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Required entity groups", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    WHEN("There is an intent with a required group at the beginning")
    {
        intentRecognizer->AddIntent("(Open|Start) {appName}", "open");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("Open Microsoft Word.").get();
        RequireIntentId(intentResult, "open");
        RequireEntity(intentResult, "appName", "microsoft word");
        intentResult = intentRecognizer->RecognizeOnceAsync("Start Microsoft Word.").get();
        RequireIntentId(intentResult, "open");
        RequireEntity(intentResult, "appName", "microsoft word");
    }

    WHEN("There is an intent with required group at the end")
    {
        intentRecognizer->AddIntent("this is my {any} (pattern|phrase)", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("this is my required pattern").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
        intentResult = intentRecognizer->RecognizeOnceAsync("this is my required phrase.").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
    }

    WHEN("There is an intent with required group next to another")
    {
        intentRecognizer->AddIntent("this is my (double|second) (pattern|phrase) {any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("this is my double pattern required").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
        intentResult = intentRecognizer->RecognizeOnceAsync("this is my second phrase required.").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
        intentResult = intentRecognizer->RecognizeOnceAsync("this is my first pattern required").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "any");
    }

    WHEN("There is an intent with required group entity")
    {
        intentRecognizer->AddIntent("this is my (double|{any2}) (pattern|phrase) {any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("this is my double pattern required").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
        intentResult = intentRecognizer->RecognizeOnceAsync("this is my second phrase required.").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "required");
        RequireEntity(intentResult, "any2", "second");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::UTF-8 characters", "[zh]")
{
    auto fromLanguage = "zh-cn";

    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    std::string modelId = "MyTestModel";
    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"天氣{something}麼樣"}, "test" });
    model->Intents.push_back({ {"氣{list}麼樣"}, "test" });
    model->Entities.push_back({ "list" , EntityType::List, EntityMatchMode::Strict, {"open", "close"} });
    models.push_back(model);
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    WHEN("There is an invalid utf8 character in an any entity.")
    {
        auto test = "天氣\xff""\xff""麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        // Although the intent is matched here. The entity extraction for invalid UTF8 characters
        // is undefined.
        RequireIntentId(intentResult, "test");
    }

    WHEN("There is an invalid utf8 character in an list entity.")
    {
        auto test = "氣\xff""\xff""麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "list");
    }

    WHEN("There is a utf8 pattern")
    {
        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "something", "怎");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::Intent id with escaped characters", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    constexpr auto test = "This is a test";
    std::string modelId = "MyTestModel";
    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {test}, "id\\2" });
    models.push_back(model);
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    auto intentResult = intentRecognizer->RecognizeOnceAsync("This is a test").get();
    RequireIntentId(intentResult, "id\\2");
}

TEST_CASE("IntentRecognizer::PatternMatching::Intent from continuous speech recognition", "[en][speech]")
{
    REQUIRE(exists(INTENT_UTTERANCE1));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE1);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    std::shared_ptr<IntentRecognitionResult> intentResult;
    std::mutex myMutex;

    speechRecognizer->Recognized.Connect([&speechRecognizer, &intentRecognizer, &intentResult, &myMutex](const SpeechRecognitionEventArgs& e)
        {
            if (e.Result->Reason != ResultReason::NoMatch)
            {
                auto recognizedText = e.Result->Text;
                std::lock_guard<std::mutex> lock(myMutex);

                if (recognizedText != "" && intentResult == nullptr)
                {
                    // Event handlers in general must NOT do SDK calls or run time-consuming operations.
                    // IntentRecognizer used here is not part of the SDK and returns quick so it's "ok".
                    intentResult = intentRecognizer->RecognizeOnceAsync(recognizedText).get();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });

    std::string modelId = "MyTestModel";
    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"turn on {any}{object}"}, "id2" }); // {any} for a/an/the
    model->Entities.push_back({ "object" , EntityType::List, EntityMatchMode::Strict, {"lamp", "lights", "TV"} });
    models.push_back(model);
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    speechRecognizer->StartContinuousRecognitionAsync();
    while (!intentResult)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    speechRecognizer->StopContinuousRecognitionAsync();

    RequireIntentId(intentResult, "id2");
    RequireEntity(intentResult, "object", "lamp");
}

TEST_CASE("IntentRecognizer::PatternMatching::Resolve ties in priority based on bytes matched", "[en]")
{
    auto intentRecognizer = IntentRecognizer::FromLanguage();

    std::string modelId = "MyTestModel";
    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"{shareList}"}, "Share3" });
    model->Entities.push_back({ "shareList" , EntityType::List, EntityMatchMode::Strict, {"share window list"} });

    models.push_back(model);

    intentRecognizer->AddIntent("share window {name}", "Share1");
    intentRecognizer->AddIntent("share window {name} with audio", "Share2");

    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    auto intentResult = intentRecognizer->RecognizeOnceAsync("share window edge with audio").get();
    RequireIntentId(intentResult, "Share2");
    RequireEntity(intentResult, "name", "edge");
    intentResult = intentRecognizer->RecognizeOnceAsync("share window list").get();
    RequireIntentId(intentResult, "Share3");
    RequireEntity(intentResult, "shareList", "share window list");
}

TEST_CASE("IntentRecognizer::PatternMatching::DE Punctuation", "[de][speech]")
{
    REQUIRE(exists(INTENT_DEDE_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_DEDE_UTTERANCE);
    auto fromLanguage = "de-de";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    intentRecognizer->AddIntent("{any} ist das Wetter", "test");
    auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
    auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "wie");
}

TEST_CASE("IntentRecognizer::PatternMatching::ES Punctuation", "[es][speech]")
{
    REQUIRE(exists(INTENT_ESES_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_ESES_UTTERANCE);
    auto fromLanguage = "es-es";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    // ¿Cómo está el tiempo?
    WHEN("There is an intent with 1 phrase and starting question mark")
    {
        intentRecognizer->AddIntent("¿{any} está el tiempo?", "test");
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "cómo");
    }

    WHEN("Starting punctuation mark is not accounted for in the Intent")
    {
        intentRecognizer->AddIntent("{any} está el tiempo?", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("¿Cómo está el tiempo?").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "cómo");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ES Prebuilt integer entities", "[es]")
{
    auto fromLanguage = "es-es";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    SECTION("Prebuilt number text")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {número}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "número" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        // "Open one hundred and thirty eight"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre ciento treinta y ocho").get();
        RequireIntentId(intentResult, "Abre");
        RequireEntity(intentResult, "número", "138");
    }

    SECTION("Prebuilt multiple number text")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {número}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "número" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        // "Open one two forty (and) one ten one hundred and thirty eight ninty seven"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre uno dos cuarenta uno diez ciento treinta y ocho noventa y siete").get();
        RequireIntentId(intentResult, "Abre");
        RequireEntity(intentResult, "número", "124011013897");
    }

    SECTION("Prebuilt multiple number text mixed with digits")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {número}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "número" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        // "Open one two forty (and) one ten 138 ninty seven"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre uno dos cuarenta uno diez 138 noventa y siete").get();
        RequireIntentId(intentResult, "Abre");
        RequireEntity(intentResult, "número", "124011013897");
    }

    SECTION("Prebuilt multiple number text with accents")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {número}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "número" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        // "Open one two forty (and) one ten 138 ninty seven"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre dieciséis").get();
        RequireIntentId(intentResult, "Abre");
        RequireEntity(intentResult, "número", "16");
    }

    SECTION("Prebuilt number text with non-number text")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {numero}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "numero" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre el numero veinticinco por favor").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "numero");
    }

    SECTION("Prebuilt number text with mispelled number")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"Abre {numero}"}, "Abre" });
        models.push_back(model);
        model->Entities.push_back({ "numero" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("Abre veinicinco").get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "numero");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::FR Punctuation", "[fr][speech]")
{
    REQUIRE(exists(INTENT_FRFR_UTTERANCE1));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_FRFR_UTTERANCE1);
    auto fromLanguage = "fr-fr";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    intentRecognizer->AddIntent("Comment est la {any} ?", "test");
    auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
    auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "météo");

    intentResult = intentRecognizer->RecognizeOnceAsync("[]{}()-‐‑–—,;\\:!?.…’\"“”«»§@ * / &#†‡․。︒﹒．｡'%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣－،٫、︐︑﹐﹑，､+⁺₊➕﬩﹢＋∶ʼ՚ߴ᾽᾿＇Comment est la météo ?").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "météo");

    // test with non-breaking space
    intentResult = intentRecognizer->RecognizeOnceAsync("Comment est la météo ?").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "météo");

    // test with narrow non-breaking space
    intentResult = intentRecognizer->RecognizeOnceAsync("Comment est la météo ?").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "météo");

    intentRecognizer->AddIntent("Si vous parlez {any} : {any2} me dire où se trouve la bibliothèque ?", "test");
    intentResult = intentRecognizer->RecognizeOnceAsync("Si vous parlez anglais : pouvez-vous me dire où se trouve la bibliothèque ?").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "anglais :");
    RequireEntity(intentResult, "any2", "pouvez-vous");
}

TEST_CASE("IntentRecognizer::PatternMatching::FR Prebuilt integer entities, text", "[fr]")
{
    auto fromLanguage = "fr-FR";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    auto model = PatternMatchingModel::FromModelId("FrenchIntegerTestModel");
    model->Entities.push_back({ "test_number", EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
    model->Intents.push_back({ { "Sélectionnez {test_number}" }, "test_intent_accent" });
    model->Intents.push_back({ { "Selectionnez {test_number}" }, "test_intent" });

    intentRecognizer->ApplyLanguageModels({ model });
    CHECK(model != nullptr);

    // now do the tests
    auto intentResult = intentRecognizer->RecognizeOnceAsync("Sélectionnez cent trente et un").get(); // Select 131
    RequireIntentId(intentResult, "test_intent_accent");
    RequireEntity(intentResult, "test_number", "131");

    intentResult = intentRecognizer->RecognizeOnceAsync("Selectionnez un trois un").get(); // Select 1 3 1
    RequireIntentId(intentResult, "test_intent");
    RequireEntity(intentResult, "test_number", "131");

    intentResult = intentRecognizer->RecognizeOnceAsync("Sélectionnez quarante quatre, soixante dix huit, vingt").get(); // Select 44 78 20
    RequireIntentId(intentResult, "test_intent_accent");
    RequireEntity(intentResult, "test_number", "447820");

    intentResult = intentRecognizer->RecognizeOnceAsync("Sélectionnez négatif cinquante neuf").get(); // Select negative 59
    RequireIntentId(intentResult, "test_intent_accent");
    RequireEntity(intentResult, "test_number", "-59");

    intentResult = intentRecognizer->RecognizeOnceAsync("Sélectionnez deux cent 31").get(); // Select 200 31
    RequireIntentId(intentResult, "test_intent_accent");
    RequireEntity(intentResult, "test_number", "231");

    intentResult = intentRecognizer->RecognizeOnceAsync("Sélectionnez cent vignt").get(); // spelling mistake
    RequireIntentId(intentResult, "");
    RequireNoEntity(intentResult, "test_number");
}

TEST_CASE("IntentRecognizer::PatternMatching::FR Prebuilt integer entities, speech", "[fr][speech]")
{
    REQUIRE(exists(INTENT_FRFR_UTTERANCE2));

    auto speechConfig = SpeechConfigForIntentTests();
    auto fromLanguage = "fr-FR";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);

    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_FRFR_UTTERANCE2);

    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    auto model = PatternMatchingModel::FromModelId("FrenchIntegerTestModel");
    model->Entities.push_back({ "test_number", EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
    model->Intents.push_back({ { "(Sélectionner|Sélectionnez) {test_number}" }, "test_intent" });

    intentRecognizer->ApplyLanguageModels({ model });
    CHECK(model != nullptr);

    auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
    auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
    RequireIntentId(intentResult, "test_intent");
    RequireEntity(intentResult, "test_number", "599");
}

TEST_CASE("IntentRecognizer::PatternMatching::JA Intent", "[ja][speech]")
{
    REQUIRE(exists(INTENT_JAJP_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_JAJP_UTTERANCE);
    auto fromLanguage = "ja-JP";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    constexpr auto test = "今日{test2}";
    intentRecognizer->AddIntent(test, "id2");

    auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
    auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
    RequireIntentId(intentResult, "id2");
}

TEST_CASE("IntentRecognizer::PatternMatching::JA Punctuation", "[ja][speech]")
{
    REQUIRE(exists(INTENT_JAJP_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_JAJP_UTTERANCE);
    auto fromLanguage = "ja-jp";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    intentRecognizer->AddIntent("{any}日は雨です。", "test");
    auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
    auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "今");

    intentResult = intentRecognizer->RecognizeOnceAsync("()[]{}=<>‾_＿\\-－‐‑—―〜・･,，、､;；:：!！?？.．‥…。｡＇‘’\"＂“”（）［］｛｝〈〉《》「｢」｣『』【】〔〕‖§¶@＠ * ＊ / ／＼ & ＆#＃†‡′″〃※∶․︒﹒'ʼ՚᾽᾿%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣،٫︐︑﹑+⁺₊➕﬩﹢＋﹐今日は雨です。").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "any", "今");
}

TEST_CASE("IntentRecognizer::PatternMatching::JA Prebuilt integer entities", "[ja][speech]")
{
    REQUIRE(exists(INTENT_UTTERANCE4));

    auto speechConfig = SpeechConfigForIntentTests();
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_UTTERANCE4);
    auto fromLanguage = "ja-JP";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    std::string modelIdAudio = "MyTestModel";
    auto model = PatternMatchingModel::FromModelId(modelIdAudio);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"{番号}行目まで選択"}, "番号" });
    model->Intents.push_back({ {"{番号}電話番号にかける"}, "番号" });
    model->Intents.push_back({ {"{番号}"}, "ける" });
    model->Entities.push_back({ "番号" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
    models.push_back(model);
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    SECTION("Prebuilt number text with non-number text (audio)")
    {
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "5");
    }

    SECTION("Prebuilt number text with non-number text (kanji)")
    {
        // "Select row number 5"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("五行目まで選択").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "5");
    }

    SECTION("Consecutive numbers single digits")
    {
        // "Call phone number 0123456789"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("れいいちにさんしごろくななはちきゅうまる電話番号にかける").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "01234567890");
    }

    SECTION("Consecutive numbers 3 digits")
    {
        // "Call phone number three hundred seventy one, six hundred ninty one, eight hundred"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("三百七十一六百九十一八百四十電話番号にかける").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "371691840");
    }

    SECTION("Direct input half width digits")
    {
        // "Call phone number 0123456789"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("0123456789電話番号にかける").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "0123456789");
    }

    SECTION("Direct input full width digits")
    {
        // "Call phone number 0123456789"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("０１２３４５６７８９電話番号にかける").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "0123456789");
    }

    SECTION("All types mixed digits")
    {
        // "Call phone number 0 1 2 3 4 five sixty seven eight nine"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("０１２34ご六十七ハチ九電話番号にかける").get();
        RequireIntentId(intentResult, "番号");
        RequireEntity(intentResult, "番号", "0123456789");
    }

    SECTION("No digits")
    {
        // "Call phone number 0 1 2 3 4 five sixty seven eight nine"
        auto intentResult = intentRecognizer->RecognizeOnceAsync("電話番号").get();
        RequireIntentId(intentResult, "");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH AnyEntity", "[zh]")
{
    auto fromLanguage = "zh-hk";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    WHEN("There is an any entity.")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣{something}麼樣"}, "test" });
        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "something", "怎");
    }

    WHEN("There is an any entity at the beginning.")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"{something}氣怎麼樣"}, "test" });
        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "something", "天");
    }

    WHEN("There is an any entity at the end.")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣怎麼{something}"}, "test" });
        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "something", "樣");
    }

    WHEN("There is two any entities together.")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣{something}{something2}"}, "test" });
        models.push_back(model);
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "something", "怎麼");
        RequireEntity(intentResult, "something2", "樣");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH ListEntity", "[zh]")
{
    auto fromLanguage = "zh-hk";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    WHEN("ListEntity strict mode")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣{list}樣"}, "test" });
        models.push_back(model);
        model->Entities.push_back({ "list", EntityType::List, EntityMatchMode::Strict, {"怎麼", "天氣"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "list", "怎麼");
        test = "天氣天氣樣";
        intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "list", "天氣");
        test = "天氣樣天氣樣";
        intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "");
        RequireNoEntity(intentResult, "list");
    }

    WHEN("ListEntity fuzzy mode")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣{list}樣"}, "test" });
        models.push_back(model);
        model->Entities.push_back({ "list", EntityType::List, EntityMatchMode::Fuzzy, {"怎麼", "天氣"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "list", "怎麼");
        test = "天氣天氣樣";
        intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "list", "天氣");
        test = "天氣樣天樣";
        intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "list", "樣天");
    }

    WHEN("strict lists don't match intents if not in the list")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"天氣{list}樣"}, "test" });
        models.push_back(model);
        model->Entities.push_back({ "list", EntityType::List, EntityMatchMode::Strict, {"怎麼", "天氣"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto test = "天氣麼樣怎麼樣";
        auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
        RequireIntentId(intentResult, "");
    }

    WHEN("List entities using instance id's and greed mechanics")
    {
        std::string modelId = "MyTestModel";
        auto model = PatternMatchingModel::FromModelId(modelId);
        std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

        model->Intents.push_back({ {"壓{any}{list}"}, "壓" });
        model->Intents.push_back({ {"{list}{any}"}, "壓" });

        model->Intents.push_back({ {"壓{list}{any}"}, "壓" });
        model->Intents.push_back({ {"壓{any}{list}{any2}"}, "壓" });
        model->Intents.push_back({ {"壓{list:one}{list:two}{any}"}, "壓" });
        model->Intents.push_back({ {"壓{list:one}{any}{list:two}"}, "壓" });
        model->Intents.push_back({ {"壓{any}{list:one}{list:two}"}, "壓" });
        model->Intents.push_back({ {"壓{list:one}{list:two}{list:three}"}, "壓" });
        models.push_back(model);
        model->Entities.push_back({ "list" , EntityType::List,   EntityMatchMode::Strict, {"左移", "左控件", "向左箭頭", "右移位"} });
        intentRecognizer->ApplyLanguageModels(models);
        CHECK(model != nullptr);

        auto intentResult = intentRecognizer->RecognizeOnceAsync("壓換檔控制向左箭頭").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list", "向左箭頭");
        RequireEntity(intentResult, "any", "換檔控制");

        intentResult = intentRecognizer->RecognizeOnceAsync("壓左控件移位選項卡").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list", "左控件");
        RequireEntity(intentResult, "any", "移位選項卡");

        intentResult = intentRecognizer->RecognizeOnceAsync("壓控制右移位標籤").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list", "右移位");
        RequireEntity(intentResult, "any", "控制");
        RequireEntity(intentResult, "any2", "標籤");

        intentResult = intentRecognizer->RecognizeOnceAsync("壓左控件左移標籤").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list:one", "左控件");
        RequireEntity(intentResult, "list:two", "左移");
        RequireEntity(intentResult, "any", "標籤");

        intentResult = intentRecognizer->RecognizeOnceAsync("壓左移控制向左箭頭").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list:one", "左移");
        RequireEntity(intentResult, "list:two", "向左箭頭");
        RequireEntity(intentResult, "any", "控制");

        intentResult = intentRecognizer->RecognizeOnceAsync("壓左移左控件向左箭頭").get();
        RequireIntentId(intentResult, "壓");
        RequireEntity(intentResult, "list:one", "左移");
        RequireEntity(intentResult, "list:two", "左控件");
        RequireEntity(intentResult, "list:three", "向左箭頭");

        intentResult = intentRecognizer->RecognizeOnceAsync("左移向左箭頭").get();
        RequireEntity(intentResult, "list", "左移");
        RequireEntity(intentResult, "any", "向左箭頭");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH Optional patterns", "[zh]")
{
    auto fromLanguage = "zh-hk";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    std::string modelId = "MyTestModel";

    auto model = PatternMatchingModel::FromModelId(modelId);
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;
    // Test optional group before an entity. 壓換檔左箭頭 and 壓換控左箭頭 and 壓換左箭頭
    model->Intents.push_back({ {"壓換[檔|控]{any}"}, "test1" });
    // Test optional group with an entity inside. 聽著向左箭頭 and 聽著檔箭頭 and 聽著控箭頭
    model->Intents.push_back({ {"聽著[檔|控|{optEntity}]箭頭"}, "test2" });
    // This is to check if an optional word is included in the following entity. "on" inside of "one". 壓換捌玖箭頭
    model->Intents.push_back({ {"壓換[捌]{list}箭頭"}, "ListTest" });
    // Optional entity at the end 壓換檔控制向左箭頭 and 壓換檔控制向左
    model->Intents.push_back({ {"壓換檔控制向左[{any}]"}, "PARK" });
    // Optional keyword 計算機聽著 and 聽著
    model->Intents.push_back({ {"[計算機]聽著"}, "KeywordTest" });
    // Any after optional 計算機聽著向左 and 計算機向左
    model->Intents.push_back({ {"計算機[聽著]{any}"}, "AnyAfterOptional" });
    // Two optionals together. 計算機 and 計算機聽著 and 計算機向 and 計算機聽著向
    model->Intents.push_back({ {"向計算機[聽著][向]"}, "test3" });
    models.push_back(model);
    model->Entities.push_back({ "list" , EntityType::List, EntityMatchMode::Strict, {"捌玖", "拾"} });
    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    // Test optional group before an entity. 壓換檔左箭頭 and 壓換控左箭頭 and 壓換左箭頭
    auto test = "壓換檔左箭頭";
    auto intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test1");
    RequireEntity(intentResult, "any", "左箭頭");

    test = "壓換控左箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test1");
    RequireEntity(intentResult, "any", "左箭頭");

    test = "壓換左箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test1");
    RequireEntity(intentResult, "any", "左箭頭");

    // Test optional group with an entity inside. 聽著向左箭頭 and 聽著檔箭頭 and 聽著控箭頭
    test = "聽著向左箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test2");
    RequireEntity(intentResult, "optEntity", "向左");

    test = "聽著檔箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test2");
    RequireNoEntity(intentResult, "optEntity");

    test = "聽著控箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test2");
    RequireNoEntity(intentResult, "optEntity");

    // This is to check if an optional word is included in the following entity. "on" inside of "one". 壓換捌玖箭頭
    test = "壓換捌玖箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "ListTest");
    RequireEntity(intentResult, "list", "捌玖");

    test = "壓換捌捌玖箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "ListTest");
    RequireEntity(intentResult, "list", "捌玖");

    // Optional entity at the end 壓換檔控制向左箭頭 and 壓換檔控制向左
    test = "壓換檔控制向左箭頭";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "PARK");
    RequireEntity(intentResult, "any", "箭頭");

    test = "壓換檔控制向左";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "PARK");
    RequireNoEntity(intentResult, "any");

    // Optional keyword 計算機聽著 and 聽著
    test = "計算機聽著";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "KeywordTest");

    test = "聽著";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "KeywordTest");

    // Any after optional 計算機聽著向左 and 計算機向左
    test = "計算機聽著向左";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "AnyAfterOptional");
    RequireEntity(intentResult, "any", "向左");

    test = "計算機向左";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "AnyAfterOptional");
    RequireEntity(intentResult, "any", "向左");

    // Two optionals together. 計算機 and 計算機聽著 and 計算機向 and 計算機聽著向
    test = "向計算機";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test3");

    test = "向計算機聽著";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test3");

    test = "向計算機向";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test3");

    test = "向計算機聽著向";
    intentResult = intentRecognizer->RecognizeOnceAsync(test).get();
    RequireIntentId(intentResult, "test3");
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH Required entity groups", "[zh]")
{
    auto fromLanguage = "zh-hk";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    WHEN("There is an intent with a required group at the beginning")
    {
        intentRecognizer->AddIntent("(向計|算機){any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("向計聽著向").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "聽著向");
        intentResult = intentRecognizer->RecognizeOnceAsync("算機聽著向").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "聽著向");
        intentResult = intentRecognizer->RecognizeOnceAsync("聽著向").get();
        RequireIntentId(intentResult, "");
    }

    WHEN("There is an intent with required group at the end")
    {
        intentRecognizer->AddIntent("這是我的{any}(模式|短語)", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("這是我的必填模式").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        intentResult = intentRecognizer->RecognizeOnceAsync("這是我的必填短語").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        intentResult = intentRecognizer->RecognizeOnceAsync("這是我的必填").get();
        RequireIntentId(intentResult, "");
    }

    WHEN("There is an intent with required group next to another")
    {
        intentRecognizer->AddIntent("這是我的(雙|第二)(模式|短語){any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("這是我的雙模式必填").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        intentResult = intentRecognizer->RecognizeOnceAsync("這是我的第二短語必填").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        intentResult = intentRecognizer->RecognizeOnceAsync("這是我的第一模式必填").get();
        RequireIntentId(intentResult, "");
    }

    WHEN("There is an intent with required group entity")
    {
        intentRecognizer->AddIntent("這是我的(雙|{any2})(模式|短語){any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("這是我的雙模式必填").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        intentResult = intentRecognizer->RecognizeOnceAsync("這是我的第二短語必填").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "必填");
        RequireEntity(intentResult, "any2", "第二");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH-HK punctuation", "[zh][speech]")
{
    // 。，、：；？！「」『』[]{}·《》〈〉…—～﹁﹂ // InputPunctuation
    // 。，、：；？！「」『』·《》〈〉…—～﹁﹂ // PatternPunctuation
    // 。？！ // SentenceEndCharacters
    REQUIRE(exists(INTENT_ZHHK_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto fromLanguage = "zh-hk";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_ZHHK_UTTERANCE);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    // 天气怎么样？
    WHEN("There is an intent with 1 phrase and punctuation")
    {
        intentRecognizer->AddIntent("天氣{any}？", "test");
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "怎麼樣");
    }

    WHEN("There is an intent with 1 phrase and symbols")
    {
        intentRecognizer->AddIntent("天。，、：；？！「」『』氣{any}", "test");
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "怎麼樣");
    }

    WHEN("There is a greedy entity that will only capture 1 word at the end of a pattern that has punctuation after it")
    {
        intentRecognizer->AddIntent("打開{any}。，、：；？！「」『』", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("打開燈。").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "燈");
    }

    WHEN("There is an entity with a colon")
    {
        intentRecognizer->AddIntent("打{any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("打開：燈").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "開：燈");
    }
}

// Because there are so many similarities between zh-cn and zh-hk punctuation is the only real scripting
// difference.
TEST_CASE("IntentRecognizer::PatternMatching::ZH-CN punctuation", "[zh][speech]")
{
    // 。，、：；？！[]{}·《》〈〉…—～﹁﹂\"' // InputPunctuation
    // 。，、：；？！·《》〈〉…—～﹁﹂\"' // PatternPunctuation
    // 。？！ // SentenceEndCharacters
    REQUIRE(exists(INTENT_ZHCN_UTTERANCE));

    auto speechConfig = SpeechConfigForIntentTests();
    auto fromLanguage = "zh-cn";
    speechConfig->SetSpeechRecognitionLanguage(fromLanguage);
    auto audioConfig = AudioConfig::FromWavFileInput(INTENT_ZHCN_UTTERANCE);
    auto speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    // 天气怎么样？
    WHEN("There is an intent with 1 phrase and punctuation")
    {
        intentRecognizer->AddIntent("天气{any}？", "test");
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "怎么样");
    }

    WHEN("There is an intent with 1 phrase and symbols")
    {
        intentRecognizer->AddIntent("天。，、：；？！'\"气{any}", "test");
        auto speechResult = speechRecognizer->RecognizeOnceAsync().get();
        auto intentResult = intentRecognizer->RecognizeOnceAsync(speechResult->Text).get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "怎么样");
    }

    WHEN("There is a greedy entity that will only capture 1 word at the end of a pattern that has punctuation after it")
    {
        intentRecognizer->AddIntent("打開{any}。，、：；？！'\"", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("打開燈。").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "燈");
    }

    WHEN("There is an entity with a colon")
    {
        intentRecognizer->AddIntent("打{any}", "test");
        auto intentResult = intentRecognizer->RecognizeOnceAsync("打開：燈").get();
        RequireIntentId(intentResult, "test");
        RequireEntity(intentResult, "any", "開：燈");
    }
}

TEST_CASE("IntentRecognizer::PatternMatching::ZH Prebuilt integer entities", "[zh]")
{
    auto fromLanguage = "zh-cn";
    auto intentRecognizer = IntentRecognizer::FromLanguage(fromLanguage);

    auto model = PatternMatchingModel::FromModelId("test");
    std::vector<std::shared_ptr<LanguageUnderstandingModel>> models;

    model->Intents.push_back({ {"{number}"}, "test" });
    model->Entities.push_back({ "number" , EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });

    models.push_back(model);

    intentRecognizer->ApplyLanguageModels(models);
    CHECK(model != nullptr);

    auto intentResult = intentRecognizer->RecognizeOnceAsync("零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "0");
    intentResult = intentRecognizer->RecognizeOnceAsync("一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1");
    intentResult = intentRecognizer->RecognizeOnceAsync("幺").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1");
    intentResult = intentRecognizer->RecognizeOnceAsync("二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2");
    intentResult = intentRecognizer->RecognizeOnceAsync("两").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2");
    intentResult = intentRecognizer->RecognizeOnceAsync("俩").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2");
    intentResult = intentRecognizer->RecognizeOnceAsync("三").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "3");
    intentResult = intentRecognizer->RecognizeOnceAsync("仨").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "3");
    intentResult = intentRecognizer->RecognizeOnceAsync("四").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "4");
    intentResult = intentRecognizer->RecognizeOnceAsync("五").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "5");
    intentResult = intentRecognizer->RecognizeOnceAsync("六").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "6");
    intentResult = intentRecognizer->RecognizeOnceAsync("〇").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "0");
    intentResult = intentRecognizer->RecognizeOnceAsync("壹").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1");
    intentResult = intentRecognizer->RecognizeOnceAsync("捌").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "8");
    intentResult = intentRecognizer->RecognizeOnceAsync("0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "0");
    intentResult = intentRecognizer->RecognizeOnceAsync("8").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "8");
    intentResult = intentRecognizer->RecognizeOnceAsync("十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "11");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "20");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "20");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("1十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("一10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "11");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "20");
    intentResult = intentRecognizer->RecognizeOnceAsync("二10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "20");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("二10一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("幺零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("幺幺").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "11");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "11");
    intentResult = intentRecognizer->RecognizeOnceAsync("10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "10");
    intentResult = intentRecognizer->RecognizeOnceAsync("11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "11");
    intentResult = intentRecognizer->RecognizeOnceAsync("20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "20");
    intentResult = intentRecognizer->RecognizeOnceAsync("21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "21");
    intentResult = intentRecognizer->RecognizeOnceAsync("100").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("101").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 0 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("110").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("111").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 10 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 2 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("112").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("110 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("120").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1 20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("121").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 20 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 2 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 2 10 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("122").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("200").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("201").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("202").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("210").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 1 10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("211").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("220").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("221").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("222").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 22").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 2 12").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 2 10 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("一零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("幺零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("一零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百一十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("十二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("十二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("十二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("两百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("俩百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("两百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百零二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二幺幺").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十一二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("二一十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("1零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("1零0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("10零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "100");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百01").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百零1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百01").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("100零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "101");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百一10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("一1 0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "110");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百1十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百1十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百1 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("1一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("十一1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "111");
    intentResult = intentRecognizer->RecognizeOnceAsync("100一十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 12").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 1 10 二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("一一2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("一 1 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "112");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("100 二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一2零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("一20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1 2十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "120");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百20一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一2一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一二1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("1  2一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("一2 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("1二1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "121");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百20二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百22").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("1百22").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百二十2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("一2二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("12  二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("1 二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "122");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("2零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("二00").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("20零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十0").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("二10零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "200");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("两百0 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("两百0一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("两百零1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("200零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("2零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "201");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百零二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("200零二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("2零二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "202");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百1十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百1 10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("200一10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 1 十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("200一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("2一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二1零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("二1 10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "210");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百1十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百一十1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百一11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("200一十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百1 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百1 11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二1幺").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("二1 1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "211");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十一二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("21 二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十一2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("二11二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("20一二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("20一2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "212");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200 2十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("二20").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2 2十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2二10").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "220");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百2十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二10一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百20一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二10一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("2二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("22一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二12一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("2十二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("2 12一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二10二1").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二21").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("2二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二20一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二11").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "221");
    intentResult = intentRecognizer->RecognizeOnceAsync("2百22").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("200二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百二12").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二百20二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二2二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("22二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("20二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十2 2").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二22").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("2 2十二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("2 二 12").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("二20二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "222");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("1000").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("1千").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("一零零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("十零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("1k").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("一k").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1000");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1001");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千零一十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1010");
    intentResult = intentRecognizer->RecognizeOnceAsync("一零一零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1010");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1100");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千一百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1100");
    intentResult = intentRecognizer->RecognizeOnceAsync("二零二二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2022");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千二百零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二二一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2221");
    intentResult = intentRecognizer->RecognizeOnceAsync("两千二百二十一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2221");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二十二01").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二零一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2201");
    intentResult = intentRecognizer->RecognizeOnceAsync("二二0一").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "2201");
    intentResult = intentRecognizer->RecognizeOnceAsync("十九三百二").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "19320");
    intentResult = intentRecognizer->RecognizeOnceAsync("十九三二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "19320");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百九十三二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "19320");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百九十三二零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "19320");
    intentResult = intentRecognizer->RecognizeOnceAsync("一九三二十").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "19320");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百九十三二百三十三千四百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932303400");
    intentResult = intentRecognizer->RecognizeOnceAsync("一百九十三二百三十九三千四百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");
    intentResult = intentRecognizer->RecognizeOnceAsync("一九三二三百九十三四零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千九百三十二三百九十三四百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");
    intentResult = intentRecognizer->RecognizeOnceAsync("一千九百三十二三九三四百").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");
    intentResult = intentRecognizer->RecognizeOnceAsync("十九三二三九三千四").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");
    intentResult = intentRecognizer->RecognizeOnceAsync("十九三千两百三十九三四零零").get();
    RequireIntentId(intentResult, "test");
    RequireEntity(intentResult, "number", "1932393400");

    // invalid cases
    // it is ambiguous number, "30 20", "32 10", "3 12 10" etc. are all correct.
    intentResult = intentRecognizer->RecognizeOnceAsync("三十二十").get();
    RequireIntentId(intentResult, "");

    intentResult = intentRecognizer->RecognizeOnceAsync("第五个").get();
    RequireIntentId(intentResult, "");
    intentResult = intentRecognizer->RecognizeOnceAsync("").get();
    RequireIntentId(intentResult, "");
    intentResult = intentRecognizer->RecognizeOnceAsync(" ").get();
    RequireIntentId(intentResult, "");
}
