//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_reco_engine_adapter.cpp: Implementation definitions for CSpxMockRecoEngineAdapter C++ class
//

#include "stdafx.h"
#include "mock_reco_engine_adapter.h"
#include "service_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxMockRecoEngineAdapter::CSpxMockRecoEngineAdapter() :
    m_cbAudioProcessed(0),
    m_cbLastIntermediateFired(0),
    m_cbLastFinalResultFired(0)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxMockRecoEngineAdapter::~CSpxMockRecoEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxMockRecoEngineAdapter::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
}

void CSpxMockRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxMockRecoEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(pformat != nullptr && HasFormat(), SPXERR_ALREADY_INITIALIZED);

    if (pformat != nullptr)
    {
        InitFormat(pformat);
    }
    else
    {
        EnsureFireFinalResult();
        TermFormat();
        End();
    }
}

void CSpxMockRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    UNUSED(data);
    UNUSED(size);

    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    m_cbAudioProcessed += size;

    if (m_cbAudioProcessed > m_cbLastFinalResultFired + m_format->nAvgBytesPerSec * 2)
    {
        FireFinalResult();
    }
    else if (m_cbAudioProcessed > m_cbLastIntermediateFired + m_format->nAvgBytesPerSec / 5)
    {
        FireIntermediateResult();
    }
}

void CSpxMockRecoEngineAdapter::InitFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(WAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);

    m_cbAudioProcessed = 0;
    m_cbLastIntermediateFired = 0;
    m_cbLastFinalResultFired = 0;
}

void CSpxMockRecoEngineAdapter::TermFormat()
{
    m_format = nullptr;
}

void CSpxMockRecoEngineAdapter::End()
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->DoneProcessingAudio(this);
}

void CSpxMockRecoEngineAdapter::FireIntermediateResult()
{
    m_mockResultText += L"mock ";

    m_cbLastIntermediateFired = m_cbAudioProcessed;

    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateIntermediateResult(nullptr,  m_mockResultText.c_str());

    auto offset = (uint32_t)m_cbLastIntermediateFired;
    GetSite()->IntermediateRecoResult(this, offset, result);
}

void CSpxMockRecoEngineAdapter::FireFinalResult()
{
    m_cbLastFinalResultFired = m_cbAudioProcessed;
    m_cbLastIntermediateFired = m_cbAudioProcessed;

    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateFinalResult(nullptr, m_mockResultText.c_str());

    auto offset = (uint32_t)m_cbLastIntermediateFired;
    GetSite()->FinalRecoResult(this, offset, result);

    m_mockResultText = L"";
}

void CSpxMockRecoEngineAdapter::EnsureFireFinalResult()
{
    if (!m_mockResultText.empty())
    {
        FireFinalResult();
    }
}


} // CARBON_IMPL_NAMESPACE
