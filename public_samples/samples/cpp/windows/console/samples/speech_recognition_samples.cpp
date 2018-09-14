//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>
#include <fstream>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
// </toplevel>

// Speech recognition using microphone.
void SpeechRecognitionWithMicrophone()
{
    // <SpeechRecognitionWithMicrophone>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = SpeechRecognizer::FromConfig(config);
    cout << "Say something...\n";

    // Performs recognition. RecognizeOnceAsync() returns when the first utterance has been recognized,
    // so it is suitable only for single shot recognition like command or query. For long-running
    // recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </SpeechRecognitionWithMicrophone>
}


// Speech recognition in the specified language, using microphone, and requesting detailed output format.
void SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat()
{
    // <SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer in the specified language using microphone as audio input.
    // Replace the language with your language in BCP-47 format, e.g. en-US.
    auto lang = "de-DE";
    config->SetSpeechRecognitionLanguage(lang);
    // Request detailed output format.
    config->SetOutputFormat(OutputFormat::Detailed);

    auto recognizer = SpeechRecognizer::FromConfig(config);
    cout << "Say something in " << lang << "...\n";

    // Performs recognition. RecognizeOnceAsync() returns when the first utterance has been recognized,
    // so it is suitable only for single shot recognition like command or query. For long-running
    // recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl
             << "  Speech Service JSON: " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult)
             << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
}

