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

#include <create_object_helpers.h>
#include <site_helpers.h>

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

void CSpxCodecAdapter::Open(AudioStreamContainerFormat containerFormat, uint16_t bitsPerSample, uint16_t numChannels, uint32_t sampleRate)
{
    SPX_DBG_TRACE_VERBOSE("%s", __FUNCTION__);

    // throw if GStreamer wrapper is not available (weak linking on iOS)
    auto gst_fun_ptr = &gst_init;
    SPX_IFTRUE_THROW_HR(NULL == gst_fun_ptr, SPXERR_GSTREAMER_NOT_FOUND_ERROR);

    gst_init(NULL, NULL);

    auto data = SpxCreateObjectWithSite<ISpxReadWriteBufferInit>("CSpxReadWriteRingBuffer", SpxGetRootSite());
    data->SetName("CodecBuffer");
    data->SetSize(BaseGstreamer::BUFFER_32KB * 2);
    auto buffer = SpxQueryInterface<ISpxReadWriteBuffer>(data);

    switch (containerFormat)
    {
    case AudioStreamContainerFormat::OGG_OPUS:
        m_gstObject = std::make_shared<OpusDecoder>(m_readCallback, buffer, bitsPerSample, numChannels, sampleRate);
        break;

    case AudioStreamContainerFormat::MP3:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, buffer, CodecsTypeInternal::MP3, bitsPerSample, numChannels, sampleRate);
        break;

    case AudioStreamContainerFormat::FLAC:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, buffer, CodecsTypeInternal::FLAC, bitsPerSample, numChannels, sampleRate);
        break;

    case AudioStreamContainerFormat::ALAW:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, buffer, CodecsTypeInternal::ALAW, bitsPerSample, numChannels, sampleRate);
        break;

    case AudioStreamContainerFormat::MULAW:
        m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, buffer, CodecsTypeInternal::MULAW, bitsPerSample, numChannels, sampleRate);
        break;

    case AudioStreamContainerFormat::AMRNB:
        //m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::AMRNB, bitsPerSample, numChannels, sampleRate);
        SPX_IFTRUE_THROW_HR(true, SPXERR_CONTAINER_FORMAT_NOT_SUPPORTED_ERROR);
        break;

    case AudioStreamContainerFormat::AMRWB:
        //m_gstObject = std::make_shared<AudioDecoder>(m_readCallback, CodecsTypeInternal::AMRWB, bitsPerSample, numChannels, sampleRate);
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
    Open(containerFormat, m_format->wBitsPerSample, m_format->nChannels, m_format->nSamplesPerSec);
}

uint16_t CSpxCodecAdapter::GetFormat(SPXWAVEFORMATEX* format, uint16_t formatSize)
{
    UNUSED(formatSize);
    if (format != nullptr)
    {
        format->wFormatTag = WAVE_FORMAT_PCM;
        format->nChannels = m_format->nChannels;
        format->nSamplesPerSec = m_format->nSamplesPerSec;
        format->nAvgBytesPerSec = m_format->nSamplesPerSec * (m_format->wBitsPerSample >> 3) * m_format->nChannels;
        format->nBlockAlign = m_format->nChannels * (m_format->wBitsPerSample >> 3);
        format->wBitsPerSample = m_format->wBitsPerSample;
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

void CSpxCodecAdapter::SetPropertyCallback2(GetPropertyCallbackFunction_Type2 getPropertyCallBack)
{
    m_getPropertyCallback = getPropertyCallBack;
}

SPXSTRING CSpxCodecAdapter::GetProperty(PropertyId propertyId)
{
    return m_getPropertyCallback(propertyId);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
