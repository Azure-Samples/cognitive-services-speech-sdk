//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_transcriber.h: Private implementation declarations for conversation transcriber.
//

#pragma once
#include "recognizer.h"
#include "thread_service.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConversationTranscriber :
    public CSpxRecognizer,
    public ISpxConversationTranscriber,
    public ISpxGetAudioConfig
{
public:
    using BaseType = CSpxRecognizer;

    CSpxConversationTranscriber();
    virtual ~CSpxConversationTranscriber();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammarList)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammar)
        SPX_INTERFACE_MAP_ENTRY(ISpxMessageParamFromUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxGetUspMessageParamsFromUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranscriber)
        SPX_INTERFACE_MAP_ENTRY(ISpxGetAudioConfig)
    SPX_INTERFACE_MAP_END()

      // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxConversationTranscriber
    virtual void Init(std::weak_ptr<ISpxAudioConfig> audio_config) override;
    virtual void JoinConversation(std::weak_ptr<ISpxConversation> conversation) override;
    virtual void LeaveConversation() override;

    // -- ISpxGetAudioConfig
    virtual std::shared_ptr<ISpxAudioConfig> GetAudioConfig() override;

    // -- ISpxRecognizerEvents
    void FireSessionStarted(const std::wstring& sessionId) override;
    void FireSessionStopped(const std::wstring& sessionId) override;

    void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) override;
    void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) override;

    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;

    // --- ISpxRecognizer
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;

private:

    std::shared_ptr<ISpxRecognizerSite> CheckAndGetSite();
    void CheckSite(const ISpxRecognizerSite * site);
    void FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset = 0);

    std::weak_ptr<ISpxAudioConfig> m_audioInput;
    std::weak_ptr<ISpxConversation> m_conversation;

    // Flag indicating participants have left conversation. If it is true, no events are sending out to outside.
    std::atomic<bool> m_has_participant{ false };

    DISABLE_COPY_AND_MOVE(CSpxConversationTranscriber);
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
