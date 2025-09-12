//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <iostream>
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

extern std::shared_ptr<EmbeddedSpeechConfig> CreateEmbeddedSpeechConfig();
extern std::shared_ptr<HybridSpeechConfig> CreateHybridSpeechConfig();


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
            } else if (gender == SynthesisVoiceGender::Neutral) {
                return "Neutral";
            } else { // SynthesisVoiceGender::Unknown
                return "Unknown";
            }
        };

        std::cout << "Voices found:" << std::endl;
        for (const auto& voice : result->Voices)
        {
            std::cout << voice->Name << std::endl;
            std::cout << " Gender: " << getGenderString(voice->Gender) << std::endl;
            std::cout << " Locale: " << voice->Locale << std::endl;
            std::cout << " Path:   " << voice->VoicePath << std::endl;
        }

        // To find a voice that supports a specific locale, for example:
        /*
        const auto& voices = result->Voices;
        auto locale = "en-US";
        auto found =
            std::find_if(voices.begin(), voices.end(), [&](std::shared_ptr<VoiceInfo> voice)
                {
                    return voice->Locale.compare(locale) == 0;
                });
        if (found != voices.end())
        {
            std::cout << "Found " << locale << " voice: " << (*found)->Name << std::endl;
        }
        */
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        std::cerr << "CANCELED: ErrorDetails=\"" << result->ErrorDetails << "\"" << std::endl;
    }
}


void SynthesizeSpeech(std::shared_ptr<SpeechSynthesizer> synthesizer)
{
    // Subscribes to events.
    synthesizer->SynthesisStarted += [](const SpeechSynthesisEventArgs& e)
    {
        UNUSED(e);
        std::cout << "Synthesis started." << std::endl;
    };

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e)
    {
        std::cout << "Synthesizing, received an audio chunk of " << e.Result->GetAudioLength() << " bytes." << std::endl;
    };

    synthesizer->WordBoundary += [](const SpeechSynthesisWordBoundaryEventArgs& e)
    {
        std::cout << "Word \"" << e.Text << "\" | "
            << "Text offset " << e.TextOffset << " | "
            // Unit of AudioOffset is tick (1 tick = 100 nanoseconds).
            << "Audio offset " << (e.AudioOffset + 5000) / 10000 << "ms"
            << std::endl;
    };

    while (true)
    {
        // Receives text from console input.
        std::cout << "Enter some text that you want to speak, or none for exit." << std::endl;
        std::cout << "> ";

        std::string text;
        std::getline(std::cin, text);
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
        std::stringstream ssml;

        // Range of rate is -100% to 100%, 100% means 2x faster.
        ssml << ssmlSynthBegin << "<prosody rate='50%'>" << text << "</prosody>" << ssmlSynthEnd;
        std::cout << "Synthesizing with rate +50%" << std::endl;
        auto result1 = synthesizer->SpeakSsmlAsync(ssml.str()).get();
        ssml.str("");

        // Range of volume is -100% to 100%, 100% means 2x louder.
        ssml << ssmlSynthBegin << "<prosody volume='50%'>" << text << "</prosody>" << ssmlSynthEnd;
        std::cout << "Synthesizing with volume +50%" << std::endl;
        auto result2 = synthesizer->SpeakSsmlAsync(ssml.str()).get();
        */

        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            std::cout << "Synthesis completed for text \"" << text << "\"" << std::endl;

            // See where the result came from, cloud (online) or embedded (offline)
            // speech synthesis.
            // This can change during a session where HybridSpeechConfig is used.
            /*
            std::cout << "Synthesis backend: " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_SynthesisBackend) << std::endl;
            */

            // To save the output audio to a WAV file:
            /*
            auto audioDataStream = AudioDataStream::FromResult(result);
            audioDataStream->SaveToWavFile("SynthesizedSpeech.wav");
            */

            // To read the output audio for e.g. processing it in memory:
            /*
            audioDataStream->SetPosition(0); // reset the stream position

            std::vector<uint8_t> buffer(16000);
            uint32_t readBytes = 0;
            uint32_t totalBytes = 0;

            while ((readBytes = audioDataStream->ReadData(buffer.data(), static_cast<uint32_t>(buffer.size()))) > 0)
            {
                std::cout << "Read " << readBytes << " bytes" << std::endl;
                totalBytes += readBytes;
            }
            std::cout << "Total " << totalBytes << " bytes" << std::endl;
            */
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            std::cout << "CANCELED: Reason=" << int(cancellation->Reason) << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                std::cerr << "CANCELED: ErrorCode=" << int(cancellation->ErrorCode) << std::endl;
                std::cerr << "CANCELED: ErrorDetails=\"" << cancellation->ErrorDetails << "\"" << std::endl;
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
