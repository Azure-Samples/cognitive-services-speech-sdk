//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Conversation;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace Microsoft::CognitiveServices::Speech::Translation;
using namespace std;

#define INITIAL_SILENCE_TIMEOUT_STRING "InitialSilenceTimeout"
#define INITIAL_BABBLE_TIMEOUT_STRING "InitialBabbleTimeout"
#define NOT_RECOGNIZED_STRING "NotRecognized"
#define UNKNOWN_REASON "unknown reason"

using MilliSeconds = chrono::duration<int, milli>;
#define WAIT_FOR_RECO_RESULT_TIME  20s

struct TestData
{
    TestData(const string& audioFilename, const string& utt)
        : m_inputDataFilename(audioFilename), m_utterance(utt)
    {
    }

    void UpdateFullFilename(const string& dir)
    {
        if (m_dir.empty())
        {
            m_dir = dir;
            m_inputDataFilename = m_dir + m_inputDataFilename;
        }
    }
    string m_dir;
    string m_inputDataFilename;
    string m_utterance;
};

// declare the test data here
extern TestData weather;
extern TestData weatherGerman;
extern TestData weathermp3;
extern TestData weatheropus;
extern TestData weatherflac;
extern TestData weatheralaw;
extern TestData weathermulaw;
extern TestData weather8Channels;
extern TestData batman;
extern TestData wrongSamplingRateFile;
extern TestData callTheFirstOne;
extern TestData turnOnLamp;
extern TestData dgiWreckANiceBeach;
extern TestData recordedAudioMessage;
extern TestData kwvAccept;
extern TestData kwvReject;
extern TestData kwvMultiturn;
extern TestData katieSteve;

struct RecoPhrase
{
    RecoPhrase(const std::string& txt, const std::string& id, const std::string& json, uint64_t offset)
        : Text{ txt }, UserId(id), Json{ json }, Offset(offset)
    {}

    RecoPhrase(const std::string& txt)
        : Text{ txt }, UserId{ string{} }, Json{ string{} }, Offset{0}
    {}

    RecoPhrase()
        :Text{ string{} }, UserId{ string{} }, Json{ string{} }, Offset{0}
    {}

    std::string Text;
    std::string UserId;
    std::string Json;
    uint64_t Offset;
};


using RecoResultVector = std::vector<RecoPhrase>;
struct RecoPhrases
{
    RecoResultVector phrases;
    std::promise<void> ready;
};
using RecoPhrasesPtr = shared_ptr<RecoPhrases>;
using CTSPtr = shared_ptr<ConversationTranscriber>;
using SRPtr = shared_ptr<SpeechRecognizer>;

void UseMocks(bool value);
void UseMockUsp(bool value);
bool IsUsingMocks(bool uspMockRequired = true);
int ReadBuffer(fstream& fs, uint8_t* dataBuffer, uint32_t size);
fstream OpenWaveFile(const string& filename);
fstream OpenFile(const string& filename);
shared_ptr<SpeechConfig> CurrentSpeechConfig();
shared_ptr<SpeechTranslationConfig> CurrentTranslationConfig();
void SetMockRealTimeSpeed(int value);
void WaitForResult(future<void>&& f, std::chrono::seconds duration);
void PushData(PushAudioInputStream* pushStream, const string& filename, bool compressed = false);
void DoContinuousReco(SpeechRecognizer * recognizer, PushAudioInputStream * pushStream);
void DoKWS(SpeechRecognizer * recognizer, PushAudioInputStream * pushStream);
void UseOfflineUnidec(std::shared_ptr<SpeechConfig> config);

enum class Callbacks { final_result, intermediate_result, no_match, session_started, session_stopped, speech_start_detected, speech_end_detected };

map<Callbacks, atomic_int> createCallbacksMap();

