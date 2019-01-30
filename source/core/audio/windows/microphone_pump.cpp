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

AUDIO_SETTINGS_HANDLE CSpxMicrophonePump::SetOptionsBeforeCreateAudioHandle()
{
    auto channels = GetChannelsFromConfig();
    if (channels > 0)
    {
        SPX_TRACE_VERBOSE("Windows microphone only supports 1 or 2 channels.");
        SPX_IFTRUE_THROW_HR(channels != 1 && channels != 2, SPXERR_MIC_ERROR);
        SPX_TRACE_VERBOSE("The number of channel of microphone is set as %d", channels);
    }

    return CSpxMicrophonePumpBase::SetOptionsBeforeCreateAudioHandle();
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


