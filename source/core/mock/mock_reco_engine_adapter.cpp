//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_reco_engine_adapter.cpp: Implementation definitions for CSpxMockRecoEngineAdapter C++ class
//

#include "stdafx.h"
#include <cstring>
#include <inttypes.h>
#include "mock_reco_engine_adapter.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxMockRecoEngineAdapter::CSpxMockRecoEngineAdapter() :
    m_cbAudioProcessed(0),
    m_cbFireNextIntermediate(0),
    m_cbFireNextFinalResult(0),
    m_cbFiredLastIntermediate(0),
    m_cbFiredLastFinal(0)
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

void CSpxMockRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    m_singleShot = singleShot;
}

void CSpxMockRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
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
        EnsureFireFinalResult();
        TermFormat();
        End();
    }
}

void CSpxMockRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    auto size = audioChunk->size;

    SPX_DBG_TRACE_VERBOSE_IF(0, "%s(..., size=%d)", __FUNCTION__, size);
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    m_cbAudioProcessed += size;

    auto sizeInMs = size * 1000 / m_format->nAvgBytesPerSec;
    auto sleepMs = sizeInMs * 5 / 1000; // consume .5% cpu
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));

    if (size != 0)
    {
        if (m_cbAudioProcessed >= m_cbFireNextFinalResult)
        {
            FireFinalResult();
        }
        else if (m_cbAudioProcessed >= m_cbFireNextIntermediate)
        {
            FireIntermediateResult();
        }
    }
}

void CSpxMockRecoEngineAdapter::InitFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);

    m_cbAudioProcessed = 0;

    m_cbFireNextIntermediate = m_numMsBeforeVeryFirstIntermediate * m_format->nAvgBytesPerSec / 1000;
    m_cbFireNextFinalResult = m_cbFireNextIntermediate + m_numMsBetweenFirstIntermediateAndFinal * m_format->nAvgBytesPerSec / 1000;

    FireSpeechStartDetected();
}

void CSpxMockRecoEngineAdapter::TermFormat()
{
    m_format = nullptr;
}

void CSpxMockRecoEngineAdapter::End()
{
    InvokeOnSite([this](const SitePtr& site) { site->AdapterCompletedSetFormatStop(this); });
}

void CSpxMockRecoEngineAdapter::FireIntermediateResult()
{
    m_mockResultText += m_mockResultText.empty()
        ? m_firstMockWord
        : m_eachIntermediateAddsMockWord;

    auto resultText = m_mockResultText;
    auto offset = (uint32_t)m_cbAudioProcessed;
    SPX_DBG_TRACE_VERBOSE("%s: text='%ls', offset=%d", __FUNCTION__, resultText.c_str(), offset);

    m_cbFiredLastIntermediate = offset;
    m_cbFireNextIntermediate += m_numMsBetweenIntermediates * m_format->nAvgBytesPerSec / 1000;

    InvokeOnSite([&](const SitePtr& site)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateIntermediateResult(nullptr, resultText.c_str(), offset, m_cbFireNextIntermediate - offset);
        site->FireAdapterResult_Intermediate(this, offset, result);
    });
}

void CSpxMockRecoEngineAdapter::FireFinalResult()
{
    auto resultText = m_mockResultText.empty()
        ? m_firstMockWord
        : m_mockResultText;

    auto offset = (uint32_t)m_cbAudioProcessed;
    SPX_DBG_TRACE_VERBOSE("%s: text='%ls', offset=%d", __FUNCTION__, resultText.c_str(), offset);

    m_cbFiredLastIntermediate = offset;
    m_cbFiredLastFinal = offset;

    m_cbFireNextIntermediate = offset + m_numMsBetweenFinalAndNextIntermediate * m_format->nAvgBytesPerSec / 1000;
    m_cbFireNextFinalResult = m_cbFireNextIntermediate + m_numMsBetweenFirstIntermediateAndFinal * m_format->nAvgBytesPerSec / 1000;
    m_mockResultText.clear();

    FireSpeechEndDetected();

    InvokeOnSite([&](const SitePtr& site)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateFinalResult(nullptr, ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, CancellationErrorCode::NoError, resultText.c_str(), offset, m_cbFireNextFinalResult - offset);
        site->FireAdapterResult_FinalResult(this, offset, result);
    });
}

void CSpxMockRecoEngineAdapter::EnsureFireFinalResult()
{
    SPX_DBG_TRACE_VERBOSE("%s: offset=%" PRIu64, __FUNCTION__, m_cbAudioProcessed);
    if (m_cbFiredLastIntermediate > m_cbFiredLastFinal)
    {
        FireFinalResult();
    }
}

void CSpxMockRecoEngineAdapter::FireSpeechStartDetected()
{
    SPX_DBG_TRACE_VERBOSE("%s: offset=%" PRIu64, __FUNCTION__, m_cbAudioProcessed);
    InvokeOnSite([this](const SitePtr& site)
    {
        site->AdapterDetectedSpeechStart(this, m_cbAudioProcessed);
    });
}

void CSpxMockRecoEngineAdapter::FireSpeechEndDetected()
{
    SPX_DBG_TRACE_VERBOSE("%s: offset=%" PRIu64, __FUNCTION__, m_cbAudioProcessed);

    InvokeOnSite([this](const SitePtr& site)
    {
        site->AdapterDetectedSpeechEnd(this, m_cbAudioProcessed);
    });
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
