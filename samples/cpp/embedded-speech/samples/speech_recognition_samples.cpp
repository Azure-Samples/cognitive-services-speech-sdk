//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <thread>
#include <speechapi_cxx.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

extern shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig();

extern uint32_t GetEmbeddedSpeechSamplesPerSecond();
extern uint8_t GetEmbeddedSpeechBitsPerSample();
extern uint8_t GetEmbeddedSpeechChannels();

extern const string GetSpeechRawAudioFileName();
extern const string GetSpeechWavAudioFileName();
extern const string GetKeywordModelFileName();
extern const string GetKeywordPhrase();
extern const string GetPerfTestAudioFileName();


// Lists available embedded speech recognition models.
void ListEmbeddedSpeechRecognitionModels()
{
    // Creates an instance of an embedded speech config.
    auto speechConfig = CreateEmbeddedSpeechConfig();
    if (!speechConfig)
    {
        return;
    }

    // Gets a list of models.
    auto models = speechConfig->GetSpeechRecognitionModels();

    if (!models.empty())
    {
        cout << "Models found:" << endl;
        for (const auto& model : models)
        {
            cout << model->Name << endl;
            cout << " Locale(s): ";
            for (const auto& locale : model->Locales)
            {
                cout << locale << " ";
            }
            cout << endl;
            cout << " Path:      " << model->Path << endl;
        }

        // To find a model that supports a specific locale, for example:
        /*
        auto locale = "en-US";
        auto found =
            find_if(models.begin(), models.end(), [&](shared_ptr<SpeechRecognitionModel> model)
                {
                    return model->Locales[0].compare(locale) == 0;
                });
        if (found != models.end())
        {
            cout << "Found " << locale << " model: " << (*found)->Name << endl;
        }
        */
    }
    else
    {
        cerr << "No models found. Either the path is not valid or the format of model(s) is unknown." << endl;
    }
}


void RecognizeSpeech(shared_ptr<SpeechRecognizer> recognizer, bool useKeyword, bool waitForUser)
{
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing += [](const SpeechRecognitionEventArgs& e)
    {
        // Intermediate result (hypothesis).
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            cout << "Recognizing:" << e.Result->Text << endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizingKeyword)
        {
            // ignored
        }
    };

    recognizer->Recognized += [](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedKeyword)
        {
            // Keyword detected, speech recognition will start.
            cout << "KEYWORD: Text=" << e.Result->Text << endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            // Final result. May differ from the last intermediate result.
            cout << "RECOGNIZED: Text=" << e.Result->Text << endl;

            // See where the result came from, cloud (online) or embedded (offline)
            // speech recognition.
            // This can change during a session where HybridSpeechConfig is used.
            /*
            cout << "Recognition backend: " << e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_RecognitionBackend) << endl;
            */

            // Recognition results in JSON format.
            //
            // Offset and duration values are in ticks, where a single tick
            // represents 100 nanoseconds or one ten-millionth of a second.
            //
            // To get word level detail, set the output format to detailed.
            // See EmbeddedSpeechRecognitionFromWavFile() in this source file
            // for a configuration example.
            //
            // If an embedded speech recognition model does not support word
            // timing, the word offset and duration values are always 0, and the
            // phrase offset and duration only indicate a time window inside of
            // which the phrase appeared, not the accurate start and end of speech.
            /*
            string jsonResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
            cout << "JSON result: " << jsonResult << endl;
            */
            // For parsing and better presentation, use e.g. nlohmann/json.
            /*
            auto json = nlohmann::json::parse(jsonResult);
            cout << json.dump(4) << endl;

            if (json.contains("NBest")) // detailed results
            {
                auto best = json["NBest"].at(0);
                if (best.contains("Words")) // word level detail
                {
                    for (const auto& word : best["Words"])
                    {
                        cout << "Word: " << word["Word"] << " | "
                            << "Offset: " << word["Offset"] / 10000 << "ms | "
                            << "Duration: " << word["Duration"] / 10000 << "ms" << endl;
                    }
                }
            }
            */
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            // NoMatch occurs when no speech phrase was recognized.
            auto reason = NoMatchDetails::FromResult(e.Result)->Reason;
            cout << "NO MATCH: Reason=";
            switch (reason)
            {
            case NoMatchReason::NotRecognized:
                // Input audio was not silent but contained no recognizable speech.
                cout << "NotRecognized" << endl;
                break;
            case NoMatchReason::InitialSilenceTimeout:
                // Input audio was silent and the (initial) silence timeout expired.
                // In continuous recognition this can happen multiple times during
                // a session, not just at the very beginning.
                cout << "InitialSilenceTimeout" << endl;
                break;
            default:
                // Other reasons are not supported in embedded speech at the moment.
                cout << int(reason) << endl;
                break;
            }
        }
    };

    recognizer->Canceled += [](const SpeechRecognitionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            // Input stream was closed or the end of an input file was reached.
            cout << "CANCELED: EndOfStream" << endl;
            break;

        case CancellationReason::Error:
            // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
            cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << endl;
            cerr << "CANCELED: ErrorDetails=\"" << e.ErrorDetails << "\"" << endl;
            break;

        default:
            cout << "CANCELED: Reason=" << int(e.Reason) << endl;
            break;
        }
    };

    recognizer->SessionStarted += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session started." << endl;
    };

    recognizer->SessionStopped += [&recognitionEnd](const SessionEventArgs& e)
    {
        UNUSED(e);
        cout << "Session stopped." << endl;
        recognitionEnd.set_value();
    };

    if (useKeyword)
    {
        // Creates an instance of a keyword recognition model.
        auto keywordModel = KeywordRecognitionModel::FromFile(GetKeywordModelFileName());

        // Starts the following routine:
        // 1. Listen for a keyword in input audio. There is no timeout.
        //    Speech that does not start with the keyword is ignored.
        // 2. If the keyword is spotted, start normal speech recognition.
        // 3. After a recognition result (that always includes at least
        //    the keyword), go back to step 1.
        // Steps 1-3 repeat until StopKeywordRecognitionAsync() is called.
        recognizer->StartKeywordRecognitionAsync(keywordModel).get();

        cout << "Say something starting with \"" << GetKeywordPhrase() << "\" (press Enter to stop)...\n";
        cin.get();

        // Stops recognition.
        recognizer->StopKeywordRecognitionAsync().get();
    }
    else
    {
        // The following lines run continuous recognition that listens for speech
        // in input audio and generates results until stopped. To run recognition
        // only once, replace this code block with
        //
        // auto result = recognizer->RecognizeOnceAsync().get();
        //
        // and optionally check result->Reason and result->Text for the outcome
        // instead of doing it in event handlers.

        // Starts continuous recognition.
        recognizer->StartContinuousRecognitionAsync().get();

        if (waitForUser)
        {
            cout << "Say something (press Enter to stop)...\n";
            cin.get();
        }
        else
        {
            recognitionEnd.get_future().get();
        }

        // Stops recognition.
        recognizer->StopContinuousRecognitionAsync().get();
    }
}


// Reads audio samples from the source and writes them into a push stream.
// Push stream can be used when input audio is not generated faster than it
// can be processed (i.e. the generation of input is the limiting factor).
// The application determines the rate of input data transfer.
void PushStreamInputReader(shared_ptr<PushAudioInputStream> pushStream)
{
    try
    {
        // In this example the input stream is a file. Modify the code to use
        // a non-file source (e.g. some audio API that returns data) as needed.
        ifstream input(GetSpeechRawAudioFileName(), ios::in | ios::binary);

        if (!input.good())
        {
            throw invalid_argument("Failed to open input file " + GetSpeechRawAudioFileName());
        }

        vector<uint8_t> buffer(3200); // 100ms of 16kHz 16-bit mono audio

        while (true)
        {
            // Read audio data from the input stream to a data buffer.
            input.read((char*)buffer.data(), buffer.size());
            auto bytesRead = input.gcount();

            // Copy audio data from the data buffer into a push stream
            // for the Speech SDK to consume.
            // Data must NOT include any headers, only audio samples.
            pushStream->Write(buffer.data(), (uint32_t)bytesRead);

            if (!input)
            {
                input.close();
                break;
            }
        }
    }
    catch (const exception& e)
    {
        cerr << "PushStreamInputReader: " << e.what() << endl;
    }

    pushStream->Close();
}


// Implements a pull stream callback that reads audio samples from the source.
// Pull stream should be used when input audio may be generated faster than
// it can be processed (i.e. the processing of input is the limiting factor).
// The Speech SDK determines the rate of input data transfer.
class PullStreamInputReader final : public PullAudioInputStreamCallback
{
private:
    ifstream m_input;

public:
    PullStreamInputReader()
    {
        // In this example the input stream is a file. Modify the code to use
        // a non-file source (e.g. audio API that returns data) as necessary.
        m_input.open(GetSpeechRawAudioFileName(), ios::in | ios::binary);
        if (!m_input.good())
        {
            throw invalid_argument("Failed to open input file " + GetSpeechRawAudioFileName());
        }
    }

