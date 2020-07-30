//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <sys/stat.h>
#include "test_utils.h"
#include "file_utils.h"
#include "synthesizer_utils.h"

namespace TTS
{
    std::shared_ptr<SpeechConfig> CurrentSpeechConfig()
    {
        return RestSpeechConfig();
    }

    std::shared_ptr<SpeechConfig> RestSpeechConfig()
    {
        auto endpoint = !DefaultSettingsMap[ENDPOINT].empty() ? DefaultSettingsMap[ENDPOINT]
            : "https://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".tts.speech.microsoft.com/cognitiveservices/v1?TrafficType=Test";
        auto config = SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        return config;
    }

    shared_ptr<SpeechConfig> UspSpeechConfig()
    {
        auto endpoint = !DefaultSettingsMap[ENDPOINT].empty() ? DefaultSettingsMap[ENDPOINT]
            : "wss://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".tts.speech.microsoft.com/cognitiveservices/websocket/v1?TrafficType=Test";
        return SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    }

    shared_ptr<SpeechConfig> MockSpeechConfig()
    {
        auto config = SpeechConfig::FromSubscription("None", "None");
        config->SetProperty("CARBON-INTERNAL-UseTtsEngine-Mock", "true");
        return config;
    }

    long GetFileSize(const string& filename)
    {
        struct stat info;
        stat(filename.data(), &info);
        auto fileSize = info.st_size;

        return fileSize;
    }

    void DoSomethingWithAudioInPullStream(shared_ptr<PullAudioOutputStream> stream, bool& canceled)
    {
        DoSomethingWithAudioInPullStream(stream, canceled, nullptr);
    }

    void DoSomethingWithAudioInPullStream(shared_ptr<PullAudioOutputStream> stream, bool& canceled, shared_ptr<vector<uint8_t>> expectedData)
    {
        uint8_t buffer[1024];
        uint32_t totalSize = 0;
        uint32_t filledSize = 0;

        auto actualData = std::make_shared<std::vector<uint8_t>>();

        while ((filledSize = stream->Read(buffer, sizeof(buffer))) > 0) { /* blocks until atleast 1024 bytes are available */
            /* do something with buffer */
            totalSize += filledSize;
            actualData->resize(totalSize);
            memcpy(actualData->data() + totalSize - filledSize, buffer, filledSize);
        }

        if (!canceled)
        {
            SPXTEST_REQUIRE(totalSize > 0);
        }

        if (expectedData != nullptr)
        {
            SPXTEST_REQUIRE(AreBinaryEqual(expectedData, actualData));
        }
    }

    future<void> DoSomethingWithAudioInPullStreamInBackground(shared_ptr<PullAudioOutputStream> stream, bool& canceled)
    {
        return DoSomethingWithAudioInPullStreamInBackground(stream, canceled, nullptr);
    }

    future<void> DoSomethingWithAudioInPullStreamInBackground(shared_ptr<PullAudioOutputStream> stream, bool& canceled, std::shared_ptr<std::vector<uint8_t>> expectedData)
    {
        auto future = std::async(launch::async, [stream, &canceled, expectedData]() {
            DoSomethingWithAudioInPullStream(stream, canceled, expectedData); // calls to stream->Read(buffer, size) will block until size data is available
        });

        return future;
    }

    void DoSomethingWithAudioInVector(shared_ptr<vector<uint8_t>> audio, size_t audioLength)
    {
        SPXTEST_REQUIRE(audio.get() != nullptr);
        SPXTEST_REQUIRE(audioLength == audio->size());
    }

