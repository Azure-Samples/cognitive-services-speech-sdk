//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_kws_engine_adapter.cpp: Implementation definitions for CSpxMockKwsEngineAdapter C++ class
//

#include "stdafx.h"
#include <cstring>
#include "mock_kws_engine_adapter.h"
#include "service_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxMockKwsEngineAdapter::CSpxMockKwsEngineAdapter() :
    m_cbAudioProcessed(0),
    m_cbLastKeywordFired(0)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxMockKwsEngineAdapter::~CSpxMockKwsEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxMockKwsEngineAdapter::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
}

void CSpxMockKwsEngineAdapter::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxMockKwsEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(pformat->wFormatTag).c_str(),
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);

    SPX_IFTRUE_THROW_HR(pformat != nullptr && HasFormat(), SPXERR_ALREADY_INITIALIZED);

    if (pformat != nullptr)
    {
        InitFormat(pformat);
    }
    else
    {
        TermFormat();
        End();
    }
}

void CSpxMockKwsEngineAdapter::ProcessAudio(AudioData_Type /* data */, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE_IF(0, "%s(..., size=%d)", __FUNCTION__, size);
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    m_cbAudioProcessed += size;
    if (m_cbAudioProcessed > m_cbLastKeywordFired + m_format->nAvgBytesPerSec * 10)
    {
        FireKeywordDetected();
    }
}

void CSpxMockKwsEngineAdapter::InitFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(WAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);

    m_cbAudioProcessed = 0;
    m_cbLastKeywordFired = 0;
}

void CSpxMockKwsEngineAdapter::TermFormat()
{
    m_format = nullptr;
}

void CSpxMockKwsEngineAdapter::End()
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->DoneProcessingAudio(this);
}

void CSpxMockKwsEngineAdapter::FireKeywordDetected()
{
    SPX_DBG_TRACE_FUNCTION();
    
    m_cbLastKeywordFired = m_cbAudioProcessed;
    auto offset = (uint32_t)m_cbLastKeywordFired;
    auto site = GetSite();

    std::shared_ptr<ISpxAudioProcessor> keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    std::packaged_task<void()> task([=](){
        auto keepAliveCopy = keepAlive;
        site->KeywordDetected(this, offset);
    });

    auto taskFuture = task.get_future();
    std::thread taskThread(std::move(task));
    taskThread.detach();
}


} // CARBON_IMPL_NAMESPACE
