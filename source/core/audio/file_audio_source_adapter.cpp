//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// file_audio_source_adapter.cpp: Implementation definitions for CSpxFileAudioSourceAdapter
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "file_audio_source_adapter.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxFileAudioSourceAdapter::~CSpxFileAudioSourceAdapter()
{
    SPX_DBG_ASSERT(GetSite() == nullptr);
    SPX_DBG_ASSERT(ISpxAudioSourceFilePumpImpl::GetState() == State::Idle);
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());
}

void CSpxFileAudioSourceAdapter::Term()
{
    TermPump();
    TermDelegatePtr();
    TermAudioSourceBufferService();
}

void CSpxFileAudioSourceAdapter::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferData>& ptr)
{
    ptr = EnsureInitAudioSourceBufferService();
}

void CSpxFileAudioSourceAdapter::TermDelegatePtr()
{
    using DelegateType = ISpxAudioSourceBufferDataDelegateImpl;
    SPX_IFTRUE_RETURN(DelegateType::IsClear());

    SpxTermAndClearDelegate(static_cast<DelegateType&>(*this));
    SPX_DBG_ASSERT(DelegateType::IsClear());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
