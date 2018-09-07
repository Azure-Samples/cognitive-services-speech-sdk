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
// </toplevel>

// Speech recognition using microphone.
void SpeechRecognitionWithMicrophone()
{
    // <SpeechRecognitionWithMicrophone>
    // Creates an instance of a speech factory with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = factory->CreateSpeechRecognizer();
    cout << "Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        cout << "Recognition Status: " << int(result->Reason) << ". ";
        if (result->Reason == Reason::Canceled)
        {
            cout << "There was an error, reason: " << result->ErrorDetails << endl;
        }
        else
        {
            cout << "No speech could be recognized.\n";
        }
    }
    else
    {
        cout << "We recognized: " << result->Text
             << " starting at " << result->Offset() << "(ticks)"
             << ", with duration of " << result->Duration() << "(ticks)"
             << endl;
    }
    // </SpeechRecognitionWithMicrophone>
}


// Speech recognition in the specified language, using microphone, and requesting detailed output format.
void SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat()
{
    // <SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
    // Creates an instance of a speech factory with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer in the specified language using microphone as audio input.
    // Replace the language with your language in BCP-47 format, e.g. en-US.
    auto lang = "de-DE";
    // Requests detailed output format.
    auto recognizer = factory->CreateSpeechRecognizer(lang, OutputFormat::Detailed);
    cout << "Say something in " << lang << "...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        cout << "Recognition Status:" << int(result->Reason);
        if (result->Reason == Reason::Canceled)
        {
            cout << "There was an error, reason: " << result->ErrorDetails << endl;
        }
        else
        {
            cout << "No speech could be recognized.\n";
        }
    }
    else
    {
        cout << L"We recognized: " << result->Text << endl
              << L"Detailed output result in JSON: " << result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_Json)
              << endl;
    }
    // </SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
}

void SpeechContinuousRecognitionWithFile()
{
    // <SpeechContinuousRecognitionWithFile>
    // Creates an instance of a speech factory with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto recognizer = factory->CreateSpeechRecognizerWithFileInput("whatstheweatherlike.wav");

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->IntermediateResult.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "IntermediateResult:" << e.Result.Text << endl;
    });

    recognizer->FinalResult.Connect([] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognition Status:" << int(e.Result.Reason) << endl;
        switch (e.Result.Reason)
        {
        case Reason::Recognized:
            cout << "We recognized: " << e.Result.Text
                << " Offset: " << e.Result.Offset() << ". Duration: " << e.Result.Duration()
                << endl;
            break;
        case Reason::InitialSilenceTimeout:
            cout << "The start of the audio stream contains only silence, and the service timed out waiting for speech.\n";
            break;
        case Reason::InitialBabbleTimeout:
            cout << "The start of the audio stream contains only noise, and the service timed out waiting for speech.\n";
            break;
        case Reason::NoMatch:
            cout << "Speech was detected in the audio stream, but no words from the target language were matched. "
                << "Possible reasons could be wrong setting of the target language or wrong format of audio stream.\n";
            break;
        case Reason::Canceled:
            cout << "There was an error, reason: " << e.Result.ErrorDetails << endl;
            break;
        default:
            cout << "Recognition Status:" << int(e.Result.Reason);
            break;
        }
    });

    recognizer->Canceled.Connect( [&recognitionEnd] (const SpeechRecognitionEventArgs& e)
    {
        cout << "Canceled:" << (int)e.Result.Reason << "- " << e.Result.ErrorDetails << endl;
        // Notify to stop recognition.
        recognitionEnd.set_value();
    });

    recognizer->SessionStopped.Connect( [&recognitionEnd] (const SessionEventArgs& e)
    {
        cout << "Session stopped.";
        // Notify to stop recognition.
        recognitionEnd.set_value();
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
    // Creates an instance of a speech factory with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Set the deployment id of your customized model
    // Replace with your own CRIS deployment id.
    recognizer->SetDeploymentId("YourDeploymentId");
    cout << "Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        cout << "Recognition Status:" << int(result->Reason);
        if (result->Reason == Reason::Canceled)
        {
            cout << "There was an error, reason: " << result->ErrorDetails << endl;
        }
    }
    else
    {
        cout << "We recognized: " << result->Text << endl;
    }
    // </SpeechRecognitionUsingCustomizedModel>
}



