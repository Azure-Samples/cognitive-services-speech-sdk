//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity_event_args.cpp: Implementation definitions for CSpxActivityEventArgs C++ class.
//
#include "stdafx.h"
#include "activity_event_args.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxActivityEventArgs::CSpxActivityEventArgs()
{
}

const std::string& CSpxActivityEventArgs::GetActivity() const
{
    SPX_IFTRUE_THROW_HR(m_activity.empty(), SPXERR_UNINITIALIZED);
    return m_activity;
}

bool CSpxActivityEventArgs::HasAudio() const
{
    return m_audio != nullptr;
}

std::shared_ptr<ISpxAudioOutput> CSpxActivityEventArgs::GetAudio() const
{
    return m_audio;
}

void CSpxActivityEventArgs::Init(std::string activity)
{
    SPX_IFTRUE_THROW_HR(!m_activity.empty(), SPXERR_ALREADY_INITIALIZED);
    m_activity = std::move(activity);
}

void CSpxActivityEventArgs::Init(std::string activity, std::shared_ptr<ISpxAudioOutput> audio)
{
    SPX_IFTRUE_THROW_HR(!m_activity.empty(), SPXERR_ALREADY_INITIALIZED);
    m_activity = std::move(activity);
    m_audio = audio;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
