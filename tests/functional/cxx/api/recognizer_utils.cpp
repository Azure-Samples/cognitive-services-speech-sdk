//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "recognizer_utils.h"

#include <fstream>
#include "wav_file_reader.h"
#include "azure_c_shared_utility_includes.h"

using namespace Microsoft::CognitiveServices::Speech::USP; // for GetISO8601Time


TestData weather {"/audio/whatstheweatherlike.wav", "What's the weather like?" };
TestData weathermp3{ "/audio/whatstheweatherlike.mp3", "What's the weather like?" };
TestData weatheropus{ "/audio/whatstheweatherlike.opus", "What's the weather like?" };
TestData weather8Channels {"/audio/whatstheweatherlike_8channels.wav", "What's the weather like?" };
TestData batman{ "/audio/batman.wav", "" };
TestData wrongSamplingRateFile {"/audio/11khztest.wav", "" };
TestData cortana {"/audio/heyCortana.wav", "Hey Cortana," };
TestData callTheFirstOne{ "/audio/CallTheFirstOne.wav", "Rufe die erste an." };
TestData turnOnLamp {"/audio/TurnOnTheLamp.wav", "Turn on lamp" };
TestData dgiWreckANiceBeach {"/audio/wreck-a-nice-beach.wav", "Wreck a nice beach." };
TestData recordedAudioMessage{ "/audio/RecordedAudioMessages.json", "" };

std::shared_ptr<SpeechConfig> CurrentSpeechConfig()
{
    return !Config::Endpoint.empty()
        ? SpeechConfig::FromEndpoint(Config::Endpoint, Keys::Speech)
        : SpeechConfig::FromSubscription(Keys::Speech, Config::Region);
}

void UseMocks(bool value)
{
    SpxSetMockParameterBool("CARBON-INTERNAL-MOCK-UspRecoEngine", value);
    SpxSetMockParameterBool("CARBON-INTERNAL-MOCK-Microphone", value);
    SpxSetMockParameterBool("CARBON-INTERNAL-MOCK-SdkKwsEngine", value);
}

void UseMockUsp(bool value)
{
    SpxSetMockParameterBool("CARBON-INTERNAL-MOCK-UspRecoEngine", value);
}

bool IsUsingMocks(bool uspMockRequired)
{
    return SpxGetMockParameterBool("CARBON-INTERNAL-MOCK-Microphone") &&
        SpxGetMockParameterBool("CARBON-INTERNAL-MOCK-SdkKwsEngine") &&
        (SpxGetMockParameterBool("CARBON-INTERNAL-MOCK-UspRecoEngine") || !uspMockRequired);
}

void SetMockRealTimeSpeed(int value)
{
    SpxSetMockParameterNumber("CARBON-INTERNAL-MOCK-RealTimeAudioPercentage", value);
}

fstream OpenFile(const string& filename)
{
    if (filename.empty())
    {
        throw invalid_argument("Audio filename is empty");
    }

    fstream fs;
    fs.open(filename, ios_base::binary | ios_base::in);
    if (!fs.good())
    {
        throw invalid_argument("Failed to open the specified audio file.");
    }


    return fs;
}

fstream OpenWaveFile(const string& filename)
{
    fstream fs = OpenFile(filename);
    //skip the wave header
    fs.seekg(44);

    return fs;
}

int ReadBuffer(fstream& fs, uint8_t* dataBuffer, uint32_t size)
{
    if (fs.eof())
    {
        // returns 0 to indicate that the stream reaches end.
        return 0;
    }

    fs.read((char*)dataBuffer, size);

    if (!fs.eof() && !fs.good())
    {
        // returns 0 to close the stream on read error.
        return 0;
    }
    else
    {
        // returns the number of bytes that have been read.
        return (int)fs.gcount();
    }
}

void WaitForResult(future<void>&& f, std::chrono::seconds duration)
{
    auto status = f.wait_for(duration);
    CAPTURE(duration);
    REQUIRE(status == future_status::ready);
}

