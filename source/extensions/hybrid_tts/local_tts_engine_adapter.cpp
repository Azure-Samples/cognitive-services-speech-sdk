//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// local_tts_engine_adapter.cpp: Implementation definitions for CSpxLocalTtsEngineAdapter C++ class
//

#include <sstream>

#include "stdafx.h"
#include "local_tts_engine_adapter.h"
#include "ms_tts_output.h"
#include "create_object_helpers.h"
#include "guid_utils.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "shared_ptr_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"

#define SPX_DBG_TRACE_LOCAL_TTS 1


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxLocalTtsEngineAdapter::CSpxLocalTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_LOCAL_TTS, __FUNCTION__);
}

CSpxLocalTtsEngineAdapter::~CSpxLocalTtsEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    Term();
}

void CSpxLocalTtsEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    m_engineError = MSTTS_CreateSpeechSynthesizer(&m_pMsTtsEngine);
    if (m_engineError == 0)
    {
        const auto voiceFolder = GetStringValue("SPEECH-SynthOfflineDataLocation", "");
        if (voiceFolder.empty())
        {
            SPX_DBG_TRACE_ERROR("Data folder is not specified, offline TTS initialization failed");
            m_engineError = 1;
        }
        else
        {
            m_engineError = m_pMsTtsEngine->InstallVoices(voiceFolder.c_str());
        }
    }

    const MSTTSVoiceInfo* pVoiceInfo = nullptr;
    uint32_t nVoiceCnt = 0;
    if (m_engineError == 0)
    {
        m_engineError = m_pMsTtsEngine->GetInstalledVoices(&pVoiceInfo, &nVoiceCnt);
    }

    if (m_engineError == 0)
    {
        m_voiceName = std::string(pVoiceInfo[0]._pcszVoiceName);
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_LOCAL_TTS, "local engine voice name: %s", m_voiceName.c_str());
        m_engineError = m_pMsTtsEngine->SetVoice(&pVoiceInfo[0]);
    }

    if (m_engineError != 0)
    {
        std::stringstream ss;
        ss << "Local TTS initialization failed, with TTS error code = ";
        ss << m_engineError;
        ss << ".";
        SPX_DBG_TRACE_ERROR(ss.str().c_str());
        if (m_pMsTtsEngine)
        {
            MSTTS_DeleteSpeechSynthesizer(m_pMsTtsEngine);
            m_pMsTtsEngine = nullptr;
        }
    }
}

void CSpxLocalTtsEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_pMsTtsEngine != nullptr)
    {
        MSTTS_DeleteSpeechSynthesizer(m_pMsTtsEngine);
        m_pMsTtsEngine = nullptr;
    }
}

void CSpxLocalTtsEngineAdapter::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_LOCAL_TTS, __FUNCTION__);
    m_audioOutput = output;
}

std::shared_ptr<ISpxSynthesisResult> CSpxLocalTtsEngineAdapter::Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_LOCAL_TTS, __FUNCTION__);

    UNUSED(retry);

    auto result = GetSite()->CreateEmptySynthesisResult();
    auto properties = SpxQueryInterface<ISpxNamedProperties>(result);
    properties->SetStringValue("SynthesisFinishedBy", "offline");
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);

    CSpxMsTtsOutput msTtsOutput;
    msTtsOutput.SetOutput(m_audioOutput);
    msTtsOutput.SetAdapterSite(GetSite());
    msTtsOutput.SetAdapter(this);
    msTtsOutput.SetRequestId(requestId);

    int charNumber = 0;

    MSTTSERROR error;
    std::string errorMessage;

    if (m_pMsTtsEngine)
    {
        error = m_pMsTtsEngine->SetOutput(&msTtsOutput);
    }
    else
    {
        error = m_engineError;
        errorMessage = "Local TTS engine not initialized, check data location and storage permissions.";
    }

    if (error == 0)
    {
        error = m_pMsTtsEngine->Speak(text.data(), isSsml ? MSTTSContentType::MSTTSContentType_SSML : MSTTSContentType::MSTTSContentType_PlainText, &charNumber);
    }

    msTtsOutput.Close();

    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(&hasHeader);

    if (error == 0)
    {
        auto buffer = SpxAllocSharedBuffer<uint8_t>(AUDIO_OUTPUT_BUFFER_SIZE);
        auto dataSize = msTtsOutput.GetAudioData(buffer.get(), AUDIO_OUTPUT_BUFFER_SIZE);

        resultInit->InitSynthesisResult(requestId, ResultReason::SynthesizingAudioCompleted,
            REASON_CANCELED_NONE, nullptr, buffer.get(), dataSize, outputFormat.get(), hasHeader);
        properties->SetStringValue("CharNumber", std::to_string(charNumber).c_str());
        properties->SetStringValue("OfflineVoiceName", m_voiceName.c_str());
    }
    else
    {
        resultInit->InitSynthesisResult(requestId, ResultReason::Canceled, CancellationReason::Error,
            nullptr, nullptr, 0, outputFormat.get(), hasHeader);

        // Set error detail string
        std::stringstream ss;
        ss << "Local TTS speak failed, with TTS error code = ";
        ss << error;
        ss << ". ";
        ss << errorMessage;
        properties->SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), ss.str().c_str());
    }

    return result;
}

void CSpxLocalTtsEngineAdapter::StopSpeaking()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_pMsTtsEngine != nullptr)
    {
        m_pMsTtsEngine->Stop();
    }
}

std::shared_ptr<ISpxNamedProperties> CSpxLocalTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

SpxWAVEFORMATEX_Type CSpxLocalTtsEngineAdapter::GetOutputFormat(bool* hasHeader)
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