void SpeechContinuousRecognitionWithFile()
{
    // <SpeechContinuousRecognitionWithFile>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl
                 << "  Offset=" << e.Result->Offset() << std::endl
                 << "  Duration=" << e.Result->Duration() << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
    {
        cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

        if (e.Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }

        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped.";
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Waits for recognition end.
    recognitionEnd.get_future().wait();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();
    // </SpeechContinuousRecognitionWithFile>
}

// Speech recognition using a customized model.
void SpeechRecognitionUsingCustomizedModel()
{
    // <SpeechRecognitionUsingCustomizedModel>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    // Set the endpoint ID of your customized model
    // Replace with your own CRIS endpoint ID.
    config->SetEndpointId("YourEndpointId");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = SpeechRecognizer::FromConfig(config);

    cout << "Say something...\n";

    // Performs recognition. RecognizeOnceAsync() returns when the first utterance has been recognized,
    // so it is suitable only for single shot recognition like command or query. For long-running
    // recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        cout << "RECOGNIZED: Text=" << result->Text << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
    // </SpeechRecognitionUsingCustomizedModel>
}



void SpeechContinuousRecognitionWithStream()
{
    // <SpeechContinuousRecognitionWithStream>
    // First, define your own pull audio input stream callback class that implements the
    // PullAudioInputStreamCallback interface. The sample here illustrates how to define such
    // a callback that reads audio data from a wav file.

    // Defines common constants for WAV format.
    constexpr uint16_t tagBufferSize = 4;
    constexpr uint16_t chunkTypeBufferSize = 4;
    constexpr uint16_t chunkSizeBufferSize = 4;

    // AudioInputFromFileCallback implements PullAudioInputStreamCallback interface, and uses a wav file as source
    class AudioInputFromFileCallback final : public PullAudioInputStreamCallback
    {
    public: 
        // Constructor that creates an input stream from a file.
        AudioInputFromFileCallback(const string& audioFileName)
        {
            if (audioFileName.empty())
                throw invalid_argument("Audio filename is empty");

            ios_base::openmode mode = ios_base::binary | ios_base::in;
            m_fs.open(audioFileName, mode);
            if (!m_fs.good())
                throw invalid_argument("Failed to open the specified audio file.");

            // Get audio format from the file header.
            GetFormatFromWavFile();
        }

        // Destructor.
        ~AudioInputFromFileCallback()
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
            if (m_fs.eof())
                // returns 0 to indicate that the stream reaches end.
                return 0;
            m_fs.read((char*)dataBuffer, size);
            if (!m_fs.eof() && !m_fs.good())
                // returns 0 to close the stream on read error.
                return 0;
            else
                // returns the number of bytes that have been read.
                return (int)m_fs.gcount();
        }

        // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
        void Close() override
        {
            m_fs.close();
        }

    private:
        // Get format data from a wav file.
        void GetFormatFromWavFile()
        {
            char tag[tagBufferSize];
            char chunkType[chunkTypeBufferSize];
            char chunkSizeBuffer[chunkSizeBufferSize];
            uint32_t chunkSize = 0;

            // Set to throw exceptions when reading file header.
            m_fs.exceptions(ifstream::failbit | ifstream::badbit);

            try {
                // Checks the RIFF tag
                m_fs.read(tag, tagBufferSize);
                if (memcmp(tag, "RIFF", tagBufferSize) != 0)
                    throw runtime_error("Invalid file header, tag 'RIFF' is expected.");

                // The next is the RIFF chunk size, ignore now.
                m_fs.read(chunkSizeBuffer, chunkSizeBufferSize);

                // Checks the 'WAVE' tag in the wave header.
                m_fs.read(chunkType, chunkTypeBufferSize);
                if (memcmp(chunkType, "WAVE", chunkTypeBufferSize) != 0)
                    throw runtime_error("Invalid file header, tag 'WAVE' is expected.");

                // The next chunk must be the 'fmt ' chunk.
                ReadChunkTypeAndSize(chunkType, &chunkSize);
                if (memcmp(chunkType, "fmt ", chunkTypeBufferSize) != 0)
                    throw runtime_error("Invalid file header, tag 'fmt ' is expected.");

                // Reads format data.
                m_fs.read((char *)&m_formatHeader, sizeof(m_formatHeader));

                // Skips the rest of format data.
                if (chunkSize > sizeof(m_formatHeader))
                    m_fs.seekg(chunkSize - sizeof(m_formatHeader), ios_base::cur);

                // The next must be the 'data' chunk.
                ReadChunkTypeAndSize(chunkType, &chunkSize);
                if (memcmp(chunkType, "data", chunkTypeBufferSize) != 0)
                    throw runtime_error("Currently the 'data' chunk must directly follow the fmt chunk.");
                if (m_fs.eof() && chunkSize > 0)
                    throw runtime_error("Unexpected end of file, before any audio data can be read.");
            }
            catch (ifstream::failure e) {
                throw runtime_error("Unexpected end of file or error when reading audio file.");
            }
            // Set to not throw exceptions when starting to read audio data, since istream::read() throws exception if the data read is less than required.
            // Instead, in AudioInputStream::Read(), we manually check whether there is an error or not.
            m_fs.exceptions(ifstream::goodbit);
        }

        void ReadChunkTypeAndSize(char* chunkType, uint32_t* chunkSize)
        {
            // Read the chunk type
            m_fs.read(chunkType, chunkTypeBufferSize);

            // Read the chunk size
            uint8_t chunkSizeBuffer[chunkSizeBufferSize];
            m_fs.read((char*)chunkSizeBuffer, chunkSizeBufferSize);

            // chunk size is little endian
            *chunkSize = ((uint32_t)chunkSizeBuffer[3] << 24) |
                ((uint32_t)chunkSizeBuffer[2] << 16) |
                ((uint32_t)chunkSizeBuffer[1] << 8) |
                (uint32_t)chunkSizeBuffer[0];
        }

        // The format structure expected in wav files.
        struct WAVEFORMAT
        {
            uint16_t FormatTag;        // format type.
            uint16_t Channels;         // number of channels (i.e. mono, stereo...).
            uint32_t SamplesPerSec;    // sample rate.
            uint32_t AvgBytesPerSec;   // for buffer estimation.
            uint16_t BlockAlign;       // block size of data.
            uint16_t BitsPerSample;    // Number of bits per sample of mono data.
        } m_formatHeader;
        static_assert(sizeof(m_formatHeader) == 16, "unexpected size of m_formatHeader");
        fstream m_fs;
    };

    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a callback that will read audio data from a WAV file.
    // Currently, the only supported WAV format is mono(single channel), 16 kHZ sample rate, 16 bits per sample.
    // Replace with your own audio file name.
    auto callback = make_shared<AudioInputFromFileCallback>("whatstheweatherlike.wav");
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates a speech recognizer from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pullStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->Recognizing.Connect([](const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognizing:" << e.Result->Text << endl;
    });

    recognizer->Recognized.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            cout << "RECOGNIZED: Text=" << e.Result->Text << std::endl
                 << "  Offset=" << e.Result->Offset() << std::endl
                 << "  Duration=" << e.Result->Duration() << std::endl;
        }
        else if (e.Result->Reason == ResultReason::NoMatch)
        {
            cout << "NOMATCH: Speech could not be recognized." << std::endl;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
    {
        cout << "CANCELED: Reason=" << (int)e.Reason << std::endl;

        if (e.Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorDetails=" << e.ErrorDetails << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }

        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped.";
        recognitionEnd.set_value(); // Notify to stop recognition.
    });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Waits for recognition end.
    recognitionEnd.get_future().wait();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();
    // </SpeechContinuousRecognitionWithStream>
}
