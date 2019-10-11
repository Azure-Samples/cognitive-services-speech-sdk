//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_properties.cpp: Implementation definitions for CSpxAudioSourceBufferProperties
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "audio_source_buffer_properties.h"
#include "create_object_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define DEFAULT_AUDIO_SOURCE_BUFFER_PROPERTY_DATA_SIZE_IN_BYTES ((uint64_t)0x40000)
#define DEFAULT_AUDIO_SOURCE_BUFFER_PROPERTY_VALUES_SIZE_IN_BYTES ((uint64_t)0x40000)

CSpxAudioSourceBufferProperties::~CSpxAudioSourceBufferProperties()
{
    SPX_DBG_ASSERT(m_values == nullptr);
    SPX_DBG_ASSERT(m_data == nullptr);
    SPX_DBG_ASSERT(m_nameIds.size() == 0);
    SPX_DBG_ASSERT(m_nameFromId.size() == 0);
}

void CSpxAudioSourceBufferProperties::Term()
{
    TermPropertyNames();
    TermPropertyDataBuffer();
    TermPropertyValuesBuffer();
}

void CSpxAudioSourceBufferProperties::SetBufferProperty(const char* name, const char* value)
{
    auto offset = OffsetFromSite();
    auto nameId = IdFromName(name);
    auto valueId = IdFromValue(value);
    WritePropertyData(nameId, offset, valueId);
}

std::shared_ptr<const char> CSpxAudioSourceBufferProperties::GetBufferProperty(const char* name, const char* defaultValue)
{
    auto offset = OffsetFromSite();
    auto nameId = IdFromName(name);
    auto value = FindPropertyDataValue(nameId, offset, -1, nullptr);
    return value != nullptr ? value : std::shared_ptr<const char>(defaultValue, [](auto) {});
}

std::shared_ptr<const char> CSpxAudioSourceBufferProperties::GetBufferProperty(const char* name, PropertyOffset_Type offset, int direction, PropertyOffset_Type* foundAtOffset)
{
    auto nameId = IdFromName(name);
    return FindPropertyDataValue(nameId, offset, direction, foundAtOffset);
}

std::list<ISpxAudioSourceBufferProperties ::FoundPropertyData_Type> CSpxAudioSourceBufferProperties::GetBufferProperties(PropertyOffset_Type offsetBegin, PropertyOffset_Type offsetEnd)
{
    auto nameId = IdFromName(nullptr);
    return FindPropertyData(nameId, offsetBegin, offsetEnd);
}

std::list<ISpxAudioSourceBufferProperties::FoundPropertyData_Type> CSpxAudioSourceBufferProperties::GetBufferProperties(const char* name, PropertyOffset_Type offsetBegin, PropertyOffset_Type offsetEnd)
{
    auto nameId = IdFromName(name);
    return FindPropertyData(nameId, offsetBegin, offsetEnd);
}

void CSpxAudioSourceBufferProperties::TermPropertyNames()
{
    m_nameIds.clear();
    SPX_DBG_ASSERT(m_nameIds.size() == 0);

    m_nameFromId.clear();
    SPX_DBG_ASSERT(m_nameFromId.size() == 0);
}

void CSpxAudioSourceBufferProperties::EnsureInitPropertyDataBuffer()
{
    SPX_IFTRUE(m_data == nullptr, InitPropertyDataBuffer());
}

void CSpxAudioSourceBufferProperties::InitPropertyDataBuffer()
{
    auto data = SpxCreateObjectWithSite<ISpxReadWriteBufferInit>("CSpxReadWriteRingBuffer", this);
    data->SetName("AudioSourceBufferPropertyData");
    data->SetSize(GetPropertyDataBufferSize());
    m_data = SpxQueryInterface<ISpxReadWriteBuffer>(data);
}

void CSpxAudioSourceBufferProperties::TermPropertyDataBuffer()
{
    SpxTermAndClear(m_data);
    SPX_DBG_ASSERT(m_data == nullptr);
}

void CSpxAudioSourceBufferProperties::EnsureInitPropertyValuesBuffer()
{
    SPX_IFTRUE(m_values == nullptr, InitPropertyValuesBuffer());
}

void CSpxAudioSourceBufferProperties::InitPropertyValuesBuffer()
{
    auto values = SpxCreateObjectWithSite<ISpxReadWriteBufferInit>("CSpxReadWriteRingBuffer", this);
    values->SetName("AudioSourceBufferPropertyValues");
    values->SetSize(GetPropertyValueBufferSize());
    m_values = SpxQueryInterface<ISpxReadWriteBuffer>(values);
}

void CSpxAudioSourceBufferProperties::TermPropertyValuesBuffer()
{
    SpxTermAndClear(m_values);
    SPX_DBG_ASSERT(m_values == nullptr);
}

size_t CSpxAudioSourceBufferProperties::GetPropertyDataBufferSize()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto size = properties->GetStringValue("AudioSourceBufferPropertyDataSizeInBytes", std::to_string(GetDefaultPropertyDataBufferSize()).c_str());
    return std::stoul(size.c_str());
}

size_t CSpxAudioSourceBufferProperties::GetDefaultPropertyDataBufferSize()
{
    return DEFAULT_AUDIO_SOURCE_BUFFER_PROPERTY_DATA_SIZE_IN_BYTES;
}

size_t CSpxAudioSourceBufferProperties::GetPropertyValueBufferSize()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto size = properties->GetStringValue("AudioSourceBufferPropertyValueSizeInBytes", std::to_string(GetDefaultPropertyValueBufferSize()).c_str());
    return std::stoul(size.c_str());
}

size_t CSpxAudioSourceBufferProperties::GetDefaultPropertyValueBufferSize()
{
    return DEFAULT_AUDIO_SOURCE_BUFFER_PROPERTY_VALUES_SIZE_IN_BYTES;
}

uint64_t CSpxAudioSourceBufferProperties::OffsetFromSite()
{
    auto site = GetSite();
    return site != nullptr ? site->GetOffset() : 0;
}

uint64_t CSpxAudioSourceBufferProperties::IdFromName(const char* name)
{
    SPX_IFTRUE_RETURN_X(name == nullptr, 0);
    SPX_IFTRUE_RETURN_X(m_nameIds.count(name) > 0, m_nameIds[name]);
    auto id = m_nameIds.size() + 1;
    m_nameFromId[id] = name;
    m_nameIds[name] = id;
    return id;
}

uint64_t CSpxAudioSourceBufferProperties::IdFromValue(const char* value)
{
    EnsureInitPropertyValuesBuffer();

    auto valueSize = strlen(value) + 1;
    m_values->Write(&valueSize, sizeof(valueSize));

    auto id = m_values->GetWritePos();
    m_values->Write(value, valueSize);

    return id;
}

ISpxAudioSourceBufferProperties::PropertyName_Type CSpxAudioSourceBufferProperties::NameFromId(uint64_t id)
{
    SPX_IFTRUE_RETURN_X(m_nameFromId.count(id) != 1, nullptr);
    auto& name = m_nameFromId[id];
    return std::shared_ptr<const char>(name.c_str(), [](auto){});
}

ISpxAudioSourceBufferProperties::PropertyValue_Type CSpxAudioSourceBufferProperties::ValueFromId(uint64_t id)
{
    SPX_IFTRUE_RETURN_X(id >= m_values->GetWritePos(), nullptr);

    auto valuePos = id;
    auto valueSize = id - id;
    auto valueSizePos = id - sizeof(valueSize);
    m_values->ReadAtBytePos(valueSizePos, &valueSize, (size_t)sizeof(valueSize));

    return m_values->ReadSharedAtBytePos<char>(valuePos, (size_t)valueSize);
}

void CSpxAudioSourceBufferProperties::WritePropertyData(uint64_t nameId, uint64_t offset, uint64_t valueId)
{
    EnsureInitPropertyDataBuffer();

    m_data->Write(&nameId, sizeof(nameId));
    m_data->Write(&offset, sizeof(offset));
    m_data->Write(&valueId, sizeof(valueId));
}

