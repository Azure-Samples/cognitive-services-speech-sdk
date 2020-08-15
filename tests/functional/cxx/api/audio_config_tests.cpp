//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <iostream>
#include <atomic>
#include <map>
#include <string>
#include <thread>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "mock_controller.h"


#ifdef _MSC_VER
#pragma warning(disable: 6237)
// Disable: (<zero> && <expression>) is always zero.  <expression> is never evaluated and might have side effects.
#endif

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace std;

static std::shared_ptr<SpeechConfig> SpeechConfigForAudioConfigTests(const std::string& trafficType = SpxGetTestTrafficType(__FILE__, __LINE__))
{
    auto config = !DefaultSettingsMap[ENDPOINT].empty()
        ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
        : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
    config->SetServiceProperty("TrafficType", trafficType, ServicePropertyChannel::UriQueryParameter);
    return config;
}

TEST_CASE("Audio Basics", "[api][cxx][audio]")
{
    std::string sessionId;

    std::string filePath = ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH);
    REQUIRE(exists(filePath));

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
            auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

            auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
            config->SetServiceProperty("TrafficType", SpxGetTestTrafficType(__FILE__, __LINE__), ServicePropertyChannel::UriQueryParameter);

            auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

            verifyAudioConfigInRecognizerProperties(recognizer);
        }

        WHEN("verify audio configuration with device name and channel count")
        {
            auto channelsExpected = "1";
            auto deviceNameExpected = "default";
            auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

            // verify number of channels
            audioConfig->SetProperty(PropertyId::AudioConfig_NumberOfChannelsForCapture, channelsExpected);
            auto channelsInAudioConfig = audioConfig->GetProperty(PropertyId::AudioConfig_NumberOfChannelsForCapture);
            REQUIRE(channelsExpected == channelsInAudioConfig);

            // verify device name
            audioConfig->SetProperty(PropertyId::AudioConfig_DeviceNameForCapture, deviceNameExpected);
            auto deviceNameInAudioConfig = audioConfig->GetProperty(PropertyId::AudioConfig_DeviceNameForCapture);
            REQUIRE(deviceNameExpected == deviceNameInAudioConfig);

            auto config = SpeechConfigForAudioConfigTests(SpxGetTestTrafficType(__FILE__, __LINE__));
            auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

            verifyAudioConfigInRecognizerProperties(recognizer, deviceNameExpected, channelsExpected);
        }
    }

    SECTION("pull stream works")
    {
        // Prepare for the stream to be "Pulled"
        auto stream = AudioDataStream::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH).c_str());

        // Create the "pull stream" object with C++ lambda callbacks
        auto pullStream = AudioInputStream::CreatePullStream(
            AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
            [&stream](uint8_t* buffer, uint32_t size) -> int { return (int)stream->ReadData(buffer, size); },
            [&stream]() { stream.reset(); }
        );

        // Create the recognizer with the pull stream
        auto config = SpeechConfigForAudioConfigTests(SpxGetTestTrafficType(__FILE__, __LINE__));
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
        auto config = SpeechConfigForAudioConfigTests(SpxGetTestTrafficType(__FILE__, __LINE__));
        auto pushStream = AudioInputStream::CreatePushStream();
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        recognizer->SessionStarted.Connect([&sessionId](const SessionEventArgs& e)
        {
            sessionId = e.SessionId;
        });

        // Prepare to use the "Push stream" by opening the file
        auto stream = AudioDataStream::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH).c_str());

        // Set up a lambda we'll use to push the data
        auto pushData = [=,&stream](const int bufferSize, int sleepBetween = 0, int sleepBefore = 0, int sleepAfter = 0, bool closeStream = true) {
            auto deleter = [](uint8_t * p) { delete[] p; };
            std::unique_ptr<uint8_t[], decltype(deleter)> buffer(new uint8_t[bufferSize], deleter);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepBefore));
            for (;;)
            {
                auto size = stream->ReadData(buffer.get(), bufferSize);
                if (size == 0) break;

                pushStream->Write(buffer.get(), size);

                std::this_thread::sleep_for(std::chrono::milliseconds(sleepBetween));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepAfter));
            if (closeStream)
            {
                pushStream->Close();
            }
            stream.reset();
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
