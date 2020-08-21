//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <iostream>
#include <atomic>
#include <map>
#include <string>

#include "speechapi_cxx.h"
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace std;

std::shared_ptr<SpeechConfig> SpeechConfigForIntentTests(const std::string& trafficType)
{
    auto config = !DefaultSettingsMap[ENDPOINT].empty()
        ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Key)
        : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Key, SubscriptionsRegionsMap[LANGUAGE_UNDERSTANDING_SUBSCRIPTION].Region);
    config->SetServiceProperty("TrafficType", trafficType, ServicePropertyChannel::UriQueryParameter);
    return config;
}

TEST_CASE("Intent Recognizer basics", "[api][cxx][intent]")
{
    SECTION("Intent Recognition works")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(INTENT_UTTERANCE)));

        auto config = SpeechConfigForIntentTests(SpxGetTestTrafficType(__FILE__, __LINE__));
        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(INTENT_UTTERANCE));
        auto recognizer = IntentRecognizer::FromConfig(config, audioConfig);

        SPXTEST_REQUIRE(!DefaultSettingsMap[LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID].empty());
        auto model = LanguageUnderstandingModel::FromAppId(DefaultSettingsMap[LANGUAGE_UNDERSTANDING_HOME_AUTOMATION_APP_ID]);
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
            SPXTEST_REQUIRE(result != nullptr);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

            std::ostringstream details;
            if (result == nullptr)
            {
                CAPTURE("Under Intent Recognition works section, result is null");
                return;
            }
            if (result->Reason == ResultReason::Canceled)
            {
                auto cancellation = CancellationDetails::FromResult(result);
                details
                    << "Canceled:"
                    << " Reason: " << int(cancellation->Reason)
                    << " Details: " << cancellation->ErrorDetails;
            }
            else if (result->Reason == ResultReason::NoMatch)
            {
                auto nomatch = NoMatchDetails::FromResult(result);
                details
                    << "NoMatch:"
                    << " Reason: " << int(nomatch->Reason);
            }
            CAPTURE(details.str());
            SPXTEST_CHECK(result->Reason == expectedReason);
            SPXTEST_CHECK(!result->Text.empty());
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6237)
// Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif
            SPXTEST_REQUIRE(result->IntentId == expectedIntentId);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        };
        SPXTEST_WHEN("using single model, all intents, no intent name, no intent ids")
        {
            recognizer->AddAllIntents(model);
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }

        SPXTEST_WHEN("using single model, all intents, no intent name, override intent id")
        {
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-all-intent-ids-with-this");
        }
        SPXTEST_WHEN("using single model, specific intent by name, no intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }

        SPXTEST_WHEN("using single model, specific intent by name, override intent id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn", "override-turn-on-id-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-turn-on-id-with-this");
        }

        SPXTEST_WHEN("using single model, other intent by name")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "", ResultReason::RecognizedSpeech);
        }

        SPXTEST_WHEN("using single model, other intent by name, with all intents override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOff");
            recognizer->AddAllIntents(model, "override-turn-on-id-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "override-turn-on-id-with-this");
        }

        SPXTEST_WHEN("using single model, specific intent by name, all other intents with override id")
        {
            recognizer->AddIntent(model, "HomeAutomation.TurnOn");
            recognizer->AddAllIntents(model, "override-all-intent-ids-with-this");
            auto result = recognizer->RecognizeOnceAsync().get();
            requireIntentId(result, "HomeAutomation.TurnOn");
        }
    }
}
