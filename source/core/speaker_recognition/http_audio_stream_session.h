//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_audio_stream_session.h: implementation for CSpxHttpAudioStreamSession
//
#pragma once

#include <chrono>
#include "ispxinterfaces.h"
#include "service_helpers.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

// forward declare of CSpxThreadService.
class CSpxThreadService;

class CSpxHttpAudioStreamSession :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioStreamSessionInit,
    public ISpxAudioProcessor,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxHttpRecoEngineAdapterSite,
    public ISpxAudioPumpSite,
    public ISpxRecoResultFactory,
    public ISpxPropertyBagImpl,
    public ISpxHttpAudioStreamSession
{
public:

    CSpxHttpAudioStreamSession() = default;
    DISABLE_COPY_AND_MOVE(CSpxHttpAudioStreamSession);
    virtual ~CSpxHttpAudioStreamSession() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPumpSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamSessionInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoResultFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxHttpAudioStreamSession)
    SPX_INTERFACE_MAP_END()

    // --- IServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY(ISpxHttpAudioStreamSession)
        SPX_SERVICE_MAP_ENTRY(ISpxRecoResultFactory)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())

    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- ISpxRecoResultFactory
    virtual RecognitionResultPtr CreateIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration) override;
    virtual RecognitionResultPtr CreateFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t* userId = nullptr) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateEndOfStreamResult() override;

    // --- ISpxAudioStreamSessionInit
    void InitFromFile(const wchar_t* pszFileName) override;
    void InitFromMicrophone() override;
    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) override;

    // --- ISpxAudioProcessor
    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // --- ISpxAudioPumpSite
    void Error(const std::string& msg) override;

    // --- ISpxHttpAudioStreamSession
    std::string CreateVoiceProfile(VoiceProfileType type, std::string&& locale) override;
    RecognitionResultPtr ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id) override;
    RecognitionResultPtr StartStreamingAudioAndWaitForResult(bool enroll, VoiceProfileType type, std::vector<std::string>&& profileIds) override;

private:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;
    static uint32_t FromBytesToMilisecond(uint32_t bytes, uint32_t bytesPerSecond);
    void StopPump();
    std::chrono::milliseconds GetMicrophoneTimeout();

    std::shared_ptr<ISpxAudioPump> m_audioPump;
    std::shared_ptr<ISpxGenericSite> m_keepFactoryAlive;

    std::shared_ptr<std::promise<RecognitionResultPtr>> m_audioIsDone;

    std::shared_ptr<CSpxThreadService> m_threadService;
    std::shared_ptr<ISpxAudioStreamReader> m_codecAdapter;
    bool m_fromMicrophone;

    std::chrono::milliseconds m_microphoneTimeoutInMS = (std::chrono::milliseconds)7000;
    std::shared_ptr<ISpxHttpRecoEngineAdapter> m_reco;

    std::packaged_task<void()> CreateTask(std::function<void()> func);
    void CleanupAfterEachAudioPumping();

    uint32_t m_avgBytesPerSecond = 16000*2;
    uint32_t m_totalAudioinMS = 0;
    std::mutex m_mutex;
};

 }}}}
