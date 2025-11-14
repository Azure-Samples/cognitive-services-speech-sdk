//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <fstream>
#include <iostream>
#include <thread>
#include <speechapi_cxx.h>
#include <nlohmann/json.hpp>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

extern std::shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern std::shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig();

extern uint32_t GetEmbeddedSpeechSamplesPerSecond();
extern uint8_t GetEmbeddedSpeechBitsPerSample();
extern uint8_t GetEmbeddedSpeechChannels();

extern const std::string GetSpeechRawAudioFileName();
extern const std::string GetSpeechWavAudioFileName();
extern const std::string GetKeywordModelFileName();
extern const std::string GetKeywordPhrase();
extern const std::string GetPerfTestAudioFileName();


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
        std::cout << "Models found:" << std::endl;
        for (const auto& model : models)
        {
            std::cout << model->Name << std::endl;
            std::cout << " Locale(s): ";
            for (const auto& locale : model->Locales)
            {
                std::cout << locale << " ";
            }
            std::cout << std::endl;
            std::cout << " Path:      " << model->Path << std::endl;
        }

        // To find a model that supports a specific locale, for example:
        /*
        auto locale = "en-US";
        auto found =
            std::find_if(models.begin(), models.end(), [&](std::shared_ptr<SpeechRecognitionModel> model)
                {
                    return model->Locales[0].compare(locale) == 0;
                });
        if (found != models.end())
        {
            std::cout << "Found " << locale << " model: " << (*found)->Name << std::endl;
        }
        */
    }
    else
    {
        std::cerr << "No models found. Either the path is not valid or the format of model(s) is unknown." << std::endl;
    }
}


