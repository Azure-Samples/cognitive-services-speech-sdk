//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_interactive_microphone.cpp: Implementation declarations for CSpxMockInteractiveMicrophone methods
//

#include "stdafx.h"
#include "ispxinterfaces.h"
#include "delegate_audio_pump_impl.h"
#include "interface_helpers.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockInteractiveMicrophone :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxDelegateAudioPumpImpl
{
public:

    CSpxMockInteractiveMicrophone();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

    void Init() override;

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


} } } } // Microsoft::CognitiveServices::Speech::Impl
