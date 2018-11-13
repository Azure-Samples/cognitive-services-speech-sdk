//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "speechapi_cxx.h"

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace std;

std::shared_ptr<SpeechConfig> SpeechConfigForIntentTests()
{
    auto config = !Config::Endpoint.empty()
        ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::LUIS)
        : SpeechConfig::FromSubscription(Keys::LUIS, Config::LuisRegion);
    return config;
}

TEST_CASE("Intent Recognizer basics", "[api][cxx][intent]")
{
    SPXTEST_SECTION("Intent Recognition works")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);        
        SPXTEST_REQUIRE(exists(turnOnLamp.m_audioFilename));

        auto config = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_audioFilename);
        auto recognizer = IntentRecognizer::FromConfig(config, audioConfig);

        SPXTEST_REQUIRE(!Config::LuisAppId.empty());
        auto model = LanguageUnderstandingModel::FromAppId(Config::LuisAppId);

        SPXTEST_WHEN("using single model, all intents, no intent name, no intent ids")
        {
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(!result->Text.empty());
            SPXTEST_REQUIRE(!result->IntentId.empty());
        }

        SPXTEST_WHEN("using single model, all intents, no intentname, override intent id")
        {
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId == "override-all-intent-ids-with-this");
        }

        SPXTEST_WHEN("using single model, specific intent by name, no intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId == "HomeAutomation.TurnOn");
        }

        SPXTEST_WHEN("using single model, specific intent by name, override intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn", "override-turn-on-id-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId == "override-turn-on-id-with-this");
        }

        SPXTEST_WHEN("using single model, other intent by name")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId.empty());
        }

        SPXTEST_WHEN("using single model, other intent by name, with all intents override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId == "override-all-intent-ids-with-this");
        }

        SPXTEST_WHEN("using single model, specific intent by name, all other intents with override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->IntentId == "HomeAutomation.TurnOn");
        }
    }
}
