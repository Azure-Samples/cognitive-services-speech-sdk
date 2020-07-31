//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <memory>
#include <list>

#include <interfaces/base.h>
#include <interfaces/types.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxBufferData : public ISpxInterfaceBaseFor<ISpxBufferData>
{
public:

    virtual uint64_t GetOffset() = 0;
    virtual uint64_t GetNewMultiReaderOffset() = 0;

    virtual uint32_t Read(uint8_t* buffer, uint32_t size) = 0;
    virtual uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) = 0;

    virtual uint64_t GetBytesDead() = 0;
    virtual uint64_t GetBytesRead() = 0;
    virtual uint64_t GetBytesReady() = 0;
    virtual uint64_t GetBytesReadyMax() = 0;
};

class ISpxBufferDataWriter : public ISpxInterfaceBaseFor<ISpxBufferDataWriter>
{
public:
    virtual void Write(uint8_t* buffer, uint32_t size) = 0;
};

class ISpxBufferProperties : public ISpxInterfaceBaseFor<ISpxBufferProperties>
{
public:
    using PropertyName_Type = std::shared_ptr<const char>;
    using PropertyValue_Type = std::shared_ptr<const char>;
    using FoundPropertyData_Type = std::tuple<OffsetType, PropertyName_Type, PropertyValue_Type>;

    virtual void SetBufferProperty(const char* name, const char* value) = 0;

    virtual PropertyValue_Type GetBufferProperty(const char* name, const char* defaultValue = nullptr) = 0;
    virtual PropertyValue_Type GetBufferProperty(const char* name, OffsetType offset, int direction = -1, OffsetType* foundAtOffset = nullptr) = 0;

    virtual std::list<FoundPropertyData_Type> GetBufferProperties(const char* name, OffsetType begin, OffsetType end) = 0;
    virtual std::list<FoundPropertyData_Type> GetBufferProperties(OffsetType begin, OffsetType end) = 0;
};


} } } }