//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_data.h: Implementation declarations for CSpxAudioSourceBufferData
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "read_write_buffer_delegate_helper.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSourceBufferData :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxAudioSourceBufferData,
    public ISpxAudioSourceBufferDataWriter
{
public:

    CSpxAudioSourceBufferData();
    virtual ~CSpxAudioSourceBufferData();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferData)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferDataWriter)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Init() override;
    void Term() override;

    // --- ISpxAudioSourceBufferData (overrides)
    uint64_t GetOffset() override;

    uint32_t Read(uint8_t* buffer, uint32_t size) override;
    uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) override;

    uint64_t GetBytesDead() override;
    uint64_t GetBytesRead() override;
    uint64_t GetBytesReady() override;
    uint64_t GetBytesReadyMax() override;

    // --- ISpxAudioSourceBufferDataWriter
    void Write(uint8_t* buffer, uint32_t size) override;

private:

    DISABLE_COPY_AND_MOVE(CSpxAudioSourceBufferData);

    void EnsureInitRingBuffer();
    void TermRingBuffer();

    uint64_t GetAudioSourceBufferDataSize();
    uint64_t GetAudioSourceBufferDataInitPos();
    uint64_t GetDefaultAudioSourceBufferDataSize();

    CSpxReadWriteBufferDelegateHelper<> m_ringBuffer;
    uint64_t m_bytesDead;
    uint64_t m_bytesRead;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
