//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
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
class ISpxAudioSourceStreamPumpImpl :
    public ISpxAudioSourceInitNotImpl,
    public ISpxAudioSourceControlAdaptsAudioPumpImpl<T>
{
public:

    ISpxAudioSourceStreamPumpImpl() = default;

    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) final
    {
        ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::InitAudioStreamPump("CSpxAudioPump", stream);
    }

protected:
    inline void* QueryInterfaceStreamPumpImpl(const char* interfaceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryInterfacePumpImpl(interfaceName);
    }

    inline std::shared_ptr<ISpxInterfaceBase> QueryServiceStreamPumpImpl(const char* serviceName)
    {
        return ISpxAudioSourceControlAdaptsAudioPumpImpl<T>::QueryServicePumpImpl(serviceName);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
