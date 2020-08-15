//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "test_utils.h"
#include "recognizer_utils.h"

#include <fstream>
#include <chrono>
#include "wav_file_reader.h"
#include "azure_c_shared_utility_includes.h"

std::shared_ptr<SpeechConfig> CurrentSpeechConfig(const std::string& trafficType)
{
    auto config = !DefaultSettingsMap[ENDPOINT].empty()
        ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
       : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
    config->SetServiceProperty("TrafficType", trafficType, ServicePropertyChannel::UriQueryParameter);
    return config;
}

std::shared_ptr<SpeechTranslationConfig> CurrentTranslationConfig(const std::string& trafficType)
{
    auto config = !DefaultSettingsMap[ENDPOINT].empty()
        ? SpeechTranslationConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key)
        : SpeechTranslationConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
    config->SetServiceProperty("TrafficType", trafficType, ServicePropertyChannel::UriQueryParameter);
    return config;
}

// pronunciation assessment service is currently only available on westus, eastasia and centralindia regions, using a westus key for tests
// TODO: switch to main test key after pronunciation assessment service deployed to northeurope
std::shared_ptr<SpeechConfig> CurrentSpeechConfigForPronunciationAssessment(const std::string& trafficType)
{
    auto config = !DefaultSettingsMap[ENDPOINT].empty()
        ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[SPEECH_SUBSCRIPTION_WEST_US].Key)
       : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[SPEECH_SUBSCRIPTION_WEST_US].Key, SubscriptionsRegionsMap[SPEECH_SUBSCRIPTION_WEST_US].Region);
    config->SetServiceProperty("TrafficType", trafficType, ServicePropertyChannel::UriQueryParameter);
    return config;    
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

void UseOfflineUnidec(std::shared_ptr<SpeechConfig> config)
{
    // Test offline speech recognition without public API
    config->SetProperty(R"(CARBON-INTERNAL-UseRecoEngine-Unidec)", "true");
    config->SetProperty(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelPathRoot)",  Config::OfflineModelPathRoot);

    if (!Config::OfflineModelLanguage.empty())
    {
        config->SetProperty(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage)", Config::OfflineModelLanguage);
    }
    else
    {
        config->SetProperty(R"(CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage)", "en-US");
    }
}

