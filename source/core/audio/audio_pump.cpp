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
#include "service_helpers.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const char* g_stateNames[] { "NoInput", "Idle", "Paused", "Processing" };
struct __StateName
{
    inline const char* operator[](ISpxAudioPump::State state)
    {
        int index = (int)state;
        SPX_DBG_ASSERT(index >= 0 && (index < (int)(sizeof(g_stateNames) / sizeof(g_stateNames[0]))));
        return g_stateNames[index];
    }
} StateName;

CSpxAudioPump::CSpxAudioPump() :
    m_state(State::NoInput),
    m_stateRequested(State::NoInput)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::CSpxAudioPump", (void*)this);
}

CSpxAudioPump::~CSpxAudioPump()
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::~CSpxAudioPump", (void*)this);
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

    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::StartPump():", (void*)this);
    m_stateRequested = State::Processing;

    auto pump = ((ISpxAudioPump*)this);
    auto keepAliveForThread = std::dynamic_pointer_cast<CSpxAudioPump>(pump->shared_from_this());
    m_thread = std::thread(&CSpxAudioPump::PumpThread, this, std::move(keepAliveForThread), pISpxAudioProcessor);

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
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::StopPump():", (void*)this);

    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::StopPump() ... mutex LOCKED", (void*)this);

    switch (m_state)
    {
    case State::NoInput:
    case State::Idle:
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::StopPump(): when we're already in State::Idle or State::NoInput state", (void*)this);
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
    bool processorFormatSet{ false };
    try
    {
        // make sure, we release the reference only after the last operation we did.
        std::shared_ptr<CSpxAudioPump> keepAliveLock = keepAlive;

        SPX_DBG_TRACE_SCOPE("*** AudioPump THREAD started! ***", "*** AudioPump THREAD stopped! ***");

        // Get the format from the reader and give it to the processor
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): getting format from reader... ", (void*)this);

        SPX_TRACE_ERROR_IF(m_reader == nullptr, "CSpxAudioPump::PumpThread(): m_reader == nullptr !!! Unexpected !!");
        auto cbFormat = m_reader->GetFormat(nullptr, 0);
        SPX_TRACE_ERROR_IF(cbFormat == 0, "CSpxAudioPump::PumpThread(): cbFormat == 0 !!! Unexpected !!");

        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        SPX_TRACE_ERROR_IF(waveformat == nullptr, "CSpxAudioPump::PumpThread(): SpxAllocWAVEFORMATEX(cbFormat) == nullptr !!! Unexpected !!");
        m_reader->GetFormat(waveformat.get(), cbFormat);

        SPX_DBG_TRACE_VERBOSE("CSpxAudioPump::PumpThread(): setting format on processor...");
        SPX_TRACE_ERROR_IF(pISpxAudioProcessor == nullptr, "CSpxAudioPump::PumpThread(): pISpxAudioProcessor == nullptr !!! Unexpected !!");
        pISpxAudioProcessor->SetFormat(waveformat.get());
        processorFormatSet = true;

        // Calculate size of the buffer to read from the reader and send to the processor; and allocate it
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): allocating our first buffer", (void*)this);
        SPX_TRACE_ERROR_IF(waveformat == nullptr, "CSpxAudioPump::PumpThread(): waveformat == nullptr !!! Unexpected !!");
        SPX_IFTRUE_THROW_HR(waveformat->wBitsPerSample % 8 != 0, SPXERR_UNSUPPORTED_FORMAT); // we only support 8bit multiples for sample size
        auto bytesPerSample = waveformat->wBitsPerSample / 8;
        auto samplesPerSec = waveformat->nSamplesPerSec;
        auto framesPerSec = 10;
        auto channels = waveformat->nChannels;
        SPX_DBG_ASSERT(channels >= 1);

        auto bytesPerFrame = samplesPerSec / framesPerSec * bytesPerSample * channels;

        uint64_t skipduration = 0;
        auto processorProperties = SpxQueryService<ISpxNamedProperties>(pISpxAudioProcessor);
        if (processorProperties)
        {
            auto overrideBytesPerFrame = processorProperties->GetStringValue("SPEECH-BytesPerFrame", "");
            if(!overrideBytesPerFrame.empty())
            {
                bytesPerFrame = stoi(overrideBytesPerFrame);
                SPX_TRACE_INFO("%s -- BytesPerFrame override used, %d bytes will be pulled for each frame", __FUNCTION__, bytesPerFrame);
            }

            try {
                std::string durationString = processorProperties->GetStringValue("SPEECH-SkipAudioDurationHNS", "0");
                skipduration = stoll(durationString);
            }
            catch (const std::exception& e)
            {
                SPX_DBG_TRACE_VERBOSE("Error Parsing for SPEECH-SkipAudioDurationHNS: %s", e.what());
                SPX_DBG_TRACE_VERBOSE("No audio data will be skipped");
                skipduration = 0;
            }
        }

        auto data = SpxAllocSharedAudioBuffer(bytesPerFrame);

        // When the pump is pumping, m_state is only changed in this lambda, on the background thread
        auto checkAndChangeState = [&]() {
            std::unique_lock<std::mutex> lock(m_mutex);

            // If we actually need to change the state, do so, and signal to waiters that we did
            if (m_stateRequested != m_state)
            {
                SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(), checkAndChangeState: changing states as requested: '%s' => '%s'", (void*)this, StateName[m_state], StateName[m_stateRequested]);
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
        SPX_DBG_TRACE_VERBOSE(
            "[%p]CSpxAudioPump::PumpThread(): Starting loop ... => frame size: %u | wFormatTag: '%s'| nChannels: %d | nSamplesPerSec:  %d | nAvgBytesPerSec: %d | nBlockAlign: %d | wBitsPerSample: %d | cbSize: %d ",
            (void*)this,
            bytesPerFrame,
            waveformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(waveformat->wFormatTag).c_str(),
            waveformat->nChannels,
            waveformat->nSamplesPerSec,
            waveformat->nAvgBytesPerSec,
            waveformat->nBlockAlign,
            waveformat->wBitsPerSample,
            waveformat->cbSize);

        uint64_t skipSize = (skipduration*(uint64_t)(waveformat->nAvgBytesPerSec)) / (uint64_t)10000000;

        uint64_t extraBytes = skipSize % waveformat->nBlockAlign;

        if (extraBytes != 0)
        {
            skipSize = skipSize - extraBytes;
        }

        uint64_t totalSkip = 0;

        while (checkAndChangeState())
        {
            // Ensure we have an unencumbered data buffer to use for this Read/ProcessAudio iteration
            if (data.use_count() > 1)
            {
                data = SpxAllocSharedAudioBuffer(bytesPerFrame);
            }

            // Measure and log the duration of the read call
            auto start = std::chrono::steady_clock::now();

            // Read audio buffer, and send it to the processor
            auto cbRead = m_reader->Read(data.get(), bytesPerFrame);
            totalSkip += (uint64_t)cbRead;

            if (totalSkip > skipSize || cbRead == 0)
            {
                auto readCallDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
                SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): read frame duration: %" PRIu64 " ms => sending audio buffer size %u", (void*)this, (uint64_t)readCallDuration.count(), cbRead);
                UNUSED(readCallDuration); // unused in release builds
                std::string capturedTime, userId;
                if (cbRead != 0)
                {
                    capturedTime = m_reader->GetProperty(PropertyId::DataBuffer_TimeStamp);
                    userId = m_reader->GetProperty(PropertyId::DataBuffer_UserId);
                }

                pISpxAudioProcessor->ProcessAudio(std::make_shared<DataChunk>(data, cbRead, std::move(capturedTime), std::move(userId)));
            }

            // If we didn't read any data, move to the 'Idle' state
            if (cbRead == 0)
            {
                SPX_TRACE_INFO("[%p]CSpxAudioPump::PumpThread(): m_reader->Read() read ZERO (0) bytes... Indicating end of stream based input.", (void*)this);
                std::unique_lock<std::mutex> lock(m_mutex);
                m_stateRequested = State::Idle;
            }
        }

        // Let the Processor know we're done for now...
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): exited while loop, pre-SetFormat(nullptr)", (void*)this);

        // if SetFormat(nullptr) throws we do not call it again
        processorFormatSet = false;
        pISpxAudioProcessor->SetFormat(nullptr);
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): exited while loop, post-SetFormat(nullptr)", (void*)this);
        return;
    }
    catch (const std::exception& e)
    {
        SPX_TRACE_ERROR("[%p]CSpxAudioPump::PumpThread(): exception caught during pumping, %s", (void*)this, e.what());
        SPX_DBG_ASSERT(GetSite() != nullptr);
        InvokeOnSite([msg = e.what()](const SitePtr& site)
        {
            site->Error(msg);
        });
    }
    catch (SPXHR& hrError)
    {
        SPX_TRACE_ERROR("[%p]CSpxAudioPump::PumpThread(): exception caught during pumping, SPXHR: %p", (void*)this, reinterpret_cast<void*>(hrError));
        SPX_DBG_ASSERT(GetSite() != nullptr);
        InvokeOnSite([&](const SitePtr& site)
        {
            site->Error("Error: unexpected exception in PumpThread");
        });
    }

    if (processorFormatSet)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): exited while loop with exception, pre-SetFormat(nullptr)", (void*)this);
        // catch and ignore errors, we are already on error path
        try
        {
            pISpxAudioProcessor->SetFormat(nullptr);
        }
        catch (const std::exception& e)
        {
            SPX_TRACE_ERROR("[%p]CSpxAudioPump::PumpThread(): pISpxAudioProcessor->SetFormat(nullptr) thrown exception, %s", (void*)this, e.what());
        }
        catch (SPXHR& hrError)
        {
            SPX_TRACE_ERROR("[%p]CSpxAudioPump::PumpThread(): pISpxAudioProcessor->SetFormat(nullptr) thrown error: %p", (void*)this, reinterpret_cast<void*>(hrError));
        }
        processorFormatSet = false;
    }

    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::PumpThread(): , exception happened, changing states as requested: '%s' => '%s'", (void*)this, StateName[m_state], StateName[State::Idle]);
    m_state = State::Idle;
    m_cv.notify_all();
}

