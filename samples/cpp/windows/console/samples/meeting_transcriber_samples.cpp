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

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Transcription;
using namespace Microsoft::CognitiveServices::Speech::Audio;

// Create voice signatures using REST API at https://signature.centralus.cts.speech.microsoft.com by using YourSubscriptionKey
// and the provided enrollment_audio_katie.wav and enrollment_audio_steve.wav.
// Replace the below voiceSignatureKatie and voiceSignatureSteve by copy the Signature value from the Response body. The Signature value contains Version, Tag and Data.
// More details are at https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-use-meeting-transcription-service
// The created signatures should look similar to ones shown below. Please note you should always create up to date signatures, and the example signatures shown below should not be used as such.
const string voiceSignatureKatie = R"(
               { "Version": 0,
                 "Tag": "VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=",
                 "Data": "BhRRgDCrg6ij5fylg5Jpf5ZW/o/uDWi199DYBbfL1Qdspj77qiuvsVKzG2g5Z9jxKtfdwtkKtaDxog9O6pGD7Ot/8mM1jUtt6LKNz4H9EFvznV/dlFk2Oisg8ZKx/RBlNFMYJkQJnxT/zLfhNWiIF5Y97jH4sgRh2orDg6/567FGktAgcESAbiDx1e7tf0TTLdwijw4p1vJ3qJ2cSCdNbXE9KeUd8sClQLDheCPo+et3rMs5W+Rju3W1SJE6ru9gAoH88CyAfI80+ysAecH3GPJYM+j1uhvmWoKIrSfS40BYOe6AUgLNb3a4Pue4oGAmuAyWfwpP1uezleSanpJc73HT91n2UsaHrQj5eK6uuBCjwmu+JI3FT+Vo6JhAARHecdb70U1wyW9vv5t0Q3tV1WNiN/30qSVydDtyrSVpgBiIwlj41Ua22JJCOMrPl7NKLnFmeZ4Hi4aIKoHAxDvrApteL60sxLX/ADAtYCB3Y6iagDyR1IOsIlbaPhL0rQDnC/0z65k7BDekietFNzvvPVoIwJ26GHrXFYYwZe3alVvCsXTpZGBknvSiaCalrixnyGqYo0nG/cd/LodEEIht/PWoFkNlbABqHMbToeI/6j+ICKuVJgTDtcsDQiWKSvrQp9kTSv+cF3LyPVkbks0JvbQhj4AkAv7Rvymgnsu6o8gGyxTX0kxujqCMPCzgFrVd"
               })";
const string voiceSignatureSteve = R"(
                { "Version": 0,
                  "Tag": "HbIvzbfAWjeR/3R+WvUEoeid1AbDaHNOMWItgs7mTxc=",
                  "Data": "DizY04Z7PH/sYu2Yw2EcL4Mvj1GnEDOWJ/DhXHGdQJsQ8/zDc13z1cwllbEo5OSr3oGoKEHLV95OUA6PgksZzvTkf42iOFEv3yifUNfYkZuIzStZoDxWu1H1BoFBejqzSpCYyvqLwilWOyUeMn+z+E4+zXjqHUCyYJ/xf0C3+58kCbmyA55yj7YZ6OtMVyFmfT2GLiXr4YshUB14dgwl3Y08SRNavnG+/QOs+ixf3UoZ6BC1VZcVQnC2tn2FB+8v6ehnIOTQedo++6RWIB0RYmQ8VaEeI0E4hkpA1OxQ9f2gBVtw3KZXWSWBz8sXig2igpwMsQoFRmmIOGsu+p6tM8/OThQpARZ7OyAxsurzmaSGZAaXYt0YwMdIIXKeDBF6/KnUyw+NNzku1875u2Fde/bxgVvCOwhrLPPuu/RZUeAkwVQge7nKYNW5YjDcz8mfg4LfqWEGOVCcmf2IitQtcIEjY3MwLVNvsAB6GT2es1/1QieCfQKy/Tdu8IUfEvekwSCxSlWhfVrLjRhGeWa9idCjsngQbNkqYUNdnIlidkn2DC4BavSTYXR5lVxV4SR/Vvj8h4N5nP/URPDhkzl7n7Tqd4CGFZDzZzAr7yRo3PeUBX0CmdrKLW3+GIXAdvpFAx592pB0ySCv5qBFhJNErEINawfGcmeWZSORxJg1u+agj51zfTdrHZeugFcMs6Be"
                 })";

