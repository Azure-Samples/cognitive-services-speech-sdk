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


namespace CARBON_IMPL_NAMESPACE() {

class CSpxUspRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter,
    protected USP::Callbacks
{
public:

    CSpxUspRecoEngineAdapter();
    ~CSpxUspRecoEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()


    // --- ISpxObject

    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;

protected:

    // --- USP::Callbacks
    virtual void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message) override;
    virtual void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message) override;
    virtual void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message) override;
    virtual void OnSpeechFragment(const USP::SpeechFragmentMsg& message) override;
    virtual void OnSpeechPhrase(const USP::SpeechPhraseMsg& message) override;
    virtual void OnTurnStart(const USP::TurnStartMsg& message) override;
    virtual void OnTurnEnd(const USP::TurnEndMsg& message) override;
    virtual void OnError(const std::string& error) override;

    virtual void OnUserMessage(const std::string& path, const std::string& contentType, const uint8_t* buffer, size_t size) override;

    // Todo: translaton messages should be in a spearate class than UspRecoEngineAdapter. This will
    // be done at a later time during refactoring and adding TTS support.
    virtual void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& message) override;
    virtual void OnTranslationPhrase(const USP::TranslationPhraseMsg& message) override;
    virtual void OnTranslationSynthesis(const USP::TranslationSynthesisMsg& message) override;


