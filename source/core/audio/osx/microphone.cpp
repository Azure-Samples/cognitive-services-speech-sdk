//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include "microphone.h"
#include "microphone_pump_base.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

class MicrophonePump : public MicrophonePumpBase
{
public:

    MicrophonePump();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

};

MicrophonePump::MicrophonePump()
{
    auto result = audio_setcallbacks(m_audioHandle,
                                     NULL, NULL,
                                     &MicrophonePump::OnInputStateChange, (void*)this,
                                     &MicrophonePump::OnInputWrite, (void*)this,
                                     NULL, NULL);
    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);

//    int val = CHANNELS;
//    result = audio_set_options(m_audioHandle, "channels", &val);
//    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
//    val = BITS_PER_SAMPLE;
//    result = audio_set_options(m_audioHandle, "bits_per_sample", &val);
//    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
//    val = SAMPLES_PER_SECOND;
//    result = audio_set_options(m_audioHandle, "sample_rate", &val);
//    SPX_IFTRUE_THROW_HR(result != AUDIO_RESULT_OK, SPXERR_MIC_ERROR);
}

shared_ptr<ISpxAudioPump> Microphone::Create()
{
    return std::make_shared<MicrophonePump>();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
