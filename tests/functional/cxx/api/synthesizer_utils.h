#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>
#include <atomic>
#include <map>
#include <string>
#include <array>
#include <chrono>

#include "speechapi_cxx.h"
#include "mock_controller.h"


using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace std;

#define EMPTY_WAVE_FILE_SIZE 46
#define GUID_LENGTH 32
#define MOCK_AUDIO_SIZE 32000
#define MOCK_AUDIO_CHUNK_SIZE 3200
#define DEFAULT_LANGUAGE "en-US"
#define DEFAULT_VOICE "Microsoft Server Speech Text to Speech Voice (en-US, AriaRUS)"
#define SSML_BUFFER_SIZE 0x10000
#define SSML_TEMPLATE "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='%s'><voice name='%s'>%s</voice></speak>"
#define BINARY_COMPARE_TOLERANCE 0

namespace TTS
{
    struct StreamResult
    {
        shared_ptr<vector<uint8_t>> audioData;
        StreamStatus status;
    };

    shared_ptr<SpeechConfig> CurrentSpeechConfig();

    shared_ptr<SpeechConfig> RestSpeechConfig();

    shared_ptr<SpeechConfig> UspSpeechConfig();

    shared_ptr<SpeechConfig> MockSpeechConfig();

    long GetFileSize(const string& filename);

    std::vector<uint8_t> ReadWavFile(const string& filename);

    void DoSomethingWithAudioInPullStream(std::shared_ptr<PullAudioOutputStream> stream, bool& canceled, std::shared_ptr<std::vector<uint8_t>> expectedData = nullptr);

    future<void> DoSomethingWithAudioInPullStreamInBackground(shared_ptr<PullAudioOutputStream> stream, bool& canceled, std::shared_ptr<std::vector<uint8_t>> expectedData = nullptr);

    void DoSomethingWithAudioInVector(shared_ptr<vector<uint8_t>> audio, size_t audioLength);

    StreamResult DoSomethingWithAudioInDataStream(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone);

    future<StreamResult> DoSomethingWithAudioInDataStreamInBackground(shared_ptr<AudioDataStream> stream, bool afterSynthesisDone);

    future<StreamResult> DoSomethingWithAudioInResultInBackground(future<shared_ptr<SpeechSynthesisResult>>& futureResult, bool afterSynthesisDone);

    bool AreBinaryEqual(std::shared_ptr<std::vector<uint8_t>> expectedData, std::shared_ptr<std::vector<uint8_t>> ActualData);

    bool AreBinaryEqual(std::shared_ptr<std::vector<uint8_t>> expectedData, const std::vector<uint8_t>& ActualData);

    bool AreBinaryEqual(const uint8_t* expectedDataBuffer, size_t expectedDataSize, const uint8_t* actualDataBuffer, size_t actualDataSize);

    std::shared_ptr<std::vector<uint8_t>> BuildMockSynthesizedAudio(const std::string& text, const std::string& language, const std::string& voice);

    std::shared_ptr<std::vector<uint8_t>> BuildMockSynthesizedAudioWithHeader(const std::string& text, const std::string& language, const std::string& voice);

    std::shared_ptr<std::vector<uint8_t>> MergeBinary(std::shared_ptr<std::vector<uint8_t>> binary1, std::shared_ptr<std::vector<uint8_t>> binary2);

    std::shared_ptr<std::vector<uint8_t>> LoadWaveFileData(const std::string& waveFile);

    std::string BuildSsml(const std::string& text, const std::string& language, const std::string& voice);

    std::string XmlEncode(const std::string& text);

    std::wstring ToWString(const std::string& string);

    void CheckStreamResult(StreamResult result, shared_ptr<vector<uint8_t>> expectedData = nullptr, StreamStatus expectedStatus = StreamStatus::AllData);

    class PushAudioOutputStreamTestCallback : public PushAudioOutputStreamCallback
    {
    public:
        PushAudioOutputStreamTestCallback()
        {
            m_audioData = std::make_shared<std::vector<uint8_t>>();
        }

        int Write(uint8_t* dataBuffer, uint32_t size) override
        {
            auto oldSize = m_audioData->size();
            m_audioData->resize(oldSize + size);
            memcpy(m_audioData->data() + oldSize, dataBuffer, size);

            return size;
        }

        void Close() override
        {
            m_closed = true;
        }

        size_t GetAudioSize()
        {
            return m_audioData->size();
        }

        std::shared_ptr<std::vector<uint8_t>> GetAudioData()
        {
            return m_audioData;
        }

        bool IsClosed()
        {
            return m_closed;
        }

    private:
        std::shared_ptr<std::vector<uint8_t>> m_audioData;
        bool m_closed = false;
    };
}
