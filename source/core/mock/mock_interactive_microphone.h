//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_interactive_microphone.cpp: Implementation declarations for CSpxMockInteractiveMicrophone methods
//

#include "stdafx.h"
#include "ispxinterfaces.h"
#include "delegate_audio_pump_impl.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxMockInteractiveMicrophone :
    public ISpxObjectWithSiteInitImpl<ISpxSite>,
    public ISpxDelegateAudioPumpImpl
{
public:

    CSpxMockInteractiveMicrophone();

    // --- ISpxAudioPump (overrides) ---
    void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) override;


private:

    CSpxMockInteractiveMicrophone(CSpxMockInteractiveMicrophone&&) = delete;
    CSpxMockInteractiveMicrophone(const CSpxMockInteractiveMicrophone&) = delete;
    CSpxMockInteractiveMicrophone& operator=(CSpxMockInteractiveMicrophone&&) = delete;
    CSpxMockInteractiveMicrophone& operator=(const CSpxMockInteractiveMicrophone&) = delete;

    void EnsureAudioPump();
    void InitAudioPump();

    void InitMockAudioPump();
    void InitWavFilePump(const std::wstring& fileName);

    std::wstring GetFileName();
};


} // CARBON_IMPL_NAMESPACE
