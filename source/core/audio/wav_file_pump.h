//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_pump.h: Implementation declarations for CSpxWavFilePump C++ class
//

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "delegate_audio_pump_impl.h"
#include "delegate_audio_file_impl.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxWavFilePump :
    public ISpxObjectWithSiteInitImpl<ISpxSite>,
    public ISpxDelegateAudioPumpImpl, 
    public ISpxDelegateAudioFileImpl
{
public:

    // Default ctor
    CSpxWavFilePump();

    // --- ISpxAudioFile overrides ---
    void Open(const wchar_t* pszFileName) override;


private:

    CSpxWavFilePump(CSpxWavFilePump&&) = delete;
    CSpxWavFilePump(const CSpxWavFilePump&) = delete;
    CSpxWavFilePump& operator=(CSpxWavFilePump&&) = delete;
    CSpxWavFilePump& operator=(const CSpxWavFilePump&) = delete;

    void EnsureFile(const wchar_t* fileName);
    void EnsurePump();
};


} // CARBON_IMPL_NAMESPACE
