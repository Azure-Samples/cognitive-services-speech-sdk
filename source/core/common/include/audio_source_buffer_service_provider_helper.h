//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_service_provider_helper.h: Implementation declarations for CSpxAudioSourceBufferServiceProviderHelper
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "create_object_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T>
class CSpxAudioSourceBufferServiceProviderHelper
{
protected:

    ~CSpxAudioSourceBufferServiceProviderHelper()
    {
        SPX_DBG_ASSERT(m_asb == nullptr);
    }

    std::shared_ptr<ISpxInterfaceBase> QueryServiceAudioSourceBuffer(const char* serviceName)
    {
        if (PAL::stricmp(serviceName, "BufferData") == 0)
        {
            return EnsureInitAudioSourceBufferService();
        }
        else if (PAL::stricmp(serviceName, "BufferProperties") == 0)
        {
            return EnsureInitAudioSourceBufferService();
        }
        return nullptr;
    }

    std::shared_ptr<ISpxBufferData> EnsureInitAudioSourceBufferService()
    {
        return m_asb != nullptr ? m_asb : InitAudioSourceBufferService();
    }

    std::shared_ptr<ISpxBufferData> InitAudioSourceBufferService()
    {
        SPX_DBG_ASSERT(m_asb == nullptr);
        auto site = static_cast<T*>(this);
        m_asb = SpxCreateObjectWithSite<ISpxBufferData>("CSpxBufferData", site);
        return m_asb;
    }

    void TermAudioSourceBufferService()
    {
        SpxTermAndClear(m_asb);
        SPX_DBG_ASSERT(m_asb == nullptr);
    }

private:

    std::shared_ptr<ISpxBufferData> m_asb;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
