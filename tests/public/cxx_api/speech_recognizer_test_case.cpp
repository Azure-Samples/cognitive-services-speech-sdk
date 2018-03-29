//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>
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

std::string g_keySpeech;
std::string g_keyCRIS;
std::string g_keyLUIS;
std::string g_keySkyman;


TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    wstring input_file(L"whatstheweatherlike.wav");
    REQUIRE(exists(input_file));

    mutex mtx;
    condition_variable cv;

    SECTION("Check for race conditions in destructor.")
    {
        bool callback_invoked = false;

        // Assuming subscription key contains only single-byte characters.
        auto keyW = std::wstring(g_keySpeech.begin(), g_keySpeech.end());
        DefaultRecognizerFactory::SetSubscriptionKey(keyW.c_str());

        auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizerWithFileInput(input_file);
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
            cv.wait(lock, [&] { return callback_invoked; });
        }
        recognizer.reset();
    }
}

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{

     wstring input_file(L"whatstheweatherlike.wav");
    REQUIRE(exists(input_file));

    mutex mtx;
    condition_variable cv;

    enum class Callbacks { final_result, no_match, session_started, session_stopped };

    SECTION("Make sure callbacks are invoked correctly.")
    {
        std::map<Callbacks, atomic_int> callbackCounts;
        callbackCounts[Callbacks::final_result] = 0;
        callbackCounts[Callbacks::no_match] = 0;
        callbackCounts[Callbacks::session_started] = 0;
        callbackCounts[Callbacks::session_stopped] = 0;

        const int numLoops = 5;

        for (int i = 0; i < numLoops; i++)
        {
            // Assuming subscription key contains only single-byte characters.
            auto keyW = std::wstring(g_keySpeech.begin(), g_keySpeech.end());
            DefaultRecognizerFactory::SetSubscriptionKey(keyW.c_str());
            auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizerWithFileInput(input_file);

            REQUIRE(recognizer != nullptr);

            bool sessionEnded = false;

            recognizer->FinalResult.Connect(
                [&](const SpeechRecognitionEventArgs&) { callbackCounts[Callbacks::final_result]++; });
            recognizer->NoMatch.Connect(
                [&](const SpeechRecognitionEventArgs&) { callbackCounts[Callbacks::no_match]++; });
            recognizer->SessionStarted.Connect(
                [&](const SessionEventArgs&) { callbackCounts[Callbacks::session_started]++; });
            recognizer->SessionStopped.Connect(
                [&](const SessionEventArgs&)
            {
                callbackCounts[Callbacks::session_stopped]++;
                unique_lock<mutex> lock(mtx);
                sessionEnded = true;
                cv.notify_one();
            });

            auto result = recognizer->RecognizeAsync().get();

            CHECK(result != nullptr);

            {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock, [&] { return sessionEnded; });
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }

        CHECK(callbackCounts[Callbacks::session_started] == numLoops);
        CHECK(callbackCounts[Callbacks::session_stopped] == numLoops);
        CHECK(callbackCounts[Callbacks::final_result] == numLoops);
        CHECK(callbackCounts[Callbacks::no_match] == 0);
    }

    // The following tests failed in VSO Linux tests, disable it for now. 
    // Logged as bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1195505
    /*GIVEN("Mocks for UspRecoEngine and Microphone...")
    {
        DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine", true);
        DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-Microphone", true);

        int gotIntermediateResults = 0;
        int gotFinalResult = 0;

        auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();
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
    }*/
}