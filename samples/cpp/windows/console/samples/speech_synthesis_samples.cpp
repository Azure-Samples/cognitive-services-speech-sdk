//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <speechapi_cxx.h>
#include <fstream>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

// Speech synthesis to the default speaker.
void SpeechSynthesisToSpeaker()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech synthesizer using the default speaker as audio output. The default spoken language is "en-us".
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Type some text that you want to speak..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized to speaker for text [" << text << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }

    // This is to give some time for the speaker to finish playing back the audio
    cout << "Press enter to exit..." << std::endl;
    cin.get();
}

// Speech synthesis in the specified spoken language.
void SpeechSynthesisWithLanguage()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Sets the synthesis language.
    // The full list of supported language can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
    auto language = "de-DE";
    config->SetSpeechSynthesisLanguage(language);

    // Creates a speech synthesizer for the specified language, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Type some text that you want to speak..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized to speaker for text [" << text << "] with language [" << language << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }

    // This is to give some time for the speaker to finish playing back the audio
    cout << "Press enter to exit..." << std::endl;
    cin.get();
}

// Speech synthesis in the specified voice.
void SpeechSynthesisWithVoice()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Sets the voice name.
    // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)".
    // The full list of supported voices can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
    auto voice = "Microsoft Server Speech Text to Speech Voice (en-US, BenjaminRUS)";
    config->SetSpeechSynthesisVoiceName(voice);

    // Creates a speech synthesizer for the specified voice, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Type some text that you want to speak..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized to speaker for text [" << text << "] with voice [" << voice << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }

    // This is to give some time for the speaker to finish playing back the audio
    cout << "Press enter to exit..." << std::endl;
    cin.get();
}

// Speech synthesis to wave file.
void SpeechSynthesisToWaveFile()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech synthesizer using file as audio output.
    // Replace with your own audio file name.
    auto fileName = "outputaudio.wav";
    auto fileOutput = AudioConfig::FromWavFileOutput(fileName);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileOutput);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to wave file.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "], and the audio was saved to [" << fileName << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }
}

// Speech synthesis to mp3 file.
void SpeechSynthesisToMp3File()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Sets the synthesis output format.
    // The full list of supported format can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
    config->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Audio16Khz32KBitRateMonoMp3);

    // Creates a speech synthesizer using file as audio output.
    // Replace with your own audio file name.
    auto fileName = "outputaudio.mp3";
    auto fileOutput = AudioConfig::FromWavFileOutput(fileName);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileOutput);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to mp3 file.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "], and the audio was saved to [" << fileName << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }
}

// Speech synthesis to pull audio output stream.
void SpeechSynthesisToPullAudioOutputStream()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates an audio out stream.
    auto stream = AudioOutputStream::CreatePullStream();

    // Creates a speech synthesizer using audio stream output.
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to pull audio output stream.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "], and the audio was written to output stream." << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }

    // Destroys the synthesizer so that the while statement below won't infinitely wait for data from the stream.
    synthesizer = nullptr;

    // Reads(pulls) data from the stream
    uint8_t buffer[32000];
    uint32_t totalSize = 0;
    uint32_t filledSize = 0;

    while ((filledSize = stream->Read(buffer, sizeof(buffer))) > 0)
    {
        cout << filledSize << " bytes received." << endl;
        totalSize += filledSize;
    }

    cout << "Totally " << totalSize << " bytes received." << endl;
}

