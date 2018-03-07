//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include "catch.hpp"
#include "test_utils.h"

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "speechapi_cxx.h"


using namespace CARBON_NAMESPACE_ROOT;
using namespace CARBON_NAMESPACE_ROOT::Recognition;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Speech;
using namespace std;



TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    wstring input_file(L"whatstheweatherlike.wav");
    REQUIRE(exists(input_file));

    mutex mtx;
    condition_variable cv;

    SECTION("Check for race conditions in destructor.")
    {
        bool callback_invoked = false;
        auto recognizer = RecognizerFactory::CreateSpeechRecognizerWithFileInput(input_file);
        recognizer->FinalResult.Connect(
            [&](const SpeechRecognitionEventArgs& args) 
        {
                {
                    UNUSED(args);
                    unique_lock<mutex> lock(mtx);
                    callback_invoked = true;
                    cv.notify_one();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
        });

        auto result = recognizer->RecognizeAsync().get();

        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&callback_invoked] { return callback_invoked; });
        }
        recognizer.reset();
    }
}

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{
    GIVEN("Mocks for UspRecoEngine and Microphone...")
    {
        GlobalParameterCollection& globalParams = GlobalParameters::Get();
        globalParams[L"__mockUspRecoEngine"] = true;
        globalParams[L"__mockMicrophone"] = true;

        int gotIntermediateResults = 0;
        int gotFinalResult = 0;

        auto recognizer = RecognizerFactory::CreateSpeechRecognizer();
        REQUIRE(recognizer != nullptr);

        WHEN("We we connect both IntermediateResult and FinalResult event handlers...")
        {
            recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
                UNUSED(e);
                gotIntermediateResults++;
            };
            recognizer->FinalResult += [&](const SpeechRecognitionEventArgs& e) {
                UNUSED(e);
                gotFinalResult++;
            };

            THEN("We should be able to verify that they are connected.")
            {
                REQUIRE(recognizer->IntermediateResult.IsConnected());
                REQUIRE(recognizer->FinalResult.IsConnected());
            }

            WHEN("We do a single recognition")
            {
                auto result = recognizer->RecognizeAsync().get();
                THEN("We should see that we got more than one Intermediate, exactly one Final, and the call to RecoAsync returned a result")
                {
                    REQUIRE(gotIntermediateResults > 1);
                    REQUIRE(gotFinalResult == 1);
                    REQUIRE(result != nullptr);
                }
            }
        }
    }
}