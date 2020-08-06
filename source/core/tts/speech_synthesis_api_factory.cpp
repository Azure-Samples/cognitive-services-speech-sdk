//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speech_synthesis_api_factory.cpp: Implementation definitions for CSpxSpeechSynthesisApiFactory C++ class
//

#include "stdafx.h"
#include "spxcore_common.h"
#include "create_object_helpers.h"
#include "speech_synthesis_api_factory.h"
#include "site_helpers.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


std::shared_ptr<ISpxSynthesizer> CSpxSpeechSynthesisApiFactory::CreateSpeechSynthesizerFromConfig(std::shared_ptr<ISpxAudioConfig> audioConfig)
{
    auto factoryAsSite = SpxSiteFromThis(this);

    // Create the synthesizer
    auto synthesizer = SpxCreateObjectWithSite<ISpxSynthesizer>("CSpxSynthesizer", factoryAsSite);

    // Set the synthesizer properties
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(synthesizer);

    std::shared_ptr<ISpxAudioOutput> output = nullptr;

    if (audioConfig != nullptr)
    {
        // See if we have a file, a stream, or neither, so we can initialize the synthesizer correctly...
        auto fileName = audioConfig->GetFileName();
        auto stream = audioConfig->GetStream();

        if (stream != nullptr)
        {
            // Set stream as output
            output = SpxQueryInterface<ISpxAudioOutput>(stream);
        }
        else if (fileName.length() > 0)
        {
            // Set file as output
            auto audioFileWriter = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFileWriter", SpxSiteFromThis(this));
            audioFileWriter->Open(fileName.data());
            output = SpxQueryInterface<ISpxAudioOutput>(audioFileWriter);
        }
        else
        {
            // Set default speaker as output
            output = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxDefaultSpeaker", SpxSiteFromThis(this));
        }
    }
    else
    {
        // Set an empty audio output which does nothing, this is used when user just wants to get the output from synthesis result
        output = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxNullAudioOutput", SpxSiteFromThis(this));
    }

    // Get audio output format string
    auto outputFormatStr = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthOutputFormat));

    // Get audio output format
    auto requiredFormatSize = GetSpeechSynthesisOutputFormatFromString(outputFormatStr, nullptr, 0, nullptr, nullptr);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    bool needHeader = false;
    std::string rawFormatString;
    GetSpeechSynthesisOutputFormatFromString(outputFormatStr, format.get(), requiredFormatSize, &needHeader, &rawFormatString);

    // Set format
    auto formatInit = SpxQueryInterface<ISpxAudioStreamInitFormat>(output);
    formatInit->SetFormat(format.get());

    auto outputFormatInit = SpxQueryInterface<ISpxAudioOutputInitFormat>(output);
    outputFormatInit->SetHeader(needHeader);
    outputFormatInit->SetFormatString(outputFormatStr);
    outputFormatInit->SetRawFormatString(rawFormatString);

    if (needHeader && rawFormatString == "raw-16khz-16bit-mono-pcm")
    {
        // In case GetSpeechSynthesisOutputFormatFromString fell back to default
        outputFormatInit->SetFormatString("riff-16khz-16bit-mono-pcm");
    }

    // Set output
    synthesizer->SetOutput(output);

    // We're done!
    return synthesizer;
}

