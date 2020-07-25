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
    SPX_DBG_ASSERT(ISpxBufferDataDelegateImpl::IsClear());
}

void CSpxMicrophoneAudioSourceAdapter::Term()
{
}

void CSpxMicrophoneAudioSourceAdapter::InitDelegatePtr(std::shared_ptr<ISpxBufferData>& ptr)
{
    ptr = EnsureInitAudioSourceBufferService();
}

void CSpxMicrophoneAudioSourceAdapter::TermDelegatePtr()
{
    using DelegateType = ISpxBufferDataDelegateImpl;
    SPX_IFTRUE_RETURN(DelegateType::IsClear());
    SpxTermAndClearDelegate(static_cast<DelegateType&>(*this));
    SPX_DBG_ASSERT(DelegateType::IsClear());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
