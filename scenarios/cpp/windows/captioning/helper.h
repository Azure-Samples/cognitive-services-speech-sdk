//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <speechapi_cxx.h>
#include <fstream>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

// Adapted from code in:
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/cpp/windows/console/samples/speech_recognition_samples.cpp
class BinaryFileReader final : public PullAudioInputStreamCallback
{
private:

    std::fstream m_fs;

public:

    // Constructor that creates an input stream from a file.
    BinaryFileReader(const std::string& audioFileName)
    {
        if (audioFileName.empty())
        {
            throw std::invalid_argument("Audio filename is empty");
        }

        std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
        m_fs.open(audioFileName, mode);
        if (!m_fs.good())
        {
            throw std::invalid_argument("Failed to open the specified audio file.");
        }
    }

    // Implements AudioInputStream::Read() which is called to get data from the audio stream.
    // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
    // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
    // If there is no data, this function must wait until data is available.
    // It returns the number of bytes that have been copied in 'dataBuffer'.
    // It returns 0 to indicate that the stream reaches end or is closed.
    int Read(uint8_t* dataBuffer, uint32_t size)
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
    void Close()
    {
        m_fs.close();
    }
};

// Adapted from code in:
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/cpp/windows/console/samples/wav_file_reader.h
// The format structure expected in wav files.
struct WAVEFORMAT
{
    uint16_t FormatTag;        // format type.
    uint16_t Channels;         // number of channels (i.e. mono, stereo...).
    uint32_t SamplesPerSec;    // sample rate.
    uint32_t AvgBytesPerSec;   // for buffer estimation.
    uint16_t BlockAlign;       // block size of data.
    uint16_t BitsPerSample;    // Number of bits per sample of mono data.
};

class WavFileReader final
{
private:

    WAVEFORMAT m_formatHeader;
    static_assert(sizeof(m_formatHeader) == 16, "unexpected size of m_formatHeader");

    // Defines common constants for WAV format.
    static constexpr uint16_t tagBufferSize = 4;
    static constexpr uint16_t chunkTypeBufferSize = 4;
    static constexpr uint16_t chunkSizeBufferSize = 4;

    std::fstream m_fs;

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

    // Get format data from a wav file.
    void GetFormatFromWavFile()
    {
        char tag[tagBufferSize];
        char chunkType[chunkTypeBufferSize];
        char chunkSizeBuffer[chunkSizeBufferSize];
        uint32_t chunkSize = 0;

        // Set to throw exceptions when reading file header.
        m_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // Checks the RIFF tag
            m_fs.read(tag, tagBufferSize);
            if (memcmp(tag, "RIFF", tagBufferSize) != 0)
            {
                throw std::runtime_error("Invalid file header, tag 'RIFF' is expected.");
            }

            // The next is the RIFF chunk size, ignore now.
            m_fs.read(chunkSizeBuffer, chunkSizeBufferSize);

            // Checks the 'WAVE' tag in the wave header.
            m_fs.read(chunkType, chunkTypeBufferSize);
            if (memcmp(chunkType, "WAVE", chunkTypeBufferSize) != 0)
            {
                throw std::runtime_error("Invalid file header, tag 'WAVE' is expected.");
            }
            
            bool foundDataChunk = false;
            while (!foundDataChunk && m_fs.good() && !m_fs.eof())
            {
                ReadChunkTypeAndSize(chunkType, &chunkSize);
                if (memcmp(chunkType, "fmt ", chunkTypeBufferSize) == 0)
                {
                    // Reads format data.
                    m_fs.read((char *)&m_formatHeader, sizeof(m_formatHeader));

                    // Skips the rest of format data.
                    if (chunkSize > sizeof(m_formatHeader))
                    {
                        m_fs.seekg(chunkSize - sizeof(m_formatHeader), std::ios_base::cur);
                    }
                }
                else if (memcmp(chunkType, "data", chunkTypeBufferSize) == 0)
                {
                    foundDataChunk = true;
                    break;
                }
                else
                {
                    m_fs.seekg(chunkSize, std::ios_base::cur);
                }
            }

            if (!foundDataChunk)
            {
                throw std::runtime_error("Did not find data chunk.");
            }
            if (m_fs.eof() && chunkSize > 0)
            {
                throw std::runtime_error("Unexpected end of file, before any audio data can be read.");
            }
        }
        catch (std::ifstream::failure e)
        {
            throw std::runtime_error("Unexpected end of file or error when reading audio file.");
        }
        // Set to not throw exceptions when starting to read audio data
        m_fs.exceptions(std::ifstream::goodbit);
    }
    
public:

    // Constructor that creates an input stream from a file.
    WavFileReader(const std::string& audioFileName)
    {
        if (audioFileName.empty())
        {
            throw std::invalid_argument("Audio filename is empty");
        }

        std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
        m_fs.open(audioFileName, mode);
        if (!m_fs.good())
        {
            throw std::invalid_argument("Failed to open the specified audio file.");
        }
        
        // Get audio format from the file header.
        GetFormatFromWavFile();
    }

    WAVEFORMAT GetFormat()
    {
        return m_formatHeader;
    }

    void Close()
    {
        m_fs.close();
    }
};

struct UserConfig
{
    const bool useCompressedAudio = false;
    const AudioStreamContainerFormat compressedAudioFormat = AudioStreamContainerFormat::ANY;
    const ProfanityOption profanityOption = ProfanityOption::Masked;
    const std::optional<std::vector<std::string>> languageIDLanguages = std::nullopt;
    const std::optional<std::string> inputFile = std::nullopt;
    const std::optional<std::string> outputFile = std::nullopt;
    const std::optional<std::string> phraseList;
    const bool suppressConsoleOutput = false;
    const bool showRecognizingResults = false;
    const std::optional<std::string> stablePartialResultThreshold = std::nullopt;
    const bool useSubRipTextCaptionFormat = false;
    const std::string subscriptionKey;
    const std::string region;
    
    UserConfig(
        bool useCompressedAudio,
        AudioStreamContainerFormat compressedAudioFormat,
        ProfanityOption profanityOption,
        std::optional<std::vector<std::string>> languageIDLanguages,
        std::optional<std::string> inputFile,
        std::optional<std::string> outputFile,
        std::optional<std::string> phraseList,
        bool suppressConsoleOutput,
        bool showRecognizingResults,
        std::optional<std::string> stablePartialResultThreshold,
        bool useSubRipTextCaptionFormat,
        std::string subscriptionKey,
        std::string region
        ) :
        useCompressedAudio(useCompressedAudio),
        compressedAudioFormat(compressedAudioFormat),
        profanityOption(profanityOption),
        languageIDLanguages(languageIDLanguages),
        inputFile(inputFile),
        outputFile(outputFile),
        phraseList(phraseList),
        suppressConsoleOutput(suppressConsoleOutput),
        showRecognizingResults(showRecognizingResults),
        stablePartialResultThreshold(stablePartialResultThreshold),
        useSubRipTextCaptionFormat(useSubRipTextCaptionFormat),
        subscriptionKey(subscriptionKey),
        region(region)
        {}
};

struct Timestamp
{
    const int startHours;
    const int endHours;
    const int startMinutes;
    const int endMinutes;
    const float startSeconds;
    const float endSeconds;

    Timestamp(
        int startHours,
        int endHours,
        int startMinutes,
        int endMinutes,
        float startSeconds,
        float endSeconds
    ) :
    startHours(startHours),
    endHours(endHours),
    startMinutes(startMinutes),
    endMinutes(endMinutes),
    startSeconds(startSeconds),
    endSeconds(endSeconds)
    {}
};
