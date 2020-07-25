//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "read_write_buffer_delegate_helper.h"
#include "function_helpers.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxBufferData :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxBufferData,
    public ISpxBufferDataWriter,
    public ISpxBufferProperties,
    private CSpxDelegateToSharedPtrHelper<ISpxBufferProperties>
{
private:
    using Properties = CSpxDelegateToSharedPtrHelper<ISpxBufferProperties>;
public:
    CSpxBufferData();
    virtual ~CSpxBufferData();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferData)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferDataWriter)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferProperties)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Init() final;
    void Term() final;

    // --- ISpxBufferData (overrides)
    uint64_t GetOffset() final;
    uint64_t GetNewMultiReaderOffset() final;

    uint32_t Read(uint8_t* buffer, uint32_t size) final;
    uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) final;

    uint64_t GetBytesDead() final;
    uint64_t GetBytesRead() final;
    uint64_t GetBytesReady() final;
    uint64_t GetBytesReadyMax() final;

    // --- ISpxBufferDataWriter
    void Write(uint8_t* buffer, uint32_t size) final;

    // --- ISpxBufferProperties
    inline virtual void SetBufferProperty(const char* name, const char* value) final
    {
        InvokeOnDelegate(Properties::GetDelegate(), &ISpxBufferProperties::SetBufferProperty, name, value);
    }

    inline virtual std::shared_ptr<const char> GetBufferProperty(const char* name, const char* defaultValue) final
    {
        auto overload = resolveOverload<const char *, const char *>(&ISpxBufferProperties::GetBufferProperty);
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, nullptr, name, defaultValue);
    }

    inline virtual PropertyValue_Type GetBufferProperty(const char* name, OffsetType offset, int direction, OffsetType* foundAtOffset) final
    {
        auto overload = resolveOverload<const char*, OffsetType, int, OffsetType*>(&ISpxBufferProperties::GetBufferProperty);
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, nullptr, name, offset, direction, foundAtOffset);
    }

    inline virtual std::list<FoundPropertyData_Type> GetBufferProperties(const char* name, OffsetType begin, OffsetType end) final
    {
        auto overload = resolveOverload<const char*, OffsetType, OffsetType>(&ISpxBufferProperties::GetBufferProperties);
        auto empty = std::list<FoundPropertyData_Type>{};
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, empty, name, begin, end);
    }

    inline virtual std::list<FoundPropertyData_Type> GetBufferProperties(OffsetType begin, OffsetType end) final
    {
        auto overload = resolveOverload<OffsetType, OffsetType>(&ISpxBufferProperties::GetBufferProperties);
        auto empty = std::list<FoundPropertyData_Type>{};
        return InvokeOnDelegateR(Properties::GetDelegate(), overload, empty, begin, end);
    }

protected:
    virtual void InitDelegatePtr(std::shared_ptr<ISpxBufferProperties>& ptr) final;
private:
    void EnsureInitRingBuffer();
    void TermRingBuffer();

    size_t GetBufferDataSize();
    uint64_t GetBufferDataInitPos();
    bool GetBufferAllowOverflow();
    uint64_t GetDefaultBufferDataSize();

    CSpxReadWriteBufferDelegateHelper<> m_ringBuffer;
    uint64_t m_bytesDead;
    uint64_t m_bytesRead;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
