//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump.cpp: Implementation definitions for CSpxAudioPump C++ class
//

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <thread>
#include "audio_pump.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxAudioPump::CSpxAudioPump() :
    m_state(State::NoInput),
    m_stateRequested(State::NoInput)
{
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
}

CSpxAudioPump::~CSpxAudioPump()
{
    // SPX_THROW_HR(SPXERR_NOT_IMPL);
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void CSpxAudioPump::SetAudioReader(std::shared_ptr<ISpxAudioReader>& reader)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(reader.get() != nullptr && m_audioReader.get() != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_state == State::Paused || m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);

    m_audioReader = reader;
    m_state = reader.get() != nullptr ? State::Idle : State::NoInput;
}

uint32_t CSpxAudioPump::GetFormat(WAVEFORMATEX* pformat, uint32_t cbFormat)
{
    return m_audioReader->GetFormat(pformat, cbFormat);
}

void CSpxAudioPump::SetFormat(const WAVEFORMATEX* pformat, uint32_t cbFormat)
{
    SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Implement CSpxAudioPump::SetFormat and hook up audio format conversion
}

void CSpxAudioPump::StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(m_audioReader.get() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_thread.joinable(), SPXERR_AUDIO_IS_PUMPING);
    SPX_IFTRUE_THROW_HR(m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);
    SPX_IFTRUE_THROW_HR(m_state == State::Paused, SPXERR_NOT_IMPL); // TODO: FUTURE: Implement PausePump
    SPX_DBG_ASSERT_WITH_MESSAGE(m_state != State::NoInput, "We have a reader; we have a joinable pump; state should != State::NoInput");
    SPX_DBG_ASSERT_WITH_MESSAGE(m_state != State::Idle, "State MUST == State::Idle at this point");

    auto keepAliveForThread = std::dynamic_pointer_cast<CSpxAudioPump>(this->shared_from_this());
    m_thread = std::move(std::thread(&CSpxAudioPump::PumpThread, this, std::move(keepAliveForThread), pISpxAudioProcessor));

    m_stateRequested = State::Processing; // it's ok we set the requested state after we 'start' the thread; PumpThread will wait for the lock
    m_cv.notify_all();
    m_cv.wait(lock, [&] { return m_state == State::Processing || m_stateRequested != State::Processing; });
}

void CSpxAudioPump::PausePump()
{
    SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Imlement CSpxAudioPump::PausePump
}

void CSpxAudioPump::StopPump()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    switch (m_state)
    {
    case State::NoInput:
    case State::Idle:
        SPX_DBG_TRACE_VERBOSE("%s when we're already in State::Idle or State::NoInput state", __FUNCTION__);
        break;

    case State::Paused:
    case State::Processing:
        m_stateRequested = State::Idle;
        m_cv.notify_all();
        m_cv.wait(lock, [&] { return m_state == State::Idle || m_stateRequested != State::Idle; });
        break;
    }
}

ISpxAudioPump::State CSpxAudioPump::GetState()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_state;
}

void CSpxAudioPump::PumpThread(std::shared_ptr<CSpxAudioPump> keepAlive, std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
{
    SPX_DBG_TRACE_SCOPE("AudioPumpThread started!", "AudioPumpThread stopped!");

    // Get the format from the reader and give it to the processor
    auto cbFormat = m_audioReader->GetFormat(nullptr, 0);
    std::unique_ptr<WAVEFORMATEX> waveformat = std::make_unique<WAVEFORMATEX>(*(WAVEFORMATEX*)new uint8_t[cbFormat]);
    
    m_audioReader->GetFormat(waveformat.get(), cbFormat);
    pISpxAudioProcessor->SetFormat(waveformat.get());

    // Calculate size of the buffer to read from the reader and send to the processor; and allocate it
    SPX_IFTRUE_THROW_HR(waveformat->wBitsPerSample % 8 != 0, SPXERR_UNSUPPORTED_FORMAT); // we only support 8bit multiples for sample size
    auto bytesPerSample = waveformat->wBitsPerSample / 8;
    auto samplesPerSec = waveformat->nSamplesPerSec;
    auto framesPerSec = 10;

    auto bytesPerFrame = samplesPerSec / framesPerSec * bytesPerSample;
    auto data = ISpxAudioProcessor::AudioData_Type(new uint8_t[bytesPerFrame]);

    // When the pump is pumping, m_state is only changed in this lambda
    auto checkAndChangeState = [&]() {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_stateRequested != m_state)
        {
            m_state = m_stateRequested;
            m_cv.notify_all();
        }

        return m_state == State::Processing;
    };

    // Continue to loop while we're in the 'Processing' state...
    while (checkAndChangeState())
    {
        // Ensure we have an unencumbered data buffer to use for this Read/ProcessAudio iteration
        if (data.use_count() > 1)
        {
            data = ISpxAudioProcessor::AudioData_Type(new uint8_t[bytesPerFrame]);
        }

        // Read the buffer, and send it to the processor
        auto cbRead = m_audioReader->Read(data.get(), bytesPerFrame);
        pISpxAudioProcessor->ProcessAudio(data, cbRead);

        // If we didn't read any data, move to the 'NoInput' state
        if (cbRead == 0)
        {
            SPX_DBG_TRACE_INFO("m_audioReader->Read() read ZERO (0) bytes... Indicating end of stream based input.");
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stateRequested = State::NoInput;
        }
    }

    // Let the Processor know we're done for now...
    pISpxAudioProcessor->SetFormat(nullptr);
}


}; // CARBON_IMPL_NAMESPACE()
