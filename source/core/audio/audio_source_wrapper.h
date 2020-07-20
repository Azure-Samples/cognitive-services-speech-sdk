//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_wrapper.h: Implementation declarations/definitions for CSpxAudioSourceWrapper
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "audio_source_simple_impl.h"
#include "audio_source_buffer_data_delegate_impl.h"
#include "audio_source_buffer_data_writer_delegate_impl.h"
#include "audio_source_control_delegate_impl.h"
#include "audio_source_init_delegate_impl.h"
#include "interface_helpers.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioSourceWrapper :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxAudioSource,
    public ISpxAudioSourceInitDelegateImpl<>,
    public ISpxAudioSourceControlDelegateImpl<>,
    public ISpxAudioSourceBufferDataDelegateImpl<>,
    public ISpxAudioSourceBufferDataWriterDelegateImpl<>
{
public:

    CSpxAudioSourceWrapper() = default;
    virtual ~CSpxAudioSourceWrapper();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSource)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceControl)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferData)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioSourceBufferDataWriter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxAudioSourceInit (overrides)
    void InitFromMicrophone() final;
    void InitFromFile(const wchar_t* pszFileName) final;
    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) final;

    inline uint64_t GetNewMultiReaderOffset() final { return 0; }

    State GetState() const final;

    inline SpxWaveFormatEx GetFormat() const final
    {
        auto ptr = SpxQueryInterfaceFromDelegate<ISpxAudioSource>(static_cast<const ISpxAudioSourceInitDelegateImpl&>(*this));
        return ptr->GetFormat();
    }

protected:

    std::shared_ptr<ISpxAudioSourceInit> InitAudioSourceDelegate();
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceInit>& ptr) override;
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceControl>& ptr) override;
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferData>& ptr) override;
    void InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferDataWriter>& ptr) override;
    void TermAudioSourceDelegate();

private:

    DISABLE_COPY_AND_MOVE(CSpxAudioSourceWrapper);

    void InitAudioSourceClassName(const char* className);

    std::string m_audioSourceClassName;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
