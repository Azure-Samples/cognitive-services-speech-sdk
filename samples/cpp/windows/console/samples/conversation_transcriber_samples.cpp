//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <speechapi_cxx.h>
#include <fstream>
#include "wav_file_reader.h"
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Transcription;
using namespace Microsoft::CognitiveServices::Speech::Audio;

void ConversationTranscriptionWithMicrophone()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    auto recognizer = ConversationTranscriber::FromConfig(config);
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            cout << "Transcribing:" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "TRANSCRIBED: Speaker=" << e.Result->SpeakerId << "" << " Text=" << e.Result->Text << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const ConversationTranscriptionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts transcribing.
    recognizer->StartTranscribingAsync().get();

    cout << "Ready to transcribe..." << endl;

    // Waits for transcription to end.
    recognitionEnd.get_future().get();

    // Stops transcription.
    recognizer->StopTranscribingAsync().get();
}

void ConversationTranscriptionWithFile()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a conversation transcriber using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("katiesteve_mono.wav");
    auto recognizer = ConversationTranscriber::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            cout << "Transcribing:" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "TRANSCRIBED: Speaker=" << e.Result->SpeakerId << "" << " Text=" << e.Result->Text << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const ConversationTranscriptionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartTranscribingAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopTranscribingAsync().get();
}

void ConversationTranscriptionWithPullAudioStream()
{
    // First, define your own pull audio input stream callback class that implements the
    // PullAudioInputStreamCallback interface. The sample here illustrates how to define such
    // a callback that reads audio data from a wav file.
    // AudioInputFromFileCallback implements PullAudioInputStreamCallback interface, and uses a wav file as source
    class AudioInputFromFileCallback final : public PullAudioInputStreamCallback
    {
    public:
        // Constructor that creates an input stream from a file.
        AudioInputFromFileCallback(const string& audioFileName)
            : m_reader(audioFileName)
        {
        }

        // Implements AudioInputStream::Read() which is called to get data from the audio stream.
        // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
        // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
        // If there is no data, this function must wait until data is available.
        // It returns the number of bytes that have been copied in 'dataBuffer'.
        // It returns 0 to indicate that the stream reaches end or is closed.
        int Read(uint8_t* dataBuffer, uint32_t size) override
        {
            return m_reader.Read(dataBuffer, size);
        }
        // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
        void Close() override
        {
            m_reader.Close();
        }

    private:
        WavFileReader m_reader;
    };

    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a callback that will read audio data from a WAV file.
    // Currently, the only supported WAV format is mono(single channel), 16 kHZ sample rate, 16 bits per sample.
    // Replace with your own audio file name.
    auto callback = make_shared<AudioInputFromFileCallback>("katiesteve_mono.wav");
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates a conversation transcriber from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pullStream);
    auto recognizer = ConversationTranscriber::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            cout << "Transcribing:" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "TRANSCRIBED: Speaker=" << e.Result->SpeakerId << "" << " Text=" << e.Result->Text << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const ConversationTranscriptionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartTranscribingAsync().get();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopTranscribingAsync().get();
}

void ConversationTranscriptionWithPushAudioStream()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a push stream
    auto pushStream = AudioInputStream::CreatePushStream();

    // Creates a conversation transcriber from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = ConversationTranscriber::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            cout << "Transcribing:" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "TRANSCRIBED: Speaker=" << e.Result->SpeakerId << "" << " Text=" << e.Result->Text << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const ConversationTranscriptionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop recognition.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    WavFileReader reader("katiesteve_mono.wav");

    vector<uint8_t> buffer(1000);

    // Starts transcription.
    recognizer->StartTranscribingAsync().wait();

    // Read data and push them into the stream
    int readSamples = 0;
    while ((readSamples = reader.Read(buffer.data(), (uint32_t)buffer.size())) != 0)
    {
        // Push a buffer into the stream
        pushStream->Write(buffer.data(), readSamples);
    }

    // Close the push stream.
    pushStream->Close();

    // Waits for recognition end.
    recognitionEnd.get_future().get();

    // Stops recognition.
    recognizer->StopTranscribingAsync().get();
}

#pragma region Language Detection related samples

void ConversationTranscriptionAndLanguageIdWithMicrophone()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Define the set of spoken languages that will need to be identified.
    // Replace the languages with your languages in BCP-47 format, e.g. "fr-FR".
    // Please see https://learn.microsoft.com/azure/cognitive-services/speech-service/language-support?tabs=language-identification
    // for the full list of supported languages
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = ConversationTranscriber::FromConfig(config, autoDetectSourceLanguageConfig);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            auto lidResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            cout << "Transcribing in " << lidResult->Language << " Text=" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const ConversationTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                auto lidResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
                cout << "TRANSCRIBED in " << lidResult->Language << " Speaker=" << e.Result->SpeakerId << " Text=" << e.Result->Text << "\n"
                    << "  Offset=" << e.Result->Offset() << "\n"
                    << "  Duration=" << e.Result->Duration() << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Could not transcribe speech." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const ConversationTranscriptionCanceledEventArgs& e)
        {
            cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

            if (e.Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << "\n"
                    << "CANCELED: ErrorDetails=" << e.ErrorDetails << "\n"
                    << "CANCELED: Did you update the subscription info?" << std::endl;

                recognitionEnd.set_value(); // Notify to stop transcribing.
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "Session stopped.";
            recognitionEnd.set_value(); // Notify to stop transcribing.
        });

    // Starts continuous recognition. Uses StopTranscribingAsync() to stop transcribing.
    recognizer->StartTranscribingAsync().get();

    cout << "Say something in English or German...\n";

    // Waits for transcription end.
    recognitionEnd.get_future().get();

    // Stops transcribing.
    recognizer->StopTranscribingAsync().get();
}
