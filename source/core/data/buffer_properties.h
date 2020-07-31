//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "spxcore_common.h"
#include "service_helpers.h"
#include "blocking_read_write_ring_buffer.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxBufferProperties :
    public ISpxObjectWithSiteInitImpl<ISpxBufferData>,
    public ISpxGenericSite,
    public ISpxBufferProperties,
    public ISpxServiceProvider
{
public:

    CSpxBufferProperties() = default;
    virtual ~CSpxBufferProperties();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Term() final;

    // --- ISpxBufferProperties (overrides)
    void SetBufferProperty(const char* name, const char* value) final;

    PropertyValue_Type GetBufferProperty(const char* name, const char* defaultValue = nullptr) override;
    PropertyValue_Type GetBufferProperty(const char* name, OffsetType offset, int direction = -1, OffsetType* foundAtOffset = nullptr) override;

    std::list<FoundPropertyData_Type> GetBufferProperties(OffsetType offsetBegin, OffsetType offsetEnd) override;
    std::list<FoundPropertyData_Type> GetBufferProperties(const char* name, OffsetType offsetBegin, OffsetType offsetEnd) override;

private:
    void TermPropertyNames();

    void EnsureInitPropertyDataBuffer();
    void InitPropertyDataBuffer();
    void TermPropertyDataBuffer();

    void EnsureInitPropertyValuesBuffer();
    void InitPropertyValuesBuffer();
    void TermPropertyValuesBuffer();

    SizeType GetPropertyDataBufferSize();
    SizeType GetDefaultPropertyDataBufferSize() const;
    SizeType GetPropertyValueBufferSize();
    SizeType GetDefaultPropertyValueBufferSize() const;

    uint64_t OffsetFromSite();

    uint64_t IdFromName(const char* name);
    uint64_t IdFromValue(const char* value);

    PropertyName_Type NameFromId(uint64_t id);
    PropertyValue_Type ValueFromId(uint64_t id);

    void WritePropertyData(uint64_t nameId, uint64_t offset, uint64_t valueId);
    void ReadPropertyData(uint64_t dataPos, uint64_t* nameId, uint64_t* offset, uint64_t* valueId);

    PropertyValue_Type FindPropertyDataValue(uint64_t nameId, uint64_t offset, int direction = -1, OffsetType* foundAtOffset = nullptr);

    std::list<FoundPropertyData_Type> FindPropertyData(uint64_t nameId, uint64_t offsetBegin, uint64_t offsetEnd);

    uint64_t FirstFindDataPos();
    uint64_t NextFindDataPos(uint64_t pos);
    inline uint64_t StopFindDataPos() { return UINT64_MAX; }

private:

    static constexpr size_t itemSize = sizeof(uint64_t) * 3;

    std::map<std::string, uint64_t> m_nameIds;
    std::map<uint64_t, std::string> m_nameFromId;
    std::shared_ptr<ISpxReadWriteBuffer> m_values;
    std::shared_ptr<ISpxReadWriteBuffer> m_data;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
