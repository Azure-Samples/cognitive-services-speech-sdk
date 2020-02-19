//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// silk_codec_impl.cpp: Implementation definitions for CSilkCodecImpl C++ class
//

#include "stdafx.h"
#include <SKP_Silk_SDK_API.h>
#include "silk_codec_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const char c_silkV3Mark[] = "#!SILK_V3";
const uint16_t c_silkTerm = 0xffff;
const char c_silkContentType[] = "audio/SILK";
const uint64_t c_silkContentTypeSize = sizeof(c_silkContentType);

CSilkCodecImpl::CSilkCodecImpl()
    : m_dataCallbackContext(nullptr),
      m_encoderBuffer{0},
      m_encoderControl{0}
{
}

CSilkCodecImpl::~CSilkCodecImpl()
{
    ReleaseEncoder();
}

int CSilkCodecImpl::Init(
                        uint32_t inputSamplesPerSecond,
                        uint8_t inputBitsPerSample,
                        uint8_t inputChannels,
                        SilkDataCallbackFunc dataCallback,
                        void* callerContext)
{
    _CRT_UNUSED(inputSamplesPerSecond);
    _CRT_UNUSED(inputBitsPerSample);
    _CRT_UNUSED(inputChannels);

    m_dataCallback = dataCallback;
    m_dataCallbackContext = callerContext;

    SKP_int ret = 0;
    SKP_int32 size = 0;
    SKP_SILK_SDK_EncControlStruct status;

    m_encoderControl.API_sampleRate = AUDIO_ENCODE_SAMPLE_RATE;
    m_encoderControl.maxInternalSampleRate = AUDIO_ENCODE_SAMPLE_RATE;
    m_encoderControl.packetSize = SILK_SAMPLESPERFRAME;
    m_encoderControl.bitRate = 36000;
    m_encoderControl.packetLossPercentage = 0;
    m_encoderControl.complexity = 0;
    m_encoderControl.useInBandFEC = 0;
    m_encoderControl.useDTX = 0;

    //
    // Allocate the encoder
    //
    ret = SKP_Silk_SDK_Get_Encoder_Size(&size);
    if (ret != 0)
    {
        return ret;
    }

    m_hEncoder = malloc((size_t)size);
    if (!m_hEncoder)
    {
        return -1;
    }

    //
    // Prepare the encoder
    //

    // TODO initialize metrics

    ret = SKP_Silk_SDK_InitEncoder(m_hEncoder, &status);
    if (ret != 0)
    {
        return ret;
    }

    // Set the SILK header
    OnEncodedData((uint8_t*)c_silkV3Mark, sizeof(c_silkV3Mark) - 1);
    m_encoderBufferOffset = 0;
    m_encodedBytes = 0;
    m_unencodedBytesSent = 0;

    return ret;
}

int CSilkCodecImpl::GetFormatType(char* buffer, uint64_t* buffersize)
{
    if (buffersize == NULL)
    {
        return -1;
    }

    if (buffer == NULL || *buffersize < c_silkContentTypeSize)
    {
        *buffersize = c_silkContentTypeSize;
        return -1;
    }

    memcpy(buffer, c_silkContentType, c_silkContentTypeSize);
    return 0;
}

int CSilkCodecImpl::Encode(const uint8_t* pBuffer, size_t bytesToWrite)
{
    SKP_int                       ret = 0;
    size_t                        chunkBytes;

    if (nullptr == m_dataCallback)
    {
        // TODO: Log error not initialized
        return -1;
    }

    if (NULL == pBuffer)
    {
        // TODO: Log error buffer 
        return -1;
    }
    else if (0 == bytesToWrite)
    {
        return 0;
    }

    m_pcmSize += bytesToWrite;

    // fill any previous buffer if there is one outstanding.
    if (m_encoderBufferOffset != 0)
    {
        chunkBytes = sizeof(m_encoderBuffer) - m_encoderBufferOffset;
        if (chunkBytes > bytesToWrite)
        {
            chunkBytes = bytesToWrite;
        }

        memcpy(m_encoderBuffer + m_encoderBufferOffset, pBuffer, chunkBytes);
        pBuffer += chunkBytes;
        bytesToWrite -= chunkBytes;
        m_encoderBufferOffset += chunkBytes;

        if (m_encoderBufferOffset == SILK_MAXBYTESPERBLOCK)
        {
            m_encoderBufferOffset = 0;
            ret = SilkEncodeFrame(m_encoderBuffer);
            if (ret != 0)
            {
                return ret;
            }
        }
    }

    while (bytesToWrite >= SILK_MAXBYTESPERBLOCK)
    {
        ret = SilkEncodeFrame(pBuffer);
        if (ret != 0)
        {
            return ret;
        }

        pBuffer += SILK_MAXBYTESPERBLOCK;
        bytesToWrite -= SILK_MAXBYTESPERBLOCK;
    }

    if (m_pcmSize >= CAPPED_SIZE)
    {
        FlushEncoderBuffer();
        m_pcmSize = 0;
    }

    //
    // There may be some bytes not yet encoded, carry them over
    //
    if (bytesToWrite > 0)
    {
        memcpy(m_encoderBuffer, pBuffer, bytesToWrite);
        m_encoderBufferOffset = bytesToWrite;
        m_pcmSize += bytesToWrite;
    }

    return ret;
}

