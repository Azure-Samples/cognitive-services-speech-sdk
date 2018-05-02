//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include "speechapi_cxx.h"
#include "mock_controller.h"
#include "test_utils.h"


using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace std;

static wstring input_file(L"tests/input/whatstheweatherlike.wav");


static std::shared_ptr<ICognitiveServicesSpeechFactory> GetFactory()
{
    // Assuming subscription key contains only single-byte characters.
    static std::shared_ptr<ICognitiveServicesSpeechFactory> factory = !Config::Endpoint.empty()
        ? SpeechFactory::FromEndpoint(PAL::ToWString(Config::Endpoint), PAL::ToWString(Keys::Speech))
        : SpeechFactory::FromSubscription(PAL::ToWString(Keys::Speech), PAL::ToWString(Config::Region));

    return factory;
}

void UseMocks(bool value)
{
    SpxSetMockParameterBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine", value);
    SpxSetMockParameterBool(L"CARBON-INTERNAL-MOCK-Microphone", value);
    SpxSetMockParameterBool(L"CARBON-INTERNAL-MOCK-SdkKwsEngine", value);
}

void UseMockUsp(bool value)
{
    SpxSetMockParameterBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine", value);
}

bool IsUsingMocks(bool uspMockRequired = true)
{
    return SpxGetMockParameterBool(L"CARBON-INTERNAL-MOCK-Microphone") &&
           SpxGetMockParameterBool(L"CARBON-INTERNAL-MOCK-SdkKwsEngine") &&
           (SpxGetMockParameterBool(L"CARBON-INTERNAL-MOCK-UspRecoEngine") || !uspMockRequired);
}

void SetMockRealTimeSpeed(int value)
{
    SpxSetMockParameterNumber(L"CARBON-INTERNAL-MOCK-RealTimeAudioPercentage", value);
}

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    GIVEN("Mocks for USP, Microphone, WaveFilePump and Reader, and then USP ...")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(true);

        REQUIRE(exists(input_file));

        mutex mtx;
        condition_variable cv;

        enum class Callbacks { final_result, no_match, session_started, session_stopped, speech_start_detected, speech_end_detected };

        WHEN("We checking to make sure callback counts are correct (checking multiple times, and multiple speeds times) ...")
        {
            std::map<Callbacks, atomic_int> callbackCounts;
            callbackCounts[Callbacks::final_result] = 0;
            callbackCounts[Callbacks::no_match] = 0;
            callbackCounts[Callbacks::session_started] = 0;
            callbackCounts[Callbacks::session_stopped] = 0;
            callbackCounts[Callbacks::speech_start_detected] = 0;
            callbackCounts[Callbacks::speech_end_detected] = 0;

            // We're going to loop thru 11 times... The first 10, we'll use mocks. The last time we'll use the USP
            // NOTE: Please keep this at 11... It tests various "race"/"speed" configurations of the core system... 
            // NOTE: When running agains the localhost, loop 20 times... Half the time, we'll use mocks, and half - the USP.
            const int numLoops = (Config::Endpoint.empty()) ? 11 : 20;

            for (int i = 0; i < numLoops; i++)
            {
                auto useMockUsp = true;

                if (!Config::Endpoint.empty() && i % 2 == 0)
                {
                    useMockUsp = false;
                }
                else
                {
                    useMockUsp = i + 1 < numLoops;
                }

                auto realTimeRate = (i + 1) * 100 / numLoops;
                SetMockRealTimeSpeed(realTimeRate);
                UseMockUsp(useMockUsp);

                SPX_TRACE_VERBOSE("%s: START of loop #%d; mockUsp=%d; realtime=%d", __FUNCTION__, i, useMockUsp, realTimeRate);

                auto recognizer = GetFactory()->CreateSpeechRecognizerWithFileInput(input_file);
                REQUIRE(recognizer != nullptr);
                REQUIRE(IsUsingMocks(useMockUsp));

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
                recognizer->SpeechStartDetected.Connect([&](const RecognitionEventArgs&) {
                    callbackCounts[Callbacks::speech_start_detected]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::speech_start_detected]=%d", callbackCounts[Callbacks::speech_start_detected].load());
                });
                recognizer->SpeechEndDetected.Connect([&](const RecognitionEventArgs&) {
                    callbackCounts[Callbacks::speech_end_detected]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::speech_end_detected]=%d", callbackCounts[Callbacks::speech_end_detected].load());
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
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return sessionEnded; });
                lock.unlock();

                SPX_TRACE_VERBOSE("%s: Make sure callbacks are invoked correctly; END of loop #%d", __FUNCTION__, i);
                CHECK(callbackCounts[Callbacks::session_started] == i+1);
                CHECK(callbackCounts[Callbacks::session_stopped] == i + 1);
                CHECK(callbackCounts[Callbacks::final_result] == i + 1);
                CHECK(callbackCounts[Callbacks::speech_start_detected] == i + 1);
                CHECK(callbackCounts[Callbacks::speech_end_detected] == i + 1);
                CHECK(callbackCounts[Callbacks::no_match] == 0);
            }

            SPX_TRACE_VERBOSE("%s: Wait some more", __FUNCTION__);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            SPX_TRACE_VERBOSE("%s: Checking callback counts ...", __FUNCTION__);

            CHECK(callbackCounts[Callbacks::session_started] == numLoops);
            CHECK(callbackCounts[Callbacks::session_stopped] == numLoops);
            CHECK(callbackCounts[Callbacks::final_result] == numLoops);
            CHECK(callbackCounts[Callbacks::speech_start_detected] == numLoops);
            CHECK(callbackCounts[Callbacks::speech_end_detected] == numLoops);
            CHECK(callbackCounts[Callbacks::no_match] == 0);
        }

        UseMocks(false);
    }

    SECTION("Check that recognition result contains original json payload.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        REQUIRE(exists(input_file));
        REQUIRE(!IsUsingMocks());

        auto recognizer = GetFactory()->CreateSpeechRecognizerWithFileInput(input_file);
        auto result = recognizer->RecognizeAsync().get();
        REQUIRE(!result->Properties[ResultProperty::Json].GetString().empty());
    }

    SECTION("Check that recognition result contains error details.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        REQUIRE(exists(input_file));
        REQUIRE(!IsUsingMocks());
        auto badKeyFactory = SpeechFactory::FromSubscription(L"invalid_key", L"invalid_region");
        auto recognizer = badKeyFactory->CreateSpeechRecognizerWithFileInput(input_file);
        auto result = recognizer->RecognizeAsync().get();

        REQUIRE(result->Reason == Reason::Canceled);
        REQUIRE(!result->ErrorDetails.empty());

        // NOTE: Looks like we still do need this...
        // TODO: there's a data race in the audio_pump thread when it tries to
        // pISpxAudioProcessor->SetFormat(nullptr); after exiting the loop.
        // Comment out the next line to see for yourself (repros on Linux build machines).
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

TEST_CASE("KWS basics", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    GIVEN("Mocks for USP, KWS, and the Microphone...")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(true);

        mutex mtx;
        condition_variable cv;

        int gotFinalResult = 0;
        int gotSessionStopped = 0;

        WHEN("We do a keyword recognition with a speech recognizer")
        {
            SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

            auto recognizer = GetFactory()->CreateSpeechRecognizer();
            REQUIRE(recognizer != nullptr);
            REQUIRE(IsUsingMocks(true));

            recognizer->FinalResult += [&](const SpeechRecognitionEventArgs& /* e */) {
                std::unique_lock<std::mutex> lock(mtx);
                gotFinalResult++;
                SPX_TRACE_VERBOSE("gotFinalResult=%d", gotFinalResult);
            };

            recognizer->SessionStopped += [&](const SessionEventArgs& /* e */) {
                std::unique_lock<std::mutex> lock(mtx);
                gotSessionStopped++;
                SPX_TRACE_VERBOSE("gotSessionStopped=%d", gotSessionStopped);
                cv.notify_all();
            };

            auto model = KeywordRecognitionModel::FromFile(L"tests/input/heycortana_en-US.table");
            recognizer->StartKeywordRecognitionAsync(model);

            THEN("We wait up to 30 seconds for a KwsSingleShot recognition and it's accompanying SessionStopped")
            {
                SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return gotFinalResult >= 1 && gotSessionStopped >= 1; });

                recognizer->StopKeywordRecognitionAsync().get();

                THEN("We should see that we got at least 1 FinalResult and the same number of SessionStopped events")
                {
                    REQUIRE(gotFinalResult >= 1);
                }
            }

            // std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }

        UseMocks(false);
    }
}

