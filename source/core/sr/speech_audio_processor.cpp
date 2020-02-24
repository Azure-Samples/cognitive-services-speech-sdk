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
    m_cbSilenceChunkCount(0),
    m_cbSilenceChunkCountMax(10),
    m_energyThreshold(180),
    m_cbAudioProcessed(0)
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
    uint16_t sample = 0;
    uint32_t size = audioChunk->size;
    uint8_t* data = audioChunk->data.get();

    m_cbAudioProcessed += size;

    // simple energy detection algorithm used by office for the detection of speech

    while (size > 0)
    {
        sample = static_cast<uint16_t>(*data);
        sum += pow(sample, 2);
        size -= 2;
        data += 2;
    }

    double mean = sqrt(sum / (audioChunk->size / 2));
    if (!m_bSpeechStarted && (mean > m_energyThreshold))
    {
        m_cbSilenceChunkCount = 0;
        m_bSpeechStarted = true;
        NotifySiteSpeechStart();
    }
    else if (m_bSpeechStarted && m_cbSilenceChunkCount < 10)
    {
        m_cbSilenceChunkCount += 1;
    }

    if (m_bSpeechStarted && m_cbSilenceChunkCount >= 10)
    {
        m_bSpeechStarted = false;
        NotifySiteSpeechEnd();
    }
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionThreshold(uint32_t threshold)
{
    m_energyThreshold = threshold;
}

void CSpxSpeechAudioProcessor::SetSpeechDetectionSilenceCount(uint32_t count)
{
    m_cbSilenceChunkCountMax = count;
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