void PushData(PushAudioInputStream* pushStream, const string& filename, bool compressed)
{
    fstream fs;
    try
    {
        if (compressed)
        {
            fs = OpenFile(filename);
        }
        else
        {
            fs = OpenWaveFile(filename);
        }
    }
    catch (const exception& e)
    {
        SPX_TRACE_VERBOSE("Error: exception in pushData, %s.", e.what());
        SPX_TRACE_VERBOSE("can't open '%s'", filename.c_str());
        throw e;
        return;
    }

    std::array<uint8_t, 1000> buffer;
    while (1)
    {
        auto readSamples = ReadBuffer(fs, buffer.data(), (uint32_t)buffer.size());
        if (readSamples == 0)
        {
            break;
        }
        pushStream->Write(buffer.data(), readSamples);
    }
    fs.close();
    pushStream->Close();
}

void DoContinuousReco(SpeechRecognizer* recognizer, PushAudioInputStream* pushStream)
{
    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks<SpeechRecognizer, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>(recognizer, result);
    PushData(pushStream, weather.m_inputDataFilename);
    recognizer->StartContinuousRecognitionAsync().get();
    WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopContinuousRecognitionAsync().get();
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(result->phrases[0].Text == weather.m_utterance);
}

void DoKWS(SpeechRecognizer* recognizer, PushAudioInputStream* pushStream)
{
    auto res = make_shared<RecoPhrases>();
    cortana.UpdateFullFilename(Config::InputDir);

    ConnectCallbacks<SpeechRecognizer, SpeechRecognitionEventArgs, SpeechRecognitionCanceledEventArgs>(recognizer, res);
    PushData(pushStream, cortana.m_inputDataFilename);
    auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/Computer/kws.table");
    recognizer->StartKeywordRecognitionAsync(model).get();
    WaitForResult(res->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopKeywordRecognitionAsync().get();
    SPXTEST_REQUIRE(!res->phrases.empty());
    SPXTEST_REQUIRE(res->phrases[0].Text == cortana.m_utterance);
}

auto createCallbacksMap() -> std::map<Callbacks, atomic_int>
{
    std::map<Callbacks, atomic_int> newMap;
    newMap[Callbacks::final_result] = 0;
    newMap[Callbacks::intermediate_result] = 0;
    newMap[Callbacks::no_match] = 0;
    newMap[Callbacks::session_started] = 0;
    newMap[Callbacks::speech_start_detected] = 0;
    newMap[Callbacks::session_stopped] = 0;
    return newMap;
}

RecordedDataReader::RecordedDataReader(const std::string& type)
{
    m_type = type;
    m_numOfBuffer = 0;
    m_totalBytes = 0;
}

RecordedDataReader::~RecordedDataReader()
{
    SPX_DBG_TRACE_VERBOSE(" %s total frame count = %d, total bytes = %" PRIu64, m_type.c_str(), m_numOfBuffer, m_totalBytes);
}

bool RecordedDataReader::Open(const std::string& fileName)
{
    SPX_DBG_TRACE_VERBOSE("Opening video file '%s'", fileName.c_str());
    ifs.open(fileName, std::ifstream::in);

    if (!ifs.good())
        throw invalid_argument("can't open " + fileName);

    //ifs.imbue(std::locale());
    return ifs.good();
}

// each line is a frame
uint32_t RecordedDataReader::Read(uint8_t* buffer, uint32_t maxSize)
{
    auto videoString = getNextBase64EncodedData();
    if (videoString.empty())
    {
        SPX_DBG_TRACE_VERBOSE("RecordedDataReader::Read return zero bytes");
        return 0;
    }

    // BUFFER_HANDLER is a BUFFER_TAG*
    auto deleter = [](BUFFER_HANDLE h) { BUFFER_delete(h); };
    std::unique_ptr<BUFFER_TAG, decltype(deleter)> videoBuffer(Base64_Decoder(videoString.c_str()), deleter);

    BUFFER_HANDLE handle = (BUFFER_HANDLE)(videoBuffer.get());

    unsigned char* srcBuffer = BUFFER_u_char(handle);
    size_t srcBufferLength = BUFFER_length(handle);
    if (srcBufferLength > maxSize)
    {
       throw invalid_argument("video buffer size is too small");
    }

    std::memcpy(buffer, srcBuffer, srcBufferLength);

    m_totalBytes += srcBufferLength;
    m_numOfBuffer++;

    SPX_DBG_TRACE_VERBOSE(" RecordedDataReader::Read: return bytes= %zu", srcBufferLength);
    SPX_DBG_TRACE_VERBOSE(" %s total frame count = %d, total bytes = %" PRIu64, m_type.c_str(), m_numOfBuffer, m_totalBytes);

    return (uint32_t)srcBufferLength;
}

#define FRAME_TYPE_STRING "\"X-Princeton-Video-FrameType\":\""
#define TIMESTAMP_STRING "\"X-Princeton-Timestamp\":\""

std::string RecordedDataReader::getField(const std::string& header, const std::string& field)
{
    std::string result;
    auto frameStringStartPos = header.find(field);
    auto frameStringEndPos = frameStringStartPos + field.size();
    if (frameStringStartPos != std::string::npos && frameStringEndPos < header.size())
    {
        // find the ending " after 3 in "X-Princeton-Video-FrameType":"3"
        std::size_t endPos = header.find("\"", frameStringEndPos + 1);
        if (endPos != std::string::npos)
        {
            auto startPos = frameStringEndPos;
            if (startPos >= header.length() || endPos - startPos <= 0)
            {
                throw invalid_argument("Frame start is bigger than the string len!");
            }
            auto type = header.substr(startPos, endPos - startPos);
            result = type;
        }
    }

    return result;
}

std::string RecordedDataReader::getNextBase64EncodedData()
{
    if (!ifs.good() || ifs.eof())
    {
        return "";
    }
    //891838472555
    uint64_t timeStamp;
    ifs >> timeStamp;

    std::string header;
    ifs >> header;

    m_frameType = getField(header, FRAME_TYPE_STRING);
    //sanity check, type value must be 1, 2, 3
    if (!m_frameType.empty())
    {
        auto iType = std::stoi(m_frameType);
        SPX_DBG_ASSERT(iType == 1 || iType == 2 || iType == 3);
    }
    m_timestamp = getField(header, TIMESTAMP_STRING);
    std::string videoDataString;
    ifs >> videoDataString;

    return videoDataString;
}

SPXSTRING RecordedDataReader::GetProperty(PropertyId propertyId)
{
    if (propertyId == PropertyId::DataBuffer_TimeStamp)
    {
        SPX_DBG_ASSERT(!m_timestamp.empty());
        return m_timestamp;
    }
    //else if (propertyId == PropertyId::DataChunk_VideoFrameType)
    //{
    //    SPX_DBG_ASSERT(m_frameType == "1" || m_frameType == "2"|| m_frameType == "3");
    //    return m_frameType;
    //}
    else
    {
        return "";
    }
}

std::shared_ptr<AudioConfig> CreateAudioPushUsingWeatherFile(std::shared_ptr<PushAudioInputStream>& pushAudio)
{
    weather8Channels.UpdateFullFilename(Config::InputDir);

    // Create the "audio pull stream" object with C++ lambda callbacks
    pushAudio = AudioInputStream::CreatePushStream(AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8));
    return AudioConfig::FromStreamInput(pushAudio);
}

