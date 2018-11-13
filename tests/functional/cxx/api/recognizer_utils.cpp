//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "test_utils.h"
#include "recognizer_utils.h"



TestData weather {"/audio/whatstheweatherlike.wav", "What's the weather like?" };
TestData wrongSamplingRateFile {"/audio/11khztest.wav", "" };
TestData cortana {"/audio/heyCortana.wav", "Hey Cortana," };
TestData callTheFirstOne{ "/audio/CallTheFirstOne.wav", "" };
TestData turnOnLamp {"/audio/TurnOnTheLamp.wav", "Turn on lamp" };

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

fstream OpenWaveFile(const string& filename)
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

// signaling when receiving the recognized Text or an error.
void ConnectCallbacks(SpeechRecognizer* recognizer, promise<string>& result)
{
    if (recognizer == nullptr)
    {
        return;
    }
    recognizer->Recognized.DisconnectAll();
    recognizer->Recognized.Connect([&result](const SpeechRecognitionEventArgs& e)
    {

        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            SPX_TRACE_VERBOSE("RECOGNIZED: Text= %s, Offset= %d, Duration= %d", e.Result->Text.c_str(), e.Result->Offset(), e.Result->Duration());
            result.set_value(e.Result->Text);
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            SPX_TRACE_VERBOSE("NOMATCH: Speech could not be recognized.");

            auto nomatch = NoMatchDetails::FromResult(e.Result);
            switch (nomatch->Reason)
            {
            case  NoMatchReason::InitialSilenceTimeout:
                result.set_value(INITIAL_SILENCE_TIMEOUT_STRING);
                break;
            case NoMatchReason::InitialBabbleTimeout:
                result.set_value(INITIAL_BABBAL_TIMEOUT_STRING);
                break;
            case NoMatchReason::NotRecognized:
                result.set_value(NOT_RECOGNIZED_STRING);
                break;
            default:
                result.set_value(UNKNOWN_REASON);
            }
        }
    });
    recognizer->Canceled.DisconnectAll();
    recognizer->Canceled.Connect([&result](const SpeechRecognitionCanceledEventArgs& e)
    {
        string error;
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            SPX_TRACE_VERBOSE("CANCELED: Reach the end of the file.");
            break;

        case CancellationReason::Error:
            error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
            SPX_TRACE_VERBOSE("CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
            result.set_value(error);
            break;

        default:
            SPX_TRACE_VERBOSE("unknown Reason!");
            result.set_value("unknown Reason!");
        }
    });
}

string  WaitForResult(future<string>&& f, MilliSeconds duration)
{
    auto status = f.wait_for(duration);
    REQUIRE(status == future_status::ready);    
    return f.get();
}

void PushData(PushAudioInputStream* pushStream, const string& filename)
{
    fstream fs;
    try
    {
        fs = OpenWaveFile(filename);
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
    promise<string> result;
    ConnectCallbacks(recognizer, result);
    PushData(pushStream, weather.m_audioFilename);
    recognizer->StartContinuousRecognitionAsync().wait();
    auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopContinuousRecognitionAsync().wait();
    SPXTEST_REQUIRE(text.compare(weather.m_utterance) == 0);
}

void DoKWS(SpeechRecognizer* recognizer, PushAudioInputStream* pushStream)
{
    promise<string> res;
    cortana.UpdateFullFilename(Config::InputDir);

    ConnectCallbacks(recognizer, res);
    PushData(pushStream, cortana.m_audioFilename);
    auto model = KeywordRecognitionModel::FromFile(Config::InputDir + "/kws/heycortana_en-US.table");
    recognizer->StartKeywordRecognitionAsync(model).wait();    
    auto text = WaitForResult(res.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopKeywordRecognitionAsync().wait();
    SPXTEST_REQUIRE(text.compare(cortana.m_utterance) == 0);
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
