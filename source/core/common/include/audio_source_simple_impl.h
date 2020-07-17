//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_simple_impl.h: Implementation declarations/definitions for ISpxAudioSourceSimpleImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxAudioSourceSimpleImpl : public ISpxAudioSource
{
public:

    inline State GetState() const
    {
        return m_state;
    }

protected:

    inline void SetState(State state)
    {
        m_state = state;
    }

private:

    State m_state;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
