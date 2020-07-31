//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// microphone_audio_source_adapter.h: Implementation declarations for CSpxMicrophoneAudioSourceAdapter
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "audio_source_microphone_pump_impl.h"
#include "audio_source_buffer_data_delegate_impl.h"
#include "audio_source_buffer_service_provider_helper.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxMicrophoneAudioSourceAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxAudioSourceMicrophonePumpImpl<CSpxMicrophoneAudioSourceAdapter>,
    public ISpxBufferDataDelegateImpl<>,
    public ISpxServiceProvider,
    public CSpxAudioSourceBufferServiceProviderHelper<CSpxMicrophoneAudioSourceAdapter>
{
public:

    CSpxMicrophoneAudioSourceAdapter() = default;
    virtual ~CSpxMicrophoneAudioSourceAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceControl)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferData)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSource)
        SPX_INTERFACE_MAP_FUNC(QueryInterfaceMicrophonePumpImpl)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceMicrophonePumpImpl)
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceAudioSourceBuffer)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    /* ISpxBufferData */
    inline uint64_t GetOffset() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::GetOffset();
    }

    inline uint64_t GetNewMultiReaderOffset() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return 0;
    }

    inline uint32_t Read(uint8_t* buffer, uint32_t size) final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::Read(buffer, size);
    }

    inline uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::ReadAt(offset, buffer, size);
    }

    inline uint64_t GetBytesDead() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::GetBytesDead();
    }

    inline uint64_t GetBytesRead() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::GetBytesRead();
    }
    inline uint64_t GetBytesReady() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::GetBytesReady();
    }

    inline uint64_t GetBytesReadyMax() final
    {
        ThrowHRIfTrue(!IsInitialized(), SPXERR_UNINITIALIZED);
        return ISpxBufferDataDelegateImpl::GetBytesReadyMax();
    }

    // --- ISpxObjectInit (overrides)
    void Term() override;
protected:

    DISABLE_COPY_AND_MOVE(CSpxMicrophoneAudioSourceAdapter);

    void InitDelegatePtr(std::shared_ptr<ISpxBufferData>& ptr) override;
    void TermDelegatePtr();
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
