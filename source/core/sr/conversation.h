//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation.h: Private implementation declarations for ISpxConversation interface.
//

#pragma once
#include "recognizer.h"
#include "thread_service.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConversation :
    public ISpxConversation,
    public ISpxConversationWithImpl,
    public ISpxServiceProvider,
    public ISpxNamedProperties,
    public ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>  // needs getDefaultSession
{
public:

   SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxConversation)
        SPX_INTERFACE_MAP_ENTRY(ISpxConversationWithImpl)
   SPX_INTERFACE_MAP_END()

    CSpxConversation();
    virtual ~CSpxConversation();

      // --- ISpxObjectInit
    void Init() override;
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_FUNC(InternalQueryService)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxConversation
    void UpdateParticipant(bool add, const std::string& userId) override;
    void UpdateParticipant(bool add, const std::string& userId, std::shared_ptr<ISpxParticipant> participant) override;
    void UpdateParticipants(bool add, std::vector<ParticipantPtr>&& participants) override;
    void SetConversationId(const std::string& id) override;
    const std::string GetConversationId() const override;
    std::string GetSpeechEventPayload(MeetingState state) override;

    virtual void CreateConversation(const std::string & nickname = "") override;
    virtual void DeleteConversation() override;
    virtual void StartConversation() override;
    virtual void EndConversation() override;
    virtual void SetLockConversation(bool lock) override;
    virtual void SetMuteAllParticipants(bool mute) override;
    virtual void SetMuteParticipant(bool mute, const std::string &) override;

    // --- ISpxConversationWithImpl
    virtual std::shared_ptr<ISpxConversation> GetConversationImpl() override { return m_impl; }

    // --- ISpxNamedProperties
    std::string GetStringValue(const char* name, const char* defaultValue = "") const override;
    void SetStringValue(const char* name, const char* value) override;
    bool HasStringValue(const char* name) const override;
    void Copy(ISpxNamedProperties* from) override;

protected:
    std::shared_ptr<ISpxSession> InternalQueryService(const char* serviceName);

private:
    std::shared_ptr<ISpxRecognizerSite> m_keepSessionAlive;
    std::shared_ptr<ISpxConversation> m_impl;

    void SetRecoMode();
    void ValidateImpl() const;

    DISABLE_COPY_AND_MOVE(CSpxConversation);
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