void CSpxAudioPump::WaitForPumpStart(std::unique_lock<std::mutex>& lock)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpStart() ... pre m_cv.wait_for()", (void*)this);
    if (!m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStartPumpRequestTimeout), [&] { return m_state == State::Processing || m_stateRequested != State::Processing; }))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpStart(): timeout waiting on a state", (void*)this);
    }

    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpStart() ... post m_cv.wait_for(); state='%s' (requestedState='%s')", (void*)this, StateName[m_state], StateName[m_stateRequested]);
}

void CSpxAudioPump::WaitForPumpIdle(std::unique_lock<std::mutex>& lock)
{
    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpIdle() ... pre m_cv.wait_for()", (void*)this);
    if (!m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStopPumpRequestTimeout), [&] { return m_state == State::Idle || m_stateRequested != State::Idle; }))
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpIdle() timeout waiting on a state", (void*)this);
    }

    SPX_DBG_TRACE_VERBOSE("[%p]CSpxAudioPump::WaitForPumpIdle() ... post m_cv.wait_for(); state='%s' (requestedState='%s')", (void*)this, StateName[m_state], StateName[m_stateRequested]);
    SPX_TRACE_WARNING_IF(m_state != State::Idle, "[%p]CSpxAudioPump::WaitForPumpIdle(): Unexpected: state != State::Idle; state='%s'", (void*)this, StateName[m_state]);
}

std::string CSpxAudioPump::GetPropertyValue(const std::string& key) const
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    return properties->GetStringValue(key.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
