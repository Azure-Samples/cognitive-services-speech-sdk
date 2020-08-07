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
#include "json.h"
#include "recognition_result.h"
#include "service_helpers.h"
#include "audio_file_logger.h"
#include "usp.h"
#include "activity_session.h"
#include "json.h"
#include <object_with_site_init_impl.h>

#ifdef _MSC_VER
#include <shared_mutex>
#endif // _MSC_VER

#define KEYWORDS_PROPERTY_NAME "SPEECH-KeywordsToDetect"

class CSpxUspRecoEngineAdapterTest;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxUspRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public USP::ISpxUspCallbacks,
    public ISpxRecoEngineAdapter,
    public ISpxPropertyBagImpl
{
public:

    CSpxUspRecoEngineAdapter();
    ~CSpxUspRecoEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        using namespace USP;
        SPX_INTERFACE_MAP_ENTRY(ISpxUspCallbacks)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
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
    void SendSpeechEventMessage(std::string&& msg) override;
    std::future<bool> SendNetworkMessage(std::string&& path, std::string&& msg) override;
    std::future<bool> SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& msg) override;

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

    USP::Client& SetUspEndpoint(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointIntent(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointTranslation(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointDefaultSpeechService(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointDialog(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpointTranscriber(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspRegion(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client, bool isIntentRegion);
    USP::Client& SetUspRecoMode(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspAuthentication(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUserDefinedHttpHeaders(USP::Client& client);
    USP::Client& SetUspProxyInfo(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspSingleTrustedCert(const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    void SetUspQueryParametersInternal(const char *queryParamName, const ISpxNamedProperties *properties, USP::Client& client) const;

    template<size_t N>
    USP::Client& SetUspQueryParameters(const std::array<const char *, N>& allowedParameterList, const std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client)
    {
        for (auto queryParamName : allowedParameterList)
        {
            SetUspQueryParametersInternal(queryParamName, properties.get(), client);
        }
        return client;
    }

    void UpdateOutputFormatOption(const std::shared_ptr<ISpxNamedProperties>& properties);
    void UpdateDefaultLanguage(const std::shared_ptr<ISpxNamedProperties>& properties);
    SPXHR GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, USP::RecognitionMode& recoMode) const;

    void SetSpeechConfigMessage(const ISpxNamedProperties& properties);
    void SetAgentConfigMessage(const ISpxNamedProperties& properties);

    void ProcessAudioFormat(SPXWAVEFORMATEX* pformat);
    void ProcessAudioChunk(const DataChunkPtr& audioChunk);
    void UspSendSpeechConfig();
    void UspSendAgentConfig();
    void UspSendSpeechContext();
    void UspSendSpeechEvent();
    void UspSendSpeechAgentContext();
    std::future<bool> UspSendMessage(const std::string& messagePath, const std::string &buffer, USP::MessageType messageType);
    std::future<bool> UspSendMessage(std::unique_ptr<USP::Message> message);
    std::future<bool> GetFalseFuture();
    void UspWriteActual(const DataChunkPtr& audioChunk);
    void FlushAudio(bool flushCodec = false);

    void OnMessageReceived(const USP::RawMsg&) override;
    void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg&) override;
    void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg&) override;
    void OnSpeechHypothesis(const USP::SpeechHypothesisMsg&) override;
    void OnSpeechFragment(const USP::SpeechFragmentMsg&) override;
    void OnSpeechKeywordDetected(const USP::SpeechKeywordDetectedMsg&) override;
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
    nlohmann::json GetDgiJsonFromListenForList(std::list<std::string>& listenForList);

    void GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key, std::string& region);
    nlohmann::json GetLanguageUnderstandingJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key, const std::string& region);
    nlohmann::json GetSpeechContextJson();
    nlohmann::json GetKeywordDetectionJson();
    nlohmann::json GetLanguageIdJson(std::vector<std::string> sourceLanguages, std::string onSuccessAction);
    nlohmann::json GetPhraseDetectionJson(bool doTranslation, std::unordered_map<std::string, std::string> languageToEndpointIdMap);
    nlohmann::json GetPhraseOutputJson(bool needSpeechMessages);
    nlohmann::json GetTranslationJson(std::vector<std::string>&& targetLangs, bool synthesis);
    nlohmann::json GetSynthesisJson(std::unordered_map<std::string, std::string>&& voiceNameMap);
    std::vector<std::string> GetLanguages(PropertyId propertyId);
    std::unordered_map<std::string, std::string> GetPerLanguageSetting(const std::vector<std::string>& languages, PropertyId propertyId);
    void FireActivityResult(std::string activity, std::shared_ptr<ISpxAudioOutput> audio);
    void FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson = "");
    void FireFinalResultLater(const USP::SpeechPhraseMsg& message);
    void FireFinalResultLater_WaitingForIntentComplete(const std::  string& luisJson = "");

    ResultReason ToReason(USP::RecognitionStatus uspRecognitionStatus);
    CancellationReason ToCancellationReason(USP::RecognitionStatus uspRecognitionStatus);
    NoMatchReason ToNoMatchReason(USP::RecognitionStatus uspRecognitionStatus);

    std::pair<std::string, std::string> GetLeftRightContext();
    std::pair<std::string, std::string> GetAnySpeechContext();

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

    SPXHR PrepareCompressionCodec(const SPXWAVEFORMATEX* format, ISpxInternalAudioCodecAdapter::SPXCompressedDataCallback dataCallback);
    void HandleCompressedAudioData(const uint8_t* outData, size_t nBytesOut);

    void PrepareFirstAudioReadyState(const SPXWAVEFORMATEX* format);
    void PrepareAudioReadyState();
    void PrepareUspAudioStream();
    void SendPreAudioMessages();

    bool ShouldResetAfterError();
    void ResetAfterError();

    bool ShouldResetAfterTurnStopped();
    void ResetAfterTurnStopped();

    bool ShouldResetBeforeFirstAudio();
    void ResetBeforeFirstAudio();

    void CreateConversationResult(std::shared_ptr<ISpxRecognitionResult>& result, const std::wstring& userId, const std::wstring& utteranceId);

#ifdef _DEBUG
    void SetupAudioDumpFile();
#endif

    DataChunkPtr MakeDataChunkForAudioFormat(SPXWAVEFORMATEX* pformat);

    CSpxStringMap GetParametersFromUser(std::string&& path);

    USP::MessageType GetMessageType(const std::string& path);

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

private:
    friend CSpxActivitySession;
    friend CSpxUspRecoEngineAdapterTest;

    static constexpr auto s_defaultRecognitionLanguage = "en-us";

    std::shared_ptr<ISpxUspCallbacks> m_uspCallbacks;
    std::shared_ptr<USP::Connection> m_uspConnection;

    bool m_isInteractiveMode = false;
    std::string m_speechConfig{};
    std::string m_agentConfig{};
    bool m_customEndpoint = false;
    bool m_customHost = false;
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
    bool m_audioFormatSent = true;
    bool m_audioFlushed = false;
    std::shared_ptr<ISpxInternalAudioCodecAdapter> m_compressionCodec;
    AudioState m_audioState;
    UspState m_uspState;

    bool m_expectIntentResponse = false;
    USP::SpeechPhraseMsg m_finalResultMessageToFireLater;

    std::string m_dialogConversationId;

    std::map<std::string, std::unique_ptr<CSpxActivitySession>> m_request_session_map;

    std::unordered_map<std::string, Microsoft::CognitiveServices::Speech::USP::MessageType> m_message_name_to_type_map;

#ifdef _DEBUG
    std::shared_ptr<CSpxAudioFileLogger> m_audioLogger;
#endif
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
