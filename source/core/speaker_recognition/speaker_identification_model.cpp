//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speaker_identification_model.cpp: implementation for CSpxSpeakerIdentificationModel
//
#include "stdafx.h"
#include "speaker_identification_model.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxSpeakerIdentificationModel::AddProfile(const std::shared_ptr<ISpxVoiceProfile>& profile)
{
    m_profiles.emplace_back(profile);
}

std::vector<std::shared_ptr<ISpxVoiceProfile>> CSpxSpeakerIdentificationModel::GetProfiles() const
{
    return m_profiles;
}

}}}}
