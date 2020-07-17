//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_file_pump_impl.h: Implementation declarations for ISpxAudioSourceFilePumpImpl
//

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
class ISpxAudioSourceFilePumpImpl :
    public ISpxAudioSourceInitNotImpl,
    public ISpxAudioSourceControlAdaptsAudioPumpImpl<T>
{
public:

    ISpxAudioSourceFilePumpImpl() = default;

    inline void InitFromFile(const wchar_t* filename) final
    {
        ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::InitAudioFilePump("CSpxWavFilePump", filename);
    }

protected:
    inline void* QueryInterfaceFilePumpImpl(const char* interfaceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryInterfacePumpImpl(interfaceName);
    }

    inline std::shared_ptr<ISpxInterfaceBase> QueryServiceFilePumpImpl(const char* serviceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryServicePumpImpl(serviceName);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
