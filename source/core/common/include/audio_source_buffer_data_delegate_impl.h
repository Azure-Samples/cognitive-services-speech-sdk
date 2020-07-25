//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_data_delegate_impl.h: Implementation declarations/definitions for ISpxAudioSourceBufferDataDelegateImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxBufferData>>
class ISpxBufferDataDelegateImpl :
    public DelegateToHelperT,
    public ISpxBufferData
{
private:
    using I = ISpxBufferData;
    using C = ISpxBufferDataDelegateImpl;
public:

    uint64_t GetOffset() override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::GetOffset, 0);
    }

    uint32_t Read(uint8_t* buffer, uint32_t size) override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::Read, 0, buffer, size);
    }

    uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::ReadAt, 0, offset, buffer, size);
    }

    uint64_t GetBytesDead() override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::GetBytesDead, 0);
    }

    uint64_t GetBytesRead() override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::GetBytesRead, 0);
    }

    uint64_t GetBytesReady() override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::GetBytesReady, 0);
    }

    uint64_t GetBytesReadyMax() override
    {
        return InvokeOnDelegateR(C::GetDelegate(), &I::GetBytesReadyMax, 0);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
