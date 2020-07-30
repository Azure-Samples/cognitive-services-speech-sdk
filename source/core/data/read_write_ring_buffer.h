//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "object_init_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxReadWriteRingBuffer :
    public ISpxObjectInitImpl,
    public ISpxReadWriteBufferInit,
    public ISpxReadWriteBuffer
{
public:

    CSpxReadWriteRingBuffer();
    ~CSpxReadWriteRingBuffer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxReadWriteBufferInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxReadWriteBuffer)
    SPX_INTERFACE_MAP_END()

    // --- ISpxReadWriteBufferInit
    size_t SetSize(size_t size) override;
    void SetInitPos(uint64_t pos) override;
    void AllowOverflow(bool allow) override;
    void SetName(const std::string& name) override;
    void Term() override;

    // --- ISpxReadWriteBuffer
    size_t GetSize() const override;
    uint64_t GetInitPos() const override;
    std::string GetName() const override;

    void Write(const void* data, size_t dataSizeInBytes, size_t* bytesWritten = nullptr) override;
    uint64_t GetWritePos() const override;

    void Read(void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) override;
    uint64_t GetReadPos() const override;
    uint64_t ResetReadPos() override;

    void ReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) override;

    std::shared_ptr<uint8_t> ReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr) override;
    std::shared_ptr<uint8_t> ReadSharedAtBytePos(uint64_t pos, size_t dataSizeInBytes, size_t* bytesRead = nullptr) override;

private:

    DISABLE_COPY_AND_MOVE(CSpxReadWriteRingBuffer);

    void ThrowExceptionIfNotInitialized() const;

    size_t SetSizeInternal(size_t size);
    size_t SetZeroRingSize();
    size_t SetNonZeroRingSize(size_t size);

    void EnsureSpaceToWrite(size_t *bytesToWrite, const size_t *bytesActuallyWritten);
    void EnsureSpaceToReadAtPos(uint64_t pos, size_t* bytesToRead, const size_t* bytesActuallyRead);

    void ConvertPosToRingPtr(uint64_t pos, void** ptr);

    void InternalWriteToRing(void const* data, size_t bytesToWrite, size_t* bytesActuallyWritten);
    void InternalWriteToRingPtr(void const* data, size_t bytesToWrite, size_t* bytesActuallyWritten, void* ptr);

    void InternalReadFromRing(void* data, size_t bytesToRead, size_t* bytesActuallyRead);
    void InternalReadFromRingPtr(void* data, size_t bytesToRead, size_t* bytesActuallyRead, void* ptr);
    void UpdateReadPosition(size_t bytesRead);

private:

    mutable std::mutex m_mutex;

    size_t m_ringSize;
    std::string m_ringName;

    bool m_allowOverflow;
    uint64_t m_initPos;
    uint64_t m_writePos;
    uint64_t m_readPos;

    SpxSharedUint8Buffer_Type m_ring;
    uint8_t* m_ptr1;
    uint8_t* m_ptr2;

    uint8_t* m_writePtr;
    uint8_t* m_readPtr;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl