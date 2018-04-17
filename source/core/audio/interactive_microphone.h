//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_microphone.h: Implementation declarations for CSpxInteractiveMicrophone C++ class
//

#pragma once
#include "spxcore_common.h"
#include "delegate_audio_pump_impl.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxInteractiveMicrophone :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxDelegateAudioPumpImpl
{
public:

    CSpxInteractiveMicrophone();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
