//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_reader.cpp: Implementation definitions for CSpxWavFileReader C++ class
//

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <cstring>
#include "platform.h"
#include "file_utils.h"
#include "wav_file_reader.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxWavFileReader::CSpxWavFileReader() :
    m_firstSeekDataChunkPos(0),
    m_dataChunkBytesLeft(0)
{
}

CSpxWavFileReader::~CSpxWavFileReader()
{
    Close(); // WavFile_Type may close via it's dtor; force close now for proper telemetry/tracing from ::Close method
}

void CSpxWavFileReader::Open(const wchar_t* fileName)
{
    m_fileName = fileName;

    SPX_DBG_TRACE_VERBOSE("Opening WAV file '%ls'", fileName);

    auto file = std::make_unique<WavFile_Type>();
    PAL::OpenStream(*file.get(), fileName, true);

    SPX_IFTRUE_THROW_HR(!file->good(), SPXERR_FILE_OPEN_FAILED);
    SPX_IFTRUE_THROW_HR(file->eof(), SPXERR_UNEXPECTED_EOF);

    m_file = std::move(file);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto hasProperty = properties->GetStringValue("CARBON-INTERNAL-MOCK-WaveFileRealTimeAudioPercentage", "0");
    m_simulateRealtimePercentage = static_cast<uint8_t>(stoi(hasProperty));
}

void CSpxWavFileReader::Close()
{
    SPX_DBG_TRACE_VERBOSE("Closing WAV file");

    if (m_file.get() != nullptr)
    {
        m_file->close();
        m_file.reset();
    }

    m_fileName.clear();
    m_waveformat = nullptr;
}

bool CSpxWavFileReader::IsOpen() const
{
    return m_file.get() != nullptr;
}

void CSpxWavFileReader::SetContinuousLoop(bool value)
{
    m_continuousAudioLoop = value;
}

void CSpxWavFileReader::SetIterativeLoop(bool value)
{
    m_iterativeAudioLoop = value;
}

uint16_t CSpxWavFileReader::GetFormat(SPXWAVEFORMATEX* format, uint16_t cbFormat)
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);

    EnsureGetFormat();
    SPX_DBG_TRACE_ERROR_IF(m_waveformat.get() == nullptr, "IsOpen() returned true; EnsureGetFormat() didn't throw; we should have a SPXWAVEFORMAT now...");
    SPX_IFTRUE_THROW_HR(m_waveformat.get() == nullptr, SPXERR_UNSUPPORTED_FORMAT);

    uint16_t cbFormatRequired = sizeof(SPXWAVEFORMATEX) + m_waveformat->cbSize;

    if (format != nullptr) // Calling with GetFormat(nullptr, ???) is valid; we don't copy bits, only return sizeof block required
    {
        size_t cb = std::min(cbFormat, cbFormatRequired);
        std::memcpy(format, m_waveformat.get(), cb);
    }

    return cbFormatRequired;
}

uint32_t CSpxWavFileReader::Read(uint8_t* pbuffer, uint32_t cbBuffer)
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);

    EnsureGetFormat();
    SPX_DBG_ASSERT_WITH_MESSAGE(m_waveformat.get() != nullptr, "IsOpen() returned true; EnsureGetFormat() didn't throw; we should have a SPXWAVEFORMAT now...");

    uint32_t cbRead = 0;
    while (cbBuffer > 0 && !m_file->eof())
    {
        EnsureDataChunk();
        cbRead += ReadFromDataChunk(&pbuffer, &cbBuffer);
    }

    if (cbBuffer > 0 && cbRead == 0 && m_iterativeAudioLoop)
    {
        SPX_DBG_TRACE_VERBOSE("ITERATIVE AUDIO LOOP: Auto-rewinding...");
        m_file->clear();
        m_file->seekg(m_firstSeekDataChunkPos, WavFile_Type::beg);
    }

    if (m_simulateRealtimePercentage > 0)
    {
        auto milliseconds = cbRead * 1000 / m_waveformat->nAvgBytesPerSec * m_simulateRealtimePercentage / 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    return cbRead;
}

void CSpxWavFileReader::EnsureGetFormat()
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);

    if (m_waveformat.get() == nullptr)
    {
        FindFormatAndDataChunks();
    }
}

