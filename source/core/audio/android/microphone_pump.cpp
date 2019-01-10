//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "microphone_pump.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxMicrophonePump::SetOptionsAfterCreateAudioHandle()
{
    auto deviceName = GetDeviceNameFromConfig();
    if (!deviceName.empty())
    {
        SPX_IFTRUE_THROW_HR(m_audioHandle == nullptr, SPXERR_MIC_NOT_AVAILABLE);
        auto result = audio_set_options(m_audioHandle, AUDIO_OPTION_DEVICENAME, deviceName.c_str());
        SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
        SPX_TRACE_INFO("The device name of microphone is set as '%s'", deviceName.c_str());
    }
    auto channels = GetChannelsFromConfig();
    if (channels > 0)
    {
        SPX_TRACE_INFO("Right now, Linux microphone only support 1 or 2 channels.");
        SPX_IFTRUE_THROW_HR(channels != 1 || channels != 2, SPXERR_MIC_ERROR);

        auto result = audio_set_options(m_audioHandle, AUDIO_OPTION_NUMBER_CHANNELS, &channels);
        SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
        SPX_TRACE_INFO("The number of channel of microphone is set as %d", channels);
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
