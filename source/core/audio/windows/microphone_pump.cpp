//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "microphone_pump.h"
#include "service_helpers.h"
#include "speechapi_cxx_enums.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

AUDIO_WAVEFORMAT CSpxMicrophonePump::SetOptionsBeforeCreateAudioHandle()
{
    auto channels = GetChannelsFromConfig();
    if (channels > 0)
    {
        m_format.nChannels = channels;
    }
    return { m_format.wFormatTag, m_format.nChannels, m_format.nSamplesPerSec, m_format.nAvgBytesPerSec, m_format.nBlockAlign, m_format.wBitsPerSample };
}

void CSpxMicrophonePump::SetOptionsAfterCreateAudioHandle()
{
    // size in samples for a buffer that holds the audio from Core Audio API.
    // 100 ms for now.
    const int val = 1600;
    auto result = audio_set_options(m_audioHandle, AUDIO_OPTION_INPUT_FRAME_COUNT, &val);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl


