//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// voice_profile.cpp: implementation declarations for  CSPXVoiceProfile
//

#include "stdafx.h"


#include "voice_profile.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxVoiceProfile::SetProfileId(std::string&& id)
{
    m_profile_id = id;
}

std::string CSpxVoiceProfile::GetProfileId() const
{
    return m_profile_id;
}

VoiceProfileType CSpxVoiceProfile::GetType() const
{
    return m_profile_type;
}

void CSpxVoiceProfile::SetType(VoiceProfileType type)
{
    m_profile_type = type;
}


 }}}}