// Transcribing meeting using a microphone using audio processing options to
// capture 7 + 1 reference channel audio (requires compatible hardware to be present).
void MeetingTranscriptionWithMicrophoneUsingAudioProcessingOptions()
{
    // Creates an instance of a speech config with your subscription key and region.
    // Replace with your own subscription key and service region (e.g., "eastasia").
    // Meeting Transcription is currently available in eastasia and centralus region.
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    // Creates an instance of audio config using default microphone as audio input and with audio processing options specified.
    shared_ptr<AudioConfig> audioInput = nullptr;
    auto audioProcessingOptions = AudioProcessingOptions::Create(
        AUDIO_INPUT_PROCESSING_NONE,
        PresetMicrophoneArrayGeometry::Circular7,
        SpeakerReferenceChannel::LastChannel);
    audioInput = AudioConfig::FromDefaultMicrophoneInput(audioProcessingOptions);
    // Create a meeting from a speech config and meeting Id.
    auto meeting = Meeting::CreateMeetingAsync(config, "MeetingTranscriberSamples").get();

    // Create a meeting transcriber given an audio config. If you don't specify any audio input, Speech SDK opens the default microphone.
    auto recognizer = MeetingTranscriber::FromConfig(audioInput);

    // Need to join a meeting before streaming audio.
    recognizer->JoinMeetingAsync(meeting).get();

    // Creates a participant. Please note that you should create a voice signature matching an actual meeting participant
    // and use that instead of the sample provided here.
    auto katie = Participant::From("katie@example.com", "en-us", voiceSignatureKatie);

    // creates another participant. Please note that you should create a voice signature matching an actual meeting participant
    // and use that instead of the sample provided here.
    auto steve = Participant::From("steve@example.com", "en-us", voiceSignatureSteve);

    // Adds katie as a participant to the meeting.
    meeting->AddParticipantAsync(katie).get();

    // Adds steve as a participant to the meeting.
    meeting->AddParticipantAsync(steve).get();

    // a promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const MeetingTranscriptionEventArgs& e)
        {
            cout << "TRANSCRIBING: Text=" << e.Result->Text << std::endl;
        });

    recognizer->Transcribed.Connect([](const MeetingTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "Transcribed: Text=" << e.Result->Text << std::endl
                    << "  Offset=" << e.Result->Offset() << std::endl
                    << "  Duration=" << e.Result->Duration() << std::endl
                    << "  UserId=" << e.Result->UserId << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    recognizer->Canceled.Connect([&recognitionEnd](const MeetingTranscriptionCanceledEventArgs& e)
        {
            switch (e.Reason)
            {
            case CancellationReason::EndOfStream:
                cout << "CANCELED: Reached the end of the file." << std::endl;
                break;

            case CancellationReason::Error:
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
                recognitionEnd.set_value();
                break;

            default:
                cout << "unknown reason ?!" << std::endl;
            }
        });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "SESSION: " << e.SessionId << " stopped." << std::endl;
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts transcribing.
    recognizer->StartTranscribingAsync().wait();

    // Waits for transcribing to end.
    recognitionEnd.get_future().wait();

    // Stops transcribing. This is optional.
    recognizer->StopTranscribingAsync().wait();
}

// Transcribing meeting using a pull audio stream with 7 + reference channel audio
void MeetingTranscriptionWithPullAudioStreamUsingMultichannelAudio()
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

    // Creates an instance of a speech config with your subscription key and region.
    // Replace with your own subscription key and service region (e.g., "eastasia").
    // Meeting Transcription is currently available in eastasia and centralus region.
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    // Creates a callback that will read audio data from a WAV file.
    shared_ptr<AudioInputFromFileCallback> callback;
    try
    {
        // Replace with your own audio file name.
        // The audio file should be in a format of 16 kHz sampling rate, 16 bits per sample, and 8 channels.
        callback = make_shared<AudioInputFromFileCallback>("katiesteve.wav");
    }
    catch (const exception& e)
    {
        cout << "Exit due to exception: " << e.what() << endl;
    }

    shared_ptr<PullAudioInputStream> pullStream = AudioInputStream::CreatePullStream(AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8), callback);
    auto audioInput = AudioConfig::FromStreamInput(pullStream);

    // Create a meeting from a speech config and conversation Id.
    auto meeting = Meeting::CreateMeetingAsync(config, "MeetingTranscriberSamples").get();

    // Create a meeting transcriber given an audio config. If you don't specify any audio input, Speech SDK opens the default microphone.
    auto transcriber = MeetingTranscriber::FromConfig(audioInput);

    // creates a participant
    auto katie = Participant::From("katie@example.com", "en-us", voiceSignatureKatie);

    // creates another participant
    auto steve = Participant::From("steve@example.com", "en-us", voiceSignatureSteve);

    // Adds katie as a participant to the meeting.
    meeting->AddParticipantAsync(katie).get();

    // Adds steve as a participant to the conversation.
    meeting->AddParticipantAsync(steve).get();

    // Need to join a meeting before streaming audio.
    transcriber->JoinMeetingAsync(meeting).get();

    // a promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    transcriber->Transcribing.Connect([](const MeetingTranscriptionEventArgs& e)
    {
        cout << "TRANSCRIBING: Text=" << e.Result->Text << std::endl;
    });

    transcriber->Transcribed.Connect([](const MeetingTranscriptionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "Transcribed: Text=" << e.Result->Text << std::endl
                << "  Offset=" << e.Result->Offset() << std::endl
                << "  Duration=" << e.Result->Duration() << std::endl
                << "  UserId=" << e.Result->UserId << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    transcriber->Canceled.Connect([&recognitionEnd](const MeetingTranscriptionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            cout << "CANCELED: Reached the end of the file." << std::endl;
            break;

        case CancellationReason::Error:
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            recognitionEnd.set_value();
            break;

        default:
            cout << "unknown reason ?!" << std::endl;
        }
    });

    transcriber->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "SESSION: " << e.SessionId << " stopped." << std::endl;
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Starts transcribing.
    transcriber->StartTranscribingAsync().wait();

    // Waits for transcribing to end.
    recognitionEnd.get_future().wait();

    // Stops transcribing. This is optional.
    transcriber->StopTranscribingAsync().wait();
}

