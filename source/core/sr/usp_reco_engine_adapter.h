//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.h: Implementation declarations for CSpxUspRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <list>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "recognition_result.h"
#include "service_helpers.h"
#include "usp.h"

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
    void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechStartDetected(m); }); }
    void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechEndDetected(m); }); }
    void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechHypothesis(m); }); }
    void OnSpeechPhrase(const USP::SpeechPhraseMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechPhrase(m); }); }
    void OnSpeechFragment(const USP::SpeechFragmentMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnSpeechFragment(m); }); }
    void OnTurnStart(const USP::TurnStartMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnStart(m); }); }
    void OnTurnEnd(const USP::TurnEndMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTurnEnd(m); }); }
    void OnError(const std::string& error) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnError(error); }); }
    void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationHypothesis(m); }); }
    void OnTranslationPhrase(const USP::TranslationPhraseMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationPhrase(m); }); }
    void OnTranslationSynthesis(const USP::TranslationSynthesisMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationSynthesis(m); }); }
    void OnTranslationSynthesisEnd(const USP::TranslationSynthesisEndMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnTranslationSynthesisEnd(m); }); }
    void OnUserMessage(const USP::UserMsg& m) override { InvokeOnSite([=](std::shared_ptr<ISpxUspCallbacks> callback) { callback->OnUserMessage(m); }); }


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

    // --- ISpxAudioProcessor
    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;

    // --- IServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()


private:

    DISABLE_COPY_AND_MOVE(CSpxUspRecoEngineAdapter);

    void EnsureUspInit();
    void UspInitialize();
    void UspTerminate();

    USP::Client& SetUspEndpoint(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    
    USP::Client& SetUspEndpoint_Cortana(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpoint_Custom(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpoint_Intent(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpoint_Translation(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspEndpoint_DefaultSpeechService(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);

    USP::Client& SetUspRecoMode(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);
    USP::Client& SetUspAuthentication(std::shared_ptr<ISpxNamedProperties>& properties, USP::Client& client);

    SPXHR GetRecoModeFromEndpoint(const std::wstring& endpoint, USP::RecognitionMode& mode);
    SPXHR GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, USP::RecognitionMode& recoMode) const;

    void UspWrite(const uint8_t* buffer, size_t byteToWrite);
    void UspSendSpeechContext();
    void UspSendMessage(const std::string& messagePath, const std::string &buffer);
    void UspSendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size);
    void UspWriteFormat(WAVEFORMATEX* pformat);
    void UspWrite_Actual(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Flush();

    void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg&) override;
    void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg&) override;
    void OnSpeechHypothesis(const USP::SpeechHypothesisMsg&) override;
    void OnSpeechFragment(const USP::SpeechFragmentMsg&) override;
    void OnSpeechPhrase(const USP::SpeechPhraseMsg&) override;
    void OnTurnStart(const USP::TurnStartMsg&) override;
    void OnTurnEnd(const USP::TurnEndMsg&) override;
    void OnError(const std::string& error) override;
    void OnUserMessage(const USP::UserMsg&) override;

    void OnTranslationHypothesis(const USP::TranslationHypothesisMsg&) override;
    void OnTranslationPhrase(const USP::TranslationPhraseMsg&) override;
    void OnTranslationSynthesis(const USP::TranslationSynthesisMsg&) override;
    void OnTranslationSynthesisEnd(const USP::TranslationSynthesisEndMsg&) override;

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
    void SendSpeechContextMessage(std::string& speechContextMessage);

    void FireFinalResultNowOrLater(const USP::SpeechPhraseMsg& message);
    void FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson = "");

    void FireFinalResultLater(const USP::SpeechPhraseMsg& message);
    void FireFinalResultLater_WaitingForIntentComplete(const std::  string& luisJson = "");

    bool IsInteractiveMode() const { return m_recoMode == USP::RecognitionMode::Interactive; }

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

    void PrepareFirstAudioReadyState(WAVEFORMATEX* format);
    void PrepareAudioReadyState();
    void SendPreAudioMessages();

    bool ShouldResetAfterError();
    void ResetAfterError();


private:

    std::shared_ptr<ISpxUspCallbacks> m_uspCallbacks;
    std::shared_ptr<USP::Connection> m_uspConnection;

    USP::RecognitionMode m_recoMode = USP::RecognitionMode::Interactive;
    bool m_customEndpoint = false;

    #ifdef _MSC_VER
    using ReadWriteMutex_Type = std::shared_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_mutex>;
    #else
    using ReadWriteMutex_Type = std::mutex;
    using WriteLock_Type = std::unique_lock<std::mutex>;
    using ReadLock_Type = std::unique_lock<std::mutex>;
    #endif

    bool m_singleShot = false;
    SpxWAVEFORMATEX_Type m_format;
    ReadWriteMutex_Type m_stateMutex;
    AudioState m_audioState;
    UspState m_uspState;

    const size_t m_servicePreferedMilliseconds = 600;
    size_t m_servicePreferedBufferSizeSendingNow;

    const bool m_fUseBufferedImplementation = true;
    std::shared_ptr<uint8_t> m_buffer;
    size_t m_bytesInBuffer;
    uint8_t* m_ptrIntoBuffer;
    size_t m_bytesLeftInBuffer;

    bool m_expectIntentResponse = true;
    USP::SpeechPhraseMsg m_finalResultMessageToFireLater;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
