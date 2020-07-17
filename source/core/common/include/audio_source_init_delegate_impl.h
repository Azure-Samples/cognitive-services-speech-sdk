//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_init_delegate_impl.h: Implementation declarations/definitions for ISpxAudioSourceInitDelegateImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxAudioSourceInit>>
class ISpxAudioSourceInitDelegateImpl :
    public DelegateToHelperT,
    public ISpxAudioSourceInit
{
private:
    using I = ISpxAudioSourceInit;
protected:
    SPX_DELEGATE_ACCESSORS(SourceInit, DelegateToHelperT, I)
public:

    void InitFromMicrophone() override
    {
        InvokeOnDelegate(GetSourceInitDelegate(), &I::InitFromMicrophone);
    }

    void InitFromFile(const wchar_t* filename) override
    {
        InvokeOnDelegate(GetSourceInitDelegate(), &I::InitFromFile, filename);
    }

    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) override
    {
        InvokeOnDelegate(GetSourceInitDelegate(), &I::InitFromStream, stream);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
