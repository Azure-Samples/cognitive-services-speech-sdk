//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "stream_audio_source_adapter.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxStreamAudioSourceAdapter::~CSpxStreamAudioSourceAdapter()
{
    SPX_DBG_ASSERT(GetSite() == nullptr);
    SPX_DBG_ASSERT(ISpxAudioSourceStreamPumpImpl::GetState() == State::Idle);
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());
}

void CSpxStreamAudioSourceAdapter::Term()
{
    TermPump();
    TermDelegatePtr();
    TermAudioSourceBufferService();
}

void CSpxStreamAudioSourceAdapter::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferData>& ptr)
{
    ptr = EnsureInitAudioSourceBufferService();
}

void CSpxStreamAudioSourceAdapter::TermDelegatePtr()
{
    SPX_IFTRUE_RETURN(IsDataDelegateClear());

    auto ptr = GetDataDelegate();

    ZombieDataDelegate(true);
    ClearDataDelegate();
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());

    SpxTermAndClear(ptr);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
