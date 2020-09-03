//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// CSpxHybridTtsEngineAdapter: a TTS adapter that handles auto switching between cloud and offline TTS backends.
//

#pragma once
#include <memory>
#include <utility>
#include "asyncop.h"
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "mstts.h"
#include "service_helpers.h"
#include <object_with_site_init_impl.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxHybridTtsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxTtsEngineAdapterSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxTtsEngineAdapter,
    public ISpxTtsEngineAdapterSite,
    public ISpxPropertyBagImpl
{
public:

    CSpxHybridTtsEngineAdapter();
    virtual ~CSpxHybridTtsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapterSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry) override;
    void StopSpeaking() override;

    // --- ISpxTtsEngineAdapterSite ---

    uint32_t Write(ISpxTtsEngineAdapter* adapter, const std::string& requestId, uint8_t* buffer, uint32_t size, std::shared_ptr<std::unordered_map<std::string, std::string>> properties) override;
    std::shared_ptr<ISpxSynthesizerEvents> GetEventsSite() override;
    std::shared_ptr<ISpxSynthesisResult> CreateEmptySynthesisResult() override;

    // --- ISpxServiceProvider ---

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

private:

    enum class SynthesisState {
        Idle,
        CloudSynthesizing_BufferFilling,
        CloudSynthesizing_BufferFilled,
        CloudCanceled,
        CloudSynthesized,
    };

    enum class SwitchingPolicy {
        ForceCloud,
        ForceOffline,
        CloudFirst,
        Parallel
    };

    enum class FallbackThreshold {
        None,
        Connect,
        Buffer,
        Finish
    };

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxHybridTtsEngineAdapter(const CSpxHybridTtsEngineAdapter&) = delete;
    CSpxHybridTtsEngineAdapter(const CSpxHybridTtsEngineAdapter&&) = delete;

    CSpxHybridTtsEngineAdapter& operator=(const CSpxHybridTtsEngineAdapter&) = delete;

    SpxWAVEFORMATEX_Type GetOutputFormat(bool* hasHeader) const;

    void UpdateStreamReader();

    void EnsureTtsCloudEngineAdapter();
    void InitializeTtsCloudEngineAdapter();
    void EnsureTtsOfflineEngineAdapter();
    void InitializeOfflineTtsEngineAdapter();

    static std::pair<SwitchingPolicy, FallbackThreshold> ClarifyPolicy(const std::string& policy);
    std::shared_ptr<ISpxSynthesisResult> SpeakByConnectPolicy(const std::string& text, bool isSsml,
                                                              const std::string& requestId, bool retry,
                                                              SwitchingPolicy switchingPolicy);
    std::shared_ptr<ISpxSynthesisResult> SpeakByBufferPolicy(const std::string& text, bool isSsml,
                                                             const std::string& requestId, bool retry,
                                                             SwitchingPolicy switchingPolicy);
    std::shared_ptr<ISpxSynthesisResult> SpeakByFinishPolicy(const std::string& text, bool isSsml,
                                                             const std::string& requestId, bool retry,
                                                             SwitchingPolicy switchingPolicy);

    // dummy speak when waiting for cloud result timeout.
    std::shared_ptr<ISpxSynthesisResult> DummySpeak(const std::string& requestId);

private:

    std::atomic<SynthesisState> m_synthesisState { SynthesisState::Idle };

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::shared_ptr<ISpxTtsEngineAdapter> m_ttsCloudAdapter;
    std::shared_ptr<ISpxTtsEngineAdapter> m_ttsOfflineAdapter;
    std::shared_ptr<ISpxAudioOutput> m_cloudAudioStream;
    std::shared_ptr<ISpxAudioOutput> m_offlineAudioStream;
    std::shared_ptr<ISpxAudioOutputReader> m_cloudAudioStreamReader;
    std::shared_ptr<ISpxAudioOutputReader> m_offlineAudioStreamReader;

    std::shared_ptr<ISpxAudioOutput> m_audioOutput;

    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> m_cloudResult;
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> m_lastCloudResult;
    std::shared_future<std::shared_ptr<ISpxSynthesisResult>> m_offlineResult;
    std::string m_currentRequestId;

    SwitchingPolicy m_switchingPolicy;
    FallbackThreshold m_fallbackThreshold;

    std::string originalCloudConnectedTimeoutMs;
    std::string originalCloudFinishedTimeoutMs;
    int cloudConnectedTimeoutMs;
    int cloudFinishedTimeoutMs;

    std::string m_offlineVoiceFolder;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
