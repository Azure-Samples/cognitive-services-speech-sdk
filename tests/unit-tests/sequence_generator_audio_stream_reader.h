//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// sequence_generator_audio_stream_reader.h: Definitions for CSpxSequenceGeneratorAudioStreamReader C++ class
//

#pragma once
#include "exception.h"
#include "ispxinterfaces.h"
#include "test_utils.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSequenceGeneratorAudioStreamReader :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioStream,
    public ISpxAudioStreamReader
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
    SPX_INTERFACE_MAP_END()

    CSpxSequenceGeneratorAudioStreamReader() = default;

    void UseSequentialBufferedData(uint32_t bufferSize);
    uint32_t GetUseSequentialBufferedData();

    // --- ISpxAudioStreamCanBeThrottled ---
    void SetRealTimeThrottlePercentage(uint8_t percentage);
    uint8_t GetRealTimeThrottlePercentage();

    // --- ISpxAudioStreamReader ---
    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;
    SPXSTRING GetProperty(PropertyId propertyId) override
    {
        UNUSED(propertyId);
        return "";
    };

    void Close() override;

    // --- ISpxObjectInit ---
    virtual void Init() override;
    virtual void Term() override;

private:

    CSpxSequenceGeneratorAudioStreamReader(CSpxSequenceGeneratorAudioStreamReader&&) = delete;
    CSpxSequenceGeneratorAudioStreamReader(const CSpxSequenceGeneratorAudioStreamReader&) = delete;
    CSpxSequenceGeneratorAudioStreamReader& operator=(CSpxSequenceGeneratorAudioStreamReader&&) = delete;
    CSpxSequenceGeneratorAudioStreamReader& operator=(const CSpxSequenceGeneratorAudioStreamReader&) = delete;

    bool m_initialized { false };
    uint8_t m_simulateRealtimePercentage = 0;     // 0 == as fast as possible; 100 == real time; 200 == 2x slower than real time
    std::unique_ptr<uint8_t[]> m_buffer;
    uint32_t m_bufferSize = 0;
    uint32_t m_seqDataValue = 0;
    uint32_t m_bufferSizeRemained = 0;

    uint32_t ReadSequentialBufferedData(uint8_t* pbuffer, uint32_t cbBuffer);
    uint32_t ReadNulldData(uint8_t* pbuffer, uint32_t cbBuffer);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