TEST_CASE("Speech on local server", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SECTION("Stress testing against the local server")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        if (Config::Endpoint.empty())
        {
            return;
        }

        UseMocks(false);
        REQUIRE(exists(input_file));
        REQUIRE(!IsUsingMocks());

        const int numLoops = 10;

        auto factory = SpeechFactory::FromEndpoint(PAL::ToWString(Config::Endpoint), LR"({"max_timeout":"0"})");
        for (int i = 0; i < numLoops; i++)
        {
            auto recognizer = factory->CreateSpeechRecognizerWithFileInput(input_file);
            auto result = recognizer->RecognizeAsync().get();
            REQUIRE(result->Reason == Reason::Recognized);
            REQUIRE(result->Text == L"Remind me to buy 5 iPhones.");
        }

        // BUGBUG: this currently fails because CSpxAudioStreamSession::WaitForRecognition() returns a nullptr on a timeout.
        /*
        factory = SpeechFactory::FromEndpoint(PAL::ToWString(Config::Endpoint), LR"({"max_timeout":"10000"})");
        for (int i = 0; i < numLoops; i++)
        {
        auto recognizer = factory->CreateSpeechRecognizerWithFileInput(input_file);
        auto result = recognizer->RecognizeAsync().get();
        REQUIRE(result->Reason == Reason::Recognized);
        REQUIRE(result->Text == L"Remind me to buy 5 iPhones.");
        }*/
    }
}

TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    REQUIRE(exists(input_file));

    mutex mtx;
    condition_variable cv;

    SECTION("Check for competing or conflicting conditions in destructor.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        bool callback_invoked = false;

        REQUIRE(!IsUsingMocks());
        auto recognizer = GetFactory()->CreateSpeechRecognizerWithFileInput(input_file);

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

TEST_CASE("Speech Factory basics", "[api][cxx]")
{
    SECTION("Check that factories have correct parameters.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        auto f1 = SpeechFactory::FromEndpoint(L"1", L"invalid_key");
        auto f2 = SpeechFactory::FromEndpoint(L"2", L"invalid_key");
        // TODO: I shouldn't need to use const cast here to read a parameter!
        auto endpoint1 = const_cast<FactoryParameterCollection&>(f1->Parameters)[FactoryParameter::Endpoint].GetString();
        auto endpoint2 = const_cast<FactoryParameterCollection&>(f2->Parameters)[FactoryParameter::Endpoint].GetString();

        //REQUIRE(endpoint1 == L"1"); BUGBUG this fails!!!
        REQUIRE(endpoint2 == L"2");
    }
}