std::shared_ptr<AudioConfig> CreateAudioPullUsingWeatherFile(std::shared_ptr<PullAudioInputStream>& pullStream)
{
    weather8Channels.UpdateFullFilename(Config::InputDir);
    auto reader = std::make_shared<CSpxWavFileReader>();
    reader->Open(PAL::ToWString(weather8Channels.m_inputDataFilename).c_str());

    // Create the "audio pull stream" object with C++ lambda callbacks
    pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8),
        [reader = reader](uint8_t* buffer, uint32_t size) -> int { return reader->Read(buffer, size); },
        [=]() {},
        [=](PropertyId propertyId) -> SPXSTRING {
        if (propertyId == PropertyId::DataBuffer_TimeStamp)
        {
            char timeString[TIME_STRING_MAX_SIZE];
            GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
            return timeString;
        }
        else if (propertyId == PropertyId::DataBuffer_UserId)
        {
            return "speakerA";
        }
        else
        {
            return "";
        }
    }
    );
    return AudioConfig::FromStreamInput(pullStream);
}

std::shared_ptr<AudioConfig> CreateAudioPullSingleChannel(std::shared_ptr<PullAudioInputStream>& pullStream)
{
    weather.UpdateFullFilename(Config::InputDir);
    auto reader = std::make_shared<CSpxWavFileReader>();
    reader->Open(PAL::ToWString(weather.m_inputDataFilename).c_str());

    // Create the "audio pull stream" object with C++ lambda callbacks
    pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
        [reader = reader](uint8_t* buffer, uint32_t size) -> int { return reader->Read(buffer, size); },
        [=]() {},
        [=](PropertyId propertyId) -> SPXSTRING {
        if (propertyId == PropertyId::DataBuffer_TimeStamp)
        {
            char timeString[TIME_STRING_MAX_SIZE];
            GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
            return timeString;
        }
        else if (propertyId == PropertyId::DataBuffer_UserId)
        {
            return "speakerA";
        }
        else
        {
            return "";
        }
    });
    return AudioConfig::FromStreamInput(pullStream);
}
std::shared_ptr<AudioConfig> CreateAudioPullFromRecordedFile(std::shared_ptr<PullAudioInputStream>& pullStream)
{
    auto audioReader = std::make_shared<RecordedDataReader>("audio");
    recordedAudioMessage.UpdateFullFilename(Config::InputDir);
    REQUIRE(exists(recordedAudioMessage.m_inputDataFilename));
    audioReader->Open(recordedAudioMessage.m_inputDataFilename);

    pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8),
        [reader = audioReader](uint8_t* buffer, uint32_t size) -> int { return reader->Read(buffer, size); },
        [=]() {},
        [reader = audioReader](PropertyId propertyId) -> SPXSTRING { return reader->GetProperty(propertyId);
    });

    return AudioConfig::FromStreamInput(pullStream);
}
#if 0
std::shared_ptr<VideoConfig> CreateVideoPullFromRecordedFile(std::shared_ptr<PullVideoInputStream>& pullVideo)
{
    recordedVideoMessage.UpdateFullFilename(Config::InputDir);
    auto videoReader = std::make_shared<RecordedDataReader>("video");
    videoReader->Open(recordedVideoMessage.m_inputDataFilename);

    pullVideo = VideoInputStream::CreatePullStream(
        VideoStreamFormat::GetDefaultInputFormat(),
        [videoReader](uint8_t* buffer, uint32_t size) -> int { return videoReader->Read(buffer, size); },
        [=]() {},
        [videoReader](PropertyId propertyId) -> SPXSTRING { return videoReader->GetProperty(propertyId); }
    );

    return VideoConfig::FromStreamInput(pullVideo);
}

