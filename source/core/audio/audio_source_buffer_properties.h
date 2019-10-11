//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_properties.h: Implementation declarations for CSpxAudioSourceBufferProperties
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "blocking_read_write_ring_buffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSourceBufferProperties :
    public ISpxObjectWithSiteInitImpl<ISpxAudioSourceBufferData>,
    public ISpxGenericSite,
    public ISpxAudioSourceBufferProperties
{
public:

    CSpxAudioSourceBufferProperties() = default;
    virtual ~CSpxAudioSourceBufferProperties();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Term() override;

    // --- ISpxAudioSourceBufferProperties (overrides)
    void SetBufferProperty(const char* name, const char* value) override;

    PropertyValue_Type GetBufferProperty(const char* name, const char* defaultValue = nullptr) override;
    PropertyValue_Type GetBufferProperty(const char* name, PropertyOffset_Type offset, int direction = -1, PropertyOffset_Type* foundAtOffset = nullptr) override;

    std::list<FoundPropertyData_Type> GetBufferProperties(PropertyOffset_Type offsetBegin, PropertyOffset_Type offsetEnd) override;
    std::list<FoundPropertyData_Type> GetBufferProperties(const char* name, PropertyOffset_Type offsetBegin, PropertyOffset_Type offsetEnd) override;

private:

    DISABLE_COPY_AND_MOVE(CSpxAudioSourceBufferProperties);

    void TermPropertyNames();

    void EnsureInitPropertyDataBuffer();
    void InitPropertyDataBuffer();
    void TermPropertyDataBuffer();

    void EnsureInitPropertyValuesBuffer();
    void InitPropertyValuesBuffer();
    void TermPropertyValuesBuffer();

    size_t GetPropertyDataBufferSize();
    size_t GetDefaultPropertyDataBufferSize();
    size_t GetPropertyValueBufferSize();
    size_t GetDefaultPropertyValueBufferSize();

    uint64_t OffsetFromSite();

    uint64_t IdFromName(const char* name);
    uint64_t IdFromValue(const char* value);

    PropertyName_Type NameFromId(uint64_t id);
    PropertyValue_Type ValueFromId(uint64_t id);

    void WritePropertyData(uint64_t nameId, uint64_t offset, uint64_t valueId);
    void ReadPropertyData(uint64_t dataPos, uint64_t* nameId, uint64_t* offset, uint64_t* valueId);

    PropertyValue_Type FindPropertyDataValue(uint64_t nameId, uint64_t offset, int direction = -1, PropertyOffset_Type* foundAtOffset = nullptr);

    std::list<FoundPropertyData_Type> FindPropertyData(uint64_t nameId, uint64_t offsetBegin, uint64_t offsetEnd);

    uint64_t FirstFindDataPos();
    uint64_t NextFindDataPos(uint64_t pos);
    inline uint64_t StopFindDataPos() { return UINT64_MAX; }

private:

    static const size_t itemSize = sizeof(uint64_t) * 3;

    std::map<std::string, uint64_t> m_nameIds;
    std::map<uint64_t, std::string> m_nameFromId;
    std::shared_ptr<ISpxReadWriteBuffer> m_values;
    std::shared_ptr<ISpxReadWriteBuffer> m_data;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