void MeetingTranscriptionWithMultichannelAudioFile()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("katiesteve.wav");

    // Create a meeting from a speech config and meeting Id.
    auto meeting = Meeting::CreateMeetingAsync(config, "MeetingTranscriberSamples").get();

    // Create a meeting transcriber given an audio config. If you don't specify any audio input, Speech SDK opens the default microphone.
    auto transcriber = MeetingTranscriber::FromConfig(audioInput);

    // creates a participant
    auto katie = Participant::From("katie@example.com", "en-us", voiceSignatureKatie);

    // creates another participant
    auto steve = Participant::From("steve@example.com", "en-us", voiceSignatureSteve);

    // Adds katie as a participant to the meeting.
    meeting->AddParticipantAsync(katie).get();

    // Adds steve as a participant to the conversation.
    meeting->AddParticipantAsync(steve).get();

    // Need to join a meeting before streaming audio.
    transcriber->JoinMeetingAsync(meeting).get();

    // a promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    transcriber->Transcribing.Connect([](const MeetingTranscriptionEventArgs& e)
        {
            cout << "TRANSCRIBING: Text=" << e.Result->Text << std::endl;
        });

    transcriber->Transcribed.Connect([](const MeetingTranscriptionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                cout << "Transcribed: Text=" << e.Result->Text << std::endl
                    << "  Offset=" << e.Result->Offset() << std::endl
                    << "  Duration=" << e.Result->Duration() << std::endl
                    << "  UserId=" << e.Result->UserId << std::endl;
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                cout << "NOMATCH: Speech could not be recognized." << std::endl;
            }
        });

    transcriber->Canceled.Connect([&recognitionEnd](const MeetingTranscriptionCanceledEventArgs& e)
        {
            switch (e.Reason)
            {
            case CancellationReason::EndOfStream:
                cout << "CANCELED: Reached the end of the file." << std::endl;
                break;

            case CancellationReason::Error:
                cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
                recognitionEnd.set_value();
                break;

            default:
                cout << "unknown reason ?!" << std::endl;
            }
        });

    transcriber->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            cout << "SESSION: " << e.SessionId << " stopped." << std::endl;
            recognitionEnd.set_value(); // Notify to stop recognition.
        });

    // Starts transcribing.
    transcriber->StartTranscribingAsync().wait();

    // Waits for transcribing to end.
    recognitionEnd.get_future().wait();

    // Stops transcribing. This is optional.
    transcriber->StopTranscribingAsync().wait();
}

