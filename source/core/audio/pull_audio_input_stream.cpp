//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// pull_audio_input_stream.cpp: Implementation definitions for CSpxPullAudioInputStream C++ class
//

#include "stdafx.h"
#include "pull_audio_input_stream.h"
#include <chrono>
#include <cstring>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxPullAudioInputStream::CSpxPullAudioInputStream()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxPullAudioInputStream::~CSpxPullAudioInputStream()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxPullAudioInputStream::SetFormat(SPXWAVEFORMATEX* format)
{
    SPX_IFTRUE_THROW_HR(m_format != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Allocate the buffer for the format
    auto formatSize = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    m_format = SpxAllocWAVEFORMATEX(formatSize);

    // Copy the format
    memcpy(m_format.get(), format, formatSize);
}

void CSpxPullAudioInputStream::SetRealTimePercentage(uint8_t percentage)
{
    m_simulateRealtimePercentage = percentage;
}

void CSpxPullAudioInputStream::SetCallbacks(ReadCallbackFunction_Type readCallback, CloseCallbackFunction_Type closeCallback)
{
    m_readCallback = readCallback;
    m_closeCallback = closeCallback;
}

uint16_t CSpxPullAudioInputStream::GetFormat(SPXWAVEFORMATEX* formatBuffer, uint16_t formatSize)
{
    uint16_t formatSizeRequired = sizeof(SPXWAVEFORMATEX) + m_format->cbSize;

    if (formatBuffer != nullptr)
    {
        size_t size = std::min(formatSize, formatSizeRequired);
        std::memcpy(formatBuffer, m_format.get(), size);
    }

    return formatSizeRequired;
}

uint32_t CSpxPullAudioInputStream::Read(uint8_t* buffer, uint32_t bytesToRead)
{
    auto bytesActuallyRead = m_readCallback(buffer, bytesToRead);
    if (m_format->wFormatTag == WAVE_FORMAT_PCM)
    {
        // for compressed format rate control is inside gstreamer adapter
        SimulateRealTime(bytesActuallyRead);
    }
    return bytesActuallyRead;
}

void CSpxPullAudioInputStream::Close()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_closeCallback != nullptr)
    {
        m_closeCallback();
    }
}

void CSpxPullAudioInputStream::SimulateRealTime(uint32_t bytesToSimulateRealTime)
{
    if (m_simulateRealtimePercentage > 0)
    {
        auto milliseconds = bytesToSimulateRealTime * 1000 / m_format->nAvgBytesPerSec * m_simulateRealtimePercentage / 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
