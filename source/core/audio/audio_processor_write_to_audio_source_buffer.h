//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_processor_write_to_audio_source_buffer.h: Implementation declarations for CSpxAudioProcessorWriteToAudioSourceBuffer
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "audio_processor_simple_impl.h"
#include "interface_helpers.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioProcessorWriteToAudioSourceBuffer :
    public ISpxGenericSite,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxSetErrorInfo,
    public ISpxServiceProvider,
    public ISpxAudioProcessorSimpleImpl
{
protected:

    using AudioProcessor_Base = ISpxAudioProcessorSimpleImpl;

public:

    CSpxAudioProcessorWriteToAudioSourceBuffer() = default;
    virtual ~CSpxAudioProcessorWriteToAudioSourceBuffer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxSetErrorInfo)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Term() override;

    // --- ISpxAudioProcessor (overrides)
    void SetFormat(const SPXWAVEFORMATEX* format) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // --- ISpxSetErrorInfo (override)

    // SetFormat(nullptr) is not always called on error and the orchestrator above may want to reset this in case of error much earlier.
    virtual void SetError(const std::string& error) override;

private:

    DISABLE_COPY_AND_MOVE(CSpxAudioProcessorWriteToAudioSourceBuffer);

    void InitFromSite();
    void Clean();

    void InitNotifyTargetFromSite();
    void CleanNotifyTarget();
    void InitBufferDataAndPropertiesFromSite();
    void CleanBufferDataAndProperties();

    void SetProperty(PropertyId id, const std::string& value);

    void NotifyTarget();

    std::shared_ptr<ISpxBufferDataWriter> m_bufferData;
    std::shared_ptr<ISpxBufferProperties> m_bufferProperties;

    std::shared_ptr<ISpxAudioProcessor> m_notifySource;
    std::shared_ptr<ISpxAudioProcessorNotifyMe> m_notifyTarget;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