    void DoSomethingWithAudioInDataStream(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone)
    {
        DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, nullptr);
    }

    void DoSomethingWithAudioInDataStream(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone, std::shared_ptr<std::vector<uint8_t>> expectedData)
    {
        if (afterSynthesisDone)
        {
            SPXTEST_REQUIRE(stream->GetStatus() == StreamStatus::AllData);
        }

        CheckAudioInDataStream(stream, expectedData);

        if (!afterSynthesisDone)
        {
            SPXTEST_REQUIRE(stream->GetStatus() == StreamStatus::AllData);
        }
    }

    void CheckAudioInDataStream(shared_ptr<AudioDataStream> stream, std::shared_ptr<std::vector<uint8_t>> expectedData)
    {
        uint8_t buffer[1024];
        uint32_t totalSize = 0;
        uint32_t filledSize = 0;

        auto actualData = std::make_shared<std::vector<uint8_t>>();

        while ((filledSize = stream->ReadData(buffer, sizeof(buffer))) > 0) { /* blocks until atleast 1024 bytes are available */
            /* do something with buffer */
            totalSize += filledSize;
            actualData->resize(totalSize);
            memcpy(actualData->data() + totalSize - filledSize, buffer, filledSize);
        }

        if (expectedData != nullptr)
        {
            SPXTEST_REQUIRE(AreBinaryEqual(expectedData, actualData));
        }

        if (stream->GetStatus() != StreamStatus::Canceled)
        {
            SPXTEST_REQUIRE(totalSize > 0);
        }

        if (stream->GetStatus() == StreamStatus::Canceled) {
            auto cancelDetails = SpeechSynthesisCancellationDetails::FromStream(stream);
            /* do something with cancellation details */
            SPXTEST_REQUIRE(cancelDetails->Reason == CancellationReason::Error);
            SPXTEST_REQUIRE(cancelDetails->ErrorCode != CancellationErrorCode::NoError);
        }
    }

    future<void> DoSomethingWithAudioInDataStreamInBackground(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone)
    {
        return DoSomethingWithAudioInDataStreamInBackground(stream, afterSynthesisDone, nullptr);
    }

    future<void> DoSomethingWithAudioInDataStreamInBackground(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone, std::shared_ptr<std::vector<uint8_t>> expectedData)
    {
        auto future = async(launch::async, [stream, afterSynthesisDone, expectedData]() {
            DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, expectedData); // calls to stream->ReadData(buffer, size) will block until size data is available
        });

        return future;
    }

    future<void> DoSomethingWithAudioInResultInBackground(future<shared_ptr<SpeechSynthesisResult>>& futureResult, bool afterSynthesisDone)
    {
        return DoSomethingWithAudioInResultInBackground(futureResult, afterSynthesisDone, nullptr);
    }

    future<void> DoSomethingWithAudioInResultInBackground(future<shared_ptr<SpeechSynthesisResult>>& futureResult, bool afterSynthesisDone, std::shared_ptr<std::vector<uint8_t>> expectedData)
    {
        auto future = std::async(std::launch::async, [&futureResult, afterSynthesisDone, expectedData]() {
            auto result = futureResult.get();
            auto stream = AudioDataStream::FromResult(result);
            DoSomethingWithAudioInDataStream(stream, afterSynthesisDone, expectedData); // calls to stream->ReadData(buffer, size) will block until size data is available
        });

        return future;
    }

    bool AreBinaryEqual(std::shared_ptr<std::vector<uint8_t>> expectedData, std::shared_ptr<std::vector<uint8_t>> ActualData)
    {
        return AreBinaryEqual(expectedData->data(), expectedData->size(), ActualData->data(), ActualData->size());
    }

    bool AreBinaryEqual(std::shared_ptr<std::vector<uint8_t>> expectedData, const std::vector<uint8_t>& ActualData)
    {
        return AreBinaryEqual(expectedData->data(), expectedData->size(), ActualData.data(), ActualData.size());
    }

    bool AreBinaryEqual(const uint8_t* expectedDataBuffer, size_t expectedDataSize, const uint8_t* actualDataBuffer, size_t actualDataSize)
    {
        if (expectedDataBuffer == nullptr || actualDataBuffer == nullptr || expectedDataSize != actualDataSize)
        {
            return false;
        }

        for (size_t i = 0; i < expectedDataSize; ++i)
        {
            auto error = expectedDataBuffer[i] > actualDataBuffer[i] ? expectedDataBuffer[i] - actualDataBuffer[i] : actualDataBuffer[i] - expectedDataBuffer[i];
            if (error > BINARY_COMPARE_TOLERANCE) // Accept some tolerance for some platforms
            {
                return false;
            }
        }

        return true;
    }

    std::shared_ptr<std::vector<uint8_t>> BuildMockSynthesizedAudio(const std::string& text, const std::string& language, const std::string& voice)
    {
        auto audioData = std::make_shared<std::vector<uint8_t>>();
        auto ssml = BuildSsml(text, language, voice);

        audioData->resize(MOCK_AUDIO_SIZE + ssml.length());

        for (int i = 0; i < MOCK_AUDIO_SIZE / 2; ++i)
        {
            double x = double(i) * 3.14159265359 * 2 * 400 / 16000; // 400Hz
            double y = sin(x);

            audioData->data()[i * 2] = uint16_t(y * 16384) % 256;
            audioData->data()[i * 2 + 1] = uint8_t(uint16_t(y * 16384) / 256);
        }

        memcpy(audioData->data() + MOCK_AUDIO_SIZE, ssml.c_str(), ssml.length());

        return audioData;
    }

    std::shared_ptr<std::vector<uint8_t>> BuildMockSynthesizedAudioWithHeader(const std::string& text, const std::string& language, const std::string& voice)
    {
        auto rawData = BuildMockSynthesizedAudio(text, language, voice);

        uint8_t header[EMPTY_WAVE_FILE_SIZE] = { 82, 73, 70, 70, 0, 0, 0, 0, 87, 65, 86, 69, 102, 109, 116, 32, 18, 0, 0, 0, 1, 0, 1, 0, 128, 62, 0, 0, 0, 125, 0, 0, 2, 0, 16, 0, 0, 0, 100, 97, 116, 97, 0, 0, 0, 0 };
        auto cData = (uint32_t)(rawData->size());
        for (size_t i = 0; i < sizeof(cData); ++i)
        {
            *(header + 42 + i) = (uint8_t)((cData >> (i * 8)) & 0xff);
        }
        cData += 38;
        for (size_t i = 0; i < sizeof(cData); ++i)
        {
            *(header + 4 + i) = (uint8_t)((cData >> (i * 8)) & 0xff);
        }

        auto audioData = std::make_shared<std::vector<uint8_t>>();
        audioData->resize(EMPTY_WAVE_FILE_SIZE + rawData->size());
        memcpy(audioData->data(), header, EMPTY_WAVE_FILE_SIZE);
        memcpy(audioData->data() + EMPTY_WAVE_FILE_SIZE, rawData->data(), rawData->size());
        return audioData;
    }

    std::shared_ptr<std::vector<uint8_t>> MergeBinary(std::shared_ptr<std::vector<uint8_t>> binary1, std::shared_ptr<std::vector<uint8_t>> binary2)
    {
        auto mergedBinary = std::make_shared<std::vector<uint8_t>>();
        mergedBinary->resize(binary1->size() + binary2->size());
        memcpy(mergedBinary->data(), binary1->data(), binary1->size());
        memcpy(mergedBinary->data() + binary1->size(), binary2->data(), binary2->size());

        return mergedBinary;
    }

    std::shared_ptr<std::vector<uint8_t>> LoadWaveFileData(const std::string& waveFile)
    {
        auto waveData = std::make_shared<std::vector<uint8_t>>();
        auto fileSize = GetFileSize(waveFile);
        SPXTEST_REQUIRE(fileSize >= EMPTY_WAVE_FILE_SIZE);

        waveData->resize(fileSize - EMPTY_WAVE_FILE_SIZE);

        auto file = std::make_unique<std::fstream>();
        PAL::OpenStream(*file.get(), PAL::ToWString(waveFile), true);
        file->seekg(EMPTY_WAVE_FILE_SIZE);
        file->read((char *)(waveData->data()), waveData->size());

        return waveData;
    }

    std::string BuildSsml(const std::string& text, const std::string& language, const std::string& voice)
    {
        char* ssmlBuf = new char[SSML_BUFFER_SIZE];
        int ssmlLength = snprintf(ssmlBuf, SSML_BUFFER_SIZE, SSML_TEMPLATE, language.data(), voice.data(), XmlEncode(text).data());
        auto ssml = std::string(ssmlBuf, ssmlLength);
        delete[] ssmlBuf;

        return ssml;
    }

    std::string XmlEncode(const std::string& text)
    {
        std::stringstream ss;
        for (size_t i = 0; i < text.length(); ++i)
        {
            char c = text[i];
            if (c == '&')
            {
                ss << "&amp;";
            }
            else if (c == '<')
            {
                ss << "&lt;";
            }
            else if (c == '>')
            {
                ss << "&gt;";
            }
            else if (c == '\'')
            {
                ss << "&apos;";
            }
            else if (c == '"')
            {
                ss << "&quot;";
            }
            else
            {
                ss << c;
            }
        }

        return ss.str();
    }

    std::wstring ToWString(const std::string& string)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.from_bytes(string);
    }
}
