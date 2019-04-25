//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.h: Implementation declarations for CSpxUspRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <list>
#include <chrono>
#include <map>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "recognition_result.h"
#include "service_helpers.h"
#include "usp.h"
#include "activity_session.h"

#ifdef _MSC_VER
#include <shared_mutex>
#endif // _MSC_VER

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxUspCallbacks :
    public ISpxInterfaceBaseFor<ISpxUspCallbacks>,
    public USP::Callbacks
{
};

class CSpxUspCallbackWrapper final :
    public ISpxObjectWithSiteInitImpl<ISpxUspCallbacks>,
    public ISpxUspCallbacks
{
public:

    CSpxUspCallbackWrapper() = default;
    ~CSpxUspCallbackWrapper() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxUspCallbacks)
    SPX_INTERFACE_MAP_END()

    // --- ISpxUspCallbacks (overrides)
    inline void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechStartDetected(m); }); }
    inline void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechEndDetected(m); }); }
    inline void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechHypothesis(m); }); }
    inline void OnSpeechPhrase(const USP::SpeechPhraseMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechPhrase(m); }); }
    inline void OnSpeechFragment(const USP::SpeechFragmentMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechFragment(m); }); }
    inline void OnTurnStart(const USP::TurnStartMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnStart(m); }); }
    inline void OnTurnEnd(const USP::TurnEndMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnEnd(m); }); }
    inline void OnMessageStart(const USP::TurnStartMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnMessageStart(m); }); }
    inline void OnMessageEnd(const USP::TurnEndMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnMessageEnd(m); }); }
    inline void OnError(bool transport, USP::ErrorCode errorCode, const std::string& errorMessage) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnError(transport, errorCode, errorMessage); }); }
    inline void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationHypothesis(m); }); }
    inline void OnTranslationPhrase(const USP::TranslationPhraseMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationPhrase(m); }); }
    inline void OnAudioOutputChunk(const USP::AudioOutputChunkMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnAudioOutputChunk(m); }); }
    inline void OnUserMessage(const USP::UserMsg& m) final { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnUserMessage(m); }); }
    inline void OnConnected() final { InvokeOnSite([](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnConnected(); }); }
    inline void OnDisconnected() final { InvokeOnSite([](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnDisconnected(); }); }


private:

    DISABLE_COPY_AND_MOVE(CSpxUspCallbackWrapper);
};


class CSpxUspRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxUspCallbacks,
    public ISpxRecoEngineAdapter
{
public:

    CSpxUspRecoEngineAdapter();
    ~CSpxUspRecoEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxUspCallbacks)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()


    // --- ISpxObject
    void Init() override;
    void Term() override;

    // --- ISpxRecoEngineAdapter
    void SetAdapterMode(bool singleShot) override;
    void OpenConnection(bool singleShot) override;
    void CloseConnection() override;
    void WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency) override;
    void SendAgentMessage(const std::string &buffer) final;

    // --- ISpxAudioProcessor
    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // --- IServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:
    using SitePtr = std::shared_ptr<ISpxRecoEngineAdapterSite>;

    DISABLE_COPY_AND_MOVE(CSpxUspRecoEngineAdapter);

    void EnsureUspInit();
    void UspInitialize();
    void UspTerminate();

    USP::Client& SetUspEndpoint(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointCortana(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointIntent(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointTranslation(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointDefaultSpeechService(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointBot(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspRegion(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client, bool isIntentRegion);
    USP::Client& SetUspAuthentication(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspProxyInfo(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspOutputFormat(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspSingleTrustedCert(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);

    SPXHR GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, USP::RecognitionMode& recoMode) const;

    void SetSpeechConfigMessage(const ISpxNamedProperties& properties);
    void SetAgentConfigMessage(const ISpxNamedProperties& properties);

    void UspWrite(const DataChunkPtr& audioChunk);
    void UspSendSpeechConfig();
    void UspSendAgentConfig();
    void UspSendSpeechContext();
    void UspSendMessage(const std::string& messagePath, const std::string &buffer, USP::MessageType messageType);
    void UspSendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, USP::MessageType messageType);
    void UspWriteFormat(SPXWAVEFORMATEX* pformat);
    void UspWriteActual(const DataChunkPtr& audioChunk);
    void UspWriteFlush();
    void UspResetConnection();
    void UspClearReconnectCache();

    void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg&) override;
    void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg&) override;
    void OnSpeechHypothesis(const USP::SpeechHypothesisMsg&) override;
    void OnSpeechFragment(const USP::SpeechFragmentMsg&) override;
    void OnSpeechPhrase(const USP::SpeechPhraseMsg&) override;
    void OnTurnStart(const USP::TurnStartMsg&) override;
    void OnTurnEnd(const USP::TurnEndMsg&) override;
    void OnMessageStart(const USP::TurnStartMsg&) final;
    void OnMessageEnd(const USP::TurnEndMsg&) final;
    void OnError(bool transport, USP::ErrorCode, const std::string& error) override;
    void OnUserMessage(const USP::UserMsg&) override;
    void OnConnected() override;
    void OnDisconnected() override;

    void OnTranslationHypothesis(const USP::TranslationHypothesisMsg&) override;
    void OnTranslationPhrase(const USP::TranslationPhraseMsg&) override;
    void OnAudioOutputChunk(const USP::AudioOutputChunkMsg&) override;

    uint8_t* FormatBufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes);
    uint8_t* FormatBufferWriteNumber(uint8_t* buffer, uint32_t number);
    uint8_t* FormatBufferWriteChars(uint8_t* buffer, const char* psz, size_t cch);
    uint32_t EndianConverter(uint32_t number)
    {
        return ((uint32_t)(number & 0x000000ff) << 24) |
               ((uint32_t)(number & 0x0000ff00) << 8) |
               ((uint32_t)(number & 0x00ff0000) >> 8) |
               ((uint32_t)(number & 0xff000000) >> 24);
    }

    std::list<std::string> GetListenForListFromSite();
    std::string GetDgiJsonFromListenForList(std::list<std::string>& listenForList);

    void GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key, std::string& region);
    std::string GetLanguageUnderstandingJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key, const std::string& region);

    std::string GetSpeechContextJson(const std::string& dgiJson, const std::string& LanguageUnderstandingJson);

    void FireActivityResult(std::shared_ptr<ISpxActivity> activity, std::shared_ptr<ISpxAudioOutput> audio);
    void FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson = "");
    void FireFinalResultLater(const USP::SpeechPhraseMsg& message);
    void FireFinalResultLater_WaitingForIntentComplete(const std::  string& luisJson = "");
    ResultReason ToReason(USP::RecognitionStatus uspRecognitionStatus);
    CancellationReason ToCancellationReason(USP::RecognitionStatus uspRecognitionStatus);
    NoMatchReason ToNoMatchReason(USP::RecognitionStatus uspRecognitionStatus);

    enum class AudioState { Idle = 0, Ready = 1, Sending = 2, Mute = 9 };

    enum class UspState {
        Error = -1,
        Idle = 0,
        WaitingForTurnStart = 1000,
        WaitingForPhrase = 1200,
        WaitingForIntent = 1250,
        WaitingForIntent2 = 1299,
        WaitingForTurnEnd = 2999,
        Terminating = 9998,
        Zombie = 9999
    };

    bool IsBadState() const { return IsState(UspState::Error) || IsState(UspState::Terminating) || IsState(UspState::Zombie); }
    bool IsState(AudioState state) const { return m_audioState == state; }
    bool IsState(UspState state) const { return m_uspState == state; }
    bool IsState(AudioState audioState, UspState uspState) const { return IsState(audioState) && IsState(uspState); }
    bool IsStateBetween(UspState state1, UspState state2) const { return m_uspState > state1 && m_uspState < state2; }
    bool IsStateBetweenIncluding(UspState state1, UspState state2) const { return m_uspState >= state1 && m_uspState < state2; }

    bool ChangeState(UspState toUspState) { return ChangeState(m_audioState, m_uspState, m_audioState, toUspState); }
    bool ChangeState(UspState fromUspState, UspState toUspState) { return ChangeState(m_audioState, fromUspState, m_audioState, toUspState); }
    bool ChangeState(AudioState toAudioState) { return ChangeState(m_audioState, m_uspState, toAudioState, m_uspState); }
    bool ChangeState(AudioState fromAudioState, AudioState toAudioState) { return ChangeState(fromAudioState, m_uspState, toAudioState, m_uspState); }
    bool ChangeState(AudioState toAudioState, UspState toUspState) { return ChangeState(m_audioState, m_uspState, toAudioState, toUspState); }
    bool ChangeState(AudioState fromAudioState, UspState fromUspState, AudioState toAudioState, UspState toUspState);

    void PrepareFirstAudioReadyState(const SPXWAVEFORMATEX* format);
    void PrepareAudioReadyState();
    void SendPreAudioMessages();

    bool ShouldResetAfterError();
    void ResetAfterError();

    bool ShouldResetAfterTurnStopped();
    void ResetAfterTurnStopped();

    bool ShouldResetBeforeFirstAudio();
    void ResetBeforeFirstAudio();


private:
    friend CSpxActivitySession;

    static constexpr auto s_defaultRecognitionLanguage = "en-us";

    std::shared_ptr<ISpxUspCallbacks> m_uspCallbacks;
    std::shared_ptr<USP::Connection> m_uspConnection;

    bool m_isInteractiveMode = false;
    std::string m_speechConfig{};
    std::string m_agentConfig{};
    bool m_customEndpoint = false;
    USP::EndpointType m_endpointType = USP::EndpointType::Speech;

    const bool m_allowUspResetAfterAudioByteCount = true;
    const size_t m_resetUspAfterAudioSeconds = 2 * 60; // 2 minutes
    uint64_t m_resetUspAfterAudioByteCount;
    uint64_t m_uspAudioByteCount;

    const bool m_allowUspResetAfterTime = true;
    const size_t m_resetUspAfterTimeSeconds = 4 * 60; // 4 minutes
    std::chrono::system_clock::time_point m_uspInitTime;
    std::chrono::system_clock::time_point m_uspResetTime;

    const bool m_allowUspResetAfterError = true;
    bool m_singleShot = false;
    SpxWAVEFORMATEX_Type m_format;
    AudioState m_audioState;
    UspState m_uspState;

    bool m_expectIntentResponse = false;
    USP::SpeechPhraseMsg m_finalResultMessageToFireLater;

    constexpr static size_t c_initialReconnectWaitingTimeMs = 10; // 10 milliseconds
    constexpr static  size_t c_reconnectWaitingTimeThreasholdMs = 2000; // 2 sec
    std::string m_botConversationId;
    size_t m_reconnectWaitingTimeMs = 10;

    std::map<std::string, std::unique_ptr<CSpxActivitySession>> m_request_session_map;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
