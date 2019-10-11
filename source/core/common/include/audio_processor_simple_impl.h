//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_processor_simple_impl.h: Implementation declarations/definitions for ISpxAudioProcessorSimpleImpl class
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxAudioProcessorSimpleImpl : public ISpxAudioProcessor
{
public:

    void SetFormat(const SPXWAVEFORMATEX* format) override
    {
        SPX_IFTRUE_THROW_HR(format != nullptr && m_format != nullptr, SPXERR_ALREADY_INITIALIZED);
        SPX_IFTRUE_THROW_HR(format == nullptr && m_format == nullptr, SPXERR_UNINITIALIZED);
        m_format = SpxCopyWAVEFORMATEX(format);
    }

    void ProcessAudio(const DataChunkPtr& audioChunk) override
    {
        UNUSED(audioChunk);
        SPX_IFTRUE_THROW_HR(m_format == nullptr, SPXERR_UNINITIALIZED);
    }

protected:

    SpxWAVEFORMATEX_Type m_format;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
