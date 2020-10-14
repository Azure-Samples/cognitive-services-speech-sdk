//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// voice_profile_client.cpp: Private implementation declarations for voice_profile_client
//
#include "stdafx.h"
#include "voice_profile_client.h"
#include "voice_profile.h"
#include "create_object_helpers.h"
#include "service_helpers.h"
#include "log_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

CSpxVoiceProfileClient::~CSpxVoiceProfileClient()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SpxTermAndClear(m_keepSiteAlive);
    m_keepSiteAlive = nullptr;
}

CSpxVoiceProfileClient::CSpxVoiceProfileClient()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxVoiceProfileClient::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    m_keepSiteAlive = GetSite();
    CheckLogFilename();
}

void CSpxVoiceProfileClient::CheckLogFilename()
{
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_keepSiteAlive);
    SpxDiagLogSetProperties(namedProperties);
}

void CSpxVoiceProfileClient::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

std::shared_ptr<ISpxVoiceProfile> CSpxVoiceProfileClient::CreateVoiceProfile(VoiceProfileType voice_profile_type, std::string&& locale) const
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    auto site = GetSite();
    if (site == nullptr)
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    auto http = SpxQueryInterface<ISpxSpeakerRecognition>(site);
    if (http == nullptr)
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return http->CreateVoiceProfile(voice_profile_type, std::move(locale));
}

RecognitionResultPtr CSpxVoiceProfileClient::Verify(VoiceProfileType type, std::string&& profileId)
{
    RecognitionResultPtr result;
    InvokeOnSite([type = type, id = std::move(profileId), &result](const SitePtr& site) mutable {
        vector<string> ids{ move(id) };
        result = site->RecognizeVoiceProfile(type, move(ids));
    });

    return result;
}

RecognitionResultPtr CSpxVoiceProfileClient::Identify(std::vector<VoiceProfilePtr>&& profiles)
{
    RecognitionResultPtr result;
    InvokeOnSite([profiles = std::move(profiles), &result](const SitePtr& site) mutable {
        std::vector<std::string> ids;
        for (const auto& profile : profiles)
        {
            ids.emplace_back(profile->GetProfileId());
        }
        result = site->RecognizeVoiceProfile(VoiceProfileType::TextIndependentIdentification, std::move(ids));
    });

    return result;
}

RecognitionResultPtr CSpxVoiceProfileClient::ModifyVoiceProfile(ModifyOperation opereation, VoiceProfileType type, std::string&& id)
{
    RecognitionResultPtr result;
    InvokeOnSite([id = id, op = opereation, type = type, &result](const SitePtr& site) mutable {
        result = site->ModifyVoiceProfile(op, type, move(id));
    });

    return result;
}

std::vector<VoiceProfilePtr> CSpxVoiceProfileClient::GetVoiceProfiles(VoiceProfileType type) const
{
    std::vector<VoiceProfilePtr> voiceProfiles;
    InvokeOnSite([type, &voiceProfiles](const SitePtr& site)  {
        voiceProfiles = site->GetVoiceProfiles(type);
        });
    return voiceProfiles;
}

RecognitionResultPtr CSpxVoiceProfileClient::Enroll(VoiceProfileType type, std::string&& profileId)
{
    RecognitionResultPtr result;
    InvokeOnSite([id = move(profileId), type = type, &result](const SitePtr& site) mutable {
        result = site->EnrollVoiceProfile(type, move(id));
        });

    return result;
}

VoiceProfilePtr CSpxVoiceProfileClient::GetVoiceProfileStatus(VoiceProfileType type, std::string&& voiceProfileId) const
{
    VoiceProfilePtr voiceProfile;
    InvokeOnSite([id = move(voiceProfileId), type = type, &voiceProfile](const SitePtr& site) mutable {
        voiceProfile = site->GetVoiceProfileStatus(type, move(id));
    });
    return voiceProfile;
}

std::shared_ptr<ISpxSpeakerRecognition> CSpxVoiceProfileClient::InternalQueryService(const char* service_name)
{
    if (!service_name)
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    if (PAL::stricmp(SpxTypeName(ISpxSpeakerRecognition), service_name) == 0)
    {
        return SpxQueryService<ISpxSpeakerRecognition>(GetSite());
    }

    return nullptr;
}

std::shared_ptr<ISpxNamedProperties> CSpxVoiceProfileClient::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}
 }}}}
