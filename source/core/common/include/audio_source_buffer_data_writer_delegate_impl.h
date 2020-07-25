//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_data_writer_delegate_impl.h: Implementation declarations/definitions for ISpxAudioSourceBufferDataWriterDelegateImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxBufferDataWriter>>
class ISpxBufferDataWriterDelegateImpl :
    public DelegateToHelperT,
    public ISpxBufferDataWriter
{
private:
    using I = ISpxBufferDataWriter;
    using C = ISpxBufferDataWriterDelegateImpl;
public:

    void Write(uint8_t* buffer, uint32_t size) override
    {
        InvokeOnDelegate(C::GetDelegate(), &I::Write, buffer, size);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
