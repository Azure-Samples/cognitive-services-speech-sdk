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

static wstring input_file(L"tests/input/whatstheweatherlike.wav");

TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
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

        auto callback = [&](const SpeechRecognitionEventArgs& args)
        {
            {
                UNUSED(args);
                unique_lock<mutex> lock(mtx);
                callback_invoked = true;
                cv.notify_one();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        };

        recognizer->FinalResult.Connect(callback);
        recognizer->Canceled.Connect(callback); // Canceled is called if there are connection issues.

        auto result = recognizer->RecognizeAsync().get();

        {
            unique_lock<mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(10), [&] { return callback_invoked; });
            REQUIRE(callback_invoked);
        }
        recognizer.reset();
    }
}

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{
    DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine", true);
    DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-Microphone", true);

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
            SPX_TRACE_VERBOSE("%s: Make sure callbacks are invoked correctly; START of loop #%d", __FUNCTION__, i);

            // Assuming subscription key contains only single-byte characters.
            auto keyW = std::wstring(g_keySpeech.begin(), g_keySpeech.end());
            DefaultRecognizerFactory::SetSubscriptionKey(keyW.c_str());
            auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizerWithFileInput(input_file);

            REQUIRE(recognizer != nullptr);

            bool sessionEnded = false;

            recognizer->FinalResult.Connect([&](const SpeechRecognitionEventArgs&) {
                callbackCounts[Callbacks::final_result]++; 
                SPX_TRACE_VERBOSE("callbackCounts[Callbacks::final_result]=%d", callbackCounts[Callbacks::final_result].load());
            });
            recognizer->NoMatch.Connect([&](const SpeechRecognitionEventArgs&) {
                callbackCounts[Callbacks::no_match]++; 
                SPX_TRACE_VERBOSE("callbackCounts[Callbacks::no_match]=%d", callbackCounts[Callbacks::no_match].load());
            });
            recognizer->SessionStarted.Connect([&](const SessionEventArgs&) {
                callbackCounts[Callbacks::session_started]++;
                SPX_TRACE_VERBOSE("callbackCounts[Callbacks::session_started]=%d", callbackCounts[Callbacks::session_started].load());
            });
            recognizer->SessionStopped.Connect([&](const SessionEventArgs&) {
                callbackCounts[Callbacks::session_stopped]++;
                SPX_TRACE_VERBOSE("callbackCounts[Callbacks::session_stopped]=%d", callbackCounts[Callbacks::session_stopped].load());
                unique_lock<mutex> lock(mtx);
                sessionEnded = true;
                cv.notify_one();
            });

            auto result = recognizer->RecognizeAsync().get();
            CHECK(result != nullptr);

            SPX_TRACE_VERBOSE("%s: Wait for session end (loop #%d)", __FUNCTION__, i);
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&] { return sessionEnded; });
            lock.unlock();

            SPX_TRACE_VERBOSE("%s: Wait some more (loop #%d)", __FUNCTION__, i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            SPX_TRACE_VERBOSE("%s: Make sure callbacks are invoked correctly; END of loop #%d", __FUNCTION__, i);
        }

        SPX_TRACE_VERBOSE("%s: Checking callback counts ...", __FUNCTION__);

        CHECK(callbackCounts[Callbacks::session_started] == numLoops);
        CHECK(callbackCounts[Callbacks::session_stopped] == numLoops);
        CHECK(callbackCounts[Callbacks::final_result] == numLoops);
        CHECK(callbackCounts[Callbacks::no_match] == 0);
    }
}

TEST_CASE("KWS basics", "[api][cxx]")
{
    GIVEN("Mocks for USP, KWS, and the Microphone...")
    {
        DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine", true);
        DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-Microphone", true);
        DefaultRecognizerFactory::Parameters::SetBool(L"CARBON-INTERNAL-MOCK-SdkKwsEngine", true);

        mutex mtx;
        condition_variable cv;

        int gotFinalResult = 0;
        int gotSessionStopped = 0;

        WHEN("We do a keyword recognition with a speech recognizer")
        {
            auto recognizer = DefaultRecognizerFactory::CreateSpeechRecognizer();
            REQUIRE(recognizer != nullptr);

            recognizer->FinalResult += [&](const SpeechRecognitionEventArgs& /* e */) {
                std::unique_lock<std::mutex> lock(mtx);
                gotFinalResult++;
                SPX_TRACE_VERBOSE("gotFinalResult=%d", gotFinalResult);
            };

            recognizer->SessionStopped += [&](const SessionEventArgs& /* e */) {
                std::unique_lock<std::mutex> lock(mtx);
                gotSessionStopped++;
                cv.notify_all();
            };

            recognizer->StartKeywordRecognitionAsync(L"mock");

            THEN("We wait up to 30 seconds for a KwsSingleShot recognition and it's accompanying SessionStopped")
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return gotFinalResult >= 1 && gotSessionStopped >= 1; });

                recognizer->StopKeywordRecognitionAsync().get();

                THEN("We should see that we got at least 1 FinalResult and the same number of SessionStopped events")
                {
                    REQUIRE(gotFinalResult >= 1);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
}
