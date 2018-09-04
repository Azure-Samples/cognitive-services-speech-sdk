//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_pump.cpp: Implementation definitions for CSpxStreamPump C++ class
//

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <thread>
#include "stream_pump.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


    CSpxStreamPump::CSpxStreamPump() :
        m_streamReader(nullptr),
        m_state(State::NoInput),
        m_stateRequested(State::NoInput)
    {
    }

    CSpxStreamPump::~CSpxStreamPump()
    {
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void CSpxStreamPump::SetAudioStream(AudioInputStream* reader)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        SPX_IFTRUE_THROW_HR(reader != nullptr && m_streamReader != nullptr, SPXERR_ALREADY_INITIALIZED);
        SPX_IFTRUE_THROW_HR(m_state == State::Paused || m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);

        m_streamReader = reader;
        m_state = reader != nullptr ? State::Idle : State::NoInput;
    }

    uint16_t CSpxStreamPump::GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat)
    {
        SPX_IFTRUE_THROW_HR(m_streamReader == nullptr, SPXERR_UNINITIALIZED);

        AudioInputStreamFormat format;
        auto retValue = m_streamReader->GetFormat(&format, cbFormat);
        if (!pformat)
        {
            return (uint16_t)retValue;
        }

        pformat->nAvgBytesPerSec = (uint32_t)format.AvgBytesPerSec;
        pformat->nBlockAlign = (uint16_t)format.BlockAlign;
        pformat->nChannels = (uint16_t)format.Channels;
        pformat->nSamplesPerSec = (uint32_t)format.SamplesPerSec;
        pformat->wBitsPerSample = (uint16_t)format.BitsPerSample;
        pformat->wFormatTag = (uint16_t)format.FormatTag;

        return (uint16_t)retValue;
    }

    void CSpxStreamPump::SetFormat(const WAVEFORMATEX* pformat, uint16_t cbFormat)
    {
        UNUSED(pformat);
        UNUSED(cbFormat);
        SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Implement CSpxStreamPump::SetFormat and hook up audio format conversion
    }

    void CSpxStreamPump::StartPump(std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        SPX_IFTRUE_THROW_HR(m_streamReader == nullptr, SPXERR_UNINITIALIZED);
        SPX_IFTRUE_THROW_HR(m_thread.joinable(), SPXERR_AUDIO_IS_PUMPING);
        SPX_IFTRUE_THROW_HR(m_state == State::NoInput, SPXERR_NO_AUDIO_INPUT);
        SPX_IFTRUE_THROW_HR(m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);
        SPX_IFTRUE_THROW_HR(m_state == State::Paused, SPXERR_NOT_IMPL); // TODO: FUTURE: Implement PausePump
        SPX_DBG_ASSERT_WITH_MESSAGE(m_state == State::Idle,
            "If the state is not one of the previous three checks, it MUST be Idle (there are only 4 states "
            "in the enumeration); unless someone adds a new state and doesn't change this code. This assert "
            "guards in DBG for that possibility.");

        auto pump = ((ISpxAudioPump*)this);
        auto keepAliveForThread = std::dynamic_pointer_cast<CSpxStreamPump>(pump->shared_from_this());
        m_thread = std::thread(&CSpxStreamPump::PumpThread, this, std::move(keepAliveForThread), pISpxAudioProcessor);

        m_stateRequested = State::Processing; // it's ok we set the requested state after we 'start' the thread; PumpThread will wait for the lock
        m_cv.notify_all();
        m_cv.wait(lock, [&] { return m_state == State::Processing || m_stateRequested != State::Processing; });
    }

    void CSpxStreamPump::PausePump()
    {
        SPX_THROW_HR(SPXERR_NOT_IMPL); // TODO: FUTURE: Imlement CSpxStreamPump::PausePump
    }

    void CSpxStreamPump::StopPump()
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

    ISpxAudioPump::State CSpxStreamPump::GetState()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_state;
    }

    void CSpxStreamPump::SetRealTimePercentage(uint8_t percentage)
    {
        m_simulateRealtimePercentage = percentage;
    }

    void CSpxStreamPump::PumpThread(std::shared_ptr<CSpxStreamPump> keepAlive, std::shared_ptr<ISpxAudioProcessor> pISpxAudioProcessor)
    {
        UNUSED(keepAlive);
        SPX_DBG_TRACE_SCOPE("StreamPumpThread started!", "StreamPumpThread stopped!");

        // Get the format from the reader and give it to the processor
        auto cbFormat = m_streamReader->GetFormat(nullptr, 0);
        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);

        AudioInputStreamFormat format;
        m_streamReader->GetFormat(&format, cbFormat);
        waveformat->cbSize = 0;
        waveformat->nAvgBytesPerSec = (uint32_t)format.AvgBytesPerSec;
        waveformat->nBlockAlign = (uint16_t)format.BlockAlign;
        waveformat->nChannels = (uint16_t)format.Channels;
        waveformat->nSamplesPerSec = (uint32_t)format.SamplesPerSec;
        waveformat->wBitsPerSample = (uint16_t)format.BitsPerSample;
        waveformat->wFormatTag = (uint16_t)format.FormatTag;
        pISpxAudioProcessor->SetFormat(waveformat.get());

        // Calculate size of the buffer to read from the reader and send to the processor; and allocate it
        SPX_IFTRUE_THROW_HR(waveformat->wBitsPerSample % 8 != 0, SPXERR_UNSUPPORTED_FORMAT); // we only support 8bit multiples for sample size
        auto bytesPerSample = waveformat->wBitsPerSample / 8;
        auto samplesPerSec = waveformat->nSamplesPerSec;
        auto framesPerSec = 10;

        auto bytesPerFrame = samplesPerSec / framesPerSec * bytesPerSample;
        auto data = SpxAllocSharedAudioBuffer(bytesPerFrame);

        // When the pump is pumping, m_state is only changed in this lambda
        auto checkAndChangeState = [&]() {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_stateRequested != m_state)
            {
                m_state = m_stateRequested;
                m_cv.notify_all();
            }

            if (m_state == State::Processing)
            {
                return true;
            }

            m_thread.detach();
            return false;
        };


        std::chrono::steady_clock::time_point lastReadTime;
        bool isFirstRead = true;
        // Continue to loop while we're in the 'Processing' state...
        while (checkAndChangeState())
        {
            // Ensure we have an unencumbered data buffer to use for this Read/ProcessAudio iteration
            if (data.use_count() > 1)
            {
                data = SpxAllocSharedAudioBuffer(bytesPerFrame);
            }

            // Read the buffer, and send it to the processor
            int cbRead = (int)m_streamReader->Read((char*)data.get(), bytesPerFrame);
            if (m_simulateRealtimePercentage > 0)
            {
                // Slow down if needed.
                auto now = std::chrono::steady_clock::now();
                if (!isFirstRead)
                {
                    auto allowedBytes = waveformat->nAvgBytesPerSec * std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReadTime).count() / 1000 * 100 / m_simulateRealtimePercentage;
                    if (cbRead > allowedBytes)
                    {
                        auto sleepMilliseconds = (cbRead - allowedBytes) * 1000 / (waveformat->nAvgBytesPerSec * 100 / m_simulateRealtimePercentage);
                        SPX_DBG_TRACE_VERBOSE("Slow down sending audio data by %d (us). Allow %d bytes in %d us, want to write %d bytes", (int)sleepMilliseconds, (int)allowedBytes, (int)(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReadTime).count()), (int)cbRead);
                        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds));
                    }
                }
                else
                {
                    isFirstRead = false;
                }
                lastReadTime = now;
            }

            pISpxAudioProcessor->ProcessAudio(data, cbRead);

            // If we didn't read any data, move to the 'Idle' state
            if (cbRead == 0)
            {
                SPX_DBG_TRACE_INFO("m_streamReader->Read() read ZERO (0) bytes... Indicating end of stream based input.");
                std::unique_lock<std::mutex> lock(m_mutex);
                m_stateRequested = State::Idle;
            }
        }

        // Let the Processor know we're done for now...
        pISpxAudioProcessor->SetFormat(nullptr);
    }


} } } } // Microsoft::CognitiveServices::Speech::Impl
