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
#include "interface_helpers.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxWavFilePump :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxDelegateAudioPumpImpl,
    public ISpxDelegateAudioFileImpl
{
public:

    // Default ctor
    CSpxWavFilePump();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioFile)
    SPX_INTERFACE_MAP_END()

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


} } } } // Microsoft::CognitiveServices::Speech::Impl
