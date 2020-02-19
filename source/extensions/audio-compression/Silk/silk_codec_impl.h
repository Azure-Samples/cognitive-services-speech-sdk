//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// silk_codec_impl.h: Implementation declarations for *CSilkCodecImpl* methods
//

#pragma once
#include <vector>
#include <functional>
#include <SKP_Silk_SDK_API.h>

#define AUDIO_SAMPLE_RATE   16000

#define SILK_SAMPLESPERFRAME 320
#define SILK_MAXBYTESPERBLOCK (SILK_SAMPLESPERFRAME * sizeof(uint16_t))  // VBR
#define MAX_BYTES_PER_FRAME 250                                          // Equals peak bitrate of 100 kbps

#define MAX_SILK_FRAMES 5
#define AUDIO_CHANNEL 1
#define AUDIO_BITS 16
#define AUDIO_BLOCK_ALIGN (AUDIO_CHANNEL * (AUDIO_BITS >> 3))
#define AUDIO_BYTE_RATE (AUDIO_SAMPLE_RATE * AUDIO_BLOCK_ALIGN)

#define AUDIO_ENCODE_SAMPLE_RATE  AUDIO_SAMPLE_RATE

/*
 * For optimal network performance, segment audio chunks sent to the Speech service
 * in CAPPED_SIZE chunks worth of uncompressed PCM audio.
 */
#define CAPPED_SIZE (AUDIO_BYTE_RATE / 10)  // 100ms worth of audio


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSilkCodecImpl
{
public:

    using SilkDataCallbackFunc = std::function<void (const uint8_t* buffer, size_t dataSize, uint64_t duration_100ns, void* context)>;

    CSilkCodecImpl();
    ~CSilkCodecImpl();

    int Init(
        uint32_t inputSamplesPerSecond,
        uint8_t inputBitsPerSample,
        uint8_t inputChannels,
        SilkDataCallbackFunc dataCallback,
        void* callerContext);
    int GetFormatType(char* buffer, uint64_t* buffersize);
    int Encode(const uint8_t* pBuffer, size_t bytesToWrite);
    int Flush();
    int EndStream();

private:
    using SKP_SILK_HANDLE = void*;

    void OnEncodedData(const uint8_t* pdata, size_t size);
    int SilkEncodeFrame(const uint8_t* pBuffer);
    int EncodeUnfilledBuffer();
    void SendSilkTerminator();
    void FlushEncoderBuffer();
    void EndEncoderStream();
    uint64_t AudioUnencodedBytesToDuration(uint64_t bytes);
    void ReleaseEncoder();

    SilkDataCallbackFunc m_dataCallback;
    void* m_dataCallbackContext;
    
    SKP_SILK_HANDLE m_hEncoder = NULL;

    uint8_t m_encoderBuffer[SILK_MAXBYTESPERBLOCK];
    size_t m_encoderBufferOffset = 0;

    // compressed buffer
    std::vector<uint8_t> m_silkEncodedBuffer;

    // compressed offset
    size_t m_silkBufferOffset = 0;

    // uncompressed offset
    size_t m_pcmSize = 0;

    // the number of raw bytes encoded so far
    uint64_t m_encodedBytes = 0;

    // the unencoded size of the encoded bytes sent so far
    uint64_t m_unencodedBytesSent = 0;

    SKP_SILK_SDK_EncControlStruct m_encoderControl;
};

}}}}