uint16_t CSpxSpeechSynthesisApiFactory::GetSpeechSynthesisOutputFormatFromString(const std::string& formatStr, SPXWAVEFORMATEX* format, uint16_t cbFormat, bool* needHeader, std::string* rawFormatString)
{
    if (formatStr.data() == nullptr)
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    uint16_t formatSize = 0;
    bool requiresHeader = false;
    std::string rawString;
    if (PAL::stricmp(formatStr.data(), "raw-8khz-8bit-mono-mulaw") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MULAW, 1, 8000, 8000, 1, 8, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "riff-16khz-16kbps-mono-siren") == 0)
    {
        uint16_t extraData = 320;
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_SIREN, 1, 16000, 2000, 40, 0, 2, (uint8_t *)(&extraData));
        requiresHeader = true;
        rawString = "audio-16khz-16kbps-mono-siren";
    }
    else if (PAL::stricmp(formatStr.data(), "audio-16khz-16kbps-mono-siren") == 0)
    {
        uint16_t extraData = 320;
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_SIREN, 1, 16000, 2000, 40, 0, 2, (uint8_t *)(&extraData));
    }
    else if (PAL::stricmp(formatStr.data(), "audio-16khz-32kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 16000, 32 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "audio-16khz-128kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 16000, 128 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "audio-16khz-64kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 16000, 64 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "audio-24khz-48kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 24000, 48 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "audio-24khz-96kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 24000, 96 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "audio-24khz-160kbitrate-mono-mp3") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MP3, 1, 24000, 160 << 7, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "raw-16khz-16bit-mono-truesilk") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_SILK_SKYPE, 1, 16000, 32000, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "riff-16khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0, nullptr);
        requiresHeader = true;
        rawString = "raw-16khz-16bit-mono-pcm";
    }
    else if (PAL::stricmp(formatStr.data(), "riff-8khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 8000, 16000, 2, 16, 0, nullptr);
        requiresHeader = true;
        rawString = "raw-8khz-16bit-mono-pcm";
    }
    else if (PAL::stricmp(formatStr.data(), "riff-24khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 24000, 48000, 2, 16, 0, nullptr);
        requiresHeader = true;
        rawString = "raw-24khz-16bit-mono-pcm";
    }
    else if (PAL::stricmp(formatStr.data(), "riff-8khz-8bit-mono-mulaw") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_MULAW, 1, 8000, 8000, 1, 8, 0, nullptr);
        requiresHeader = true;
        rawString = "raw-8khz-8bit-mono-mulaw";
    }
    else if (PAL::stricmp(formatStr.data(), "raw-16khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "raw-24khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 24000, 48000, 2, 16, 0, nullptr);
    }
    else if (PAL::stricmp(formatStr.data(), "raw-8khz-16bit-mono-pcm") == 0)
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 8000, 16000, 2, 16, 0, nullptr);
    }
    else if (formatStr == "ogg-16khz-16bit-mono-opus")
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_OPUS, 1, 16000, 8192, 2, 16, 0, nullptr);
    }
    else if (formatStr == "ogg-24khz-16bit-mono-opus")
    {
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_OPUS, 1, 24000, 8192, 2, 16, 0, nullptr);
    }
    else
    {
        // Set default format to riff-16khz-16bit-mono-pcm
        formatSize = BuildSpeechSynthesisOutputFormat(format, cbFormat, WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0, nullptr);
        requiresHeader = true;
        rawString = "raw-16khz-16bit-mono-pcm";
    }

    if (needHeader != nullptr)
    {
        *needHeader = requiresHeader;
    }

    if (rawFormatString != nullptr)
    {
        *rawFormatString = rawString;
    }

    return formatSize;
}

uint16_t CSpxSpeechSynthesisApiFactory::BuildSpeechSynthesisOutputFormat( \
    SPXWAVEFORMATEX* format, uint16_t cbFormat, \
    uint16_t wFormatTag, uint16_t nChannels, uint32_t nSamplesPerSec, uint32_t nAvgBytesPerSec, uint16_t nBlockAlign, uint16_t wBitsPerSample, uint16_t cbSize, \
    uint8_t* extraData)
{
    auto srcFormat = SPXWAVEFORMATEX{ wFormatTag, nChannels, nSamplesPerSec, nAvgBytesPerSec, nBlockAlign, wBitsPerSample, cbSize };
    uint16_t basicSize = sizeof(SPXWAVEFORMATEX);
    uint16_t requiredSize = basicSize + srcFormat.cbSize;
    if (format != nullptr)
    {
        memcpy(format, &srcFormat, std::min(cbFormat, basicSize)); // Copy basic data
        if (cbFormat > basicSize && cbSize > 0 && extraData != nullptr)
        {
            memcpy(format, extraData, std::min((uint16_t)(cbFormat - basicSize), cbSize)); // Copy extra data
        }
    }

    return requiredSize;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