std::string GetUserId(const ConversationTranscriptionResult* ctr);
std::string GetUserId(const SpeechRecognitionResult* ctr);
template<typename RecogType, typename EventArgType, typename CancelArgType>
void ConnectCallbacks(RecogType* recognizer, RecoPhrasesPtr result)
{
    if (recognizer == nullptr || result == nullptr)
    {
        return;
    }
    recognizer->Recognizing.DisconnectAll();
    recognizer->Recognizing.Connect([result](const EventArgType& e)
    {
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            auto userId = GetUserId(e.Result.get());
            ostringstream os;
            os << "Text= " << e.Result->Text
                << " Offset= " << e.Result->Offset()
                << " Duration= " << e.Result->Duration()
                << " UserId= " << userId;

            SPX_TRACE_VERBOSE("CXX_API_TEST RECOGNIZING: %s", os.str().c_str());
        }
    });

    recognizer->Recognized.DisconnectAll();
    recognizer->Recognized.Connect([result](const EventArgType& e)
    {
        auto userId = GetUserId(e.Result.get());
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            ostringstream os;
            auto json = e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
            os << "Text= " << e.Result->Text
                << " Offset= " << e.Result->Offset()
                << " Duration= " << e.Result->Duration()
                << " UserId= " << userId;

            SPX_TRACE_VERBOSE("CXX_API_TEST RECOGNIZED: %s", os.str().c_str());

            result->phrases.push_back(RecoPhrase{ e.Result->Text, userId, json, e.Result->Offset() });
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            SPX_TRACE_VERBOSE("CXX_API_TEST NOMATCH: Speech could not be recognized.");
            auto nomatch = NoMatchDetails::FromResult(e.Result);
            switch (nomatch->Reason)
            {
            case  NoMatchReason::InitialSilenceTimeout:
                result->phrases.push_back(RecoPhrase{ INITIAL_SILENCE_TIMEOUT_STRING });
                break;
            case NoMatchReason::InitialBabbleTimeout:
                result->phrases.push_back(RecoPhrase{ INITIAL_BABBLE_TIMEOUT_STRING });
                break;
            case NoMatchReason::NotRecognized:
                result->phrases.push_back(RecoPhrase{ NOT_RECOGNIZED_STRING });
                break;
            default:
                result->phrases.push_back(RecoPhrase{ UNKNOWN_REASON });
            }
            result->ready.set_value();
        }
    });
    recognizer->Canceled.DisconnectAll();
    recognizer->Canceled.Connect([result](const CancelArgType& e)
    {
        string error;
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: Reach the end of the file.");
            break;

        case CancellationReason::Error:
            error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
            SPX_TRACE_VERBOSE("CXX_API_TEST CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
            result->phrases.push_back(RecoPhrase{ error });
            result->ready.set_value();
            break;

        default:
            SPX_TRACE_VERBOSE("CXX_API_TEST unknown Reason!");
            result->phrases.push_back(RecoPhrase{ "unknown Reason!" });
            result->ready.set_value();
        }
    });
    recognizer->SessionStopped.DisconnectAll();
    recognizer->SessionStopped.Connect([result](const SessionEventArgs& e)
    {
        SPX_TRACE_VERBOSE("CXX_API_TEST SessionStopped: session id %s", e.SessionId.c_str());
        result->ready.set_value();
    });

    recognizer->SessionStarted.DisconnectAll();
    recognizer->SessionStarted.Connect([result](const SessionEventArgs& e)
    {
        SPX_TRACE_VERBOSE("CXX_API_TEST SessionStarted: session id %s", e.SessionId.c_str());
    });
}

class RecordedDataReader
{
public:
    RecordedDataReader(const std::string& audio);
    ~RecordedDataReader();

    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer);

    bool Open(const std::string& filename);
    SPXSTRING GetProperty(PropertyId propertyId);

private:

    std::string getField(const std::string& header, const std::string& field);
    std::string getNextBase64EncodedData();

    std::vector<unsigned char> m_buffer;
    std::ifstream ifs;
    std::string m_frameType;
    std::string m_timestamp;
    uint64_t m_totalBytes;
    int m_numOfBuffer;
    std::string m_type;
};

bool VerifyText(std::string& text, const std::string& ref = string{});
std::shared_ptr<AudioConfig> CreateAudioPullSingleChannel(std::shared_ptr<PullAudioInputStream>& pullStream);
std::shared_ptr<AudioConfig> CreateAudioPushUsingKatieSteveFile(std::shared_ptr<PushAudioInputStream>& pushAudio);
std::shared_ptr<AudioConfig> CreateAudioPullUsingKatieSteveFile(std::shared_ptr<PullAudioInputStream>& pullAudio);
std::shared_ptr<AudioConfig> CreateAudioPullFromRecordedFile(std::shared_ptr<PullAudioInputStream>& pullAudio);
void StartMeetingAndVerifyResult(ConversationTranscriber* recognizer, const std::shared_ptr<Participant>&  participant, RecoPhrasesPtr&& result, const string& ref = string{});
void CreateVoiceSignatures(std::string* katieVoiceSignature, std::string* steveVoiceSignature);
bool VerifyTextAndSpeaker(const RecoResultVector& phrases, const std::string& text, const std::string& speakerId);
std::string GetText(const RecoResultVector& phrases);
using My90kHzDuration = std::chrono::duration<double, std::ratio<1, 90000>>;
std::string CreateTimestamp();
uint64_t VerifySpeaker(const RecoResultVector& phrases, const std::wstring& speakerId);
