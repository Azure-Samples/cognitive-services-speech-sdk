//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_microphone.h: Implementation declarations for CSpxInteractiveMicrophone C++ class
//

#pragma once
#include "spxcore_common.h"
#include "delegate_audio_pump_impl.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxInteractiveMicrophone : public ISpxDelegateAudioPumpImpl
{
public:

    CSpxInteractiveMicrophone();
};


} // CARBON_IMPL_NAMESPACE
