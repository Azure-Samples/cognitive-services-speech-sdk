//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_wav_file_pump.cpp: Implementation definitions for CSpxMockWavFilePump methods
//

#include "stdafx.h"
#include "mock_wav_file_pump.h"
#include "create_object_helpers.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxMockWavFilePump::StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    EnsureAudioPump();

    m_delegateToAudioPump->StartPump(pISpxAudioProcessor);
}

void CSpxMockWavFilePump::EnsureAudioPump()
{
    if (m_delegateToAudioPump == nullptr)
    {
        InitAudioPump();
    }
}

void CSpxMockWavFilePump::InitAudioPump()
{
    // Create the mock reader...
    auto reader = SpxCreateObjectWithSite<ISpxAudioStreamReader>("CSpxMockAudioReader", GetSite());

    // Create an audio pump, and set the reader
    auto pumpInit = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", GetSite());
    pumpInit->SetReader(reader);

    // And ... We're finished
    m_delegateToAudioPump = SpxQueryInterface<ISpxAudioPump>(pumpInit);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
