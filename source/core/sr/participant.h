//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// participant.h: Private implementation declarations for participant
//
#pragma once

#include "ispxinterfaces.h"
#include "service_helpers.h"
#include "interface_helpers.h"
#include "user.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxParticipant :
    public CSpxUser,
    public ISpxParticipant
{
public:

    CSpxParticipant();
    virtual ~CSpxParticipant();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxParticipant)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit


    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxParticipant
    virtual void SetPreferredLanguage(std::string&& preferredLanguage) override;
    virtual void SetVoiceSignature(std::string&& voiceSignature) override;

    virtual inline std::string GetPreferredLanguage() const override;
    virtual inline std::string GetVoiceSignature() const override;
    virtual inline std::string GetId() const override;

private:

    std::string m_preferred_language;
    std::string m_voice;

    DISABLE_COPY_AND_MOVE(CSpxParticipant);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
