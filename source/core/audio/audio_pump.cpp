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


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxAudioPump::CSpxAudioPump() :
    m_state(State::NoInput),
    m_stateRequested(State::NoInput)
{
}

CSpxAudioPump::~CSpxAudioPump()
{
}

void CSpxAudioPump::SetReader(std::shared_ptr<ISpxAudioStreamReader> reader)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(reader.get() != nullptr && m_reader.get() != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_state == State::Paused || m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);

    m_reader = reader;
    m_state = reader.get() != nullptr ? State::Idle : State::NoInput;
}

uint16_t CSpxAudioPump::GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat)
{
    SPX_IFTRUE_THROW_HR(m_reader.get() == nullptr, SPXERR_UNINITIALIZED);
    return m_reader->GetFormat(pformat, cbFormat);
}

void CSpxAudioPump::SetFormat(const SPXWAVEFORMATEX* pformat, uint16_t cbFormat)
{
    UNUSED(pformat);
    UNUSED(cbFormat);
    SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Implement CSpxAudioPump::SetFormat and hook up audio format conversion
}

void CSpxAudioPump::StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    SPX_IFTRUE_THROW_HR(m_reader.get() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_thread.joinable(), SPXERR_AUDIO_IS_PUMPING);
    SPX_IFTRUE_THROW_HR(m_state == State::NoInput, SPXERR_NO_AUDIO_INPUT);
    SPX_IFTRUE_THROW_HR(m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);
    SPX_IFTRUE_THROW_HR(m_state == State::Paused, SPXERR_NOT_IMPL); // TODO: FUTURE: Implement PausePump
    SPX_DBG_ASSERT_WITH_MESSAGE(m_state == State::Idle, 
        "If the state is not one of the previous three checks, it MUST be Idle (there are only 4 states "
        "in the enumeration); unless someone adds a new state and doesn't change this code. This assert "
        "guards in DBG for that possibility.");

    auto pump = ((ISpxAudioPump*)this);
    auto keepAliveForThread = std::dynamic_pointer_cast<CSpxAudioPump>(pump->shared_from_this());
    m_thread = std::thread(&CSpxAudioPump::PumpThread, this, std::move(keepAliveForThread), pISpxAudioProcessor);

    m_stateRequested = State::Processing; // it's ok we set the requested state after we 'start' the thread; PumpThread will wait for the lock

    // We must detach the thread because it will call into the session to destroy the session, thus trying to
    // delete itself. Therefore, we must not join, but instead make sure it holds the instance alive until it
    // ends.
    m_thread.detach();

    WaitForPumpStart(lock);
}

void CSpxAudioPump::PausePump()
{
    SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Imlement CSpxAudioPump::PausePump
}

