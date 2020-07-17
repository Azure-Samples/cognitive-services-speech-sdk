//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump_delegate_helper.h: Implementation declarations/definitions for CSpxAudioPumpDelegateHelper
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxAudioPump>>
class CSpxAudioPumpDelegateHelper : public DelegateToHelperT
{
private:
    using I = ISpxAudioPump;
public:

    using State = ISpxAudioPump::State;
    SPX_DELEGATE_ACCESSORS(Pump, DelegateToHelperT, ISpxAudioPump)

    uint16_t DelegateGetFormat(SPXWAVEFORMATEX* format, uint16_t size) const
    {
        return InvokeOnDelegateR(GetPumpConstDelegate(), &I::GetFormat, 0, format, size);
    }

    void DelegateSetFormat(const SPXWAVEFORMATEX* format, uint16_t size)
    {
        InvokeOnDelegate(GetPumpDelegate(), &I::SetFormat, format, size);
    }

    void DelegateStartPump(std::shared_ptr<ISpxAudioProcessor> audioProcessor)
    {
        InvokeOnDelegate(GetPumpDelegate(), &I::StartPump, audioProcessor);
    }

    void DelegatePausePump()
    {
        InvokeOnDelegate(GetPumpDelegate(), &I::PausePump);
    }

    void DelegateStopPump()
    {
        InvokeOnDelegate(GetPumpDelegate(), &I::StopPump);
    }

    State DelegateGetState() const
    {
        return InvokeOnDelegateR(GetPumpConstDelegate(), &I::GetState, State::NoInput);
    }

    std::string DelegateGetPropertyValue(const std::string& key)
    {
        return InvokeOnDelegateR(GetPumpDelegate(), &I::GetPropertyValue, std::string{}, key);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
