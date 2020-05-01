//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speaker_verification_model.cpp: implementation for CSpxSpeakerVerificationModel
//
#include "stdafx.h"
#include "speaker_verification_model.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxSpeakerVerificationModel::InitModel(const std::shared_ptr<ISpxVoiceProfile>& profile)
{
    m_profile = profile;
}

std::shared_ptr<ISpxVoiceProfile> CSpxSpeakerVerificationModel::GetProfile() const
{
    return m_profile;
}

}}}}
