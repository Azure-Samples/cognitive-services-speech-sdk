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
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer using the default speaker as audio output. The default spoken language is "en-us".
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    while (true)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Enter some text that you want to speak, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis in the specified spoken language.
void SpeechSynthesisWithLanguage()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Sets the synthesis language.
    // The full list of supported language can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
    auto language = "de-DE";
    config->SetSpeechSynthesisLanguage(language);

    // Creates a speech synthesizer for the specified language, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    while (true)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Enter some text that you want to speak, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis in the specified voice.
void SpeechSynthesisWithVoice()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Sets the voice name.
    // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)".
    // The full list of supported voices can be found here:
    // https://aka.ms/csspeech/voicenames
    // And, you can try GetVoicesAsync method to get all available voices (see SpeechSynthesisGetAvailableVoices() sample below).
    auto voice = "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)";
    config->SetSpeechSynthesisVoiceName(voice);

    // Creates a speech synthesizer for the specified voice, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    while (true)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Enter some text that you want to speak, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}


// Speech synthesis using Custom Voice (https://aka.ms/customvoice)
void SpeechSynthesisUsingCustomVoice()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");
    // Replace with the endpoint id of your Custom Voice model.
    config->SetEndpointId("YourEndpointId");
    // Replace with the voice name of your Custom Voice model.
    config->SetSpeechSynthesisVoiceName("YourVoiceName");

    // Creates a speech synthesizer for Custom Voice, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    while (true)
    {
        // Receives a text from console input and synthesize it to speaker.
        cout << "Enter some text that you want to speak, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized to speaker for text [" << text << "]" << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << static_cast<int>(cancellation->Reason) << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << static_cast<int>(cancellation->ErrorCode) << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }
        }
    }
}

// Speech synthesis to wave file.
void SpeechSynthesisToWaveFile()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer using file as audio output.
    // Replace with your own audio file name.
    auto fileName = "outputaudio.wav";
    auto fileOutput = AudioConfig::FromWavFileOutput(fileName);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileOutput);

    while (true)
    {
        // Receives a text from console input and synthesize it to wave file.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis to mp3 file.
void SpeechSynthesisToMp3File()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Sets the synthesis output format.
    // The full list of supported format can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
    config->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Audio16Khz32KBitRateMonoMp3);

    // Creates a speech synthesizer using file as audio output.
    // Replace with your own audio file name.
    auto fileName = "outputaudio.mp3";
    auto fileOutput = AudioConfig::FromWavFileOutput(fileName);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileOutput);

    while (true)
    {
        // Receives a text from console input and synthesize it to mp3 file.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis to pull audio output stream.
void SpeechSynthesisToPullAudioOutputStream()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates an audio out stream.
    auto stream = AudioOutputStream::CreatePullStream();

    // Creates a speech synthesizer using audio stream output.
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    while (true)
    {
        // Receives a text from console input and synthesize it to pull audio output stream.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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

    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates an instance of the callback class inherited from PushAudioOutputStreamCallback.
    auto callback = std::make_shared<PushAudioOutputStreamSampleCallback>();

    // Creates an audio out stream from the callback.
    auto stream = AudioOutputStream::CreatePushStream(callback);

    // Creates a speech synthesizer using audio stream output.
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    while (true)
    {
        // Receives a text from console input and synthesize it to push audio output stream.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }

    cout << "Totally " << callback->GetAudioSize() << " bytes received." << endl;
}

// Gets synthesized audio data from result.
void SpeechSynthesisToResult()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    while (true)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis to audio data stream.
void SpeechSynthesisToAudioDataStream()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    while (true)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "]" << std::endl;
            auto audioDataStream = AudioDataStream::FromResult(result);

            // You can save all the data in the audio data stream to a file
            stringstream fileName;
            fileName << "outputaudio.wav";
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
        }
    }
}

// Speech synthesis events.
void SpeechSynthesisEvents()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

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

    while (true)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis word boundary event.
void SpeechSynthesisWordBoundaryEvent()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    // Subscribes to word boundary event
    synthesizer->WordBoundary += [](const SpeechSynthesisWordBoundaryEventArgs& e)
    {
        cout << "Word boundary event received. "
            // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
            << "Audio offset: " << (e.AudioOffset + 5000) / 10000 << "ms, "
            << "text offset: " << e.TextOffset << ", "
            << "word length: " << e.WordLength << "."
            << endl;
    };

    while (true)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

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
        }
    }
}

