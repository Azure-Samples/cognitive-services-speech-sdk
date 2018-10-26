//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    // wFormatTag value for PCM data
    static constexpr unsigned short WAVE_FORMAT_PCM = 1;
    static constexpr unsigned short BITS_PER_SAMPLE = 16;
    static constexpr unsigned short CHANNELS = 1;
    static constexpr unsigned short SAMPLES_PER_SECOND = 16000;
    static constexpr unsigned short BLOCK_ALIGN = (BITS_PER_SAMPLE >> 3)*CHANNELS;
    static constexpr unsigned short AVG_BYTES_PER_SECOND = BLOCK_ALIGN * SAMPLES_PER_SECOND;

} } } } // Microsoft::CognitiveServices::Speech::Impl