std::shared_ptr<VideoConfig> CreateVideoPush(std::shared_ptr<PushVideoInputStream>& pushVideo)
{
    recordedVideoMessage.UpdateFullFilename(Config::InputDir);
    auto videoReader = std::make_shared<RecordedDataReader>("video");
    videoReader->Open(recordedVideoMessage.m_inputDataFilename);

    pushVideo = VideoInputStream::CreatePushStream(VideoStreamFormat::GetDefaultInputFormat());

    return VideoConfig::FromStreamInput(pushVideo);
}
#endif
void StartMeetingAndVerifyResult(ConversationTranscriber* recognizer, const shared_ptr<Participant>& participant, RecoPhrasesPtr&& result, const string& ref)
{
    recognizer->SetConversationId("fromStartMeetingAndVerifyResult");

    // add the speaker1 to usp
    recognizer->AddParticipant(participant);

    recognizer->StartTranscribingAsync().get();

    WaitForResult(result->ready.get_future(), 10min);

    recognizer->StopTranscribingAsync().get();
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(result->phrases[0].Text == ref);
}

void CreateVoiceSignatures(std::string* voice006, std::string* voice022, std::string* voice042, std::string* voice023)
{
    *voice006 = R"(
      {
          "Version": 0,
          "Tag": "9cS5rFEgDkqaXSkFRoI0ab2/SDtqJjFlsRoI2Tht3fg=",
          "Data": "r4tJwSq280QIBWRX8tKcjxYwDySvX6VZFGkqLLroFV3HIlARgA1xXdFcVK9a2xbylLNQUSNwdUUsIpBDB+jlz6W97XgJ9GlBYLf6xVzUmBg1Qhac32DH3c810HDtpwJk3FkEveM7ohLjhvnYKwjBNqbAVGUONyLYpO28kcxRhvSOxe5/2PeVOgpXMGMcBt3IKN3OmNSOokg4QkqoRUNuRMg5jdoq7BraOyr7CEOP2/GsicmUcONNhFaLuEwy97WRUXE0RWTdDxeR9dn2ngSESq+vYiCkudDi/TGh0ZhxABTxU6EiFQl7uiYG28drjosWdrOV5FPGe2pP8omEoBgtc+yOxYa40HG/yQ160Enqv8umCTcTeW6bkA9CZJ7K8740oZkA8pdpsWkurpFJlMDK3e3Y6w/W1/P55gz/jegYTusDDoz5fINcoWj1zbyLMaFgig3PlEDLKG2hb09Jy4OhEeaBgVqEXiUTEX/R44pd7nUK49xrRJ9yM2gfUq8S+229hJ40N5ZMe+9G848jtsGOziPs20KNlqpL6tiXGAeynhclHyt3pITJjOJi9/cYKYbNm3dR+PtxuLL1WAgIuaK65aGhyW0NmFYm/r7hfAK9a2nTNJIgTsFLG32jljkpaurtwvHuAtIhK8KnopeN6OPXjGl2q06bqI2U92eBxKRroeGUEq3PiXHwVk9DOIFzOAdz"
        }
     )";

    *voice023 = R"(
       {
          "Version": 0,
          "Tag": "9cS5rFEgDkqaXSkFRoI0ab2/SDtqJjFlsRoI2Tht3fg=",
          "Data": "qW513Jnvb5w1k4y/Z9J3jveiE3D42HCQBYCpldleFYRY+aPPTsI0IBMjCBXnStNSd4zWfdoGbzRlxFWz+kdjpzIA+9jM5/Sb1bqMgEcHod2cK/Cr8+uOUfO6KQ/rEg77B/z/ffb0P6wClO2ECw9KoVtLjdGTdFo5AytiFq3ngVADzRX6jtT5bVM4b9XJCg7VTKa9Im7F6yqMqgIgTJP+E8XEjO5cD2QD+Ea+9FyqQ2AOHHqiP2oHxcNYNXkwCO6vh7E2mKo97DekFGrqLLFwn8gczFxM08g/qoccs1SVvbtjpj7mGTNiq18WTuT1G78X7eal9MFMT4NZNiRzK1haSbM4gxt3HxprA3DlvTJnMWBdttzwzS9II1L/UZgEiw4ldJqDBKHogWEuyKej/DRFZMjsRustw0f+ioVADR9uyImA/lINJFgTMgFpqfcHU+bjy7hGz9sGBOopCvNC/pYfCytS1S3tkr8VVhuKHgdK3yDXoOWf57I8DBn1e88FM0SUmpGHLmE4xQreAmwm0dJRHQAIullTlwM54NIEd3CVETOJO5R3bqLbNH9rioStslHB30UAemSN5u21Hz/hLVlPsNRoEZ1kDB7wzwayE5NtXdCz4PSDrEXSBawUH7/xXiaurWgkvqCsDli+tD58RbxlTXxbOJ9caXjwLCRINu4SLLHXbuf3ksSY+XwiS4BznMUJ"
        }
        )";

    *voice022 = R"(
      {
          "Version": 0,
          "Tag": "9cS5rFEgDkqaXSkFRoI0ab2/SDtqJjFlsRoI2Tht3fg=",
          "Data": "T8JbMJW8OGEhkWEm9Q6+p94LfPrbgyxz5LWs695CPzGPR64TJf3pw5FPqaXR1r0zf0UeckLAnHWzgD7St7aUy+DucbFGekXXMazDHeKs7jgvh4UhRVIvdukcKuNBOR+M1Hcdo4ZGLNACi7HPjSCqpq4x8GVWsBv0olBmv0f9MODmwqLV4ZdigpFRxjqQNLVFX1E7m3mReZj7Dwe1jnGM2IjqCVXLsvO9i218Gm73h2JmcoUbZt/PZQwNwfD3EAN9GbTBMaN/E65Xd/i43Hwhk9GzQufKWzQUW5UB13BIyGfO97z64CaEq4dbFd5pWxk5N9wyDEdEo+PnchqN0FJaPaHjxqv63BN7UdPRYMJ9coMSXFpQRU9t57tNvy77tGrvQ4RufaIYgkTzZvJDN19vEQXlL+oBU30dU+7tFmICRiTCqPRjYYb9Q8O32Sxpr7iIhMwkviUcfo9Y08eqAv9m4aQO6Lhz0cx4vK7UhZShagZ9dqMVA99CGWW4zp4vY7uThPiYMGqAcePA+Sv8fIBR2GB2YG9OZj/Ao80xWMuK6yoEehqOuY1oj07HJ5wHuoy1gL+6AblKNT29Q8i7zKKflW5MimdVby2WvBP59sdaDjZYXYJFFnAV1g63k7Rotvq6OvhytR2hc3rpSAuqs9pmohg8ssKdmG2bBpp5IxS7VRu7uUwOBNWsyMDAs48pPFae"
        }
    )";

    *voice042 = R"(
      {
          "Version": 0,
          "Tag": "9cS5rFEgDkqaXSkFRoI0ab2/SDtqJjFlsRoI2Tht3fg=",
          "Data": "IlgqQLfiEofDG1asXEAReulsL3GfTNFOFyfRvImCNlzKsPDUz4NycKT6IahqQYptAcjwAKyOq/OL+hxnjXHYvaf4dNUzL7rriG0ezgFdykxgfGM0MLtuS8S+OFHX2JYCpub4gRjbLo+GpNB1Vn+nt8sILVFjwLbQ93WiI2i4ThzV3tsYrBdCGLXeFEQ3QUS1epuVUWFHxKauzc9d97uv6f6BlBqfeR/n9cytAz1zRupeIijFKrW118qyTIewLuqx/HjE3E54Cdxdv7aNz/0xdo16rGk0RwhT4L9zqKqLw0g7vNJ20q3vmEOgWioCWRRM2517cGdzmezanRXXcHeEBNosihUK2TkNbfCRbwXh8UzD1arPfv2eh4GxEB3zC2WUJPM4+hsF8DDjPbc418NFJmVx7FQTjXNe7HHwyGNs18qFiDDilG/m4wA3k+hRRCsuRcHcjaipNBwfuHiwsNv3ZE+Jymtx0lmhlzmP/4uwezI1JuFo4j0d3QdjF68aHj1iEOFWNvSc3xujkZoi5fYvVHPtEuVgb21ZZ/OtiQmgGy6IDDz61sgVX+b8oQkcsKo93Yg3eM0kmOiFW9CVnIs7BKJWegYfT2sCN44ARNSpHTxAjNXEaeoydq6LhAtkDcUtDhiaUgavoSCYZGDv6w3CMZCYBfleWPUEUAQk4bykAGoHY1D1wf3alqYclqPaYKcm"
        }
    )";
}

bool VerifyResult(std::string& text, const std::string& ref)
{
    auto lowercaseText = text;
    transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(), [](unsigned char c) ->char { return (char)::tolower(c); });

    auto lowercaseRef = ref;
    transform(lowercaseRef.begin(), lowercaseRef.end(), lowercaseRef.begin(), [](unsigned char c) ->char { return (char)::tolower(c); });

    // found the match
    if (text.find(ref) != string::npos)
    {
        return true;
    }
    else if (lowercaseText.find(lowercaseRef) != string::npos)
    {
        return true;
    }
    //or different but no errors is OK too.
    else if (text.find("WebSocket Upgrade failed") != std::string::npos ||
        text.find("Connection was closed by the remote host") != std::string::npos
        )
    {
        return false;
    }

    return true;
}
std::string GetUserId(const ConversationTranscriptionResult* ctr)
{
    return ctr ? ctr->UserId : string{};
}

std::string GetUserId(const SpeechRecognitionResult* ctr)
{
    UNUSED(ctr);
    return  string{};
}
