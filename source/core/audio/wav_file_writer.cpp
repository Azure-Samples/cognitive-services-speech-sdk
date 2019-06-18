//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_writer.cpp: Implementation definitions for CSpxWavFileWriter C++ class
//

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <cstring>
#include "platform.h"
#include "file_utils.h"
#include "wav_file_writer.h"


#define BUFFERWRITE(buf, value) *(decltype(value)*)(buf) = value; (buf) += sizeof(value);

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxWavFileWriter::CSpxWavFileWriter()
{
    m_hasHeader = true;
}

CSpxWavFileWriter::~CSpxWavFileWriter()
{
    Close(); // WavFile_Type may close via it's dtor; force close now for proper telemetry/tracing from ::Close method
}

void CSpxWavFileWriter::Open(const wchar_t* fileName)
{
    m_fileName = fileName;

    SPX_DBG_TRACE_VERBOSE("Opening WAV file '%ls'", fileName);

    auto file = std::make_unique<std::fstream>();
    PAL::OpenStream(*file.get(), fileName, false);

    SPX_IFTRUE_THROW_HR(!file->good(), SPXERR_FILE_OPEN_FAILED);

    m_file = std::move(file);
}

void CSpxWavFileWriter::Close()
{
    SPX_DBG_TRACE_VERBOSE("Closing WAV file");

    if (m_file.get() != nullptr)
    {
        m_file->close();
        m_file.reset();
    }

    m_fileName.clear();
    m_format = nullptr;
}

bool CSpxWavFileWriter::IsOpen() const
{
    return m_file.get() != nullptr;
}

void CSpxWavFileWriter::SetContinuousLoop(bool value)
{
    UNUSED(value);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
}

void CSpxWavFileWriter::SetIterativeLoop(bool value)
{
    UNUSED(value);
    SPX_THROW_HR(SPXERR_NOT_IMPL);
}

uint32_t CSpxWavFileWriter::Write(uint8_t* buffer, uint32_t size)
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_format.get() == nullptr, SPXERR_UNINITIALIZED);

    EnsureRiffHeader(); // Make sure the wave header is written to the file

    m_file->seekg(0, WavFile_Type::end); // Seek to end of file
    m_file->write((const char *)buffer, size); // Write the audio data

    m_nWrittenBytes += size;

    UpdateWaveBodySize(m_nWrittenBytes); // Update wave header with the body size

    if (m_simulateRealtimePercentage > 0)
    {
        auto milliseconds = size * 1000 * m_simulateRealtimePercentage / m_format->nAvgBytesPerSec / 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    return size;
}

void CSpxWavFileWriter::WaitUntilDone()
{
    m_file->flush();
}

void CSpxWavFileWriter::EnsureRiffHeader()
{
    if (m_hasHeader && !m_bHeaderIsWritten)
    {
        WriteRiffHeader(0, 0);
        m_bHeaderIsWritten = true;
    }
}

void CSpxWavFileWriter::WriteRiffHeader(uint32_t cData, uint32_t cEventData)
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);

    RIFFHDR riff(0);
    BLOCKHDR block(0);
    DATAHDR dataHdr(0);

    uint32_t cRiff = sizeof(riff);
    uint32_t cBlock = sizeof(block);
    uint32_t cWaveEx = 18 + m_format->cbSize; // use 18 for actual size to avoid compiler alignment difference.
    uint32_t cDataHdr = sizeof(dataHdr);

    uint32_t total = cRiff + cBlock + cWaveEx + cDataHdr;
    if (m_format->wFormatTag == WAVE_FORMAT_SIREN)
    {
        total += 12;
    }

    if (cEventData > 0)
    {
        total += (8 + cEventData);
    }

    uint8_t tmpBuf[128];
    uint8_t* p = tmpBuf;
    // Write the RIFF section
    riff._len = total + cData - 8/* - cRiff*/; // for the "WAVE" 4 characters
    BUFFERWRITE(p, riff._id);
    BUFFERWRITE(p, riff._len);
    BUFFERWRITE(p, riff._type);

    // Write the wave header section
    block._len = cWaveEx;
    BUFFERWRITE(p, block._id);
    BUFFERWRITE(p, block._len);

    // Write the FormatEx structure
    BUFFERWRITE(p, m_format->wFormatTag);
    BUFFERWRITE(p, m_format->nChannels);
    BUFFERWRITE(p, m_format->nSamplesPerSec);
    BUFFERWRITE(p, m_format->nAvgBytesPerSec);
    BUFFERWRITE(p, m_format->nBlockAlign);
    BUFFERWRITE(p, m_format->wBitsPerSample);
    BUFFERWRITE(p, m_format->cbSize);

    if (m_format->wFormatTag == WAVE_FORMAT_SIREN)
    {
        BUFFERWRITE(p, (uint16_t)320);
        BUFFERWRITE(p, 'f');
        BUFFERWRITE(p, 'a');
        BUFFERWRITE(p, 'c');
        BUFFERWRITE(p, 't');
        BUFFERWRITE(p, (uint32_t)4);
        uint32_t factSize = (cData * 320) / m_format->nBlockAlign;
        BUFFERWRITE(p, factSize);
    }

    // Write the data section
    dataHdr._len = cData;
    BUFFERWRITE(p, dataHdr._id);
    BUFFERWRITE(p, dataHdr._len);

    m_file->seekg(0, WavFile_Type::beg);
    m_file->write((const char *)tmpBuf, p - tmpBuf);
}

void CSpxWavFileWriter::UpdateWaveBodySize(uint32_t size)
{
    SPX_IFTRUE_THROW_HR(!IsOpen(), SPXERR_UNINITIALIZED);

    if (m_hasHeader)
    {
        WriteRiffHeader(size, 0);
        m_file->seekg(0, WavFile_Type::end);
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
