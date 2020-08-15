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
    m_cbFireFinal(0),
    m_cbResultStartsAt(0),
    m_cbFireIntermediate(0),
    m_numIntermediatesFired(0)
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
    SPX_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
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
        if (m_cbAudioProcessed >= m_cbFireFinal)
        {
            FireFinalResult();
        }
        else if (m_cbAudioProcessed >= m_cbFireIntermediate && m_numIntermediatesFired < m_numIntermediates)
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

    m_cbFireFinal = (m_numMsFinalEvery - m_numMsFinalOffset + m_numMsFinalDuration + m_numMsFinalUPL) * m_format->nAvgBytesPerSec / 1000;
    m_cbResultStartsAt = (m_numMsFinalEvery - m_numMsFinalOffset) * m_format->nAvgBytesPerSec / 1000;
    m_cbFireIntermediate = m_cbResultStartsAt + m_numMsBetweenIntermediates * m_format->nAvgBytesPerSec / 1000;
    m_numIntermediatesFired = 0;
    
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

    m_numIntermediatesFired++;
    auto offset = ticksPerSecond * m_cbResultStartsAt / m_format->nAvgBytesPerSec;
    auto duration = ticksPerSecond * m_numIntermediatesFired * m_numMsBetweenIntermediates / 1000;

    auto resultText = m_mockResultText;
    SPX_DBG_TRACE_VERBOSE("%s: text='%ls', offset=%d", __FUNCTION__, resultText.c_str(), offset);

    m_cbFireIntermediate = m_numIntermediatesFired < m_numIntermediates
        ? m_cbFireIntermediate + m_numMsBetweenIntermediates * m_format->nAvgBytesPerSec / 1000
        : m_cbFireFinal + m_numMsFinalEvery * m_format->nAvgBytesPerSec / 1000;

    InvokeOnSite([&](const SitePtr& site)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateIntermediateResult(resultText.c_str(), offset, duration);
        site->FireAdapterResult_Intermediate(this, offset, result);
    });
}

void CSpxMockRecoEngineAdapter::FireFinalResult()
{
    auto resultText = m_mockResultText.empty()
        ? m_firstMockWord
        : m_mockResultText;

    auto offset = ticksPerSecond * m_cbResultStartsAt / m_format->nAvgBytesPerSec;
    auto duration = ticksPerSecond * m_numMsFinalDuration / 1000;

    SPX_DBG_TRACE_VERBOSE("%s: text='%ls', offset=%d", __FUNCTION__, resultText.c_str(), offset);

    m_cbFireFinal += m_numMsFinalEvery * m_format->nAvgBytesPerSec / 1000;
    m_cbResultStartsAt += m_numMsFinalEvery * m_format->nAvgBytesPerSec / 1000;
    m_cbFireIntermediate = m_cbResultStartsAt + m_numMsBetweenIntermediates * m_format->nAvgBytesPerSec / 1000;
    m_numIntermediatesFired = 0;

    FireSpeechEndDetected();

    InvokeOnSite([&](const SitePtr& site)
    {
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateFinalResult(ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, resultText.c_str(), offset, duration);
        site->FireAdapterResult_FinalResult(this, offset, result);
    });
}

void CSpxMockRecoEngineAdapter::EnsureFireFinalResult()
{
    SPX_DBG_TRACE_VERBOSE("%s: offset=%" PRIu64, __FUNCTION__, m_cbAudioProcessed);
    if (m_numIntermediatesFired > 0)
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
