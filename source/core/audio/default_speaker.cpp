//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// default_speaker.cpp: Implementation definitions for CSpxDefaultSpeaker C++ class
//

#include "stdafx.h"
#include "default_speaker.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxDefaultSpeaker::CSpxDefaultSpeaker()
{
}

CSpxDefaultSpeaker::~CSpxDefaultSpeaker()
{
    Term();
}

void CSpxDefaultSpeaker::Init()
{
    m_audioStream = std::make_shared<CSpxPullAudioOutputStream>();
}

void CSpxDefaultSpeaker::Term()
{
#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    while (PlayState::Playing == m_playState)
    {
        // Wait until speak complete
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (m_haudio != nullptr)
    {
        audio_destroy(m_haudio);
    }

    if (m_hsetting != nullptr)
    {
        audio_format_destroy(m_hsetting);
    }
#endif
    m_audioInitialized = false;
}

void CSpxDefaultSpeaker::StartPlayback()
{
    SPX_IFTRUE_THROW_HR(m_audioFormat.get() == nullptr, SPXERR_UNINITIALIZED);

    // May need filter the audio format here
    // In case the audio playback library doesn't support the format and unexpected behavior is triggered

#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    // Only make playback when audio device is initialized, in order not to abort the synthesizer when audio device is not available
    if (m_audioInitialized && PlayState::Stopped == m_playState)
    {
        auto result = audio_output_startasync(m_haudio, m_audioFormat.get(), PlayAudioReadCallback, AudioCompleteCallback, BufferUnderRunCallback, this);
        // Throw error if start failed. Users should ensure that the speaker is available.
        SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT::AUDIO_RESULT_OK, SPXERR_RUNTIME_ERROR);
        m_playState = PlayState::Playing;
    }
#endif
}

void CSpxDefaultSpeaker::PausePlayback()
{
#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    // Only make the operation when audio device is initialized, in order not to abort the synthesizer when audio device is not available
    if (m_audioInitialized && PlayState::Playing == m_playState)
    {
        auto result = audio_output_pause(m_haudio);
        SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT::AUDIO_RESULT_OK, SPXERR_RUNTIME_ERROR);
    }
#endif
}

void CSpxDefaultSpeaker::StopPlayback()
{
#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    // Only make the operation when audio device is initialized, in order not to abort the synthesizer when audio device is not available
    if (m_audioInitialized && (PlayState::Playing == m_playState || PlayState::Paused == m_playState))
    {
        auto result = audio_output_stop(m_haudio); // This will wait 5s for the remaining rendering to complete
        SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT::AUDIO_RESULT_OK, SPXERR_RUNTIME_ERROR);
    }
#endif
}

uint32_t CSpxDefaultSpeaker::Write(uint8_t* buffer, uint32_t size)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_IFTRUE_THROW_HR(m_audioFormat.get() == nullptr, SPXERR_UNINITIALIZED);

    // Initialize audio device when data is received for the first time
    InitializeAudio();   

    uint32_t writtenSize = size;
    // Only send the data when audio device is initialized
    if (m_audioInitialized)
    {
        writtenSize = m_audioStream->Write(buffer, size);
    }

    StartPlayback();

    return writtenSize;
}

void CSpxDefaultSpeaker::WaitUntilDone()
{
#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    std::unique_lock<std::mutex> lock(m_playMutex);
    m_audioStream->Close();
#ifdef _DEBUG
    while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] { return PlayState::Stopped == m_playState; }))
    {
        SPX_DBG_TRACE_VERBOSE("%s: waiting ...", __FUNCTION__);
    }
#else
    m_cv.wait(lock, [&] { return PlayState::Stopped == m_playState; });
#endif // _DEBUG
#endif // AUDIO_OUTPUT_DEVICE_AVAILABLE
}

void CSpxDefaultSpeaker::Close()
{
    m_audioStream->Close();
}

uint16_t CSpxDefaultSpeaker::GetFormat(SPXWAVEFORMATEX* formatBuffer, uint16_t formatSize)
{
    uint16_t formatSizeRequired = sizeof(SPXWAVEFORMATEX);
    SPX_DBG_TRACE_VERBOSE("CSpxDefaultSpeaker::GetFormat is called formatBuffer is %s formatSize=%d", formatBuffer ? "not null" : "null", formatSize);

    if (formatBuffer != nullptr)
    {
        size_t size = std::min(formatSize, (uint16_t)sizeof(AUDIO_WAVEFORMAT));
        memcpy(formatBuffer, m_audioFormat.get(), size);

        if (formatSize >= (uint16_t)sizeof(SPXWAVEFORMATEX))
        {
            formatBuffer->cbSize = 0;
        }
    }

    return formatSizeRequired;
}

void CSpxDefaultSpeaker::SetFormat(SPXWAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(pformat == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(m_audioFormat != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Allocate the buffer for the format
    auto formatSize = sizeof(AUDIO_WAVEFORMAT);
    m_audioFormat = SpxAllocSharedBuffer<AUDIO_WAVEFORMAT>(formatSize);
    SPX_DBG_TRACE_VERBOSE("CSpxDefaultSpeaker::SetFormat is called with format 0x%p", (void*)pformat);

    // Copy the format
    memcpy(m_audioFormat.get(), pformat, formatSize);
}

void CSpxDefaultSpeaker::InitializeAudio()
{
    SPX_IFTRUE_THROW_HR(m_audioFormat.get() == nullptr, SPXERR_UNINITIALIZED);

#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE
    if (!m_audioInitialized)
    {
        m_hsetting = audio_format_create();
        if (m_hsetting == nullptr)
        {
            return;
        }

        memcpy(m_hsetting, m_audioFormat.get(), sizeof(AUDIO_WAVEFORMAT));
        m_hsetting->eDataFlow = AUDIO_RENDER;

        // If the audio format is not PCM, this will fail with internal error code 0x88890008
        m_haudio = audio_create_with_parameters(m_hsetting);

        // Just return, instead of abort, in order to make the synthesizer still running without audio device available
        if (m_haudio == nullptr)
        {
            return;
        }

        m_audioInitialized = true;

    }
#endif
}

#ifdef AUDIO_OUTPUT_DEVICE_AVAILABLE

int CSpxDefaultSpeaker::PlayAudioReadCallback(void* pContext, uint8_t* pBuffer, uint32_t size)
{
    auto defaultSpeaker = (CSpxDefaultSpeaker*)pContext;
    auto filledSize = defaultSpeaker->m_audioStream->Read(pBuffer, std::min(size, (uint32_t)CHUNK_SIZE)); // The default buffer size is 1600000, which is too long
    return (int)filledSize;
}

void CSpxDefaultSpeaker::AudioCompleteCallback(void* pContext)
{
    auto defaultSpeaker = (CSpxDefaultSpeaker*)pContext;
    std::unique_lock<std::mutex> lock(defaultSpeaker->m_playMutex);
    defaultSpeaker->m_playState = PlayState::Stopped;
    defaultSpeaker->m_cv.notify_all();
}

void CSpxDefaultSpeaker::BufferUnderRunCallback(void* pContext)
{
    UNUSED(pContext);
}

#endif


} } } } // Microsoft::CognitiveServices::Speech::Impl
