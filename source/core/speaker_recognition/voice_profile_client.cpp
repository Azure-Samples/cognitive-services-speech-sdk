//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// voice_profile_client.cpp: Private implementation declarations for voice_profile_client
//
#include "stdafx.h"
#include "voice_profile_client.h"
#include "voice_profile.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxVoiceProfileClient::~CSpxVoiceProfileClient()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxVoiceProfileClient::CSpxVoiceProfileClient()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxVoiceProfileClient::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    m_keepSiteAlive = GetSite();
}

void CSpxVoiceProfileClient::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    m_keepSiteAlive = nullptr;
}

std::shared_ptr<ISpxVoiceProfile> CSpxVoiceProfileClient::Create(VoiceProfileType voice_profile_type, std::string&& locale)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    auto site = GetSite();
    if (site == nullptr)
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    auto http = SpxQueryInterface<ISpxHttpAudioStreamSession>(site);
    if (http == nullptr)
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    return std::make_shared<CSpxVoiceProfile>(http->CreateVoiceProfile(voice_profile_type, std::move(locale)), voice_profile_type);
}

RecognitionResultPtr CSpxVoiceProfileClient::ProcessProfileAction(Action actionType, VoiceProfileType type, std::string&& profileId)
{
    RecognitionResultPtr result;
    InvokeOnSite([actionType = actionType, type = type, id = std::move(profileId), &result](const SitePtr& site) mutable {
        switch (actionType)
        {
            case Action::Enroll:
            {
                std::vector<std::string> ids{ id };
                result = site->StartStreamingAudioAndWaitForResult(true, type, std::move(ids));
            }
            break;
            case Action::Verify:
            {
                if (type == VOICE_PROFILE_TYPE_NONE)
                {
                    ThrowInvalidArgumentException("Voice profile type should not be none in speaker verification!");
                }
                std::vector<std::string> ids{ id };
                result = site->StartStreamingAudioAndWaitForResult(false, type, std::move(ids));
            }
            break;
            case Action::Delete:
                result = site->ModifyVoiceProfile(false, type, std::move(id));
            break;
            case Action::Reset:
                result = site->ModifyVoiceProfile(true, type, std::move(id));
            break;
        }
    });

    return result;
}

RecognitionResultPtr CSpxVoiceProfileClient::Identify(std::vector<std::shared_ptr<ISpxVoiceProfile>>&& profiles)
{
    RecognitionResultPtr result;
    InvokeOnSite([profiles = std::move(profiles), &result](const SitePtr& site) mutable {
        std::vector<std::string> ids;
        for (const auto& profile : profiles)
        {
            ids.emplace_back(profile->GetProfileId());
        }
        result = site->StartStreamingAudioAndWaitForResult(false, VoiceProfileType::TextIndepdentIdentification, std::move(ids));
    });

    return result;
}

std::shared_ptr<ISpxHttpAudioStreamSession> CSpxVoiceProfileClient::InternalQueryService(const char* service_name)
{
    if (!service_name)
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    if (PAL::stricmp(SpxTypeName(ISpxHttpAudioStreamSession), service_name) == 0)
    {
        return SpxQueryService<ISpxHttpAudioStreamSession>(GetSite());
    }

    return nullptr;
}

std::shared_ptr<ISpxNamedProperties> CSpxVoiceProfileClient::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}
 }}}}
