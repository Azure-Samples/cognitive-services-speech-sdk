//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_interactive_microphone.cpp: Implementation definitions for CSpxMockInteractiveMicrophone methods
//

#include "stdafx.h"
#include "mock_interactive_microphone.h"
#include "create_object_helpers.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxMockInteractiveMicrophone::CSpxMockInteractiveMicrophone() :
    ISpxDelegateAudioPumpImpl(nullptr)
{
}

void CSpxMockInteractiveMicrophone::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    ISpxObjectWithSiteInitImpl<ISpxGenericSite>::Init();
    EnsureAudioPump();
}

void CSpxMockInteractiveMicrophone::StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    EnsureAudioPump();

    m_delegateToAudioPump->StartPump(pISpxAudioProcessor);
}

void CSpxMockInteractiveMicrophone::EnsureAudioPump()
{
    SPX_DBG_TRACE_FUNCTION();

    if (m_delegateToAudioPump == nullptr)
    {
        InitAudioPump();
    }
}

void CSpxMockInteractiveMicrophone::InitAudioPump()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_delegateToAudioPump != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Try to get the filename from our site's property collection
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto fileName = properties->GetStringValue("CARBON-INTERNAL-MOCK-WavFileAudio");

    // Create the wav file pump if we have a filename, otherwise create the mock audio pump
    fileName.empty() ? InitMockAudioPump() : InitWavFilePump(PAL::ToWString(fileName));
}

void CSpxMockInteractiveMicrophone::InitMockAudioPump()
{
    SPX_DBG_TRACE_FUNCTION();

    // Create the mock reader...
    auto reader = SpxCreateObjectWithSite<ISpxAudioStreamReader>("CSpxMockAudioReader", GetSite());

    // Create an audio pump, and set the reader
    auto pumpInit = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", GetSite());
    pumpInit->SetReader(reader);

    // Set other various properties that control the mock
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    if (!properties->HasStringValue("CARBON-INTERNAL-MOCK-RealTimeAudioPercentage"))
    {
        properties->SetStringValue("CARBON-INTERNAL-MOCK-RealTimeAudioPercentage", "100");
    }
    
    // And ... We're finished
    m_delegateToAudioPump = SpxQueryInterface<ISpxAudioPump>(pumpInit);
}

void CSpxMockInteractiveMicrophone::InitWavFilePump(const std::wstring& fileName)
{
    SPX_DBG_TRACE_FUNCTION();

    // Create the wav file pump
    auto audioFilePump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", GetSite());

    // Open the WAV file
    audioFilePump->Open(fileName.c_str());

    // Set other various properties that control the mock
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    
    audioFilePump->SetContinuousLoop(PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-MOCK-ContinuousAudio").c_str()));
    audioFilePump->SetIterativeLoop(PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-MOCK-IterativeAudio").c_str()));

    // Set other various properties that control the mock
    auto maybeProperty = properties->GetStringValue("CARBON-INTERNAL-MOCK-RealTimeAudioPercentage", "100");
    properties->SetStringValue("CARBON-INTERNAL-MOCK-WaveRealTimeAudioPercentage", maybeProperty.c_str());
    
    // and ... We're finished
    m_delegateToAudioPump = SpxQueryInterface<ISpxAudioPump>(audioFilePump);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