void RecognizeSpeech(std::shared_ptr<SpeechRecognizer> recognizer, bool useKeyword, bool waitForUser)
{
    std::promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing += [](const SpeechRecognitionEventArgs& e)
    {
        // Intermediate result (hypothesis).
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            std::cout << "Recognizing:" << e.Result->Text << std::endl;
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
            std::cout << "KEYWORD: Text=" << e.Result->Text << std::endl;
        }
        else if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            // Final result. May differ from the last intermediate result.
            std::cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl;

            // See where the result came from, cloud (online) or embedded (offline)
            // speech recognition.
            // This can change during a session where HybridSpeechConfig is used.
            /*
            std::cout << "Recognition backend: " << e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_RecognitionBackend) << std::endl;
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
            std::string jsonResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
            std::cout << "JSON result: " << jsonResult << std::endl;
            */
            // For parsing and better presentation, use e.g. nlohmann/json.
            /*
            auto json = nlohmann::json::parse(jsonResult);
            std::cout << json.dump(4) << std::endl;

            if (json.contains("NBest")) // detailed results
            {
                auto best = json["NBest"].at(0);
                if (best.contains("Words")) // word level detail
                {
                    for (const auto& word : best["Words"])
                    {
                        std::cout << "Word: " << word["Word"] << " | "
                            << "Offset: " << word["Offset"] / 10000 << "ms | "
                            << "Duration: " << word["Duration"] / 10000 << "ms" << std::endl;
                    }
                }
            }
            */
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            // NoMatch occurs when no speech phrase was recognized.
            auto reason = NoMatchDetails::FromResult(e.Result)->Reason;
            std::cout << "NO MATCH: Reason=";
            switch (reason)
            {
            case NoMatchReason::NotRecognized:
                // Input audio was not silent but contained no recognizable speech.
                std::cout << "NotRecognized" << std::endl;
                break;
            case NoMatchReason::InitialSilenceTimeout:
                // Input audio was silent and the (initial) silence timeout expired.
                // In continuous recognition this can happen multiple times during
                // a session, not just at the very beginning.
                std::cout << "InitialSilenceTimeout" << std::endl;
                break;
            default:
                // Other reasons are not supported in embedded speech at the moment.
                std::cout << int(reason) << std::endl;
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
            std::cout << "CANCELED: EndOfStream" << std::endl;
            break;

        case CancellationReason::Error:
            // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
            std::cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << std::endl;
            std::cerr << "CANCELED: ErrorDetails=\"" << e.ErrorDetails << "\"" << std::endl;
            break;

        default:
            std::cout << "CANCELED: Reason=" << int(e.Reason) << std::endl;
            break;
        }
    };

    recognizer->SessionStarted += [](const SessionEventArgs& e)
    {
        UNUSED(e);
        std::cout << "Session started." << std::endl;
    };

    recognizer->SessionStopped += [&recognitionEnd](const SessionEventArgs& e)
    {
        UNUSED(e);
        std::cout << "Session stopped." << std::endl;
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

        std::cout << "Say something starting with \"" << GetKeywordPhrase() << "\" (press Enter to stop)..." << std::endl;
        std::cin.get();

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
            std::cout << "Say something (press Enter to stop)..." << std::endl;
            std::cin.get();
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
void PushStreamInputReader(std::shared_ptr<PushAudioInputStream> pushStream)
{
    try
    {
        // In this example the input stream is a file. Modify the code to use
        // a non-file source (e.g. some audio API that returns data) as needed.
        std::ifstream input(GetSpeechRawAudioFileName(), std::ios::in | std::ios::binary);

        if (!input.good())
        {
            throw std::invalid_argument("Failed to open input file " + GetSpeechRawAudioFileName());
        }

        std::vector<uint8_t> buffer(3200); // 100ms of 16kHz 16-bit mono audio

        while (true)
        {
            // Read audio data from the input stream to a data buffer.
            input.read((char*)buffer.data(), buffer.size());
            std::streamsize bytesRead = input.gcount();

            // Copy audio data from the data buffer into a push stream
            // for the Speech SDK to consume.
            // Data must NOT include any headers, only audio samples.
            // If the method used to read the input stream can return
            // a negative number of bytes in case of an error, check
            // the value and do not pass a negative number to the SDK.
            if (bytesRead >= 0)
            {
                pushStream->Write(buffer.data(), (uint32_t)bytesRead);
            }

            if (!input || bytesRead <= 0)
            {
                input.close();
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "PushStreamInputReader: " << e.what() << std::endl;
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
    std::ifstream m_input;

public:
    PullStreamInputReader()
    {
        // In this example the input stream is a file. Modify the code to use
        // a non-file source (e.g. audio API that returns data) as necessary.
        m_input.open(GetSpeechRawAudioFileName(), std::ios::in | std::ios::binary);
        if (!m_input.good())
        {
            throw std::invalid_argument("Failed to open input file " + GetSpeechRawAudioFileName());
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
        std::streamsize bytesRead = m_input.gcount();

        // If the method used to read the input stream can return
        // a negative number of bytes in case of an error, check
        // the value and do not pass a negative number to the SDK.
        return (bytesRead >= 0) ? (int)bytesRead : 0;
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
    bool useKeyword = false;
    bool waitForUser = true;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using embedded speech config and the system default microphone device.
// Recognition is triggered with a keyword.
void EmbeddedSpeechRecognitionWithKeywordFromMicrophone()
{
    bool useKeyword = true;
    bool waitForUser = true;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using embedded speech config and a WAV file.
void EmbeddedSpeechRecognitionFromWavFile()
{
    bool useKeyword = false;
    bool waitForUser = false;

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
    bool useKeyword = false;
    bool waitForUser = false;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(GetEmbeddedSpeechSamplesPerSecond(), GetEmbeddedSpeechBitsPerSample(), GetEmbeddedSpeechChannels());
    auto pushStream = AudioInputStream::CreatePushStream(audioFormat);
    auto audioConfig = AudioConfig::FromStreamInput(pushStream);

    // Push data into the stream in another thread.
    auto pushStreamThread = std::thread(PushStreamInputReader, pushStream);

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
    bool useKeyword = false;
    bool waitForUser = false;

    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(GetEmbeddedSpeechSamplesPerSecond(), GetEmbeddedSpeechBitsPerSample(), GetEmbeddedSpeechChannels());
    auto pullStreamCallback = std::make_shared<PullStreamInputReader>();
    auto pullStream = AudioInputStream::CreatePullStream(audioFormat, pullStreamCallback);
    auto audioConfig = AudioConfig::FromStreamInput(pullStream);

    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    RecognizeSpeech(recognizer, useKeyword, waitForUser);
}


// Recognizes speech using hybrid (cloud & embedded) speech config and the system default microphone device.
void HybridSpeechRecognitionFromMicrophone()
{
    bool useKeyword = false;
    bool waitForUser = true;

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

    std::promise<void> recognitionEnd;
    int resultCount = 0;

    // Subscribes to events.
    recognizer->SpeechStartDetected += [](const RecognitionEventArgs&)
    {
        std::cout << "Processing, please wait..." << std::endl;
    };

    recognizer->Recognized += [&resultCount](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            resultCount++;
            std::cout << "[" << resultCount << "] RECOGNIZED: Text=" << e.Result->Text << std::endl;

            // Recognition results in JSON format.
            std::string jsonResult = e.Result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
            auto json = nlohmann::json::parse(jsonResult);

            if (json.contains("PerformanceCounters"))
            {
                const auto& perfCounters = json["PerformanceCounters"];
                std::cout << "[" << resultCount << "] PerformanceCounters: " << perfCounters.dump(4) << std::endl;
            }
            else
            {
                std::cerr << "ERROR: No performance counters data found." << std::endl;
            }
        }
    };

    recognizer->Canceled += [](const SpeechRecognitionCanceledEventArgs& e)
    {
        if (e.Reason == CancellationReason::Error)
        {
            std::cerr << "CANCELED: ErrorCode=" << int(e.ErrorCode) << " ErrorDetails=" << e.ErrorDetails << std::endl;
        }
    };

    recognizer->SessionStopped += [&recognitionEnd](const SessionEventArgs&)
    {
        std::cout << "All done! Please go to https://aka.ms/embedded-speech for information on how to evaluate the results." << std::endl;
        recognitionEnd.set_value();
    };

    // Runs continuous recognition.
    recognizer->StartContinuousRecognitionAsync().get();
    recognitionEnd.get_future().get();
    recognizer->StopContinuousRecognitionAsync().get();
}
