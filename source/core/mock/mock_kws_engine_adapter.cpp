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
#include "try_catch_helpers.h"
#include "mock_controller.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxMockKwsEngineAdapter::CSpxMockKwsEngineAdapter() :
    m_cbAudioProcessed(0),
    m_cbFireKeyword(0)
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

void CSpxMockKwsEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
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
        TermFormat();
        End();
    }
}

void CSpxMockKwsEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_VERBOSE_IF(0, "%s(..., size=%d)", __FUNCTION__, audioChunk->size);
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    m_audio.push_front(audioChunk);
    m_cbAudioProcessed += audioChunk->size;
    SPX_IFTRUE(m_cbAudioProcessed >= m_cbFireKeyword, FireKeywordDetected());
}

void CSpxMockKwsEngineAdapter::InitFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);

    m_cbAudioProcessed = 0;
    m_cbFireKeyword = (m_numMsKeywordEvery - m_numMsKeywordOffset + m_numMsKeywordDuration + m_numMsKeywordUPL) * m_format->nAvgBytesPerSec / 1000;
}

void CSpxMockKwsEngineAdapter::TermFormat()
{
    m_format = nullptr;
}

void CSpxMockKwsEngineAdapter::End()
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdapterCompletedSetFormatStop(this);
}

void CSpxMockKwsEngineAdapter::FireKeywordDetected()
{
    SPX_DBG_TRACE_FUNCTION();
    std::string error;

    SPXAPI_TRY()
    {
        auto offset = ticksPerSecond * ((m_cbFireKeyword * 1000 / m_format->nAvgBytesPerSec) - m_numMsKeywordDuration - m_numMsKeywordUPL) / 1000;
        auto duration = ticksPerSecond * m_numMsKeywordDuration / 1000;
        m_cbFireKeyword += m_numMsKeywordEvery * m_format->nAvgBytesPerSec / 1000;

        auto site = GetSite();
        auto properties = SpxQueryInterface<ISpxNamedProperties>(site);
        auto keyword = properties->GetStringValue("CARBON-INTERNAL-MOCK-KWS-Keyword", "Computer");
        SPX_TRACE_INFO("%s: text='%s', offset=%" PRIu64 ", duration=%" PRIu64, __FUNCTION__, keyword.c_str(), offset, duration);

        auto size = (size_t)(m_cbFireKeyword - (m_numMsKeywordDuration + m_numMsKeywordUPL) * m_format->nAvgBytesPerSec / 1000 + (m_cbAudioProcessed - m_cbFireKeyword));
        auto buffer = SpxAllocSharedAudioBuffer(size);
        auto write = buffer.get() + size;

        size_t remaining = size;
        for (auto chunk : m_audio)
        {
            if (remaining == 0) break;

            if (remaining >= chunk->size)
            {
                write = write - chunk->size;
                memcpy(write, chunk->data.get(), chunk->size);
                remaining -= chunk->size;
            }
            else
            {
                write = write - remaining;
                memcpy(write, chunk->data.get() + chunk->size - remaining, remaining);
                remaining -= remaining;
            }
        }

        // MUST call on the same thread as the CSpxMockKwsEngineAdapter::ProcessAudio was called
        auto chunk = std::make_shared<DataChunk>(buffer, (uint32_t)size);
        site->KeywordDetected(this, offset, duration, 1.0, keyword, chunk);
    }
    SPXAPI_CATCH_ONLY()
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
