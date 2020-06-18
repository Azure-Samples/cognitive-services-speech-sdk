//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// voice_profile.h : Private implementation declarations for ISpxVoiceProfile.
//

#pragma once

#include "ispxinterfaces.h"
#include "factory_helpers.h"
#include "interface_helpers.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxVoiceProfile :
    public ISpxVoiceProfile,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxVoiceProfile)
   SPX_INTERFACE_MAP_END()

    CSpxVoiceProfile() : m_profile_type(VOICE_PROFILE_TYPE_NONE) {};
    virtual ~CSpxVoiceProfile() = default;

    CSpxVoiceProfile(std::string&& profile_id, VoiceProfileType type) :
        m_profile_id{ std::move(profile_id) },
        m_profile_type {type}
    {
    }

    // --- ISpxVoiceProfile
    void SetProfileId(std::string&& id) override;

    std::string GetProfileId() const override;

    VoiceProfileType GetType() const override;

private:

    std::string m_profile_id;
    VoiceProfileType m_profile_type;
};

}}}}


