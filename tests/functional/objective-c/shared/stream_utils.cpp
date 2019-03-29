//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stream_utils.hpp"
#include <iostream>
#include <fstream>

constexpr uint16_t tagBufferSize = 4;
constexpr uint16_t chunkTypeBufferSize = 4;
constexpr uint16_t chunkSizeBufferSize = 4;

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

void ReadChunkTypeAndSize(std::fstream& fs, char* chunkType, uint32_t* chunkSize)
{
    // Read the chunk type
    fs.read(chunkType, chunkTypeBufferSize);
    
    // Read the chunk size
    uint8_t chunkSizeBuffer[chunkSizeBufferSize];
    fs.read((char*)chunkSizeBuffer, chunkSizeBufferSize);
    
    // chunk size is little endian
    *chunkSize = ((uint32_t)chunkSizeBuffer[3] << 24) |
    ((uint32_t)chunkSizeBuffer[2] << 16) |
    ((uint32_t)chunkSizeBuffer[1] << 8) |
    (uint32_t)chunkSizeBuffer[0];
}

// Get format data from a wav file.
void GetFormatFromWavFile(std::fstream& fs)
{
    char tag[tagBufferSize];
    char chunkType[chunkTypeBufferSize];
    char chunkSizeBuffer[chunkSizeBufferSize];
    uint32_t chunkSize = 0;
    
    // Set to throw exceptions when reading file header.
    fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Checks the RIFF tag
        fs.read(tag, tagBufferSize);
        if (memcmp(tag, "RIFF", tagBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'RIFF' is expected.");
        
        // The next is the RIFF chunk size, ignore now.
        fs.read(chunkSizeBuffer, chunkSizeBufferSize);
        
        // Checks the 'WAVE' tag in the wave header.
        fs.read(chunkType, chunkTypeBufferSize);
        if (std::memcmp(chunkType, "WAVE", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'WAVE' is expected.");
        
        // The next chunk must be the 'fmt ' chunk.
        ReadChunkTypeAndSize(fs, chunkType, &chunkSize);
        if (std::memcmp(chunkType, "fmt ", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'fmt ' is expected.");
        
        // Reads format data.
        fs.read((char *)&m_formatHeader, sizeof(m_formatHeader));
        
        // Skips the rest of format data.
        if (chunkSize > sizeof(m_formatHeader))
            fs.seekg(chunkSize - sizeof(m_formatHeader), std::ios_base::cur);
        
        // The next must be the 'data' chunk.
        ReadChunkTypeAndSize(fs, chunkType, &chunkSize);
        if (std::memcmp(chunkType, "data", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Currently the 'data' chunk must directly follow the fmt chunk.");
        if (fs.eof() && chunkSize > 0)
            throw std::runtime_error("Unexpected end of file, before any audio data can be read.");
    }
    catch (std::ifstream::failure e) {
        throw std::runtime_error("Unexpected end of file or error when reading audio file.");
    }
    // Set to not throw exceptions when starting to read audio data, since istream::read() throws exception if the data read is less than required.
    // Instead, in AudioInputStream::Read(), we manually check whether there is an error or not.
    fs.exceptions(std::ifstream::goodbit);
}

