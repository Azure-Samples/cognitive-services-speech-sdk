//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_wav_file_pump.h: Implementation definitions for CSpxMockWavFilePump C++ class
//

#pragma once
#include "stdafx.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "mock_audio_file_impl.h"
#include "mock_audio_reader_impl.h"
#include "delegate_audio_pump_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockWavFilePump : 
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxMockAudioFileImpl,
    public ISpxDelegateAudioPumpImpl
{
public:

    CSpxMockWavFilePump() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioFile)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioReaderRealTime)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioPump (overrides) ---
    void StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor) override;


private:

    CSpxMockWavFilePump(CSpxMockWavFilePump&&) = delete;
    CSpxMockWavFilePump(const CSpxMockWavFilePump&) = delete;
    CSpxMockWavFilePump& operator=(CSpxMockWavFilePump&&) = delete;
    CSpxMockWavFilePump& operator=(const CSpxMockWavFilePump&) = delete;

    void EnsureAudioPump();
    void InitAudioPump();

    void InitMockAudioPump();
    void InitWavFilePump(const std::wstring& fileName);

};


} } } } // Microsoft::CognitiveServices::Speech::Impl