void SpeechContinuousRecognitionWithStream()
{
    // <SpeechContinuousRecognitionWithStream>
    // First, defines your own audio input stream class that implements AudioInputStream interface.
    // The sample here illustrates how to define such an audio input stream that reads from a wav file.

    // Defines common constants for WAV format.
    constexpr uint16_t tagBufferSize = 4;
    constexpr uint16_t chunkTypeBufferSize = 4;
    constexpr uint16_t chunkSizeBufferSize = 4;

    using AudioFormat = ::Microsoft::CognitiveServices::Speech::AudioInputStreamFormat;

    // AudioInputStreamFromFile implements AudioInputStream interface, and uses a wav file as stream source
    class AudioInputStreamFromFile final : public AudioInputStream
    {
    public: 
        // Constructor that creates an input stream from a file.
        AudioInputStreamFromFile(const string& audioFileName)
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
        ~AudioInputStreamFromFile()
        {
        }

        // Implements AudioInputStream::GetFormat() which is called to get the format of the audio stream.
        // The parameter formatBuffer is passed by the caller, and used to return the audio input format data, and cbFormat is the buffer size.
        // If formatBuffer is null, then only the required format buffer size is returned, but no data is written into the buffer.
        // Otherwise, it returns the size in bytes of the format buffer that has been written, and the formatBuffer contains the format data.
        size_t GetFormat(AudioFormat* formatBuffer, size_t cbFormat) override
        {
            if (formatBuffer == nullptr)
            {
                return sizeof(AudioFormat);
            }
            else
            {
                if (cbFormat < sizeof(AudioFormat))
                    throw invalid_argument("cbFormat is too small to hold AudioInputStreamFormat");

                formatBuffer->FormatTag = m_formatHeader.FormatTag;
                formatBuffer->Channels = m_formatHeader.Channels;
                formatBuffer->SamplesPerSec = m_formatHeader.SamplesPerSec;
                formatBuffer->AvgBytesPerSec = m_formatHeader.AvgBytesPerSec;
                formatBuffer->BlockAlign = m_formatHeader.BlockAlign;
                formatBuffer->BitsPerSample = m_formatHeader.BitsPerSample;
                return sizeof(AudioFormat);
            }
        }

        // Implements AudioInputStream::Read() which is called to get data from the audio stream.
        // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
        // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
        // If there is no data, this function must wait until data is available.
        // It returns the number of bytes that have been copied in 'dataBuffer'.
        // It returns 0 to indicate that the stream reaches end or is closed.
        size_t Read(char* dataBuffer, size_t size) override
        {
            if (m_fs.eof())
                // returns 0 to indicate that the stream reaches end.
                return 0;
            m_fs.read(dataBuffer, size);
            if (!m_fs.eof() && !m_fs.good())
                // returns 0 to close the stream on read error.
                return 0;
            else
                // returns the number of bytes that have been read.
                return m_fs.gcount();
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

    // Creates an instance of a speech factory with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates an input stream from file.
    // Currently, the only supported WAV format is mono(single channel), 16 kHZ sample rate, 16 bits per sample.
    // Replace with your own audio file name.
    auto inputStream = make_shared<AudioInputStreamFromFile>("whatstheweatherlike.wav");
    
    // Creates a speech recognizer from stream input;
    auto recognizer = factory->CreateSpeechRecognizerWithStream(inputStream);

    // promise for synchronization of recognition end.
    promise<void> recognitionEnd;

    // Subscribes to events.
    recognizer->IntermediateResult.Connect([](const SpeechRecognitionEventArgs& e)
    {
        cout << "IntermediateResult:" << e.Result.Text << endl;
    });

    recognizer->FinalResult.Connect([](const SpeechRecognitionEventArgs& e)
    {
        cout << "Recognition Status:" << int(e.Result.Reason) << endl;
        switch (e.Result.Reason)
        {
        case Reason::Recognized:
            cout << "We recognized: " << e.Result.Text
                << " Offset: " << e.Result.Offset() << ". Duration: " << e.Result.Duration()
                << endl;
            break;
        case Reason::InitialSilenceTimeout:
            cout << "The start of the audio stream contains only silence, and the service timed out waiting for speech.\n";
            break;
        case Reason::InitialBabbleTimeout:
            cout << "The start of the audio stream contains only noise, and the service timed out waiting for speech.\n";
            break;
        case Reason::NoMatch:
            cout << "Speech was detected in the audio stream, but no words from the target language were matched. "
                << "Possible reasons could be wrong setting of the target language or wrong format of audio stream.\n";
            break;
        case Reason::Canceled:
            cout << "There was an error, reason: " << e.Result.ErrorDetails << endl;
            break;
        default:
            cout << "Recognition Status:" << int(e.Result.Reason);
            break;
        }
    });

    recognizer->Canceled.Connect([&recognitionEnd](const SpeechRecognitionEventArgs& e)
    {
        cout << "Canceled:" << (int)e.Result.Reason << "- " << e.Result.ErrorDetails << endl;
        // Notify to stop recognition.
        recognitionEnd.set_value();
    });

    recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
    {
        cout << "Session stopped.";
        // Notify to stop recognition.
        recognitionEnd.set_value();
    });

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    // Waits for recognition end.
    recognitionEnd.get_future().wait();

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();
    // </SpeechContinuousRecognitionWithStream>
}