void UseOfflineRnnt(std::shared_ptr<SpeechConfig> config)
{
    // Test offline speech recognition without public API
    config->SetProperty(R"(CARBON-INTERNAL-UseRecoEngine-Rnnt)", "true");
    config->SetProperty(R"(CARBON-INTERNAL-RNNT-ModelSpec)", Config::RnntModelSpec);
    config->SetProperty(R"(CARBON-INTERNAL-RNNT-Tokens)", Config::RnntTokens);
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

void PushData(PushAudioInputStream* push, const string& fileName, bool compressed)
{
    try
    {
        std::array<uint8_t, 1000> buffer;
        if (compressed)
        {
            auto stream = OpenFile(fileName);
            for (;;)
            {
                auto size = ReadBuffer(stream, buffer.data(), (uint32_t)buffer.size());
                if (size == 0) break;

                push->Write(buffer.data(), size);
            }
            push->Close();
            stream.close();
        }
        else
        {
            auto stream = AudioDataStream::FromWavFileInput(fileName);
            for (;;)
            {
                auto size = stream->ReadData(buffer.data(), (uint32_t)buffer.size());
                if (size == 0) break;

                push->Write(buffer.data(), size);
            }
            push->Close();
            stream.reset();
        }
    }
    catch (const exception& e)
    {
        SPX_TRACE_VERBOSE("Error: exception in pushData, %s.", e.what());
        SPX_TRACE_VERBOSE("can't open '%s'", fileName.c_str());
        throw e;
        return;
    }
}

void DoContinuousReco(SpeechRecognizer* recognizer, PushAudioInputStream* pushStream)
{
    auto result = make_shared<RecoPhrases>();
    ConnectCallbacks(recognizer, result);
    PushData(pushStream, ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    recognizer->StartContinuousRecognitionAsync().get();
    WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopContinuousRecognitionAsync().get();
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->phrases[0].Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
}

void DoKWS(SpeechRecognizer* recognizer, PushAudioInputStream* pushStream)
{
    auto res = make_shared<RecoPhrases>();

    ConnectCallbacks(recognizer, res);
    PushData(pushStream, ROOT_RELATIVE_PATH(COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1));
    auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");
    recognizer->StartKeywordRecognitionAsync(model).get();
    WaitForResult(res->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopKeywordRecognitionAsync().get();
    SPXTEST_REQUIRE(!res->phrases.empty());
    SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(res->phrases[0].Text, AudioUtterancesMap[COMPUTER_KEYWORD_WITH_SINGLE_UTTERANCE_1].Utterances["en-US"][0].Text));
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
        UNUSED(iType); // unused in release builds
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

std::shared_ptr<AudioConfig> CreateAudioPushUsingKatieSteveFile(std::shared_ptr<PushAudioInputStream>& pushAudio)
{
    // Create the "audio pull stream" object with C++ lambda callbacks
    pushAudio = AudioInputStream::CreatePushStream(AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8));
    return AudioConfig::FromStreamInput(pushAudio);
}

std::shared_ptr<AudioConfig> CreateAudioPullUsingKatieSteveFile(std::shared_ptr<PullAudioInputStream>& pullStream)
{
    auto reader = std::make_shared<CSpxWavFileReader>();
    reader->Open(PAL::ToWString(ROOT_RELATIVE_PATH(CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH)).c_str());

    // Create the "audio pull stream" object with C++ lambda callbacks
    pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8),
        [reader = reader](uint8_t* buffer, uint32_t size) -> int { return reader->Read(buffer, size); },
        [=]() {},
        [=](PropertyId propertyId) -> SPXSTRING {
        if (propertyId == PropertyId::DataBuffer_TimeStamp)
        {
            return CreateTimestamp();
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
    auto reader = std::make_shared<CSpxWavFileReader>();
    reader->Open(PAL::ToWString(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)).c_str());

    // Create the "audio pull stream" object with C++ lambda callbacks
    pullStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetWaveFormatPCM(16000, 16, 1),
        [reader = reader](uint8_t* buffer, uint32_t size) -> int { return reader->Read(buffer, size); },
        [=]() {},
        [=](PropertyId propertyId) -> SPXSTRING {
        if (propertyId == PropertyId::DataBuffer_TimeStamp)
        {
            return CreateTimestamp();
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
    REQUIRE(exists(DefaultSettingsMap[INPUT_DIR] + ("/audio/RecordedAudioMessages.json")));
    audioReader->Open(DefaultSettingsMap[INPUT_DIR] + ("/audio/RecordedAudioMessages.json"));

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

 std::string PumpAudioAndWaitForResult(ConversationTranscriber* recognizer, RecoPhrasesPtr result)
{
    recognizer->StartTranscribingAsync().get();

    WaitForResult(result->ready.get_future(), 10min);

    recognizer->StopTranscribingAsync().get();
    if (!result->phrases.empty())
    {
        return GetText(result->phrases);
    }
    else
    {
        return std::string{ };
    }
}

void CreateVoiceSignatures(std::string* katieVoiceSignature, std::string* steveVoiceSignature)
{
    *katieVoiceSignature = R"(
        {"Version":"0",
         "Tag" : "VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=",
         "DDD" : "EEE",
         "Data" : "rrukpyUGGFGHidLz51PatCKm/jIh4uwtgVKUkAo3tBjQZbvdKdIHRb47nfsk2RycDutA09sb5UtVkYPdwnj/SuamcOJi+sGNczeqvkcF5dQHCnJiPMHl56/rPgJm9gNaxSbvvanthk6h6NFRcpfxw1mrp3bq1BtflduuoVcBrIBuEqE3JlT7P4VuaeSaQ2KHGffIM6xi/4ylqA3FKYyRGJefZnzjGIjGQl6GauV/YwSSDR19n7ovKAJCYuJKfTXG0qgrO0zdSQ5bHiTBTiS52B3WgkYglyn5aDMKlcZjpCyg7xAWcA3lnz2Fsmjjo/e0bz99mARNCHkk4OrwImgNzxpnqPqHi3YLhdSo9W0/sKM+QQ625U2YlD0hASh97YDPenzEGSNnMD8mLn5EJs2Naf8u52yIQO+6SHB3HcmjIJRhaXZXJAznFUQ6+XD3k0uKBNvg8C0kNuUzi77aKqbohO8OS4b2hGbQqSmHep3lCdRr8W2SAf7UzabJq8qxTGb7E/qh9AuOdHI1iqrIHpx3oL68O7FUzSY7sBwVlsPsziPfYu1FAxjuICsqxha6kqyemjiSc3SjzLsHz5V0cz0f3X6K4QQw6TonqICLLRuaTJwhUNnBGlDXi1wc5265TdJi3hEqqVtwjYGeqYAYhSItKbE+h76zsQcNu2qUYeQRCUm30+k2KrqDcgmskAdaV85+"}
        )";

    *steveVoiceSignature = R"(
    {
        "Version":0,
         "Tag" : "HbIvzbfAWjeR/3R+WvUEoeid1AbDaHNOMWItgs7mTxc=",
         "Data" : "Yuw5OvaCRB+gi74UdCa6N4cHy23c293XwPfj3PDMr2U1S/CWkUIMNSV/GsUTuwDYxn5u0oOfLpLnhy9EIlJbtpvY7GXp6JxDv3PylsZIAPx9uzs8Shc9JvtY3PSmI4IHNRbsa4tg4JLI/C3rxsGA6DNfiVsmhJmgEtX5M12xX7BbzaFE4m4ax+BGYdNNpMMz2EZ0Mmnxyiy1cqkJWGgcOn7rfZYbLVMxsCT55wtB/1vSz4faWMfsfQxoDiEmHCmHTBuUuHZ8B05JAgpfHhSLwCJPVKyndYSxnsV3sWvp1Ou3HrOcz0ug5cjFYl/gR0U8XSwEFOHThNybKgpKAKzQ8mUaElvYcYySpJuHhPOyH21MWtRTS8OmSCDoeP6reTpgeoQnOjxJaojNHVpDDK4mG9u03oT7GnbLD2qlUkAwu/HywplexR9qufa36fqWGesVjjdd6C62PULkgSkhq9zQc3g7L5y0cci5RXgOBdP2KHa85anELGg4fUT05Hf6DDnO72GM8w40p2CQiswNlvcmnlRXD732koBXTzUpYaO1lhikXYpSg+C4bTjww6S0/3u/+A30kq8c4GYw6ngHybNLgDv8Z6ZVt4XiXDRFwpoSVZzrmSeuz4DOLlL+jX8dK8QwRBS92Jkay561tirmDd9fyz76+hljtt72rYKg9KpVAqoausBdGrzy0iEE3LsfXUTH"}
    )";
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

std::string GetUtteranceId(const ConversationTranscriptionResult* ctr)
{
    return ctr ? ctr->UtteranceId : string{};
}

std::string GetUtteranceId(const SpeechRecognitionResult* ctr)
{
    UNUSED(ctr);
    return  string{};
}

bool VerifyTextAndSpeaker(const RecoResultVector& phrases, const std::string& text, const std::string& speakerId)
{
    if (phrases.empty())
    {
        return false;
    }
    bool found = false;
    for (const auto& phrase : phrases)
    {
        if ( phrase.Text.find(text) != string::npos && phrase.UserId == speakerId)
        {
            found = true;
            break;
        }
    }
    return found;
}

uint64_t VerifySpeaker(const RecoResultVector& phrases_in, const std::wstring& wantedSpeaker)
{
    uint64_t offset = 0;
    auto phrases = phrases_in;
    sort(begin(phrases), end(phrases), [](const RecoPhrase& lhs, const RecoPhrase& rhs) { return lhs.Offset < rhs.Offset; });
    for (const auto& phrase : phrases)
    {
        wstring lowercaseUserId = PAL::ToWString(phrase.UserId);
        transform(lowercaseUserId.begin(), lowercaseUserId.end(), lowercaseUserId.begin(), [](wchar_t c) ->wchar_t { return std::tolower(c, std::locale("")); } );

        if (lowercaseUserId == wantedSpeaker)
        {
            offset = phrase.Offset;
            break;
        }
    }

    return offset;
}
// the ref must be the first in time.
bool FindTheRef(RecoResultVector phrases, const std::string& reference)
{
    sort(begin(phrases), end(phrases), [](const RecoPhrase& lhs, const RecoPhrase& rhs) { return lhs.Offset < rhs.Offset; });
    int index = 0;
    bool same = false;
    for (auto& phrase : phrases)
    {
        // there are empty strings coming from CTSInRoom service.
        if (phrase.Text.empty())
        {
            continue;
        }
        auto& text = phrase.Text;
        auto ref = reference;

        // remove punctuation
        text.erase(std::remove_if(text.begin(), text.end(), [](unsigned char x) { return std::ispunct(x) ; }), text.end());
        ref.erase(std::remove_if(ref.begin(), ref.end(), [](unsigned char x) { return std::ispunct(x); }), ref.end());
        index++;

        transform(text.begin(), text.end(), text.begin(), [](char c) ->char { return std::tolower(c, std::locale("")); });
        transform(ref.begin(), ref.end(), ref.begin(), [](char c) ->char { return std::tolower(c, std::locale("")); });
        if (text == ref)
        {
            same = true;
            break;
        }
    }

    return index >= 1 && same;
}

std::string GetText(const RecoResultVector& phrases)
{
    ostringstream os;
    for (const auto& phrase : phrases)
    {
        os << "Recognized Text: '" << phrase.Text << "' UserId: '" << phrase.UserId << "' "  << "offset: " << phrase.Offset << endl;
    }
    return os.str();
}

std::string CreateTimestamp()
{
    auto tm = std::chrono::time_point_cast<My90kHzDuration>(std::chrono::high_resolution_clock::now());

    // according to Jun Zhang, the time interval Princeton expecting is in seconds.
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(tm.time_since_epoch());

    ostringstream os;
    os << duration.count();

    return os.str();
}

bool MatchText(std::string& text, const std::string& ref)
{
    auto lowercaseText = text;
    transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(), [](unsigned char c) ->char { return (char)::tolower(c); });

    auto lowercaseRef = ref;
    transform(lowercaseRef.begin(), lowercaseRef.end(), lowercaseRef.begin(), [](unsigned char c) ->char { return (char)::tolower(c); });

    if (lowercaseText.find(lowercaseRef) != string::npos)
    {
        return true;
    }

    return false;
}

void ConnectCallbacks(SpeechRecognizer* sr, RecoPhrasesPtr result)
{
    if (sr == nullptr || result == nullptr)
    {
        return;
    }
    auto callback = ParseRecogEvents<SpeechRecognitionEventArgs>(result);
    sr->Recognizing.DisconnectAll();
    sr->Recognizing.Connect(callback);

    sr->Recognized.DisconnectAll();
    sr->Recognized.Connect(callback);

    ConnectNonRecoEvents<SpeechRecognizer, SpeechRecognitionCanceledEventArgs>(sr, result);
}

void ConnectCallbacks(ConversationTranscriber* ct, RecoPhrasesPtr result)
{
    if (ct == nullptr || result == nullptr)
    {
        return;
    }
    auto callback = ParseRecogEvents<ConversationTranscriptionEventArgs>(result);
    ct->Transcribing.DisconnectAll();
    ct->Transcribing.Connect(callback);

    ct->Transcribed.DisconnectAll();
    ct->Transcribed.Connect(callback);

    ConnectNonRecoEvents<ConversationTranscriber, ConversationTranscriptionCanceledEventArgs>(ct, result);
}
