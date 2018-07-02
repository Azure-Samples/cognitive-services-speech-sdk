//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "microphone_pump_base.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;


MicrophonePumpBase::MicrophonePumpBase():
    m_state {State::NoInput},
    m_format { WAVE_FORMAT_PCM, CHANNELS, SAMPLES_PER_SECOND, AVG_BYTES_PER_SECOND, BLOCK_ALIGN, BITS_PER_SAMPLE, 0 }
{
    m_audioHandle = audio_create();
    SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_MIC_NOT_AVAILABLE);
}

MicrophonePumpBase::~MicrophonePumpBase()
{
    audio_destroy(m_audioHandle);
}

uint16_t MicrophonePumpBase::GetFormat(WAVEFORMATEX* format, uint16_t size)
{
    auto totalSize = uint16_t(sizeof(WAVEFORMATEX) + m_format.cbSize);
    if (format != nullptr)
    {
        memcpy(format, &m_format, min(totalSize, size));
    }
    return totalSize;
}

void MicrophonePumpBase::StartPump(SinkType processor)
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::StartPump() ...", "MicrophonePumpBase::StartPump ... Done!");

    {
        unique_lock<mutex> lock(m_mutex);

        SPX_IFTRUE_THROW_HR(processor == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);

        m_sink = std::move(processor);
    }

    auto result = audio_input_start(m_audioHandle);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);

    unique_lock<mutex> lock(m_mutex);
    // wait for audio capture thread finishing getAudioReady.
    bool pred = m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStartPumpRequestTimeout), [&state = m_state] { return (state != State::NoInput && state != State::Idle ); });
    SPX_IFTRUE_THROW_HR(pred == false, SPXERR_TIMEOUT);
}

void MicrophonePumpBase::StopPump()
{
    ReleaseSink resetSinkWhenExit(m_sink);

    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::StopPump ...", "MicrophonePumpBase::StopPump ... Done");

    SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(m_sink == nullptr, SPXERR_INVALID_ARG);

    {
        unique_lock<mutex> lock(m_mutex);
        if (m_state == State::NoInput || m_state == State::Idle)
        {
            SPX_DBG_TRACE_VERBOSE("%s when we're already in State::Idle or State::NoInput state", __FUNCTION__);
            return;
        }
    }

    auto result = audio_input_stop(m_audioHandle);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);

    // wait for the audio capture thread finishing setFormat(null)
    {
        unique_lock<mutex> lock(m_mutex);
        m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStopPumpRequestTimeout), [&state = m_state] { return state != State::Processing; });
    }

    // not release the sink may result in assert in m_resetRecoAdapter == nullptr in ~CSpxAudioStreamSession
}

ISpxAudioPump::State MicrophonePumpBase::GetState()
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::GetState() ...", "MicrophonePumpBase::GetState ... Done");
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_state;
}

// this is called by audioCaptureThread, state value only changes here.
void MicrophonePumpBase::UpdateState(AUDIO_STATE state)
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::UpdateState() ...", "MicrophonePumpBase::UpdateState ... Done!");
    unique_lock<mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(m_sink == nullptr, SPXERR_INVALID_ARG);

    SPX_DBG_TRACE_VERBOSE("%s: UpdateState with state as %d.", __FUNCTION__, int(state));
    switch (state)
    {
    case AUDIO_STATE_STARTING:
        m_sink->SetFormat(const_cast<WAVEFORMATEX*>(&m_format));
        m_state = State::Processing;
        m_cv.notify_one();
        break;

    case AUDIO_STATE_STOPPED:
        // Let the sink know we're done for now...
        m_sink->SetFormat(nullptr);
        m_state = State::Idle;
        m_cv.notify_one();
        break;

    case AUDIO_STATE_RUNNING:
        break;

    default:
        SPX_DBG_TRACE_VERBOSE("%s: unexpected audio state: %d.", __FUNCTION__, int(state));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_STATE);
    }

}

} } } } // Microsoft::CognitiveServices::Speech::Impl