// Speech synthesis viseme event.
void SpeechSynthesisVisemeEvent()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    // Subscribes to viseme received event
    synthesizer->VisemeReceived += [](const SpeechSynthesisVisemeEventArgs& e)
    {
        cout << "viseme event received. "
            // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
            << "Audio offset: " << e.AudioOffset / 10000 << "ms, "
            << "viseme id: " << e.VisemeId << "." << endl;
    };

    while (true)
    {
        // Receives a text from console input and synthesize it to result.
        cout << "Enter some text that you want to synthesize, or enter empty text to exit." << std::endl;
        cout << "> ";
        std::string text;
        getline(cin, text);
        if (text.empty())
        {
            break;
        }

        auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized for text [" << text << "]" << std::endl;
            const auto audioData = result->GetAudioData();
            cout << audioData->size() << " bytes of audio data received for text [" << text << "]" << endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << static_cast<int>(cancellation->Reason) << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << static_cast<int>(cancellation->ErrorCode) << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }
        }
    }
}

// Speech synthesis bookmark event.
void SpeechSynthesisBookmarkEvent()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer with a null output stream.
    // This means the audio output data will not be written to any stream.
    // You can just get the audio from the result.
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    // Subscribes to bookmark reached event
    synthesizer->BookmarkReached += [](const SpeechSynthesisBookmarkEventArgs& e)
    {
        cout << "bookmark reached. "
            // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
            << "Audio offset: " << e.AudioOffset / 10000 << "ms, "
            << "Bookmark text: " << e.Text << "." << endl;
    };

    // Bookmark tag is needed in the SSML, e.g.
    const auto ssml = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'><voice name='Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)'><bookmark mark='bookmark_one'/> one. <bookmark mark='bookmark_two'/> two. three. four.</voice></speak>";

    cout << "Press Enter to start synthesizing." << std::endl;
    std::string text;
    getline(cin, text);
    const auto result = synthesizer->SpeakSsmlAsync(ssml).get();

    // Checks result.
    if (result->Reason == ResultReason::SynthesizingAudioCompleted)
    {
        cout << "Speech synthesized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << static_cast<int>(cancellation->Reason) << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << static_cast<int>(cancellation->ErrorCode) << std::endl;
            cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
}

// Speech synthesis with auto detection for source language
// Note: this is a preview feature, which might be updated in future versions.
void SpeechSynthesisWithSourceLanguageAutoDetection()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    const auto speechConfig = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Currently this feature only supports open languages range
    const auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromOpenRange();

    // Creates a speech synthesizer with auto detection for source language, using the default speaker as audio output.
    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig);

    while (true)
    {
        std::string frText = "Bonjour le monde.";
        std::string enText = "Hello world.";
        auto text = frText + " " + enText;
        cout << "Trying to synthesize [" << text << "], you will hear [" << frText << "] spoken in a French voice and ["
            << enText << "] in an English voice." << endl;
        const auto result = synthesizer->SpeakTextAsync(text).get();

        // Checks result.
        if (result->Reason == ResultReason::SynthesizingAudioCompleted)
        {
            cout << "Speech synthesized to speaker for text [" << text << "] with auto detection for source language." << std::endl;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
            cout << "CANCELED: Reason=" << static_cast<int>(cancellation->Reason) << std::endl;

            if (cancellation->Reason == CancellationReason::Error)
            {
                cout << "CANCELED: ErrorCode=" << static_cast<int>(cancellation->ErrorCode) << std::endl;
                cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
                cout << "CANCELED: Did you update the subscription info?" << std::endl;
            }
        }
    }
}

// Speech synthesis get available voices
void SpeechSynthesisGetAvailableVoices()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    const auto speechConfig = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Creates a speech synthesizer.
    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, nullptr);

    cout << "Enter a locale in BCP-47 format (e.g. en-US) that you want to get the voices of, or enter empty to get voices in all locales." << std::endl;
    cout << "> ";
    std::string text;
    getline(cin, text);

    const auto result = synthesizer->GetVoicesAsync(text).get();

    // Checks result.
    if (result->Reason == ResultReason::VoicesListRetrieved)
    {
        cout << "Voices successfully retrieved, they are:" << std::endl;
        for (const auto& voice : result->Voices)
        {
            cout << voice->Name << endl;
        }
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        cout << "CANCELED: ErrorDetails=[" << result->ErrorDetails << "]" << std::endl;
        cout << "CANCELED: Did you update the subscription info?" << std::endl;
    }
}