void CSpxAudioSourceBufferProperties::ReadPropertyData(uint64_t dataPos, uint64_t* nameId, uint64_t* offset, uint64_t* valueId)
{
    EnsureInitPropertyDataBuffer();

    uint64_t data[3];
    m_data->ReadAtBytePos(dataPos, &data, itemSize * 3);

    if (nameId != nullptr) *nameId = data[0];
    if (offset != nullptr) *offset = data[1];
    if (valueId != nullptr) *valueId = data[0];
}

ISpxAudioSourceBufferProperties::PropertyValue_Type CSpxAudioSourceBufferProperties::FindPropertyDataValue(uint64_t nameId, uint64_t offset, int direction, PropertyOffset_Type* foundAtOffset)
{
    const uint64_t notFound = UINT64_MAX;
    auto foundOffsetRead = notFound;
    auto foundValueIdRead = notFound;

    for (auto dataPos = FirstFindDataPos(); dataPos != StopFindDataPos(); dataPos = NextFindDataPos(dataPos))
    {
        uint64_t nameIdRead, offsetRead, valueIdRead;
        ReadPropertyData(dataPos, &nameIdRead, &offsetRead, &valueIdRead);

        if (direction == +1 && offsetRead < offset)
        {
            break; // looking right ... found an offset to the left ... we're outta here!!
        }
        else if (nameIdRead != nameId)
        {
            continue; // not a match ... keep looking ...
        }
        else if (offsetRead == offset)
        {
            foundValueIdRead = valueIdRead;
            foundOffsetRead = offsetRead;
            break; // direction independent ... found an exact offset match ... awesome!!
        }
        else if (direction == -1 && offsetRead < offset)
        {
            foundValueIdRead = valueIdRead;
            foundOffsetRead = offsetRead;
            break; // looking left ... found first offset to the left... we're outta here!
        }
        else if (direction == +1 && offsetRead > offset)
        {
            foundValueIdRead = valueIdRead;
            foundOffsetRead = offsetRead;
            continue; // looking right ... found an offset to the right ... we might find a better one, so keep looking ...
        }
    }

    auto value = ValueFromId(foundValueIdRead);
    if (value != nullptr && foundAtOffset != nullptr)
    {
        *foundAtOffset = foundOffsetRead;
    }

    return value;
}

std::list<ISpxAudioSourceBufferProperties::FoundPropertyData_Type> CSpxAudioSourceBufferProperties::FindPropertyData(uint64_t nameId, uint64_t offsetBegin, uint64_t offsetEnd)
{
    std::list<FoundPropertyData_Type> list;

    auto dataPos = FirstFindDataPos();
    while (dataPos != StopFindDataPos())
    {
        uint64_t nameIdRead, offsetRead, valueIdRead;
        ReadPropertyData(dataPos, &nameIdRead, &offsetRead, &valueIdRead);

        if (nameIdRead == nameId && offsetRead <= offsetEnd)
        {
            auto name = NameFromId(nameIdRead);
            auto value = ValueFromId(valueIdRead);
            list.push_front(std::make_tuple(offsetRead, name, value));

            bool atBegin = (offsetRead == offsetBegin);
            bool firstLeftOfBegin = (offsetRead < offsetBegin);
            if (atBegin || firstLeftOfBegin)
            {
                break;
            }
        }

        dataPos = NextFindDataPos(dataPos);
    }

    return list;
}

uint64_t CSpxAudioSourceBufferProperties::FirstFindDataPos()
{
    EnsureInitPropertyDataBuffer();
    auto writePos = m_data->GetWritePos();

    return writePos == 0
        ? StopFindDataPos()
        : (writePos - itemSize);
}

uint64_t CSpxAudioSourceBufferProperties::NextFindDataPos(uint64_t pos)
{
    SPX_IFTRUE_RETURN_X(pos == StopFindDataPos(), StopFindDataPos());

    auto size = m_data->GetSize();
    auto writePos = m_data->GetWritePos();
    auto firstReadPossible = size < writePos ? writePos - size : 0;

    return pos <= firstReadPossible ? StopFindDataPos() : (pos - itemSize);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
