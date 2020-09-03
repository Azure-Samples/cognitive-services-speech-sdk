//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_tts_engine_adapter.cpp: Implementation definitions for CSpxMockTtsEngineAdapter C++ class
//

#include "stdafx.h"
#include <cmath>
#include "synthesis_helper.h"
#include "mock_tts_engine_adapter.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "service_helpers.h"
#include "shared_ptr_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"
#include "create_object_helpers.h"
#include "site_helpers.h"

#define SPX_DBG_TRACE_MOCK_TTS 1


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxMockTtsEngineAdapter::CSpxMockTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_MOCK_TTS, __FUNCTION__);
}

CSpxMockTtsEngineAdapter::~CSpxMockTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_MOCK_TTS, __FUNCTION__);
}

void CSpxMockTtsEngineAdapter::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_MOCK_TTS, __FUNCTION__);
    m_audioOutput = output;
}

std::shared_ptr<ISpxSynthesisResult> CSpxMockTtsEngineAdapter::Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_MOCK_TTS, __FUNCTION__);

    UNUSED(retry);

    std::shared_ptr<ISpxSynthesisResult> result;

    InvokeOnSite([this, text, isSsml, requestId, &result](SitePtr p) {
        auto audioOutputStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());

        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));

            // Generate sine wave as the mock audio
            uint8_t* audioBuffer = new uint8_t[3200];
            for (int j = 0; j < 1600; ++j)
            {
                double x = double(i * 1600 + j) * 3.14159265359 * 2 * 400 / 16000; // 400Hz
                double y = sin(x);

                audioBuffer[j * 2] = uint16_t(y * 16384) % 256;
                audioBuffer[j * 2 + 1] = uint8_t(uint16_t(y * 16384) / 256);
            }

            p->Write(this, requestId, audioBuffer, 3200, nullptr);
            audioOutputStream->Write(audioBuffer, 3200);

            delete[] audioBuffer;
        }

        // Append SSML to the end of the audio, this is part of the mock strategy
        auto ssml = text;
        if (!isSsml)
        {
            const auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
            ssml = CSpxSynthesisHelper::BuildSsml(text, properties);
        }

        p->Write(this, requestId, (uint8_t *)(ssml.data()), (uint32_t)(ssml.length()), nullptr);
        audioOutputStream->Write((uint8_t *)(ssml.data()), (uint32_t)(ssml.length()));

        // Build result
        bool hasHeader = false;
        auto outputFormat = GetOutputFormat(&hasHeader);

        auto totalAudio = SpxAllocSharedAudioBuffer(32000 + (uint32_t)(ssml.length()));
        auto audioDataStream = SpxQueryInterface<ISpxAudioOutputReader>(audioOutputStream);
        SPX_DBG_ASSERT(audioDataStream != nullptr);
        audioDataStream->Read(totalAudio.get(), 32000 + (uint32_t)(ssml.length()));

        result = p->CreateEmptySynthesisResult();
        auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
        resultInit->InitSynthesisResult(requestId, ResultReason::SynthesizingAudioCompleted,
            REASON_CANCELED_NONE, nullptr, totalAudio.get(), 32000 + (uint32_t)(ssml.length()), outputFormat.get(), hasHeader);
    });

    return result;
}

void CSpxMockTtsEngineAdapter::StopSpeaking()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_MOCK_TTS, __FUNCTION__);
}

std::shared_ptr<ISpxNamedProperties> CSpxMockTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

SpxWAVEFORMATEX_Type CSpxMockTtsEngineAdapter::GetOutputFormat(bool* hasHeader)
{
    auto audioStream = SpxQueryInterface<ISpxAudioStream>(m_audioOutput);
    auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    audioStream->GetFormat(format.get(), requiredFormatSize);

    if (hasHeader != nullptr)
    {
        *hasHeader = SpxQueryInterface<ISpxAudioOutputFormat>(m_audioOutput)->HasHeader();
    }

    return format;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