// Speech synthesis to push audio output stream.
void SpeechSynthesisToPushAudioOutputStream()
{
    // First, defines push audio output stream callback class that implements the
    // PushAudioOutputStreamCallback interface. The sample here illustrates how to define such
    // a callback that writes audio data to a byte vector.
    // PushAudioOutputStreamSampleCallback implements PushAudioOutputStreamCallback interface
    class PushAudioOutputStreamSampleCallback : public PushAudioOutputStreamCallback
    {
    public:
        PushAudioOutputStreamSampleCallback()
        {
            m_audioData = std::make_shared<std::vector<uint8_t>>();
        }

        /// <summary>
        /// The callback function which is invoked when the synthesizer has a output audio chunk to write out.
        /// </summary>
        /// <param name="dataBuffer">The output audio chunk sent by synthesizer.</param>
        /// <param name="size">Size of the output audio chunk in bytes.</param>
        /// <returns>Tell synthesizer how many bytes are received.</returns>
        int Write(uint8_t* dataBuffer, uint32_t size) override
        {
            auto oldSize = m_audioData->size();
            m_audioData->resize(oldSize + size);
            memcpy(m_audioData->data() + oldSize, dataBuffer, size);

            cout << size << " bytes received." << endl;

            return size;
        }

        /// <summary>
        /// The callback which is invoked when the synthesizer is about to close the stream.
        /// </summary>
        void Close() override
        {
            cout << "Push audio output stream closed." << endl;
        }

        /// <summary>
        /// Gets the received audio data size
        /// </summary>
        /// <returns>The received audio data size</returns>
        size_t GetAudioSize()
        {
            return m_audioData->size();
        }

        /// <summary>
        /// Gets the received audio data
        /// </summary>
        /// <returns>The received audio data in byte vector</returns>
        std::shared_ptr<std::vector<uint8_t>> GetAudioData()
        {
            return m_audioData;
        }

    private:
        std::shared_ptr<std::vector<uint8_t>> m_audioData;
    };

    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates an instance of the callback class inherited from PushAudioOutputStreamCallback.
    auto callback = std::make_shared<PushAudioOutputStreamSampleCallback>();

    // Creates an audio out stream from the callback.
    auto stream = AudioOutputStream::CreatePushStream(callback);

    // Creates a speech synthesizer using audio stream output.
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to push audio output stream.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "], and the audio was written to output stream." << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }

    cout << "Totally " << callback->GetAudioSize() << " bytes received." << endl;
}

// Gets synthesized audio data from result.
void SpeechSynthesisToResult()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "]" << std::endl;
            auto audioData = result->GetAudioData();
            cout << audioData->size() << " bytes of audio data received for text [" << text << "]" << endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }
}

// Speech synthesis to audio data stream.
void SpeechSynthesisToAudioDataStream()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "]" << std::endl;
            auto audioDataStream = AudioDataStream::FromResult(result);

            // You can save all the data in the audio data stream to a file
            stringstream fileName;
            fileName << "outputaudio" << i + 1 << ".wav";
            audioDataStream->SaveToWavFile(fileName.str());
            cout << "Audio data for text [" << text << "] was saved to [" << fileName.str() << "]" << endl;

            // You can also read data from audio data stream and process it in memory
            // Reset the stream position to the beginnging since saving to file puts the postion to end.
            audioDataStream->SetPosition(0);

            uint8_t buffer[16000];
            uint32_t totalSize = 0;
            uint32_t filledSize = 0;

            while ((filledSize = audioDataStream->ReadData(buffer, sizeof(buffer))) > 0)
            {
                cout << filledSize << " bytes received." << endl;
                totalSize += filledSize;
            }

            cout << "Totally " << totalSize << " bytes received for text [" << text << "]" << endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }
}

// Speech synthesis events.
void SpeechSynthesisEvents()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    // Subscribes to events
    synthesizer->SynthesisStarted += [](const SpeechSynthesisEventArgs& e)
    {
        UNUSED(e);
        cout << "Synthesis started." << endl;
    };

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e)
    {
        cout << "Synthesizing event received with audio chunk of " << e.Result->GetAudioData()->size() << " bytes" << endl;
    };

    synthesizer->SynthesisCompleted += [](const SpeechSynthesisEventArgs& e)
    {
        UNUSED(e);
        cout << "Synthesis completed." << endl;
    };

    for (int i = 0; i < 2; ++i)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Type some text that you want to synthesize..." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "]" << std::endl;
            auto audioData = result->GetAudioData();
            cout << audioData->size() << " bytes of audio data received for text [" << text << "]" << endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }

            break;
        }
    }
}
