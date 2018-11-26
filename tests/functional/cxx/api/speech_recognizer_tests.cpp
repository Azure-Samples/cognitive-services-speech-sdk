//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

template< typename RecogType>
static std::shared_ptr<RecogType> CreateRecognizers(const string& filename)
{
    auto audioInput = AudioConfig::FromWavFileInput(filename);
    return RecogType::FromConfig(CurrentSpeechConfig(), audioInput);
}

TEST_CASE("continuousRecognitionAsync using push stream", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioConfig = AudioConfig::FromStreamInput(pushStream);

    // Creates a speech recognizer from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
    weather.UpdateFullFilename(Config::InputDir);

    SPXTEST_SECTION("continuous and once")
    {
        promise<string> result;
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().wait();
        PushData(pushStream.get(), weather.m_audioFilename);
        auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().wait();
        SPXTEST_REQUIRE(text.compare(weather.m_utterance) == 0);

        //todo: we should allow switching recog mode. Fix this later.
        //pushData(pushStream.get(), weather.m_audioFilename);
        //REQUIRE_THROWS(recognizer->RecognizeOnceAsync().get());
    }
    SPXTEST_SECTION("continuous and kws")
    {
        DoContinuousReco(recognizer.get(), pushStream.get());
        DoKWS(recognizer.get(), pushStream.get());
    }
    SPXTEST_SECTION("continuous start stop 3 times")
    {
        auto loop = 3;
        for (int i = 0; i < loop; i++)
        {
            promise<string> result;
            ConnectCallbacks(recognizer.get(), result);
            // TODO: move PushData() after StartContinuousRecongtion
            // after fixing the bug Bug 1506194: PushStream Improvement, endOfStream
            PushData(pushStream.get(), weather.m_audioFilename);
            recognizer->StartContinuousRecognitionAsync().wait();

            auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().wait();
            SPXTEST_REQUIRE(text.compare(weather.m_utterance) == 0);
        }
    }
#if 0
    // Created #1502076
    //todo: we need to turn it on after refactor audio session stream
    SPXTEST_SECTION("kws and continuous")
    {
        doKWS(recognizer.get(), pushStream.get());
        // the KWS does not go back to idle, stays as "StoppingPump"
        doContinuousReco(recognizer.get(), pushStream.get());
    }
#endif

}

TEST_CASE("ContinuousRecognitionAsync using file input", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    weather.UpdateFullFilename(Config::InputDir);
    SPXTEST_REQUIRE(exists(weather.m_audioFilename));

    auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);

    // a normal case.
    SPXTEST_SECTION("start and stop once")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().wait();

        ConnectCallbacks(recognizer.get(), result);
        auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().wait();
        SPXTEST_REQUIRE(text.compare(weather.m_utterance) == 0);
    }
    // Another normal case. no stop is a valid user case.
    SPXTEST_SECTION("start without stop")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().wait();

        ConnectCallbacks(recognizer.get(), result);
        auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);

        SPXTEST_REQUIRE(text.compare(weather.m_utterance) == 0);
    }
    SPXTEST_SECTION("two starts in a row")
    {
        promise<string> result;
        ConnectCallbacks(recognizer.get(), result);

        recognizer->StartContinuousRecognitionAsync().wait();
        recognizer->StartContinuousRecognitionAsync().wait();

        auto text = WaitForResult(result.get_future(), 1s);

        SPXTEST_REQUIRE(text.find("SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION") != string::npos);
    }
#if 0
    // start and stop immediately
    SPXTEST_SECTION("start and then stop immediately")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().wait();

        ConnectCallbacks(recognizer.get(), result);
        recognizer->StopContinuousRecognitionAsync().wait();

        // the recognizer should be idle after stop.
        auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    }
    // start and stop in the middle
    SPXTEST_SECTION("stop in the middle of reco")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().wait();

        ConnectCallbacks(recognizer.get(), result);

        // wait for 500 ms and stop recognition.
        // hopefully, we cut the audio session while pumping data.
        this_thread::sleep_for(500ms);

        recognizer->StopContinuousRecognitionAsync().wait();
    }
