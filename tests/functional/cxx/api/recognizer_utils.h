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
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
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
        : m_audioFilename(audioFilename), m_utterance(utt)
    {
    }

    void UpdateFullFilename(const string& dir)
    {
        if (m_dir.empty())
        {
            m_dir = dir;
            m_audioFilename = m_dir + m_audioFilename;
        }
    }
    string m_dir;
    string m_audioFilename;
    string m_utterance;
};

// declare the test data here
extern TestData weather;
extern TestData batman;
extern TestData wrongSamplingRateFile;
extern TestData callTheFirstOne;
extern TestData turnOnLamp;

void UseMocks(bool value);
void UseMockUsp(bool value);
bool IsUsingMocks(bool uspMockRequired = true);
int ReadBuffer(fstream& fs, uint8_t* dataBuffer, uint32_t size);
fstream OpenWaveFile(const string& filename);
shared_ptr<SpeechConfig> CurrentSpeechConfig();
void SetMockRealTimeSpeed(int value);
void ConnectCallbacks(SpeechRecognizer * pRecognizer, promise<string>& result);
string  WaitForResult(future<string>&& f, MilliSeconds duration);
void PushData(PushAudioInputStream* pushStream, const string& filename);
void DoContinuousReco(SpeechRecognizer * recognizer, PushAudioInputStream * pushStream);
void DoKWS(SpeechRecognizer * recognizer, PushAudioInputStream * pushStream);

enum class Callbacks { final_result, intermediate_result, no_match, session_started, session_stopped, speech_start_detected, speech_end_detected };

map<Callbacks, atomic_int> createCallbacksMap();