void CSpxWavFileReader::FindFormatAndDataChunks()
{
    SPX_IFTRUE_THROW_HR(m_waveformat.get() != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);
    
    uint8_t tag[cbTag];
    uint8_t chunkType[cbChunkType];
    uint8_t chunkSizeBuffer[cbChunkSize];
    uint32_t chunkSize = 0;

    // RIFF tag MUST be 'RIFF'
    SPX_IFTRUE_THROW_HR(!m_file->read((char*)tag, cbTag), SPXERR_UNEXPECTED_EOF);
    SPX_IFTRUE_THROW_HR(m_file->eof(), SPXERR_UNEXPECTED_EOF);
    SPX_IFTRUE_THROW_HR(0 != std::memcmp(tag, "RIFF", 4), SPXERR_INVALID_HEADER);

    // RIFF chunk size comes next
    SPX_IFTRUE_THROW_HR(!m_file->read((char*)chunkSizeBuffer, cbChunkSize), SPXERR_UNEXPECTED_EOF);
    SPX_IFTRUE_THROW_HR(m_file->eof(), SPXERR_UNEXPECTED_EOF);

    // Format chunk MUST be 'WAVE'
    SPX_IFTRUE_THROW_HR(!m_file->read((char*)chunkType, cbChunkType), SPXERR_UNEXPECTED_EOF);
    SPX_IFTRUE_THROW_HR(m_file->eof(), SPXERR_UNEXPECTED_EOF);
    SPX_IFTRUE_THROW_HR(0 != std::memcmp(chunkType, "WAVE", 4), SPXERR_INVALID_HEADER);

    // Initialize the first data chunk seek position to zero
    m_firstSeekDataChunkPos = 0;
    
    // Read chunks until we've read the SPXWAVEFORMAT and found the 'data' chunk position
    while ((m_waveformat.get() == nullptr || m_firstSeekDataChunkPos == 0) && ReadChunkTypeAndSize(chunkType, &chunkSize))
    {
        if (0 == std::memcmp(chunkType, "fmt ", cbChunkType)) // Is this the format chunk?
        {
            ReadFormatChunk(chunkSize);
        }
        else if (0 == std::memcmp(chunkType, "data", cbChunkType)) // Is this a 'data' chunk?
        {
            m_firstSeekDataChunkPos = m_file->tellg();
            m_firstSeekDataChunkPos -= sizeof(chunkType) + sizeof(chunkSizeBuffer); // account for bytes read w/ReadChunkTypeAndSize()
        }
        else // We don't care about this chunk; let's ignore it and move to the next...
        {
            m_file->seekg(chunkSize, WavFile_Type::cur);
        }
    }

    // Did we find everything we needed? 
    SPX_IFTRUE_THROW_HR(m_waveformat.get() == nullptr || m_firstSeekDataChunkPos == 0, SPXERR_UNEXPECTED_EOF);

    // Finally, move back to the very beginning of the 'data' chunk...
    m_file->seekg(m_firstSeekDataChunkPos, WavFile_Type::beg);
}

bool CSpxWavFileReader::ReadChunkTypeAndSize(uint8_t* pchunkType, uint32_t* pchunkSize)
{
    bool fSuccess = false;

    if (m_file->read((char*)pchunkType, cbChunkType) && !m_file->eof())
    {
        // Read the chunk type
        SPX_IFTRUE_THROW_HR(m_file->gcount() < cbChunkType, SPXERR_UNEXPECTED_EOF);
        SPX_IFTRUE_THROW_HR(m_file->eof(), SPXERR_UNEXPECTED_EOF);

        // Read the chunk size
        uint8_t chunkSizeBuffer[cbChunkSize];
        SPX_IFTRUE_THROW_HR(!m_file->read((char*)chunkSizeBuffer, cbChunkSize), SPXERR_UNEXPECTED_EOF);
        SPX_IFTRUE_THROW_HR(m_file->eof(), SPXERR_UNEXPECTED_EOF);

        // chunk size is little endian
        *pchunkSize = ((uint32_t)chunkSizeBuffer[3] << 24) | 
                      ((uint32_t)chunkSizeBuffer[2] << 16) | 
                      ((uint32_t)chunkSizeBuffer[1] <<  8) |
                       (uint32_t)chunkSizeBuffer[0];

        fSuccess = true; // we're done!
    }

    return fSuccess;
}

void CSpxWavFileReader::ReadFormatChunk(uint32_t chunkSize)
{
    SPX_IFTRUE_THROW_HR(chunkSize < sizeof(SPXWAVEFORMATEX) && chunkSize != sizeof(SPXWAVEFORMAT), SPXERR_INVALID_HEADER);
    
    auto cbAllocate = std::max((size_t)chunkSize, sizeof(SPXWAVEFORMATEX)); // allocate space for EX structure, no matter what
    auto waveformat = SpxAllocWAVEFORMATEX(cbAllocate);
    waveformat->cbSize = 0;
    
    // Read the SPXWAVEFORMAT/SPXWAVEFORMATEX
    SPX_IFTRUE_THROW_HR(!m_file->read((char*)waveformat.get(), chunkSize), SPXERR_UNEXPECTED_EOF);
    SPX_DBG_TRACE_VERBOSE_IF(m_file->eof(), "It's very uncommon, but possible, to hit EOF after reading SPXWAVEFORMAT/SPXWAVEFORMATEX");

    // Finally, store the format
    m_waveformat = waveformat;
}

void CSpxWavFileReader::EnsureDataChunk()
{
    uint8_t chunkType[cbChunkType];
    uint32_t chunkSize = 0;

    while (!m_file->eof() && m_dataChunkBytesLeft == 0)
    {
        if (ReadChunkTypeAndSize(chunkType, &chunkSize))
        {
            if (0 == std::memcmp(chunkType, "data", cbChunkSize))
            {
                m_dataChunkBytesLeft = chunkSize;
            }
            else
            {
                m_file->seekg(chunkSize, WavFile_Type::cur);
            }
        }
        else if (m_file->eof() && m_continuousAudioLoop)
        {
            SPX_DBG_TRACE_VERBOSE("CONTINUOUS AUDIO LOOP: Auto-rewinding...");
            m_file->clear();
            m_file->seekg(m_firstSeekDataChunkPos, WavFile_Type::beg);
        }
    }
}

uint32_t CSpxWavFileReader::ReadFromDataChunk(uint8_t** ppbuffer, uint32_t* pcbBuffer)
{
    auto cbRead = std::min(*pcbBuffer, m_dataChunkBytesLeft);
    SPX_IFTRUE_THROW_HR(cbRead > 0 && !m_file->read((char*)*ppbuffer, cbRead), SPXERR_UNEXPECTED_EOF);

    *ppbuffer += cbRead; // move the buffer forward
    *pcbBuffer -= cbRead; // reduce the count of bytes left to read

    m_dataChunkBytesLeft -= cbRead;

    return cbRead;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
