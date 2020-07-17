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
#include "function_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSourceBufferData :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxAudioSourceBufferData,
    public ISpxAudioSourceBufferDataWriter,
    public ISpxAudioSourceBufferProperties,
    private CSpxDelegateToSharedPtrHelper<ISpxAudioSourceBufferProperties>

{
private:
    using Properties = CSpxDelegateToSharedPtrHelper<ISpxAudioSourceBufferProperties>;
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
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferProperties)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Init() override;
    void Term() override;

    // --- ISpxAudioSourceBufferData (overrides)
    uint64_t GetOffset() override;
    uint64_t GetNewMultiReaderOffset() override;

    uint32_t Read(uint8_t* buffer, uint32_t size) override;
    uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) override;

    uint64_t GetBytesDead() override;
    uint64_t GetBytesRead() override;
    uint64_t GetBytesReady() override;
    uint64_t GetBytesReadyMax() override;

    // --- ISpxAudioSourceBufferDataWriter
    void Write(uint8_t* buffer, uint32_t size) override;

    // --- ISpxAudioSourceBufferProperties
    inline virtual void SetBufferProperty(const char* name, const char* value) final
    {
        InvokeOnDelegate(Properties::GetDelegate(), &ISpxAudioSourceBufferProperties::SetBufferProperty, name, value);
    }

    inline virtual std::shared_ptr<const char> GetBufferProperty(const char* name, const char* defaultValue) final
    {
        auto overload = resolveOverload<const char *, const char *>(&ISpxAudioSourceBufferProperties::GetBufferProperty);
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, nullptr, name, defaultValue);
    }

    inline virtual PropertyValue_Type GetBufferProperty(const char* name, OffsetType offset, int direction, OffsetType* foundAtOffset) final
    {
        auto overload = resolveOverload<const char*, OffsetType, int, OffsetType*>(&ISpxAudioSourceBufferProperties::GetBufferProperty);
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, nullptr, name, offset, direction, foundAtOffset);
    }

    inline virtual std::list<FoundPropertyData_Type> GetBufferProperties(const char* name, OffsetType begin, OffsetType end) final
    {
        auto overload = resolveOverload<const char*, OffsetType, OffsetType>(&ISpxAudioSourceBufferProperties::GetBufferProperties);
        auto empty = std::list<FoundPropertyData_Type>{};
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, empty, name, begin, end);
    }

    inline virtual std::list<FoundPropertyData_Type> GetBufferProperties(OffsetType begin, OffsetType end) final
    {
        auto overload = resolveOverload<OffsetType, OffsetType>(&ISpxAudioSourceBufferProperties::GetBufferProperties);
        auto empty = std::list<FoundPropertyData_Type>{};
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, empty, begin, end);
    }

protected:
    virtual void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferProperties>& ptr) final;
private:

    DISABLE_COPY_AND_MOVE(CSpxAudioSourceBufferData);

    void EnsureInitRingBuffer();
    void TermRingBuffer();

    size_t GetAudioSourceBufferDataSize();
    uint64_t GetAudioSourceBufferDataInitPos();
    bool GetAudioSourceBufferAllowOverflow();
    uint64_t GetDefaultAudioSourceBufferDataSize();

    CSpxReadWriteBufferDelegateHelper<> m_ringBuffer;
    uint64_t m_bytesDead;
    uint64_t m_bytesRead;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