void CSpxAudioPump::StopPump()
{
    SPX_DBG_TRACE_SCOPE("CSpxAudioPump::StopPump() ...", "CSpxAudioPump::StopPump ... Done!");

    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::StopPump() ... mutex LOCKED");

    switch (m_state)
    {
    case State::NoInput:
    case State::Idle:
        SPX_DBG_TRACE_VERBOSE("%s when we're already in State::Idle or State::NoInput state", __FUNCTION__);
        break;

    case State::Paused:
    case State::Processing:
        m_stateRequested = State::Idle;
        WaitForPumpIdle(lock);
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
    try
    {
        // make sure, we release the reference only after the last operation we did.
        std::shared_ptr<CSpxAudioPump> keepAliveLock = keepAlive;

        SPX_DBG_TRACE_SCOPE("*** AudioPump THREAD started! ***", "*** AudioPump THREAD stopped! ***");

        // Get the format from the reader and give it to the processor
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): getting format from reader...");

        SPX_TRACE_ERROR_IF(m_reader == nullptr, "CSpxAudioPump::PumpThread(): m_reader == nullptr !!! Unexpected !!");
        auto cbFormat = m_reader->GetFormat(nullptr, 0);
        SPX_TRACE_ERROR_IF(cbFormat == 0, "CSpxAudioPump::PumpThread(): cbFormat == 0 !!! Unexpected !!");

        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        SPX_TRACE_ERROR_IF(waveformat == nullptr, "CSpxAudioPump::PumpThread(): SpxAllocWAVEFORMATEX(cbFormat) == nullptr !!! Unexpected !!");
        m_reader->GetFormat(waveformat.get(), cbFormat);

        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): setting format on processor...");
        SPX_TRACE_ERROR_IF(pISpxAudioProcessor == nullptr, "CSpxAudioPump::PumpThread(): pISpxAudioProcessor == nullptr !!! Unexpected !!");
        pISpxAudioProcessor->SetFormat(waveformat.get());

        // Calculate size of the buffer to read from the reader and send to the processor; and allocate it
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): allocating our first buffer");
        SPX_TRACE_ERROR_IF(waveformat == nullptr, "CSpxAudioPump::PumpThread(): waveformat == nullptr !!! Unexpected !!");
        SPX_IFTRUE_THROW_HR(waveformat->wBitsPerSample % 8 != 0, SPXERR_UNSUPPORTED_FORMAT); // we only support 8bit multiples for sample size
        auto bytesPerSample = waveformat->wBitsPerSample / 8;
        auto samplesPerSec = waveformat->nSamplesPerSec;
        auto framesPerSec = 10;

        auto bytesPerFrame = samplesPerSec / framesPerSec * bytesPerSample;
        auto data = SpxAllocSharedAudioBuffer(bytesPerFrame);

        // When the pump is pumping, m_state is only changed in this lambda, on the background thread
        auto checkAndChangeState = [&]() {
            std::unique_lock<std::mutex> lock(m_mutex);

            // If we actually need to change the state, do so, and signal to waiters that we did
            if (m_stateRequested != m_state)
            {
                SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(), checkAndChangeState: changing states as requested: %d => %d", m_state, m_stateRequested);
                m_state = m_stateRequested;
                m_cv.notify_all();
            }

            // If we're still in the processing state, we can go ahead return true (indicating to the while() loop below
            // that we should keep processing audio...)
            if (m_state == State::Processing)
            {
                return true;
            }

            // Returning true will cause the while() loop (see below) to exit, thus exiting the thread
            return false;
        };

        // Continue to loop while we're in the 'Processing' state...
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): starting loop ...");
        while (checkAndChangeState())
        {
            // Ensure we have an unencumbered data buffer to use for this Read/ProcessAudio iteration
            if (data.use_count() > 1)
            {
                data = SpxAllocSharedAudioBuffer(bytesPerFrame);
            }

            // Read the buffer, and send it to the processor
            auto cbRead = m_reader->Read(data.get(), bytesPerFrame);
            pISpxAudioProcessor->ProcessAudio(data, cbRead);

            // If we didn't read any data, move to the 'Idle' state
            if (cbRead == 0)
            {
                SPX_DBG_TRACE_INFO("m_reader->Read() read ZERO (0) bytes... Indicating end of stream based input.");
                std::unique_lock<std::mutex> lock(m_mutex);
                m_stateRequested = State::Idle;
            }
        }

        // Let the Processor know we're done for now...
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): exited while loop, pre-SetFormat(nullptr)");
        pISpxAudioProcessor->SetFormat(nullptr);
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): exited while loop, post-SetFormat(nullptr)");
        return;
    }
    catch (const std::exception& e)
    {
        UNUSED(e);
        SPX_DBG_TRACE_ERROR("ERROR! Unexpected exception happened during pump '%s', setting state to idle.", e.what());
    }
    catch (...)
    {
        SPX_DBG_TRACE_ERROR("ERROR! Unexpected exception happened during pump, setting state to idle.");
    }

    // TODO: This is not correct error handling, we should communicate the error to the user and
    // switch the audio session in an error state.
    // The work is scheduled in the following work item:
    // https://msasg.visualstudio.com/DefaultCollection/Skyman/_workitems/edit/1394625
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(), exception happened, changing states as requested: %d => %d", m_state, State::Idle);
    m_state = State::Idle;
    m_cv.notify_all();
}

void CSpxAudioPump::WaitForPumpStart(std::unique_lock<std::mutex>& lock)
{
    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpStart() ... pre m_cv.wait_for()");
    if (!m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStartPumpRequestTimeout), [&] { return m_state == State::Processing || m_stateRequested != State::Processing; }))
    {
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpStart() timeout waiting on a state");
    }

    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpStart() ... post m_cv.wait_for(); state=%d (requestedState=%d)", m_state, m_stateRequested);
}

void CSpxAudioPump::WaitForPumpIdle(std::unique_lock<std::mutex>& lock)
{
    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpIdle() ... pre m_cv.wait_for()");
    if (!m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStopPumpRequestTimeout), [&] { return m_state == State::Idle || m_stateRequested != State::Idle; }))
    {
        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpIdle() timeout waiting on a state");
    }

    SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::WaitForPumpIdle() ... post m_cv.wait_for(); state=%d (requestedState=%d)", m_state, m_stateRequested);
    SPX_DBG_TRACE_WARNING_IF(m_state != State::Idle, "CSpxAudioPump::WaitForPumpIdle(): Unexpected: state != State::Idle; state=%d", m_state);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
