//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// delegate_audio_pump_impl.h: Implementation declarations for ISpxDelegateAudioPumpImpl C++ class
//

#pragma once
#include "spxcore_common.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxDelegateAudioPumpImpl : public ISpxAudioPump
{
    // --- ISpxAudioPump

    uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) override { return m_delegateToAudioPump->GetFormat(pformat, cbFormat); }
    void SetFormat(const WAVEFORMATEX* pformat, uint16_t cbFormat) override { m_delegateToAudioPump->SetFormat(pformat, cbFormat); }

    void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) override { m_delegateToAudioPump->StartPump(pISpxAudioProcessor); }
    void PausePump() override { m_delegateToAudioPump->PausePump(); }
    void StopPump() override { m_delegateToAudioPump->StopPump(); }

    State GetState() override { return m_delegateToAudioPump->GetState(); }


protected:

    ISpxDelegateAudioPumpImpl() { }
    ISpxDelegateAudioPumpImpl(std::shared_ptr<ISpxAudioPump> delegateTo) : m_delegateToAudioPump(delegateTo) { }

    ISpxDelegateAudioPumpImpl(ISpxDelegateAudioPumpImpl&&) = delete;
    ISpxDelegateAudioPumpImpl(const ISpxDelegateAudioPumpImpl&) = delete;
    ISpxDelegateAudioPumpImpl& operator=(ISpxDelegateAudioPumpImpl&&) = delete;
    ISpxDelegateAudioPumpImpl& operator=(const ISpxDelegateAudioPumpImpl&) = delete;

    ~ISpxDelegateAudioPumpImpl() { m_delegateToAudioPump = nullptr; }

    std::shared_ptr<ISpxAudioPump> m_delegateToAudioPump;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
