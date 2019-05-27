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
#include "recognizer_utils.h"

#include "speechapi_cxx.h"
#include "mock_controller.h"


#ifdef _MSC_VER
#pragma warning(disable: 6237)
// Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace std;


static std::shared_ptr<SpeechConfig> SpeechConfigForAudioConfigTests()
{
    return !Config::Endpoint.empty()
        ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech)
        : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
}

TEST_CASE("Audio Basics", "[api][cxx][audio]")
{
    std::string sessionId;

    weather.UpdateFullFilename(Config::InputDir);
    REQUIRE(exists(weather.m_inputDataFilename));

    auto requireRecognizedSpeech = [&sessionId](std::shared_ptr<SpeechRecognitionResult> result)
    {
        CAPTURE(sessionId);
        REQUIRE(result != nullptr);

        std::ostringstream details;

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
        REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        REQUIRE(!result->Text.empty());
    };
    SECTION("Audio Config Properties")
    {
        auto verifyAudioConfigInRecognizerProperties = [](std::shared_ptr< SpeechRecognizer> recognizer, std::string deviceNameExpected = "",  std::string channelsExpected = "1")
        {
            // verify device name in recognizer
            auto deviceNameInRecognizer = recognizer->Properties.GetProperty(PropertyId::AudioConfig_DeviceNameForCapture);
            REQUIRE(deviceNameExpected == deviceNameInRecognizer);

            // verify number of channels in recognizer
            auto channelsInRecognizer = recognizer->Properties.GetProperty(PropertyId::AudioConfig_NumberOfChannelsForCapture);
            REQUIRE(channelsExpected == channelsInRecognizer);

            // verify sample rate in recognizer
            auto sampleRateExpected = "16000";
            auto sampleRateInRecognizer = recognizer->Properties.GetProperty(PropertyId::AudioConfig_SampleRateForCapture);
            REQUIRE(sampleRateExpected == sampleRateInRecognizer);

            // verify sample rate in recognizer
            auto bitsPerSampleExpected = "16";
            auto bitsPerSampleInRecognizer = recognizer->Properties.GetProperty(PropertyId::AudioConfig_BitsPerSampleForCapture);
            REQUIRE(bitsPerSampleExpected == bitsPerSampleInRecognizer);
        };

        WHEN("verify audio configuration from audio file")
        {
            auto audioConfig = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);

            auto config = SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
            auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

            verifyAudioConfigInRecognizerProperties(recognizer);
        }

        WHEN("verify audio configuration with device name and channel count")
        {
            auto channelsExpected = "1";
            auto deviceNameExpected = "default";
            auto audioConfig = AudioConfig::FromWavFileInput(weather.m_inputDataFilename);

            // verify number of channels
            audioConfig->SetProperty(PropertyId::AudioConfig_NumberOfChannelsForCapture, channelsExpected);
            auto channelsInAudioConfig = audioConfig->GetProperty(PropertyId::AudioConfig_NumberOfChannelsForCapture);
            REQUIRE(channelsExpected == channelsInAudioConfig);

            // verify device name
            audioConfig->SetProperty(PropertyId::AudioConfig_DeviceNameForCapture, deviceNameExpected);
            auto deviceNameInAudioConfig = audioConfig->GetProperty(PropertyId::AudioConfig_DeviceNameForCapture);
            REQUIRE(deviceNameExpected == deviceNameInAudioConfig);

            auto config = SpeechConfigForAudioConfigTests();
            auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

            verifyAudioConfigInRecognizerProperties(recognizer, deviceNameExpected, channelsExpected);
        }
    }

    SECTION("pull stream works")
    {
        // Prepare for the stream to be "Pulled"
        auto fs = OpenWaveFile(weather.m_inputDataFilename);

        // Create the "pull stream" object with C++ lambda callbacks
        auto pullStream = AudioInputStream::CreatePullStream(
            AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
            [&fs](uint8_t* buffer, uint32_t size) -> int { return (int)ReadBuffer(fs, buffer,size); },
            [=]() {}
        );

        // Create the recognizer with the pull stream
        auto config = SpeechConfigForAudioConfigTests();
        auto audioConfig = AudioConfig::FromStreamInput(pullStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        recognizer->SessionStarted.Connect([&sessionId](const SessionEventArgs& e)
        {
            sessionId = e.SessionId;
        });

        auto result = recognizer->RecognizeOnceAsync().get();
        requireRecognizedSpeech(result);
    }

    SECTION("push stream works")
    {
        // Create the recognizer "with stream input" with a "push stream"
        auto config = SpeechConfigForAudioConfigTests();
        auto pushStream = AudioInputStream::CreatePushStream();
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        recognizer->SessionStarted.Connect([&sessionId](const SessionEventArgs& e)
        {
            sessionId = e.SessionId;
        });

        // Prepare to use the "Push stream" by opening the file, and moving to head of data chunk
        FILE* hfile = nullptr;
        PAL::fopen_s(&hfile, weather.m_inputDataFilename.c_str(), "rb");
        fseek(hfile, 44, SEEK_CUR);

        // Set up a lambda we'll use to push the data
        auto pushData = [=](const int bufferSize, int sleepBetween = 0, int sleepBefore = 0, int sleepAfter = 0, bool closeStream = true) {
            auto deleter = [](uint8_t * p) { delete[] p; };
            std::unique_ptr<uint8_t[], decltype(deleter)> buffer(new uint8_t[bufferSize], deleter);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepBefore));
            for (;;)
            {
                auto size = (int)fread(buffer.get(), 1, bufferSize, hfile);
                if (size == 0) break;

                pushStream->Write(buffer.get(), size);

                std::this_thread::sleep_for(std::chrono::milliseconds(sleepBetween));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepAfter));
            if (closeStream)
            {
                pushStream->Close();
            }
            fclose(hfile);
        };

        WHEN("pushing audio data BEFORE recognition starts, 100000 byte buffer")
        {
            pushData(100000);
            auto result = recognizer->RecognizeOnceAsync().get();
            requireRecognizedSpeech(result);
        }

        WHEN("pushing audio data BEFORE recognition starts, 1000 byte buffer")
        {
            pushData(1000);
            auto result = recognizer->RecognizeOnceAsync().get();
            requireRecognizedSpeech(result);
        }

        WHEN("pushing audio data BEFORE recognition starts, 1000 byte buffer, 50ms between buffers")
        {
            pushData(1000, 50);
            auto result = recognizer->RecognizeOnceAsync().get();
            requireRecognizedSpeech(result);
        }

        WHEN("pushing audio data 2000ms AFTER recognition starts, 100000 byte buffer, 50ms between buffers")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(100000, 50, 2000);
            auto result = future.get();
            requireRecognizedSpeech(result);
        }

        WHEN("pushing audio data 2000ms AFTER recognition starts, 1000 byte buffer, 50ms between buffers")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(1000, 50, 2000);
            auto result = future.get();
            requireRecognizedSpeech(result);
        }

        WHEN("pushing audio data 2000ms AFTER recognition starts, 1000 byte buffer, 100ms between each")
        {
            auto future = recognizer->RecognizeOnceAsync();
            pushData(1000, 100, 2000);
            auto result = future.get();
            requireRecognizedSpeech(result);
        }
    }
}
