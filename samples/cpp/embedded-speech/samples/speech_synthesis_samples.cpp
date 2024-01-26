//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

extern shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig();


// Lists available embedded speech synthesis voices.
void ListEmbeddedSpeechSynthesisVoices()
{
    // Creates an instance of an embedded speech config.
    auto speechConfig = CreateEmbeddedSpeechConfig();
    if (!speechConfig)
    {
        return;
    }

    // Creates a speech synthesizer.
    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, nullptr);

    // Gets a list of voices.
    auto result = synthesizer->GetVoicesAsync("").get();

    if (result->Reason == ResultReason::VoicesListRetrieved)
    {
        auto getGenderString = [](auto gender)
        {
            if (gender == SynthesisVoiceGender::Female) {
                return "Female";
            } else if (gender == SynthesisVoiceGender::Male) {
                return "Male";
            } else { // SynthesisVoiceGender::Unknown
                return "Unknown";
            }
        };

        cout << "Voices found:" << endl;
        for (const auto& voice : result->Voices)
        {
            cout << voice->Name << endl;
            cout << " Gender: " << getGenderString(voice->Gender) << endl;
            cout << " Locale: " << voice->Locale << endl;
            cout << " Path:   " << voice->VoicePath << endl;
        }

        // To find a voice that supports a specific locale, for example:
        /*
        const auto& voices = result->Voices;
        auto locale = "en-US";
        auto found =
            find_if(voices.begin(), voices.end(), [&](shared_ptr<VoiceInfo> voice)
                {
                    return voice->Locale.compare(locale) == 0;
                });
        if (found != voices.end())
        {
            cout << "Found " << locale << " voice: " << (*found)->Name << endl;
        }
        */
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        cerr << "CANCELED: ErrorDetails=\"" << result->ErrorDetails << "\"" << endl;
    }
}


void SynthesizeSpeech(shared_ptr<SpeechSynthesizer> synthesizer)
{
    // Subscribes to events.
    synthesizer->SynthesisStarted += [](const SpeechSynthesisEventArgs& e)
    {
        UNUSED(e);
        cout << "Synthesis started." << endl;
    };

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e)
    {
        cout << "Synthesizing, received an audio chunk of " << e.Result->GetAudioLength() << " bytes." << endl;
    };

    synthesizer->WordBoundary += [](const SpeechSynthesisWordBoundaryEventArgs& e)
    {
        cout << "Word \"" << e.Text << "\" | "
            << "Text offset " << e.TextOffset << " | "
            // Unit of AudioOffset is tick (1 tick = 100 nanoseconds).
            << "Audio offset " << (e.AudioOffset + 5000) / 10000 << "ms"
            << endl;
    };

    while (true)
    {
        // Receives text from console input.
        cout << "Enter some text that you want to speak, or none for exit." << endl;
        cout << "> ";

        string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

        // Synthesizes text to speech.
        auto result = synthesizer->SpeakTextAsync(text).get();

        // To adjust the rate or volume, use SpeakSsmlAsync with prosody tags instead of SpeakTextAsync.
        /*
        const auto ssmlSynthBegin = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'>";
        const auto ssmlSynthEnd = "</speak>";
        stringstream ssml;

        // Range of rate is -100% to 100%, 100% means 2x faster.
        ssml << ssmlSynthBegin << "<prosody rate='50%'>" << text << "</prosody>" << ssmlSynthEnd;
        std::cout << "Synthesizing with rate +50%" << endl;
        auto result1 = synthesizer->SpeakSsmlAsync(ssml.str()).get();
        ssml.str("");

        // Range of volume is -100% to 100%, 100% means 2x louder.
        ssml << ssmlSynthBegin << "<prosody volume='50%'>" << text << "</prosody>" << ssmlSynthEnd;
        std::cout << "Synthesizing with volume +50%" << endl;
        auto result2 = synthesizer->SpeakSsmlAsync(ssml.str()).get();
        */

        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Synthesis completed for text \"" << text << "\"" << endl;

            // See where the result came from, cloud (online) or embedded (offline)
            // speech synthesis.
            // This can change during a session where HybridSpeechConfig is used.
            /*
            cout << "Synthesis backend: " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_SynthesisBackend) << endl;
            */

            // To save the output audio to a WAV file:
            /*
            auto audioDataStream = AudioDataStream::FromResult(result);
            audioDataStream->SaveToWavFile("SynthesizedSpeech.wav");
            */

            // To read the output audio for e.g. processing it in memory:
            /*
            audioDataStream->SetPosition(0); // reset the stream position

            vector<uint8_t> buffer(16000);
            uint32_t readBytes = 0;
            uint32_t totalBytes = 0;

            while ((readBytes = audioDataStream->ReadData(buffer.data(), static_cast<uint32_t>(buffer.size()))) > 0)
            {
                cout << "Read " << readBytes << " bytes" << endl;
                totalBytes += readBytes;
            }
            cout << "Total " << totalBytes << " bytes" << endl;
            */
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << int(cancellation->Reason) << endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cerr << "CANCELED: ErrorCode=" << int(cancellation->ErrorCode) << endl;
                cerr << "CANCELED: ErrorDetails=\"" << cancellation->ErrorDetails << "\"" << endl;
            }
        }
    }
}


// Synthesizes speech using embedded speech config and the system default speaker device.
void EmbeddedSpeechSynthesisToSpeaker()
{
    auto speechConfig = CreateEmbeddedSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultSpeakerOutput();

    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, audioConfig);
    SynthesizeSpeech(synthesizer);
}


// Synthesizes speech using hybrid (cloud & embedded) speech config and the system default speaker device.
void HybridSpeechSynthesisToSpeaker()
{
    auto speechConfig = CreateHybridSpeechConfig();
    auto audioConfig = AudioConfig::FromDefaultSpeakerOutput();

    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, audioConfig);
    SynthesizeSpeech(synthesizer);
}
