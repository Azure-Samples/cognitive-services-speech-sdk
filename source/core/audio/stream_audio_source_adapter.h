//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "spxcore_common.h"
#include "audio_source_stream_pump_impl.h"
#include "audio_source_buffer_data_delegate_impl.h"
#include "audio_source_buffer_service_provider_helper.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxStreamAudioSourceAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxAudioSourceStreamPumpImpl<CSpxStreamAudioSourceAdapter>,
    public ISpxBufferDataDelegateImpl<>,
    public ISpxServiceProvider,
    public CSpxAudioSourceBufferServiceProviderHelper<CSpxStreamAudioSourceAdapter>
{
public:

    CSpxStreamAudioSourceAdapter() = default;
    virtual ~CSpxStreamAudioSourceAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceControl)
        SPX_INTERFACE_MAP_ENTRY(ISpxBufferData)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSource)
        SPX_INTERFACE_MAP_FUNC(QueryInterfaceStreamPumpImpl)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceStreamPumpImpl)
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceAudioSourceBuffer)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Term() override;

    inline uint64_t GetNewMultiReaderOffset() final { return 0; }

protected:

    DISABLE_COPY_AND_MOVE(CSpxStreamAudioSourceAdapter);

    void InitDelegatePtr(std::shared_ptr<ISpxBufferData>& ptr) override;
    void TermDelegatePtr();
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