#endif
}

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    weather.UpdateFullFilename(Config::InputDir);

    SPXTEST_SECTION("push stream with continuous reco")
    {
        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));

        auto config = CurrentSpeechConfig();
        auto pushStream = AudioInputStream::CreatePushStream();
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);

        // Creates a speech recognizer from stream input;
        auto audioInput = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        // promise for synchronization of recognition end.
        promise<void> recognitionEnd;

        string result;
        recognizer->Recognized.Connect([&result](const SpeechRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                result = e.Result->Text;
                SPX_TRACE_VERBOSE("RECOGNIZED: Text= %s, Offset= %d, Duration= %d", e.Result->Text.c_str(), e.Result->Offset(), e.Result->Duration());
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                SPX_TRACE_VERBOSE("NOMATCH: Speech could not be recognized.");
            }
        });
        string error;
        recognizer->Canceled.Connect([&recognitionEnd, &error](const SpeechRecognitionCanceledEventArgs& e)
        {
            switch (e.Reason)
            {
            case CancellationReason::EndOfStream:
                SPX_TRACE_VERBOSE("CANCELED: Reach the end of the file.");
                break;

            case CancellationReason::Error:
                error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
                SPX_TRACE_VERBOSE("CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
                recognitionEnd.set_value();
                break;

            default:
                SPX_TRACE_VERBOSE("unknown Reason!");
            }
        });
        recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            SPX_TRACE_VERBOSE("Session stopped. Id = %s", e.SessionId.c_str());
            recognitionEnd.set_value();
        });

        auto fs = OpenWaveFile(weather.m_audioFilename);

        std::array<uint8_t, 1000> buffer;
        recognizer->StartContinuousRecognitionAsync().wait();
        while (1)
        {
            auto readSamples = ReadBuffer(fs, buffer.data(), (uint32_t)buffer.size());
            if (readSamples == 0)
            {
                break;
            }
            pushStream->Write(buffer.data(), readSamples);
        }

        pushStream->Close();
        recognitionEnd.get_future().wait();
        recognizer->StopContinuousRecognitionAsync().wait();

        SPXTEST_REQUIRE(result.compare(weather.m_utterance) == 0);
        SPXTEST_REQUIRE(error.empty());
    }

    SPXTEST_SECTION("KWS throws exception given 11khz sampling rate")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);
        wrongSamplingRateFile.UpdateFullFilename(Config::InputDir);
        UseMocks(false);
        SPXTEST_REQUIRE(exists(wrongSamplingRateFile.m_audioFilename));

        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
        auto audio = AudioConfig::FromWavFileInput(wrongSamplingRateFile.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/heycortana_en-US.table");
        REQUIRE_THROWS(recognizer->StartKeywordRecognitionAsync(model).get());
    }
    SPXTEST_SECTION("throw exception when the file does not existing")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);

        SPXTEST_REQUIRE(!IsUsingMocks());
        REQUIRE_THROWS(CreateRecognizers<SpeechRecognizer>("non-existing-file.wav"));
    }
    SPXTEST_SECTION("return an error message in RecognizeOnceAsync given an invalid endpoint")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromEndpoint("Invalid-endpoint", Keys::Speech);
        auto audio = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        auto result = recognizer->RecognizeOnceAsync().get();

        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Failed to create transport request.") != std::string::npos);
    }
    SPXTEST_SECTION("return canceled in StartContinuousRecognitionAsync given an invalid endpoint")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromEndpoint("Invalid-endpoint", Keys::Speech);
        auto audio = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        std::string errorDetails;
        CancellationErrorCode errorCode;
        recognizer->Canceled += [&](const SpeechRecognitionCanceledEventArgs& e)
        {
            if (e.Reason == CancellationReason::Error)
            {
                errorDetails = e.ErrorDetails;
                errorCode = e.ErrorCode;
            }
        };
        recognizer->StartContinuousRecognitionAsync().get();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        recognizer->StopContinuousRecognitionAsync().wait();

        SPXTEST_REQUIRE(errorCode == CancellationErrorCode::ConnectionFailure);
        SPXTEST_REQUIRE(errorDetails.find("Failed to create transport request.") != std::string::npos);
    }
    SPXTEST_SECTION("Check that recognition can set authorization token")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);

        std::string token("Thursday");
        recognizer->SetAuthorizationToken(token);
        SPXTEST_REQUIRE(recognizer->GetAuthorizationToken() == token);

        token = "Friday";
        recognizer->SetAuthorizationToken(token);
        SPXTEST_REQUIRE(recognizer->GetAuthorizationToken() == token);
    }

    SPXTEST_SECTION("Check that recognizer does not crash while async op is in progress")
    {
        UseMocks(true);
        SPXTEST_REQUIRE(IsUsingMocks());

        const int numLoops = 10;

        {
            vector<std::future<std::shared_ptr<SpeechRecognitionResult>>> futures;
            for (int i = 0; i < numLoops; i++)
            {
                auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
                SPXTEST_REQUIRE(recognizer != nullptr);
                futures.push_back(recognizer->RecognizeOnceAsync());
            }
        }

        auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/heycortana_en-US.table");
        SPXTEST_REQUIRE(model != nullptr);

        {
            vector<std::future<void>> futures;
            int numAsyncMethods = 4;
            for (int i = 0; i < numLoops*numAsyncMethods; i++)
            {
                auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
                SPXTEST_REQUIRE(recognizer != nullptr);
                if (i % numAsyncMethods == 0)
                {
                    futures.push_back(recognizer->StartContinuousRecognitionAsync());
                }
                else if (i % numAsyncMethods == 1)
                {
                    futures.push_back(recognizer->StopContinuousRecognitionAsync());
                }
                else if (i % numAsyncMethods == 2)
                {
                    futures.push_back(recognizer->StartKeywordRecognitionAsync(model));
                }
                else
                {
                    futures.push_back(recognizer->StopKeywordRecognitionAsync());
                }
            }
        }
    }

    GIVEN("Mocks for USP, Microphone, WavFilePump and Reader, and then USP ...")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(true);

        SPXTEST_REQUIRE(exists(weather.m_audioFilename));

        mutex mtx;
        condition_variable cv;

        SPXTEST_WHEN("We checking to make sure callback counts are correct (checking multiple times, and multiple speeds times) ...")
        {
            std::map<Callbacks, atomic_int> callbackCounts = createCallbacksMap();

            // We're going to loop thru 11 times... The first 10, we'll use mocks. The last time we'll use the USP
            // NOTE: Please keep this at 11... It tests various "race"/"speed" configurations of the core system...
            // NOTE: When running against the localhost, loop 20 times... Half the time, we'll use mocks, and half - the USP.
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

                auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);
                SPXTEST_REQUIRE(recognizer != nullptr);
                SPXTEST_REQUIRE(IsUsingMocks(useMockUsp));

                bool sessionEnded = false;

                recognizer->Recognized.Connect([&](const SpeechRecognitionEventArgs&) {
                    callbackCounts[Callbacks::final_result]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::final_result]=%d", callbackCounts[Callbacks::final_result].load());
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

                auto result = recognizer->RecognizeOnceAsync().get();
                CHECK(result != nullptr);

                SPX_TRACE_VERBOSE("%s: Wait for session end (loop #%d)", __FUNCTION__, i);
                unique_lock<mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return sessionEnded; });
                lock.unlock();

                SPX_TRACE_VERBOSE("%s: Make sure callbacks are invoked correctly; END of loop #%d", __FUNCTION__, i);
                CHECK(callbackCounts[Callbacks::session_started] == i + 1);
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

    SPXTEST_SECTION("Check that recognition result contains original json payload.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(!result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult).empty());
    }

    SPXTEST_SECTION("Check that recognition result contains error details.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());
        auto badKeyConfig = SpeechConfig::FromSubscription("invalid_key", "invalid_region");
        auto audioConfig = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(badKeyConfig, audioConfig);
        auto result = recognizer->RecognizeOnceAsync().get();

        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1480495
        //SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        SPXTEST_REQUIRE(!cancellation->ErrorDetails.empty());

        // NOTE: Looks like we still do need this...
        // TODO: there's a data race in the audio_pump thread when it tries to
        // pISpxAudioProcessor->SetFormat(nullptr); after exiting the loop.
        // Comment out the next line to see for yourself (repros on Linux build machines).
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    SPXTEST_SECTION("Wrong Key triggers Canceled Event")
    {
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        UseMocks(false);
        mutex mtx;
        condition_variable cv;

        bool connectionReportedError = false;
        bool canceled = false;
        string wrongKey = "wrongKey";

        auto sc = SpeechConfig::FromSubscription(wrongKey, "westus");
        auto a = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(sc, a);

        recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& args) {
            unique_lock<mutex> lock(mtx);
            canceled = true;
            connectionReportedError =
                args.Reason == CancellationReason::Error &&
                args.ErrorCode == CancellationErrorCode::AuthenticationFailure &&
                !args.ErrorDetails.empty();
            cv.notify_one();
        });

        auto result = recognizer->RecognizeOnceAsync().get();
        CHECK(result->Reason == ResultReason::Canceled);
        CHECK(canceled);
        REQUIRE(connectionReportedError);
    }

    SPXTEST_SECTION("German Speech Recognition works")
    {
        callTheFirstOne.UpdateFullFilename(Config::InputDir);
        SPXTEST_REQUIRE(exists(callTheFirstOne.m_audioFilename));

        auto sc = !Config::Endpoint.empty() ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech) : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
        sc->SetSpeechRecognitionLanguage("de-DE");
        auto audioConfig = AudioConfig::FromWavFileInput(callTheFirstOne.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(!result->Text.empty());
    }

    SPXTEST_SECTION("Canceled/EndOfStream works")
    {
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));

        auto sc = !Config::Endpoint.empty() ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech) : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
        auto audioConfig = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);

        WHEN("using RecognizeOnceAsync() result")
        {
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(!result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::NoMatch);

            auto nomatch = NoMatchDetails::FromResult(result);
            SPXTEST_REQUIRE(nomatch != nullptr);
            SPXTEST_REQUIRE(nomatch->Reason == NoMatchReason::InitialSilenceTimeout);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

            auto cancellation = CancellationDetails::FromResult(result);
            SPXTEST_REQUIRE(cancellation != nullptr);
            SPXTEST_REQUIRE(cancellation->ErrorDetails.empty());
            SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::EndOfStream);
        }

        WHEN("using RecognizeOnceAsync() and SpeechRecognitionCanceledEventArgs e.Result")
        {
            mutex mtx;
            condition_variable cv;
            auto sessionStoppedCount = 0;
            std::vector<std::string> events;

            recognizer->Recognized.Connect([&](const SpeechRecognitionEventArgs& e) {
                auto result = e.Result;
                if (result->Reason == ResultReason::RecognizedSpeech)
                {
                    if (!result->Text.empty())
                    {
                        events.push_back("RecognizedSpeech-NonEmpty");
                    }
                    else
                    {
                        events.push_back("RecognizedSpeech-Other");
                    }
                }
                else if (result->Reason == ResultReason::NoMatch)
                {
                    auto nomatch = NoMatchDetails::FromResult(result);
                    if (result->Text.empty() &&
                        nomatch->Reason == NoMatchReason::InitialSilenceTimeout)
                    {
                        events.push_back("NoMatch-InitialSilenceTimeout");
                    }
                    else
                    {
                        events.push_back("NoMatch-Other");
                    }
                }
                else
                {
                    events.push_back("Recognized-Other");
                }
            });

            recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& e) {
                auto result = e.Result;

                if (e.Reason == CancellationReason::EndOfStream)
                {
                    auto cancellation = CancellationDetails::FromResult(result);
                    if (result->Text.empty() &&
                        result->Reason == ResultReason::Canceled &&
                        cancellation->ErrorDetails.empty() &&
                        cancellation->Reason == CancellationReason::EndOfStream)
                    {
                        events.push_back("EndOfStream");
                    }
                    else
                    {
                        events.push_back("EndOfStream-Malformed");
                    }
                }
                else
                {
                    events.push_back("Canceled-Other");
                }
            });

            recognizer->SessionStarted.Connect([&](const SessionEventArgs&) {
                events.push_back("SessionStarted");
                cv.notify_all();
            });

            recognizer->SessionStopped.Connect([&](const SessionEventArgs&) {
                events.push_back("SessionStopped");
                ++sessionStoppedCount;
                cv.notify_all();
            });

            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(30), [&] { return sessionStoppedCount == 3; });
            lock.unlock();

            std::vector<std::string> expectedEvents = {
                "SessionStarted", "RecognizedSpeech-NonEmpty", "SessionStopped",
                "SessionStarted", "NoMatch-InitialSilenceTimeout", "SessionStopped",
                "SessionStarted", "EndOfStream", "SessionStopped"
            };

            REQUIRE(expectedEvents == events);
        }
    }

    SPXTEST_SECTION("German Speech Recognition works")
    {
        callTheFirstOne.UpdateFullFilename(Config::InputDir);
        SPXTEST_REQUIRE(exists(callTheFirstOne.m_audioFilename));

        auto sc = !Config::Endpoint.empty() ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech) : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
        sc->SetSpeechRecognitionLanguage("de-DE");
        auto audioConfig = AudioConfig::FromWavFileInput(callTheFirstOne.m_audioFilename);
        auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(!result->Text.empty());
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

        SPXTEST_WHEN("We do a keyword recognition with a speech recognizer")
        {
            SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

            auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
            SPXTEST_REQUIRE(recognizer != nullptr);
            SPXTEST_REQUIRE(IsUsingMocks(true));

            recognizer->Recognized += [&](const SpeechRecognitionEventArgs& /* e */) {
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

            auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/heycortana_en-US.table");
            recognizer->StartKeywordRecognitionAsync(model);

            THEN("We wait up to 30 seconds for a KwsSingleShot recognition and it's accompanying SessionStopped")
            {
                SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return gotFinalResult >= 1 && gotSessionStopped >= 1; });
                lock.unlock();

                recognizer->StopKeywordRecognitionAsync().get();

                THEN("We should see that we got at least 1 Recognized and the same number of SessionStopped events")
                {
                    SPXTEST_REQUIRE(gotFinalResult >= 1);
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
    weather.UpdateFullFilename(Config::InputDir);

    SPXTEST_SECTION("Stress testing against the local server")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        if (Config::Endpoint.empty())
        {
            return;
        }

        UseMocks(false);
        SPXTEST_REQUIRE(exists(weather.m_audioFilename));
        SPXTEST_REQUIRE(!IsUsingMocks());

        const int numLoops = 10;

        auto sc = SpeechConfig::FromEndpoint(Config::Endpoint, R"({"max_timeout":"0"})");
        for (int i = 0; i < numLoops; i++)
        {
            auto audioConfig = AudioConfig::FromWavFileInput(weather.m_audioFilename);
            auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(result->Text == "Remind me to buy 5 iPhones.");
        }

        // BUGBUG: this currently fails because CSpxAudioStreamSession::WaitForRecognition() returns a nullptr on a timeout.
        /*
        factory = SpeechFactory::FromEndpoint(PAL::ToWString(Config::Endpoint), LR"({"max_timeout":"10000"})");
        for (int i = 0; i < numLoops; i++)
        {
        auto audioConfig = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = factory->CreateSpeechRecognizerFromConfig(audioConfig);
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(result->Text == L"Remind me to buy 5 iPhones.");
        }
        */
    }
}

TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    weather.UpdateFullFilename(Config::InputDir);

    SPXTEST_REQUIRE(exists(weather.m_audioFilename));

    mutex mtx;
    condition_variable cv;

    SPXTEST_SECTION("Check for competing or conflicting conditions in destructor.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        bool callback_invoked = false;

        SPXTEST_REQUIRE(!IsUsingMocks());
        auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);

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

        auto canceledCallback = [&](const SpeechRecognitionCanceledEventArgs& args) { callback(args); };

        recognizer->Recognized.Connect(callback);
        recognizer->Canceled.Connect(canceledCallback); // Canceled is called if there are connection issues.

        auto result = recognizer->RecognizeOnceAsync().get();

        {
            unique_lock<mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(10), [&] { return callback_invoked; });
            SPXTEST_REQUIRE(callback_invoked);
        }
        recognizer.reset();
    }

    SPXTEST_SECTION("Check for a deadlock in disconnect.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        SPXTEST_REQUIRE(!IsUsingMocks());
        auto recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);

        auto callback1 = [&](const SpeechRecognitionEventArgs& args)
        {
            if (args.Result->Reason == ResultReason::RecognizedSpeech)
            {
                recognizer->Canceled.DisconnectAll();
            }
            else
            {
                recognizer->Recognized.DisconnectAll();
            }
        };
        auto canceledCallback1 = [&](const SpeechRecognitionCanceledEventArgs& args) { callback1(args); };

        recognizer->Recognized.Connect(callback1);
        recognizer->Canceled.Connect(canceledCallback1);

        auto result = recognizer->RecognizeOnceAsync().get();
        UNUSED(result);

        auto callback2 = [&](const SpeechRecognitionEventArgs&)
        {
            recognizer->Canceled.DisconnectAll();
            recognizer->Recognized.DisconnectAll();
        };
        auto canceledCallback2 = [&](const SpeechRecognitionCanceledEventArgs& args) { callback2(args); };

        recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);
        recognizer->Recognized.Connect(callback2);
        recognizer->Canceled.Connect(canceledCallback2);

        result = recognizer->RecognizeOnceAsync().get();
        UNUSED(result);

        auto callback3 = [&](const SpeechRecognitionEventArgs&)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // TODO: fix test
            // This captures a reference to stack-memory which is no longer active when the callback is called
            // recognizer.reset();
        };

        auto canceledCallback3 = [&](const SpeechRecognitionEventArgs& args)
        {
            callback3(args);
        };

        recognizer = CreateRecognizers<SpeechRecognizer>(weather.m_audioFilename);
        recognizer->Recognized.Connect(callback3);
        recognizer->Canceled.Connect(canceledCallback3);
        auto future = recognizer->RecognizeOnceAsync();
        UNUSED(future);
    }
}

TEST_CASE("Speech Recognizer SpeechConfig validations", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_SECTION("Check that FromEndpoint raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromEndpoint("", ""));
        CHECK_THROWS(SpeechConfig::FromEndpoint("", "illegal-subscription"));
        CHECK_NOTHROW(SpeechConfig::FromEndpoint("illegal-endpoint", "illegal-subscription"));
    }

    SPXTEST_SECTION("Check that FromSubscription raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromSubscription("", ""));
        CHECK_THROWS(SpeechConfig::FromSubscription("", "illegal-region"));
        CHECK_THROWS(SpeechConfig::FromSubscription("illegal-subscription", ""));
        CHECK_NOTHROW(SpeechConfig::FromSubscription("illegal-subscription", "illegal-region"));
    }

    SPXTEST_SECTION("Check that FromAuthorizationToken raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("", ""));
        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("", "illegal-region"));
        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("illegal-token", ""));
        CHECK_NOTHROW(SpeechConfig::FromAuthorizationToken("illegal-token", "illegal-region"));
    }
}