int CSilkCodecImpl::Flush()
{
    if (nullptr == m_dataCallback)
    {
        // TODO: Log error not initialized
        return -1;
    }

    m_pcmSize = 0;

    EncodeUnfilledBuffer();
    SendSilkTerminator();
    FlushEncoderBuffer();
    EndEncoderStream();
    return 0;

}

int CSilkCodecImpl::EndStream()
{
    // pretend there isn't anything in the buffer and shut it down
    m_pcmSize = 0;
    m_silkBufferOffset = 0;
    m_encodedBytes = 0;
    m_unencodedBytesSent = 0;

    SendSilkTerminator();
    FlushEncoderBuffer();

    return 0;
}

void CSilkCodecImpl::OnEncodedData(const uint8_t* pdata, size_t size)
{
    // Save the encoded data into a local buffer
    size_t avail = m_silkEncodedBuffer.size() - m_silkBufferOffset;
    if (size > avail)
    {
        m_silkEncodedBuffer.resize(m_silkEncodedBuffer.size() - avail + size);
    }
    memcpy(m_silkEncodedBuffer.data() + m_silkBufferOffset, pdata, size);
    m_silkBufferOffset += size;
}

void CSilkCodecImpl::FlushEncoderBuffer()
{
    if (m_silkBufferOffset != 0)
    {
        // TODO: metrics

        // convert the raw sample size (bitrate) into duration
        const uint64_t rawBytes = m_encodedBytes - m_unencodedBytesSent;
        const uint64_t duration = AudioUnencodedBytesToDuration(rawBytes);

        // Give the data to the caller.
        m_dataCallback(m_silkEncodedBuffer.data(), m_silkBufferOffset, duration, m_dataCallbackContext);
        m_silkBufferOffset = 0;
        m_silkEncodedBuffer.resize(0);
        m_unencodedBytesSent = m_encodedBytes;
    }
}

uint64_t CSilkCodecImpl::AudioUnencodedBytesToDuration(uint64_t bytes)
{
    return bytes * 10000000 / AUDIO_BYTE_RATE;
}


int CSilkCodecImpl::SilkEncodeFrame(const uint8_t* pBuffer)
{
    struct
    {
        SKP_int16 nBytes;
        SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_SILK_FRAMES];
    } outSample;

    SKP_int ret = 0;

    outSample.nBytes = sizeof(outSample.payload);
    ret = SKP_Silk_SDK_Encode(
        m_hEncoder, &m_encoderControl, (short*)pBuffer, SILK_SAMPLESPERFRAME, outSample.payload, &outSample.nBytes);

    if (ret != 0)
    {
        // Log failed to encode
        return ret;
    }

    if (outSample.nBytes != 0)
    {
        m_encodedBytes += SILK_MAXBYTESPERBLOCK;
        OnEncodedData((const uint8_t*)&outSample, sizeof(uint16_t) + (size_t)outSample.nBytes);
    }

    return ret;
}

int CSilkCodecImpl::EncodeUnfilledBuffer()
{
    // The codec does not encode small pieces of audio only multiples of our buffer size. We will fill everything with 0 for alignment purposes. 
    SKP_int ret = 0;

    if (m_encoderBufferOffset > 0) // == SILK_MAXBYTESPERBLOCK)
    {
        // Pad the end with zeros
        memset(m_encoderBuffer + m_encoderBufferOffset, 0, sizeof(m_encoderBuffer) - m_encoderBufferOffset);
        m_encoderBufferOffset = 0;
        ret = SilkEncodeFrame(m_encoderBuffer);
    }
    return ret;
}

void CSilkCodecImpl::SendSilkTerminator()
{
    OnEncodedData((const uint8_t*)&c_silkTerm, sizeof(c_silkTerm));
}

void CSilkCodecImpl::EndEncoderStream()
{

    // signal end of stream with a 0 length array
    m_dataCallback(NULL, 0, 0, m_dataCallbackContext);
    m_silkBufferOffset = 0;
}

void CSilkCodecImpl::ReleaseEncoder()
{
    if (m_hEncoder != NULL)
    {
        free(m_hEncoder);
        m_hEncoder = NULL;
    }
}

}}}}