// Transcribing meeting using a push audio stream
void MeetingTranscriptionWithPushAudioStreamUsingMultichannelAudio()
{
    // Creates an instance of a speech config with your subscription key and region.
    // Replace with your own subscription key and service region (e.g., "eastasia").
    // Meeting Transcription is currently available in eastasia and centralus region.
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    config->SetProperty("ConversationTranscriptionInRoomAndOnline", "true");

    // Creates a push stream using 16kHz, 16bits per sample and 8 channels audio.
    auto pushStream = AudioInputStream::CreatePushStream(AudioStreamFormat::GetWaveFormatPCM(16000, 16, 8));
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto meeting = Meeting::CreateMeetingAsync(config, "MeetingTranscriberSamples").get();
    auto recognizer = MeetingTranscriber::FromConfig(audioInput);
    recognizer->JoinMeetingAsync(meeting).get();

    // creates a participant
    auto katie = Participant::From("katie@example.com", "en-us", voiceSignatureKatie);

    // creates another participant
    auto steve = Participant::From("steve@example.com", "en-us", voiceSignatureSteve);

    // adds katie as a participant to the meeting.
    meeting->AddParticipantAsync(katie).get();

    // adds steve as a participant to the meeting.
    meeting->AddParticipantAsync(steve).get();

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Transcribing.Connect([](const MeetingTranscriptionEventArgs& e)
    {
        cout << "TRANSCRIBING: Text=" << e.Result->Text << std::endl;
    });

    recognizer->Transcribed.Connect([](const MeetingTranscriptionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl
                << "  Offset=" << e.Result->Offset() << std::endl
                << "  Duration=" << e.Result->Duration() << std::endl
                << "  UserId=" << e.Result->UserId << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const MeetingTranscriptionCanceledEventArgs& e)
    {
        switch (e.Reason)
        {
        case CancellationReason::EndOfStream:
            cout << "CANCELED: Reached the end of the file." << std::endl;
            break;

        case CancellationReason::Error:
            cout << "CANCELED: ErrorCode=" << (int)e.ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            recognitionEnd.set_value();
            break;

        default:
            cout << "unknown reason ?!" << std::endl;
        }
    });

    // Starts transcribing meeting.
    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "SESSION: " << e.SessionId << " stopped." << std::endl;

        // Notify transcribing ends.
        recognitionEnd.set_value();
    });

    // open and read the wave file and push the buffers into the recognizer
    recognizer->StartTranscribingAsync().wait();

    // The audio file should be in a format of 16 kHz sampling rate, 16 bits per sample, and 8 channels.
    try
    {
        WavFileReader reader("katiesteve.wav");
        vector<uint8_t> buffer(1000);

        // Read data and push them into the stream
        int readSamples = 0;
        while ((readSamples = reader.Read(buffer.data(), (uint32_t)buffer.size())) != 0)
        {
            // Push a buffer into the stream
            pushStream->Write(buffer.data(), readSamples);
            this_thread::sleep_for(10ms);
        }
    }
    catch (const exception& e)
    {
        cout << "Exit due to exception " << e.what() << endl;
    }

    // Close the push stream.
    pushStream->Close();

    // Waits for completion.
    recognitionEnd.get_future().wait();

    // Leaves the meeting.
    recognizer->StopTranscribingAsync().wait();
}
