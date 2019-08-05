//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// codec_adapter.cpp: Implementation definitions for CSpxCodecAdapter C++ class
//

#include "stdafx.h"
#include "codec_adapter.h"
#include "audio_decoder.h"
#include "opus_decoder.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxCodecAdapter::CSpxCodecAdapter()
{
    SPX_DBG_TRACE_VERBOSE("%s", __FUNCTION__);
}

CSpxCodecAdapter::~CSpxCodecAdapter()
{
    SPX_DBG_TRACE_VERBOSE("%s", __FUNCTION__);
    if (m_gstObject != nullptr)
    {
        m_gstObject->Stop();
        m_gstObject = nullptr;
    }
}

void CSpxCodecAdapter::Open(AudioStreamContainerFormat containerFormat)
{
    SPX_DBG_TRACE_VERBOSE("%s", __FUNCTION__);

    // throw if GStreamer wrapper is not available (weak linking on iOS)
    auto gst_fun_ptr = &gst_init;
    SPX_IFTRUE_THROW_HR(NULL == gst_fun_ptr, SPXERR_GSTREAMER_NOT_FOUND_ERROR);

    gst_init(NULL, NULL);
    switch (containerFormat)
    {
    case AudioStreamContainerFormat::OGG_OPUS:
        m_gstObject = std::make_shared<OpusDecoder>(m_readCallback);
        break;

    case AudioStreamContainerFormat::MP3:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::MP3);
        break;

    case AudioStreamContainerFormat::FLAC:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::FLAC);
        break;

    case AudioStreamContainerFormat::ALAW:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::ALAW);
        break;

    case AudioStreamContainerFormat::MULAW:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::MULAW);
        break;

    case AudioStreamContainerFormat::AMRNB:
        //m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::AMRNB);
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;

    case AudioStreamContainerFormat::AMRWB:
        //m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::AMRWB);
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;

    default:
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;
    }
}

void CSpxCodecAdapter::SetFormat(SPXWAVEFORMATEX* format)
{
    SPX_IFTRUE_THROW_HR(m_format != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Allocate the buffer for the format
    auto formatSize = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    m_format = SpxAllocWAVEFORMATEX(formatSize);

    // Copy the format
    memcpy(m_format.get(), format, formatSize);
    AudioStreamContainerFormat containerFormat = static_cast<AudioStreamContainerFormat>(m_format->wFormatTag);
    Open(containerFormat);
}

uint16_t CSpxCodecAdapter::GetFormat(SPXWAVEFORMATEX* format, uint16_t formatSize)
{
    UNUSED(formatSize);
    if (format != nullptr)
    {
        format->wFormatTag = WAVE_FORMAT_PCM;
        format->nChannels = 1;
        format->nSamplesPerSec = 16000;
        format->nAvgBytesPerSec = 16000 * (16 / 8) * 1;
        format->nBlockAlign = (1 * 16) / 8;
        format->wBitsPerSample = 16;
        format->cbSize = 0;
    }
    return sizeof(SPXWAVEFORMATEX);
}

uint32_t CSpxCodecAdapter::Read(uint8_t* buffer, uint32_t bytesToRead)
{
    uint32_t numBytesRead = 0;
    bool stop = false;

    if (!m_streamStarted && m_gstObject != nullptr)
    {
        m_gstObject->StartReader();
        m_streamStarted = true;
    }

    while (!stop)
    {
        if (buffer != nullptr && bytesToRead > 0)
        {
            numBytesRead += m_gstObject->Read(&buffer[numBytesRead], bytesToRead - numBytesRead);
            if (numBytesRead == bytesToRead || m_gstObject->GetStatus())
            {
                stop = true;
            }
        }
        else
        {
            stop = true;
        }
    }

    return numBytesRead;
}

void CSpxCodecAdapter::Close()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_closeCallback != nullptr)
    {
        m_closeCallback();
    }
}

void CSpxCodecAdapter::SetCallbacks(ReadCallbackFunction_Type readCallback, CloseCallbackFunction_Type closeCallback)
{
    m_readCallback = readCallback;
    m_closeCallback = closeCallback;
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
