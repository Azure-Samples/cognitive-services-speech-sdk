//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// sequence_generator_audio_stream_reader.cpp: Implementation definitions for CSpxSequenceGeneratorAudioStreamReader C++ class
//

#include <cstring>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include <sstream>

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "test_utils.h"
#include "sequence_generator_audio_stream_reader.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxSequenceGeneratorAudioStreamReader::SetRealTimeThrottlePercentage(uint8_t percentage)
{
    m_simulateRealtimePercentage = percentage;
}

uint8_t CSpxSequenceGeneratorAudioStreamReader::GetRealTimeThrottlePercentage()
{
    return m_simulateRealtimePercentage;
}

uint16_t CSpxSequenceGeneratorAudioStreamReader::GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat)
{
    uint16_t cbFormatRequired = sizeof(SPXWAVEFORMATEX);

    if (pformat != nullptr) // Calling with GetFormat(nullptr, ???) is valid; we don't copy bits, only return sizeof block required
    {
        SPXWAVEFORMATEX format;
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = 1;
        format.nSamplesPerSec = 16000;
        format.nAvgBytesPerSec = 16000 * 2;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.cbSize = 0;

        size_t cb = std::min(cbFormat, cbFormatRequired);
        std::memcpy(pformat, &format, cb);
    }

    return cbFormatRequired;
}

uint32_t CSpxSequenceGeneratorAudioStreamReader::Read(uint8_t* pbuffer, uint32_t cbBuffer)
{
    if (m_simulateRealtimePercentage > 0)
    {
        auto nAvgBytesPerSec = 16000 * 2;
        auto milliseconds = cbBuffer * 1000 / nAvgBytesPerSec * m_simulateRealtimePercentage / 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    if (GetUseSequentialBufferedData() > 0)
    {
        return ReadSequentialBufferedData(pbuffer, cbBuffer);
    }
    else
    {
        return ReadNulldData(pbuffer, cbBuffer);
    }
}

void CSpxSequenceGeneratorAudioStreamReader::UseSequentialBufferedData(uint32_t bufferSize)
{
    if (bufferSize > 0)
    {
        m_buffer.reset(new uint8_t[bufferSize]);
    }
    else
    {
        m_buffer.reset();
    }

    m_bufferSize = bufferSize;
}

uint32_t CSpxSequenceGeneratorAudioStreamReader::GetUseSequentialBufferedData()
{
    return m_bufferSize;
}

uint32_t CSpxSequenceGeneratorAudioStreamReader::ReadSequentialBufferedData(uint8_t* pbuffer, uint32_t cbBuffer)
{
    auto requestedSize = cbBuffer;
    auto destBuffer = pbuffer;

    while (requestedSize != 0)
    {
        if (m_bufferSizeRemained == 0)
        {
            for (uint32_t i = 0; i < m_bufferSize; i++)
            {
                m_seqDataValue = (m_seqDataValue + 1) % 256;
                m_buffer[i] = (uint8_t)m_seqDataValue;
            }
            m_bufferSizeRemained = m_bufferSize;
        }

        auto sourceOffset = m_bufferSize - m_bufferSizeRemained;
        auto copiedDataSize = std::min(requestedSize, m_bufferSizeRemained);
        std::memcpy(destBuffer, m_buffer.get() + sourceOffset, copiedDataSize);

        m_bufferSizeRemained -= copiedDataSize;
        requestedSize -= copiedDataSize;
        destBuffer += copiedDataSize;
    }

    return cbBuffer;
}

uint32_t CSpxSequenceGeneratorAudioStreamReader::ReadNulldData(uint8_t* pbuffer, uint32_t cbBuffer)
{
    memset(pbuffer, 0, cbBuffer);
    return cbBuffer;
}

void CSpxSequenceGeneratorAudioStreamReader::Close()
{
    if (m_initialized)
    {
        m_initialized = false;
    }
}

void CSpxSequenceGeneratorAudioStreamReader::Init()
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_initialized);

    if (m_bufferSize > 0)
    {
        m_buffer.reset(new uint8_t[m_bufferSize]);
    }
    else
    {
        m_buffer.reset();
    }

    m_initialized = true;
}

void CSpxSequenceGeneratorAudioStreamReader::Term()
{
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