    // This method is called to synchronously get data (at most 'size' bytes)
    // from the input stream.
    // It returns the number of bytes copied into the data buffer.
    // If there is no data, the method must wait until data becomes available
    // or return 0 to indicate the end of stream.
    int Read(uint8_t* buffer, uint32_t size) override
    {
        // Copy audio data from the input stream into a data buffer for the
        // Speech SDK to consume.
        // Data must NOT include any headers, only audio samples.
        m_input.read((char*)buffer, size);
        return (int)m_input.gcount();
    }

    // This method is called for cleanup of resources.
    void Close() override
    {
        m_input.close();
    }
};


// Recognizes speech using embedded speech config and the system default microphone device.
void EmbeddedSpeechRecognitionFromMicrophone()
{
    auto useKeyword = false;
    auto waitForUser = true;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using embedded speech config and the system default microphone device.
// Recognition is triggered with a keyword.
void EmbeddedSpeechRecognitionWithKeywordFromMicrophone()
{
    auto useKeyword = true;
    auto waitForUser = true;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using embedded speech config and a WAV file.
void EmbeddedSpeechRecognitionFromWavFile()
{
    auto useKeyword = false;
    auto waitForUser = false;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromWavFileInput(GetSpeechWavAudioFileName());

    // Enables detailed results (from PropertyId::SpeechServiceResponse_JsonResult).
    // See the event handler of 'Recognized' event for how to read the JSON result.
    /*
    speechConfig->SetSpeechRecognitionOutputFormat(OutputFormat::Detailed);
    */

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using embedded speech config and a push stream.
void EmbeddedSpeechRecognitionFromPushStream()
{
    auto useKeyword = false;
    auto waitForUser = false;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(GetEmbeddedSpeechSamplesPerSecond(), GetEmbeddedSpeechBitsPerSample(), GetEmbeddedSpeechChannels());
    auto pushStream = AudioInputStream::CreatePushStream(audioFormat);
    auto audioConfig = AudioConfig::FromStreamInput(pushStream);

    // Push data into the stream in another thread.
    auto pushStreamThread = thread(PushStreamInputReader, pushStream);

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);

    if (pushStreamThread.joinable())
    {
        pushStreamThread.join();
    }
}


// Recognizes speech using embedded speech config and a pull stream.
void EmbeddedSpeechRecognitionFromPullStream()
{
    auto useKeyword = false;
    auto waitForUser = false;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(GetEmbeddedSpeechSamplesPerSecond(), GetEmbeddedSpeechBitsPerSample(), GetEmbeddedSpeechChannels());
    auto pullStreamCallback = make_shared<PullStreamInputReader>();
    auto pullStream = AudioInputStream::CreatePullStream(audioFormat, pullStreamCallback);
    auto audioConfig = AudioConfig::FromStreamInput(pullStream);

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using hybrid (cloud & embedded) speech config and the system default microphone device.
void HybridSpeechRecognitionFromMicrophone()
{
    auto useKeyword = false;
    auto waitForUser = true;

    auto speechConfig = CreateHybridSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Measures the device performance when running embedded speech recognition.
void EmbeddedSpeechRecognitionPerformanceTest()
{
    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromWavFileInput(GetPerfTestAudioFileName());

    // Enables performance metrics to be included with recognition results.
    speechConfig->SetProperty(PropertyId::EmbeddedSpeech_EnablePerformanceMetrics, "true");

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);

    promise<void> recognitionEnd;
    int resultCount = 0;

    // Subscribes to events.
    recognizer->SpeechStartDetected += [](const RecognitionEventArgs&)
    {
        cout << "Processing, please wait...\n";
    };

    recognizer->Recognized += [&resultCount](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            resultCount++;
            cout << "[" << resultCount << "] RECOGNIZED: Text=" << e.Result->Text << endl;

            // Recognition results in JSON format.
            string jsonResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
            auto json = nlohmann::json::parse(jsonResult);

            if (json.contains("PerformanceCounters"))
            {
                const auto& perfCounters = json["PerformanceCounters"];
                cout << "[" << resultCount << "] PerformanceCounters: " << perfCounters.dump(4) << endl;
            }
            else
            {
                cerr << "ERROR: No performance counters data found.\n";
            }
        }
    };

    recognizer->Canceled += [](const SpeechRecognitionCanceledEventArgs& e)
    {
        if (e.Reason == CancellationReason::Error)
        {
            cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << " ErrorDetails=" << e.ErrorDetails << endl;
        }
    };

    recognizer->SessionStopped += [&recognitionEnd](const SessionEventArgs&)
    {
        cout << "All done! Please go to https://aka.ms/embedded-speech for information on how to evaluate the results.\n";
        recognitionEnd.set_value();
    };

    // Runs continuous recognition.
    recognizer->StartContinuousRecognitionAsync().get();
    recognitionEnd.get_future().get();
    recognizer->StopContinuousRecognitionAsync().get();
}
