//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_reco_engine_adapter.h: Implementation declarations for CSpxRnntRecoEngineAdapter C++ class
//

#pragma once

#include <memory>
#include <cstdint>
#include <string>

#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "object_with_site_init_impl.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "audio_file_logger.h"
#include "rnnt.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxRnntCallbacks :
    public ISpxInterfaceBaseFor<ISpxRnntCallbacks>,
    public RNNT::Callbacks
{
};

class CSpxRnntCallbackWrapper final :
    public ISpxObjectWithSiteInitImpl<ISpxRnntCallbacks>,
    public ISpxRnntCallbacks
{
public:

    CSpxRnntCallbackWrapper() = default;
    ~CSpxRnntCallbackWrapper() = default;

    SPX_INTERFACE_MAP_BEGIN()
    SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
    SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
    SPX_INTERFACE_MAP_ENTRY(ISpxRnntCallbacks)
    SPX_INTERFACE_MAP_END()

    // --- ISpxRnntCallbacks (overrides)
    inline void OnSpeechStartDetected(const RNNT::SpeechStartDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnSpeechStartDetected(m); }); }
    inline void OnSpeechEndDetected(const RNNT::SpeechEndDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnSpeechEndDetected(m); }); }
    inline void OnSpeechHypothesis(const RNNT::SpeechHypothesisMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnSpeechHypothesis(m); }); }
    inline void OnSpeechPhrase(const RNNT::SpeechPhraseMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnSpeechPhrase(m); }); }
    inline void OnTurnStart(const RNNT::TurnStartMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnTurnStart(m); }); }
    inline void OnTurnEnd() final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnTurnEnd(); }); }
    inline void OnError(const std::string& errorMessage) final { InvokeOnSite([=](std::shared_ptr<ISpxRnntCallbacks> callback) { callback->OnError(errorMessage); }); }

private:

    DISABLE_COPY_AND_MOVE(CSpxRnntCallbackWrapper);
};

class CSpxRnntRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxRnntCallbacks,
    public ISpxRecoEngineAdapter
{
public:

    CSpxRnntRecoEngineAdapter();
    ~CSpxRnntRecoEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
    SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
    SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
    SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_ENTRY(ISpxRnntCallbacks)
    SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapter)
    SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()


    // --- ISpxObject
    void Init() override;
    void Term() override;

    // -- ISpxRecoEngineAdapter
    void SetAdapterMode(bool singleShot) override;
    void OpenConnection(bool singleShot) override;
    void CloseConnection() override;

    // -- ISpxAudioProcessor
    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // -- ISpxServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    using SitePtr = std::shared_ptr<ISpxRecoEngineAdapterSite>;

    DISABLE_COPY_AND_MOVE(CSpxRnntRecoEngineAdapter);

    void EnsureRnntInit();
    void RnntInitialize();
    void RnntTerminate();

    void SetRnntRecoMode(const std::shared_ptr<ISpxNamedProperties>& properties, RNNT::IRnntClientPtr& rnntClient);

    SPXHR GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, RNNT::RecognitionMode& recoMode) const;

    void ProcessAudioChunk(const DataChunkPtr& audioChunk);
    void RnntWrite(const DataChunkPtr& audioChunk);
    void FlushAudio();

    void OnSpeechStartDetected(const RNNT::SpeechStartDetectedMsg&) override;
    void OnSpeechEndDetected(const RNNT::SpeechEndDetectedMsg&) override;
    void OnSpeechHypothesis(const RNNT::SpeechHypothesisMsg&) override;
    void OnSpeechPhrase(const RNNT::SpeechPhraseMsg&) override;
    void OnTurnStart(const RNNT::TurnStartMsg&) override;
    void OnTurnEnd() override;
    void OnError(const std::string&) override;

    void FireFinalResultNow(const RNNT::SpeechPhraseMsg& message);

    ResultReason ToReason(RNNT::RecognitionStatus rnntRecognitionStatus);
    CancellationReason ToCancellationReason(RNNT::RecognitionStatus rnntRecognitionStatus);
    NoMatchReason ToNoMatchReason(RNNT::RecognitionStatus rnntRecognitionStatus);

    enum class AudioState { Idle = 0, Ready = 1, Sending = 2, Mute = 9 };

    enum class RnntState {
        Error = -1,
        Idle = 0,
        WaitingForTurnStart = 1000,
        WaitingForPhrase = 1200,
        WaitingForTurnEnd = 2999,
        Terminating = 9998,
        Zombie = 9999
    };

    bool IsBadState() const { return IsState(RnntState::Error) || IsState(RnntState::Terminating) || IsState(RnntState::Zombie); }
    bool IsState(AudioState state) const { return m_audioState == state; }
    bool IsState(RnntState state) const { return m_rnntState == state; }
    bool IsState(AudioState audioState, RnntState rnntState) const { return IsState(audioState) && IsState(rnntState); }
    bool IsStateBetweenIncluding(RnntState state1, RnntState state2) const { return m_rnntState >= state1 && m_rnntState < state2; }

    bool ChangeState(RnntState toRnntState) { return ChangeState(m_audioState, m_rnntState, m_audioState, toRnntState); }
    bool ChangeState(RnntState fromRnntState, RnntState toRnntState) { return ChangeState(m_audioState, fromRnntState, m_audioState, toRnntState); }
    bool ChangeState(AudioState toAudioState) { return ChangeState(m_audioState, m_rnntState, toAudioState, m_rnntState); }
    bool ChangeState(AudioState fromAudioState, AudioState toAudioState) { return ChangeState(fromAudioState, m_rnntState, toAudioState, m_rnntState); }
    bool ChangeState(AudioState fromAudioState, RnntState fromRnntState, AudioState toAudioState, RnntState toRnntState);

    void PrepareFirstAudioReadyState(const SPXWAVEFORMATEX* format);
    void PrepareAudioReadyState();

    bool ShouldResetAfterError();
    void ResetAfterError();

#ifdef _DEBUG
    void PrepareRnntAudioStream();
    void ProcessAudioFormat(SPXWAVEFORMATEX* pformat);
    void SetupAudioDumpFile();
    DataChunkPtr MakeDataChunkForAudioFormat(SPXWAVEFORMATEX* pformat);
    uint8_t* FormatBufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes);
    uint8_t* FormatBufferWriteNumber(uint8_t* buffer, uint32_t number);
    uint8_t* FormatBufferWriteChars(uint8_t* buffer, const char* psz, size_t cch);
#endif

private:

    std::shared_ptr<ISpxRnntCallbacks> m_rnntCallbacks;
    RNNT::IRnntClientPtr m_rnntClient;

    bool m_isInteractiveMode = false;

    const bool m_allowRnntResetAfterError = true;
    bool m_singleShot = false;
    SpxWAVEFORMATEX_Type m_format;
    AudioState m_audioState;
    RnntState m_rnntState;

#ifdef _DEBUG
    std::shared_ptr<CSpxAudioFileLogger> m_audioLogger;
#endif
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
