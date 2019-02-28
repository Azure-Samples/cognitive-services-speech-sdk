//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "create_object_helpers.h"
#include "service_helpers.h"
#include "microphone_pump_base.h"
#include "speechapi_cxx_enums.h"
#include "property_id_2_name_map.h"
#include "audio_chunk.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

CSpxMicrophonePumpBase::CSpxMicrophonePumpBase():
    m_state {State::NoInput},
    m_format { WAVE_FORMAT_PCM, CHANNELS, SAMPLES_PER_SECOND, AVG_BYTES_PER_SECOND, BLOCK_ALIGN, BITS_PER_SAMPLE, 0 }
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxMicrophonePumpBase::~CSpxMicrophonePumpBase()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_ASSERT(m_audioHandle == nullptr);
}

void CSpxMicrophonePumpBase::Init()
{
    auto sys_audio_format = SetOptionsBeforeCreateAudioHandle();

    m_audioHandle = audio_create_with_parameters(sys_audio_format);
    audio_format_destroy(sys_audio_format);
    sys_audio_format = NULL;

    SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_MIC_NOT_AVAILABLE);

    auto result = audio_setcallbacks(m_audioHandle,
        NULL, NULL,
        &CSpxMicrophonePumpBase::OnInputStateChange, (void*)this,
        &CSpxMicrophonePumpBase::OnInputWrite, (void*)this,
        NULL, NULL);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);

    SetOptionsAfterCreateAudioHandle();

    // get the nice name of the audio input device and store it in a property.
    STRING_HANDLE nice_name = get_input_device_nice_name(m_audioHandle);
    string niceNameStr = STRING_c_str(nice_name) == nullptr ? "" : STRING_c_str(nice_name);
    STRING_delete(nice_name);
    SPX_DBG_TRACE_VERBOSE("Received '%s' as nice name of the audio device", niceNameStr.c_str());
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    properties->SetStringValue("SPEECH-MicrophoneNiceName", niceNameStr.c_str());
}

void CSpxMicrophonePumpBase::Term()
{
    audio_destroy(m_audioHandle);
    m_audioHandle = nullptr;
}

AUDIO_SETTINGS_HANDLE CSpxMicrophonePumpBase::SetOptionsBeforeCreateAudioHandle()
{
    auto channels = GetChannelsFromConfig();
    if (channels > 0)
    {
        m_format.nChannels = channels;
    }

    AUDIO_SETTINGS_HANDLE format = audio_format_create();

    format->wFormatTag = m_format.wFormatTag;
    format->nChannels = m_format.nChannels;
    format->nSamplesPerSec = m_format.nSamplesPerSec;
    format->nAvgBytesPerSec = m_format.nAvgBytesPerSec;
    format->nBlockAlign = m_format.nBlockAlign;
    format->wBitsPerSample = m_format.wBitsPerSample;

    auto deviceName = GetDeviceNameFromConfig();
    STRING_copy(format->hDeviceName, deviceName.c_str());

    return format;
}

void CSpxMicrophonePumpBase::SetOptionsAfterCreateAudioHandle()
{
}

uint16_t CSpxMicrophonePumpBase::GetFormat(SPXWAVEFORMATEX* format, uint16_t size)
{
    auto totalSize = uint16_t(sizeof(SPXWAVEFORMATEX) + m_format.cbSize);
    if (format != nullptr)
    {
        memcpy(format, &m_format, min(totalSize, size));
    }
    return totalSize;
}

void CSpxMicrophonePumpBase::StartPump(SinkType processor)
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::StartPump() ...", "MicrophonePumpBase::StartPump ... Done!");

    {
        unique_lock<mutex> lock(m_mutex);

        SPX_IFTRUE_THROW_HR(processor == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(m_state == State::Processing, SPXERR_AUDIO_IS_PUMPING);

        m_sink = std::move(processor);
    }

    SPX_DBG_TRACE_VERBOSE("%s starting audio input", __FUNCTION__);
    auto result = audio_input_start(m_audioHandle);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
    SPX_DBG_TRACE_VERBOSE("%s audio input started!", __FUNCTION__);

    unique_lock<mutex> lock(m_mutex);
    // wait for audio capture thread finishing getAudioReady.
    bool pred = m_cv.wait_for(lock, std::chrono::milliseconds(m_waitMsStartPumpRequestTimeout), [&state = m_state] { return (state != State::NoInput && state != State::Idle ); });
    SPX_IFTRUE_THROW_HR(pred == false, SPXERR_TIMEOUT);
}

void CSpxMicrophonePumpBase::StopPump()
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

ISpxAudioPump::State CSpxMicrophonePumpBase::GetState()
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::GetState() ...", "MicrophonePumpBase::GetState ... Done");
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_state;
}

// this is called by audioCaptureThread, state value only changes here.
void CSpxMicrophonePumpBase::UpdateState(AUDIO_STATE state)
{
    SPX_DBG_TRACE_SCOPE("MicrophonePumpBase::UpdateState() ...", "MicrophonePumpBase::UpdateState ... Done!");
    unique_lock<mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(m_sink == nullptr, SPXERR_INVALID_ARG);

    SPX_DBG_TRACE_VERBOSE("%s: UpdateState with state as %d.", __FUNCTION__, int(state));
    switch (state)
    {
    case AUDIO_STATE_STARTING:
        m_sink->SetFormat(const_cast<SPXWAVEFORMATEX*>(&m_format));
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

int CSpxMicrophonePumpBase::Process(const uint8_t* pBuffer, uint32_t size)
{
    int result = 0;
    SPX_IFTRUE_THROW_HR(m_sink == nullptr, SPXERR_INVALID_ARG);

    if (pBuffer != nullptr)
    {
        auto sharedBuffer = SpxAllocSharedAudioBuffer(size);
        memcpy(sharedBuffer.get(), pBuffer, size);
        m_sink->ProcessAudio(std::make_shared<DataChunk>(sharedBuffer, size));
    }

    return result;
}

uint16_t CSpxMicrophonePumpBase::GetChannelsFromConfig()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto channels = properties->GetStringValue(GetPropertyName(PropertyId::AudioConfig_NumberOfChannelsForCapture));
    SPX_DBG_TRACE_VERBOSE("The number of channels as a property is '%s' in CSpxMicrophonePump", channels.c_str());
    return channels.empty() ? 0 : static_cast<uint16_t>(stoi(channels));
}

std::string CSpxMicrophonePumpBase::GetDeviceNameFromConfig()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_INVALID_ARG);

    auto deviceName = properties->GetStringValue(GetPropertyName(PropertyId::AudioConfig_DeviceNameForCapture));
    SPX_DBG_TRACE_VERBOSE("The device name of microphone as a property is '%s'", deviceName.c_str());

    return deviceName;
}


std::string CSpxMicrophonePumpBase::GetPropertyValue(const std::string& key) const
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    return properties->GetStringValue(key.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

