//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// read_write_buffer_delegate_helper.h: Implementation declarations/definitions for CSpxReadWriteBufferDelegateHelper
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxReadWriteBuffer>>
class CSpxReadWriteBufferDelegateHelper : public DelegateToHelperT
{
public:

    using Base_Type = DelegateToHelperT;
    using Delegate_Type = std::shared_ptr<ISpxReadWriteBuffer>;
    inline Delegate_Type GetReadWriteBufferDelegate ()
    {
        return Base_Type::GetDelegate();
    }

    inline void SetReadWriteBufferDelegate (Delegate_Type ptr)
    {
        Base_Type::SetDelegate(ptr);
    }

    inline bool IsReadWriteBufferDelegateZombie()
    {
        return Base_Type::IsZombie();
    }

    inline void ZombieReadWriteBufferDelegate(bool zombie = true)
    {
        Base_Type::Zombie(zombie);
    }

    inline bool IsReadWriteBufferDelegateClear()
    {
        return Base_Type::IsClear();
    }

    inline void ClearReadWriteBufferDelegate()
    {
        Base_Type::Clear();
    }

    inline bool IsReadWriteBufferDelegateReady()
    {
        return Base_Type::IsReady();
    }

    size_t DelegateGetSize()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->GetSize() : 0;
    }

    uint64_t DelegateGetInitPos()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->GetInitPos() : 0;
    }

    std::string DelegateGetName()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->GetName() : "";
    }

    void DelegateWrite(const void* data, size_t dataSizeInBytes, size_t* bytesWritten = nullptr)
    {
        auto buffer = GetReadWriteBufferDelegate();
        SPX_IFTRUE(buffer != nullptr, buffer->Write(data, dataSizeInBytes, bytesWritten));
    }

    uint64_t DelegateGetWritePos()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->GetWritePos() : 0;
    }

    void DelegateRead(void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto buffer = GetReadWriteBufferDelegate();
        SPX_IFTRUE(buffer != nullptr, buffer->Read(data, dataSizeInBytes, bytesRead));
    }

    uint64_t DelegateGetReadPos()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->GetReadPos() : 0;
    }

    uint64_t DelegateResetReadPos()
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->ResetReadPos() : 0;
    }

    void DelegateReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto buffer = GetReadWriteBufferDelegate();
        SPX_IFTRUE(buffer != nullptr, buffer->ReadAtBytePos(pos, data, dataSizeInBytes, bytesRead));
    }

    std::shared_ptr<uint8_t> DelegateReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->ReadShared(dataSizeInBytes, bytesRead) : nullptr;
    }

    std::shared_ptr<uint8_t> DelegateReadSharedAtBytePos(uint64_t pos, size_t dataSizeInBytes, size_t* bytesRead = nullptr)
    {
        auto buffer = GetReadWriteBufferDelegate();
        return buffer != nullptr ? buffer->ReadSharedAtBytePos(pos, dataSizeInBytes, bytesRead) : nullptr;
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
