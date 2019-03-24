//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_format_id_2_name_map.h: internal maping function from id to its name
//

#pragma once

#include "../stdafx.h"
#include "speechapi_cxx_enums.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const char* GetAudioFormatName(const SpeechSynthesisOutputFormat& formatId)
{
    switch (formatId)
    {
    case SpeechSynthesisOutputFormat::Raw8Khz8BitMonoMULaw: return "raw-8khz-8bit-mono-mulaw";
    case SpeechSynthesisOutputFormat::Riff16Khz16KbpsMonoSiren: return "riff-16khz-16kbps-mono-siren";
    case SpeechSynthesisOutputFormat::Audio16Khz16KbpsMonoSiren: return "audio-16khz-16kbps-mono-siren";
    case SpeechSynthesisOutputFormat::Audio16Khz32KBitRateMonoMp3: return "audio-16khz-32kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Audio16Khz128KBitRateMonoMp3: return "audio-16khz-128kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Audio16Khz64KBitRateMonoMp3: return "audio-16khz-64kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Audio24Khz48KBitRateMonoMp3: return "audio-24khz-48kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Audio24Khz96KBitRateMonoMp3: return "audio-24khz-96kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Audio24Khz160KBitRateMonoMp3: return "audio-24khz-160kbitrate-mono-mp3";
    case SpeechSynthesisOutputFormat::Raw16Khz16BitMonoTrueSilk: return "raw-16khz-16bit-mono-truesilk";
    case SpeechSynthesisOutputFormat::Riff16Khz16BitMonoPcm: return "riff-16khz-16bit-mono-pcm";
    case SpeechSynthesisOutputFormat::Riff8Khz16BitMonoPcm: return "riff-8khz-16bit-mono-pcm";
    case SpeechSynthesisOutputFormat::Riff24Khz16BitMonoPcm: return "riff-24khz-16bit-mono-pcm";
    case SpeechSynthesisOutputFormat::Riff8Khz8BitMonoMULaw: return "riff-8khz-8bit-mono-mulaw";
    case SpeechSynthesisOutputFormat::Raw16Khz16BitMonoPcm: return "raw-16khz-16bit-mono-pcm";
    case SpeechSynthesisOutputFormat::Raw24Khz16BitMonoPcm: return "raw-24khz-16bit-mono-pcm";
    case SpeechSynthesisOutputFormat::Raw8Khz16BitMonoPcm: return "raw-8khz-16bit-mono-pcm";
    default:
        LogError("undefined SpeechSynthesisOutputFormat of %d", static_cast<int>(formatId));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
        return "";
    }
}


}}}}
