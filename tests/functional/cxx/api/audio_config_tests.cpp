//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>
#include <thread>

#include "test_utils.h"
#include "file_utils.h"

#include "speechapi_cxx.h"
#include "mock_controller.h"

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace std;

static string get_input_file()
{
    return Config::InputDir + "/audio/whatstheweatherlike.wav";
}

static std::shared_ptr<SpeechConfig> SpeechConfigForAudioConfigTests()
{
    return !Config::Endpoint.empty()
        ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech)
        : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
}

TEST_CASE("Audio Config Basics", "[api][cxx][audio]")
{
    SPXTEST_SECTION("Audio Pull Stream works")
    {
        SPXTEST_REQUIRE(exists(PAL::ToWString(get_input_file())));

        // Prepare for the stream to be "Pulled"
        auto fs = OpenWaveFile(get_input_file());

        // Create the "pull stream" object with C++ lambda callbacks
        auto pullStream = AudioInputStream::CreatePullStream(
            AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
            [ &fs](uint8_t* buffer, uint32_t size) -> int { return (int)ReadBuffer(fs, buffer,size); },
            [=]() { });

        // Create the recognizer with the pull stream
        auto config = SpeechConfigForAudioConfigTests();
        auto audioConfig = AudioConfig::FromStreamInput(pullStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
    }
    SPXTEST_SECTION("Audio Push Stream works")
    {
        SPXTEST_REQUIRE(exists(PAL::ToWString(get_input_file())));

        // Create the recognizer "with stream input" with a "push stream"
        auto config = SpeechConfigForAudioConfigTests();
        auto pushStream = AudioInputStream::CreatePushStream();
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        // Prepare to use the "Push stream" by opening the file, and moving to head of data chunk
        FILE* hfile = nullptr;
        PAL::fopen_s(&hfile, get_input_file().c_str(), "rb");
        fseek(hfile, 44, SEEK_CUR);

        // Set up a lambda we'll use to push the data
        auto pushData = [=](const int bufferSize, int sleepBetween = 0, int sleepBefore = 0, int sleepAfter = 0, bool closeStream = true) {
            auto deleter = [](uint8_t * p) { delete[] p; };
            std::unique_ptr<uint8_t[], decltype(deleter)> buffer(new uint8_t[bufferSize], deleter);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepBefore));
            for (;;) {
                auto size = (int)fread(buffer.get(), 1, bufferSize, hfile);
                if (size == 0) break;
                pushStream->Write(buffer.get(), size);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepBetween));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepAfter));
            if (closeStream) {
                pushStream->Close();
            }
            fclose(hfile);
        };

        SPXTEST_WHEN("pushing audio data BEFORE recognition starts, 100000 byte buffer")
        {
            pushData(100000);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }

        SPXTEST_WHEN("pushing audio data BEFORE recognition starts, 1000 byte buffer")
        {
            pushData(1000);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }

        SPXTEST_WHEN("pushing audio data BEFORE recognition starts, 1000 byte buffer, 50ms between buffers")
        {
            pushData(1000, 50);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }

        SPXTEST_WHEN("pushing audio data 2000ms AFTER recognition starts, 100000 byte buffer, 50ms between buffers")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(100000, 50, 2000);
            auto result = future.get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }

        SPXTEST_WHEN("pushing audio data 2000ms AFTER recognition starts, 1000 byte buffer, 50ms between buffers")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(1000, 50, 2000);
            auto result = future.get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }

        SPXTEST_WHEN("pushing audio data 2000ms AFTER recognition starts, 1000 byte buffer, 100ms between each")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(1000, 100, 2000);
            auto result = future.get();
            SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result);
        }
    }
}
