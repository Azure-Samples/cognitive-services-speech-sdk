//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// microphone_audio_source_adapter.cpp: Implementation definitions for CSpxMicrophoneAudioSourceAdapter
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "microphone_audio_source_adapter.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxMicrophoneAudioSourceAdapter::~CSpxMicrophoneAudioSourceAdapter()
{
    TermPump();
    TermDelegatePtr();
    TermAudioSourceBufferService();
    SPX_DBG_ASSERT(GetSite() == nullptr);
    SPX_DBG_ASSERT(ISpxAudioSourceMicrophonePumpImpl::GetState() == State::Idle);
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());
}

void CSpxMicrophoneAudioSourceAdapter::Term()
{
}

void CSpxMicrophoneAudioSourceAdapter::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferData>& ptr)
{
    ptr = EnsureInitAudioSourceBufferService();
}

void CSpxMicrophoneAudioSourceAdapter::TermDelegatePtr()
{
    SPX_IFTRUE_RETURN(IsDataDelegateClear());

    auto ptr = GetDataDelegate();

    ZombieDataDelegate(true);
    ClearDataDelegate();
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());

    SpxTermAndClear(ptr);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
