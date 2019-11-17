//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_tts_engine_adapter.h: Implementation declarations for CSpxMockTtsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "asyncop.h"
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "usp_reco_engine_adapter.h"
#include "authenticator.h"
#include "usp.h"

#define METADATA_TYPE_WORD_BOUNDARY "WordBoundary"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxUspTtsEngineAdapter :
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxUspCallbacks,
    public ISpxTtsEngineAdapter,
    public ISpxPropertyBagImpl
{
public:

    CSpxUspTtsEngineAdapter();
    virtual ~CSpxUspTtsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxUspCallbacks)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::wstring& requestId) override;

    // --- IServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()


protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxUspTtsEngineAdapter(const CSpxUspTtsEngineAdapter&) = delete;
    CSpxUspTtsEngineAdapter(const CSpxUspTtsEngineAdapter&&) = delete;

    CSpxUspTtsEngineAdapter& operator=(const CSpxUspTtsEngineAdapter&) = delete;

    void GetProxySetting();

    std::shared_ptr<ISpxSynthesisResult> SpeakInternal(const std::string& text, bool isSsml, const std::wstring& requestId);

    void SetSpeechConfigMessage();
    void UspSendSpeechConfig();
    void UspSendSynthesisContext(const std::string& requestId);
    void UspSendSsml(const std::string& ssml, const std::string& requestId);
    void UspSendMessage(const std::string& messagePath, const std::string &buffer, USP::MessageType messageType, const std::string& requestId="");
    static void DoSendMessageWork(std::weak_ptr<USP::Connection> connectionPtr, const std::string& messagePath, const std::string& buffer, USP::MessageType messageType, const std::string& requestId);

    void EnsureUspConnection();
    void UspInitialize();
    void UspTerminate();

    void OnTurnStart(const USP::TurnStartMsg& message) override;
    void OnAudioOutputChunk(const USP::AudioOutputChunkMsg& message) override;
    void OnAudioOutputMetadata(const USP::AudioOutputMetadataMsg& message) override;
    void OnTurnEnd(const USP::TurnEndMsg& message) override;
    void OnError(bool transport, USP::ErrorCode errorCode, const std::string& errorMessage) override;

    SpxWAVEFORMATEX_Type GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader);
    std::string GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output);
    bool WordBoundaryEnabled() const;
    static bool InSsmlTag(size_t currentPos, const std::wstring& ssml, size_t beginningPos);
    static CancellationErrorCode UspErrorCodeToCancellationErrorCode(USP::ErrorCode uspErrorCode);


private:

    enum class UspState {
        Error = -1,
        Idle = 0,
        Connecting = 1,     // from trying to connect to request sent
        Sending = 2,
        TurnStarted = 3,    // from turn.start received to first audio message received
        ReceivingData = 4   // from first audio message received to turn.end received
    };

    std::string m_endpoint;
    bool m_isCustomHost = false;
    std::shared_ptr<ISpxAudioOutput> m_audioOutput;
    std::shared_ptr<CSpxRestTtsAuthenticator> m_authenticator;

    std::string m_proxyHost;
    int m_proxyPort { 0 };
    std::string m_proxyUsername;
    std::string m_proxyPassword;

    std::shared_ptr<ISpxThreadService> m_threadService;
    std::shared_ptr<ISpxUspCallbacks> m_uspCallbacks;
    std::shared_ptr<USP::Connection> m_uspConnection;
    std::chrono::system_clock::time_point m_lastConnectTime;

    std::string m_speechConfig;

    std::atomic<UspState> m_uspState { UspState::Idle };
    std::vector<uint8_t> m_currentReceivedData;

    std::wstring m_currentRequestId;
    std::wstring m_currentText;
    bool m_currentTextIsSsml;
    uint32_t m_currentTextOffset;

    USP::ErrorCode m_currentErrorCode { static_cast<USP::ErrorCode>(0) };
    std::string m_currentErrorMessage;

    std::mutex m_mutex;
    std::condition_variable m_cv;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