private:

    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&) = delete;
    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&&) = delete;

    CSpxUspRecoEngineAdapter& operator=(const CSpxUspRecoEngineAdapter&) = delete;

    void UspInitialize();

    USP::Client& SetUspEndpoint(USP::Client& client);
    USP::Client& SetUspRecoMode(USP::Client& client);
    USP::Client& SetUspModelId(USP::Client& client);
    USP::Client& SetUspLanguage(USP::Client& client);
    USP::Client& SetUspAuthentication(USP::Client& client);

    void UspWrite(const uint8_t* buffer, size_t byteToWrite);
    void UspSendSpeechContext();
    void UspSendMessage(const std::string& messagePath, const std::string &buffer);
    void UspSendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size);
    void UspWriteFormat(WAVEFORMATEX* pformat);
    void UspWrite_Actual(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Flush();

    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(const USP::TurnStartMsg& message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this
    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(const USP::TurnEndMsg& message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this

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

    inline void SaveToWavInit()
    {
        // std::string filename = "uspaudiodump_" + std::to_string(m_saveToWavCounter++) + ".wav";
        // PAL::fopen_s(&m_saveToWavAudioFile, filename.c_str(), "wb");
    }

    inline void SaveToWavWrite(const uint8_t* /* buffer */, size_t /* bytesToWrite */)
    {
        // if (m_saveToWavAudioFile != nullptr)
        // {
        //     fwrite(buffer, 1, bytesToWrite, m_saveToWavAudioFile);
        // }
    }

    inline void SaveToWavClose()
    {
        // if (m_hfile != nullptr)
        // {
        //     fclose(m_saveToWavAudioFile);
        //     m_saveToWavAudioFile = nullptr;
        // }
    }

    std::list<std::string> GetListenForListFromSite();
    std::string GetDgiJsonFromListenForList(std::list<std::string>& listenForList);

    void GetIntentInfoFromSite(std::string& provider, std::string& id, std::string& key);
    std::string GetIntentJsonFromIntentInfo(const std::string& provider, const std::string& id, const std::string& key);

    std::string GetSpeechContextJson(const std::string& dgiJson, const std::string& intentJson);
    void SendSpeechContextMessage(std::string& speechContextMessage);

    void FireFinalResultNow(const USP::SpeechPhraseMsg& message, const std::string& luisJson = "");
    void FireFinalResultLater(const USP::SpeechPhraseMsg& message);
    void FireFinalResultLater_WaitingForIntentComplete(const std::string& luisJson = "");

    enum class AudioState { Idle = 0, ReadyToProcess = 1, ProcessingAudio = 2, Paused = 3, WaitingForDone = 4 };

    enum class UspState {
        Error = -1,
        Idle = 0,
        ReadyForFormat = 1,
        SendingAudioData = 999,
        TurnStarted = 1000,
        SpeechStarted = 1200,
        SpeechHypothesis = 1250,
        SpeechEnded = 1299,
        ReceivedSpeechResult = 2000,
        WaitingForIntent = 2500,
        FiredFinalResult = 2999,
        TurnEnded = 9900,
        Terminating = 9998,
        Zombie = 9999
    };

    // --------------------------------------------------------------------------------------------------------------
    //
    //      NOTE: The following table is not intended to be complete. It is for example purposes only.
    //
    // --------------------------------------------------------------------------------------------------------------

    // Idle/Idle                               ==> Init()                      ==> Idle/ReadyForFormat

    // ANYTHING/ANYTHING(not Zombie)           ==> Term()                      ==> ANYTHING/Terminating
    //                                                                         ==> ANYTHING/Zombie

    // Idle/ReadyForFormat                     ==> SetFormat(!nullptr)         ==> ReadyToProcess/SendingAudioData
    // ReadyToProcess/SendingAudioData         ==> ProcessAudio(..., !0)       ==> ProcessingAudio/SendingAudioData
    // ANYTHING/ANYTHING(not Zombie)           ==> SetFormat(nullptr)          ==> WaitingForDone/ANYTHING

    // ProcessingAudio/SendingAudioData        ==> OnTurnStart()               ==> ProcessingAudio/TurnStarted
    // ProcessingAudio/TurnStarted             ==> OnSpeechStartDetected()     ==> ProcessingAudio/SpeechStarted
    // ProcessingAudio/SpeechStarted           ==> OnSpeechHypothesis()        ==> ProcessingAudio/SpeechHypothesis

    // ProcessingAudio/SendingAudioData        ==> OnSpeechEndDetected()       ==> Paused/SpeechEnded
    // ProcessingAudio/TurnStarted             ==> OnSpeechEndDetected()       ==> Paused/SpeechEnded
    // ProcessingAudio/SpeechStarted           ==> OnSpeechEndDetected()       ==> Paused/SpeechEnded
    // ProcessingAudio/SpeechHypothesis        ==> OnSpeechEndDetected()       ==> Paused/SpeechEnded

    // WaitingForDone/SendingAudioData         ==> OnSpeechEndDetected()       ==> WaitingForDone/SpeechEnded
    // WaitingForDone/TurnStarted              ==> OnSpeechEndDetected()       ==> WaitingForDone/SpeechEnded
    // WaitingForDone/SpeechStarted            ==> OnSpeechEndDetected()       ==> WaitingForDone/SpeechEnded
    // WaitingForDone/SpeechHypothesis         ==> OnSpeechEndDetected()       ==> WaitingForDone/SpeechEnded

    // Paused/SpeechEnded                      ==> OnSpeechPhrase()            ==> Paused/ReceivedSpeechResult
    // Paused/ReceivedSpeechResult             --> FireFinalResultNow()        ==> Paused/FiredFinalResult
    // Paused/ReceivedSpeechResult             --> FireFinalResultLater()      ==> Paused/WaitingForIntent
    // Paused/WaitingForIntent                 ==> OnUserMessage('response')
    //                                         ==> FireFinalResultNow()        ==> Paused/FiredFinalResult
    // Paused/FiredFinalResult                 ==> OnTurnEnd()                 ==> ProcessingAudio/SendingAudioData

    // WaitingForDone/SpeechEnded              ==> OnSpeechPhrase()            ==> WaitingForDone/ReceivedSpeechResult
    // WaitingForDone/ReceivedSpeechResult     --> FireFinalResultNow()        ==> WaitingForDone/FiredFinalResult
    // WaitingForDone/ReceivedSpeechResult     --> FireFinalResultLater()      ==> WaitingForDone/WaitingForIntent
    // WaitingForDone/WaitingForIntent         ==> OnUserMessage('response')
    //                                         ==> FireFinalResultNow()        ==> WaitingForDone/FiredFinalResult
    // WaitingForDone/FiredFinalResult         ==> OnTurnEnd()                 ==> Idle/ReadyForFormat

    bool IsState(AudioState state) const { return m_audioState == state; }
    bool IsState(UspState state) const { return m_uspState == state; }
    bool IsStateBetween(UspState state1, UspState state2) const { return m_uspState > state1 && m_uspState < state2; }

    bool ChangeState(UspState state);
    bool ChangeState(UspState fromState, UspState toState);
    bool ChangeState(AudioState state);
    bool ChangeState(AudioState from, AudioState to);
    bool ChangeState(AudioState fromAudioState, UspState fromUspState, AudioState toAudioState, UspState toUspState);


private:

    USP::ConnectionPtr m_handle;

    #ifdef _MSC_VER
    using ReadWriteMutex_Type = std::shared_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_mutex>;
    #else
    using ReadWriteMutex_Type = std::mutex;
    using WriteLock_Type = std::unique_lock<std::mutex>;
    using ReadLock_Type = std::unique_lock<std::mutex>;
    #endif

    ReadWriteMutex_Type m_stateMutex;
    AudioState m_audioState;
    UspState m_uspState;

    const size_t m_servicePreferedMilliseconds = 600;
    size_t m_servicePreferedBufferSize;

    const bool m_fUseBufferedImplementation = true;
    std::shared_ptr<uint8_t> m_buffer;
    size_t m_bytesInBuffer;
    uint8_t* m_ptrIntoBuffer;
    size_t m_bytesLeftInBuffer;

    FILE* m_saveToWavAudioFile = nullptr;
    int static m_saveToWavCounter;

    bool m_expectIntentResponse = true;
    USP::SpeechPhraseMsg m_finalResultMessageToFireLater;
};


} // CARBON_IMPL_NAMESPACE
