//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_microphone_pump_impl.h: Implementation declarations for ISpxAudioSourceMicrophonePumpImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "audio_source_init_not_impl.h"
#include "audio_source_control_adapts_audio_pump_impl.h"
#include "audio_source_simple_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
class ISpxAudioSourceMicrophonePumpImpl :
    public ISpxAudioSourceInitNotImpl,
    public ISpxAudioSourceControlAdaptsAudioPumpImpl<T>
{
public:

    ISpxAudioSourceMicrophonePumpImpl() = default;

    void InitFromMicrophone() final
    {
        ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::InitMicrophonePump("CSpxInteractiveMicrophone");
    }

protected:
    inline void* QueryInterfaceMicrophonePumpImpl(const char* interfaceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryInterfacePumpImpl(interfaceName);
    }

    inline std::shared_ptr<ISpxInterfaceBase> QueryServiceMicrophonePumpImpl(const char* serviceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryServicePumpImpl(serviceName);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
