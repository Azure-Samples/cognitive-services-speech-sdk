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

// Intent disabled due to service issues.
// https://msasg.visualstudio.com/Skyman/_workitems/edit/1550174
TEST_CASE("Intent Recognizer basics", "[api][cxx][intent][!hide]")
{
    SECTION("Intent Recognition works")
    {
        turnOnLamp.UpdateFullFilename(Config::InputDir);
        REQUIRE(exists(turnOnLamp.m_audioFilename));

        auto config = SpeechConfigForIntentTests();
        auto audioConfig = AudioConfig::FromWavFileInput(turnOnLamp.m_audioFilename);
        auto recognizer = IntentRecognizer::FromConfig(config, audioConfig);

        REQUIRE(!Config::LuisAppId.empty());
        auto model = LanguageUnderstandingModel::FromAppId(Config::LuisAppId);
        std::string sessionId;

        recognizer->SessionStarted.Connect([&sessionId](const SessionEventArgs& e)
        {
            sessionId = e.SessionId;
        });

        auto requireIntentId = [&sessionId](std::shared_ptr<IntentRecognitionResult> result, std::string expectedIntentId, ResultReason expectedReason = ResultReason::RecognizedIntent)
        {
            CAPTURE(sessionId);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6237)
// Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif
            REQUIRE(result != nullptr);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
            if (result->Reason == ResultReason::Canceled)
            {
                auto cancellation = CancellationDetails::FromResult(result);
                CAPTURE(cancellation->Reason);
                CAPTURE(cancellation->ErrorDetails);
                CAPTURE(cancellation->ErrorCode);
            }
            else if (result->Reason == ResultReason::NoMatch)
            {
                auto nomatch = NoMatchDetails::FromResult(result);
                CAPTURE(nomatch->Reason);
            }
            CHECK(result->Reason == expectedReason);
            CHECK(!result->Text.empty());
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6237)
// Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif
            REQUIRE(result->IntentId == expectedIntentId);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        };

        WHEN("using single model, all intents, no intent name, no intent ids")
        {
            recognizer->AddAllIntents(model);
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }

        WHEN("using single model, all intents, no intent name, override intent id")
        {
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-all-intent-ids-with-this");
        }

        WHEN("using single model, specific intent by name, no intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }

        WHEN("using single model, specific intent by name, override intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn", "override-turn-on-id-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-turn-on-id-with-this");
        }

        WHEN("using single model, other intent by name")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "", ResultReason::RecognizedSpeech);
        }

        WHEN("using single model, other intent by name, with all intents override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            recognizer->AddAllIntents(model, "override-turn-on-id-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-turn-on-id-with-this");
        }

        WHEN("using single model, specific intent by name, all other intents with override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }
    }
}
