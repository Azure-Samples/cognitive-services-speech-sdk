//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_audio_processor.cpp: Implementation definitions for CSpxSpeechAudioProcessor C++ class
//

#include "stdafx.h"
#include <cstring>
#include <cmath>
#include "speech_audio_processor.h"
#include "service_helpers.h"
#include "try_catch_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxSpeechAudioProcessor::CSpxSpeechAudioProcessor() :
    m_bSpeechStarted(false),
    m_cbSilence(0),
    m_cbSilenceMax(19200),
    m_energyThreshold(180),
    m_cbAudioProcessed(0),
    m_cbWarmup(0),
    m_cbWarmupMax(9600),
    m_cbSkip(0),
    m_cbSkipMax(6400),
    m_baselineCount(0),
    m_baselineMean(0.0f)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxSpeechAudioProcessor::~CSpxSpeechAudioProcessor()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxSpeechAudioProcessor::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
}

void CSpxSpeechAudioProcessor::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxSpeechAudioProcessor::SetFormat(const SPXWAVEFORMATEX* pformat)
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

void CSpxSpeechAudioProcessor::ProcessAudio(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_VERBOSE_IF(0, "%s(..., size=%d)", __FUNCTION__, audioChunk->size);
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    if (!HasFormat() || m_format->wBitsPerSample != 16)
    {
        return;
    }

    double sum = 0;
    int16_t sample = 0;
    uint32_t size = audioChunk->size / 2;
    uint32_t osize = size;
    int16_t* data = reinterpret_cast<int16_t*>(audioChunk->data.get());

    m_cbAudioProcessed += size * 2;

    // manage skipped data
    if (m_cbSkip < m_cbSkipMax)
    {
        uint32_t skipbytes = m_cbSkipMax - m_cbSkip;
        if (skipbytes >= size * 2)
        {
            m_cbSkip += size * 2;
            return;
        }
        else
        {
            m_cbSkip += skipbytes;
            size -= skipbytes / 2;
            data += skipbytes / 2;
            osize -= size;
        }
    }

    // loop the samples
    while (size > 0)
    {
        sample = *data;
        sum += sample*sample;
        size--;
        data++;
    }

    //calculate the mean for this chunk of data
    double chunkmean = sqrt(sum / osize);

    // dont use the weighted mean - treat all chunks the same independent of their size
    // for now, if the base duration is set to the middle of the chunk, the full chunk is used.
    // however the silence duration can take part of the first chunk
    if (m_cbWarmup < m_cbWarmupMax)
    {
        m_baselineMean = (m_baselineMean + chunkmean) / ++m_baselineCount;
        m_cbWarmup += osize*2;
        return;
    }

    if (chunkmean > (m_baselineMean + m_energyThreshold))
    {
        m_cbSilence = 0;
        if (!m_bSpeechStarted)
        {
            m_bSpeechStarted = true;
            NotifySiteSpeechStart();
        }
    }
    else if (m_bSpeechStarted)
    {
        m_cbSilence += osize*2;
    }

    // fire at the end of the chunk processing, doesnt matter if duration was in the middle of the block
    if (m_bSpeechStarted && m_cbSilence >= m_cbSilenceMax)
    {
        m_bSpeechStarted = false;
        NotifySiteSpeechEnd();
    }
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionThreshold(uint32_t threshold)
{
    m_energyThreshold = threshold;
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionSilenceMs(uint32_t duration)
{
    m_cbSilenceMax = duration * 32;
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionSkipMs(uint32_t duration)
{
    m_cbSkipMax = duration * 32;
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionBaselineMs(uint32_t duration)
{
    m_cbWarmupMax = duration * 32;
}

void CSpxSpeechAudioProcessor::InitFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);

    m_cbAudioProcessed = 0;
}

void CSpxSpeechAudioProcessor::TermFormat()
{
    m_format = nullptr;
}

void CSpxSpeechAudioProcessor::End()
{
}

void CSpxSpeechAudioProcessor::NotifySiteSpeechStart()
{
    auto site = GetSite();
    std::string error;
    SPXAPI_TRY()
    {
        site->SpeechStartDetected(m_cbAudioProcessed);
    }
    SPXAPI_CATCH_ONLY()
}

void CSpxSpeechAudioProcessor::NotifySiteSpeechEnd()
{
    auto site = GetSite();
    std::string error;
    SPXAPI_TRY()
    {
        site->SpeechEndDetected(m_cbAudioProcessed);
    }
    SPXAPI_CATCH_ONLY()
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
