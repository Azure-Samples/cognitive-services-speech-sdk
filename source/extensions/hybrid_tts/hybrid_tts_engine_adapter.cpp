//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// hybrid_tts_engine_adapter.cpp: Implementation definitions for CSpxHybridTtsEngineAdapter C++ class
//

#include <sstream>

#include "stdafx.h"
#include "hybrid_tts_engine_adapter.h"
#include "ms_tts_output.h"
#include "create_object_helpers.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include "property_id_2_name_map.h"
#include "site_helpers.h"

#define SPX_DBG_TRACE_HYBRID_TTS 1

namespace Microsoft
{
namespace CognitiveServices
{
namespace Speech
{
namespace Impl
{

CSpxHybridTtsEngineAdapter::CSpxHybridTtsEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_HYBRID_TTS, __FUNCTION__);
}

CSpxHybridTtsEngineAdapter::~CSpxHybridTtsEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    CSpxHybridTtsEngineAdapter::Term();
}

void CSpxHybridTtsEngineAdapter::Init()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_HYBRID_TTS, __FUNCTION__);

    std::tie(m_switchingPolicy, m_fallbackThreshold) = ClarifyPolicy(
        GetStringValue("SPEECH-SynthBackendSwitchingPolicy", "cloud_first_connect"));

    originalCloudConnectedTimeoutMs = GetStringValue("SpeechSynthesis_FirstChunkTimeoutMs", "10000");
    originalCloudFinishedTimeoutMs = GetStringValue("SpeechSynthesis_AllChunkTimeoutMs", "30000");

    if (m_switchingPolicy != SwitchingPolicy::ForceOffline)
    {
        InitializeTtsCloudEngineAdapter();
    }

    if (m_switchingPolicy != SwitchingPolicy::ForceCloud)
    {
        InitializeOfflineTtsEngineAdapter();
    }
}

void CSpxHybridTtsEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SpxTermAndClear(m_ttsCloudAdapter);
    SpxTermAndClear(m_ttsOfflineAdapter);
}

void CSpxHybridTtsEngineAdapter::SetOutput(std::shared_ptr<ISpxAudioOutput> output)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_HYBRID_TTS, __FUNCTION__);
    m_audioOutput = output;
    if (m_ttsOfflineAdapter)
    {
        m_ttsOfflineAdapter->SetOutput(output);
    }

    if (m_ttsCloudAdapter)
    {
        m_ttsCloudAdapter->SetOutput(output);
    }
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::Speak(const std::string& text, bool isSsml,
                                                                       const std::wstring& requestId, bool retry)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_HYBRID_TTS, __FUNCTION__);

    // store configs as local variables
    std::tie(m_switchingPolicy, m_fallbackThreshold) = ClarifyPolicy(
        GetStringValue("SPEECH-SynthBackendSwitchingPolicy", "cloud_first_connect"));

    m_currentRequestId = requestId;

    if (m_switchingPolicy == SwitchingPolicy::CloudFirst || m_switchingPolicy == SwitchingPolicy::Parallel)
    {
        cloudConnectedTimeoutMs = std::stoi(GetStringValue("SPEECH-SynthBackendFallbackConnectedTimeoutMs", "2000"));
        SetStringValue("SpeechSynthesis_FirstChunkTimeoutMs", std::to_string(cloudConnectedTimeoutMs).c_str());

        EnsureTtsCloudEngineAdapter();
        EnsureTtsOfflineEngineAdapter();
    }

    if (m_switchingPolicy == SwitchingPolicy::ForceCloud)
    {
        // set timeout value to default
        SetStringValue("SpeechSynthesis_FirstChunkTimeoutMs", originalCloudConnectedTimeoutMs.c_str());
        SetStringValue("SpeechSynthesis_AllChunkTimeoutMs", originalCloudFinishedTimeoutMs.c_str());
        EnsureTtsCloudEngineAdapter();
        return m_ttsCloudAdapter->Speak(text, isSsml, requestId, retry);
    }
    else if (m_switchingPolicy == SwitchingPolicy::ForceOffline)
    {
        EnsureTtsOfflineEngineAdapter();
        return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
    }
    else if (m_fallbackThreshold == FallbackThreshold::Connect)
    {
        return SpeakByConnectPolicy(text, isSsml, requestId, retry, m_switchingPolicy);
    }
    else if (m_fallbackThreshold == FallbackThreshold::Buffer)
    {
        return SpeakByBufferPolicy(text, isSsml, requestId, retry, m_switchingPolicy);
    }
    else if (m_fallbackThreshold == FallbackThreshold::Finish)
    {
        cloudFinishedTimeoutMs = std::stoi(GetStringValue("SPEECH-SynthBackendFallbackFinishedTimeoutMs", "3000"));
        SetStringValue("SpeechSynthesis_AllChunkTimeoutMs", std::to_string(cloudFinishedTimeoutMs).c_str());
        return SpeakByFinishPolicy(text, isSsml, requestId, retry, m_switchingPolicy);
    }

    return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
}

void CSpxHybridTtsEngineAdapter::StopSpeaking()
{
    if (m_ttsOfflineAdapter != nullptr)
    {
        m_ttsOfflineAdapter->StopSpeaking();
    }
    if (m_ttsCloudAdapter != nullptr)
    {
        m_ttsCloudAdapter->StopSpeaking();
    }
}

uint32_t CSpxHybridTtsEngineAdapter::Write(ISpxTtsEngineAdapter *adapter, const std::wstring &requestId,
                                           uint8_t *buffer, uint32_t size, std::shared_ptr<std::unordered_map<std::string, std::string>> properties)
{
    UNUSED(properties);

    auto offlineProperty = std::make_shared<std::unordered_map<std::string, std::string>>();
    offlineProperty->insert(std::make_pair<std::string, std::string>("SynthesisFinishedBy", "offline"));
    if (requestId != m_currentRequestId)
    {
        // audio not in current request, ignore
        return size;
    }

    if (m_fallbackThreshold == FallbackThreshold::Connect && adapter == m_ttsCloudAdapter.get())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_synthesisState = SynthesisState::CloudSynthesizing_BufferFilled;
    }

    if (m_fallbackThreshold != FallbackThreshold::None && adapter == m_ttsCloudAdapter.get())
    {
        if (m_ttsCloudAdapter == nullptr)
        {
            if (m_synthesisState == SynthesisState::CloudCanceled)
            {
                return size;
            }
        }
        else if (m_synthesisState == SynthesisState::CloudSynthesizing_BufferFilling)
        {
            return m_cloudAudioStream->Write(buffer, size);
        }
        else if (m_synthesisState == SynthesisState::CloudSynthesizing_BufferFilled || m_synthesisState ==
            SynthesisState::CloudSynthesized)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_cloudAudioStream != nullptr)
            {
                m_cloudAudioStream->Close();
                if (m_cloudAudioStreamReader->AvailableSize() > 0)
                {
                    std::vector<uint8_t> tmpBuffer(m_cloudAudioStreamReader->AvailableSize());
                    const auto tmpSize = m_cloudAudioStreamReader->Read(tmpBuffer.data(), static_cast<uint32_t>(tmpBuffer.size()));
                    GetSite()->Write(adapter, requestId, tmpBuffer.data(), tmpSize, nullptr);
                }
                m_cloudAudioStream.reset();
            }
        }
        else if (m_synthesisState == SynthesisState::CloudCanceled)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_cloudAudioStream != nullptr)
            {
                m_cloudAudioStream->Close();
                m_cloudAudioStream.reset();
            }

            return size;
        }
    }
    else if (m_fallbackThreshold != FallbackThreshold::None && adapter == m_ttsOfflineAdapter.get())
    {
        if (m_offlineAudioStream == nullptr)
        {
            if (m_synthesisState != SynthesisState::CloudCanceled)
            {
                return size;
            }
        }
        else if (m_synthesisState == SynthesisState::CloudSynthesizing_BufferFilling)
        {
            return m_offlineAudioStream->Write(buffer, size);
        }
        else if (m_synthesisState == SynthesisState::CloudSynthesizing_BufferFilled || m_synthesisState ==
            SynthesisState::CloudSynthesized)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_offlineAudioStream != nullptr)
            {
                m_offlineAudioStream->Close();
                m_offlineAudioStream.reset();
            }
            return size;
        }
        else if (m_synthesisState == SynthesisState::CloudCanceled)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_offlineAudioStream != nullptr)
            {
                m_offlineAudioStream->Close();
                if (m_offlineAudioStreamReader->AvailableSize() > 0)
                {
                    std::vector<uint8_t> tmpBuffer(m_offlineAudioStreamReader->AvailableSize());
                    const auto tmpSize = m_offlineAudioStreamReader->Read(tmpBuffer.data(), static_cast<uint32_t>(tmpBuffer.size()));
                    GetSite()->Write(adapter, requestId, tmpBuffer.data(), tmpSize, offlineProperty);
                }
                m_offlineAudioStream.reset();
            }
        }
    }

    if (size <= 0)
    {
        return size;
    }

    auto site = GetSite();
    return site->Write(adapter, requestId, buffer, size, adapter == m_ttsCloudAdapter.get() ? nullptr : offlineProperty);
}

std::shared_ptr<ISpxSynthesizerEvents> CSpxHybridTtsEngineAdapter::GetEventsSite()
{
    return GetSite()->GetEventsSite();
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::CreateEmptySynthesisResult()
{
    return GetSite()->CreateEmptySynthesisResult();
}

std::shared_ptr<ISpxNamedProperties> CSpxHybridTtsEngineAdapter::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

SpxWAVEFORMATEX_Type CSpxHybridTtsEngineAdapter::GetOutputFormat(bool *hasHeader) const
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

void CSpxHybridTtsEngineAdapter::UpdateStreamReader()
{
    if (m_cloudAudioStream != nullptr)
    {
        m_cloudAudioStreamReader = SpxQueryInterface<ISpxAudioOutputReader>(m_cloudAudioStream);
        SPX_DBG_ASSERT(m_cloudAudioStreamReader != nullptr);
    }
    if (m_offlineAudioStream != nullptr)
    {
        m_offlineAudioStreamReader = SpxQueryInterface<ISpxAudioOutputReader>(m_offlineAudioStream);
        SPX_DBG_ASSERT(m_offlineAudioStreamReader != nullptr);
    }
}

void CSpxHybridTtsEngineAdapter::EnsureTtsCloudEngineAdapter()
{
    if (m_ttsCloudAdapter == nullptr)
    {
        InitializeTtsCloudEngineAdapter();
    }
}

void CSpxHybridTtsEngineAdapter::InitializeTtsCloudEngineAdapter()
{
    m_ttsCloudAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxUspTtsEngineAdapter", this);
    if (m_audioOutput)
    {
        m_ttsCloudAdapter->SetOutput(m_audioOutput);
    }
}

void CSpxHybridTtsEngineAdapter::EnsureTtsOfflineEngineAdapter()
{
    if (m_ttsOfflineAdapter == nullptr)
    {
        InitializeOfflineTtsEngineAdapter();
    }
}

void CSpxHybridTtsEngineAdapter::InitializeOfflineTtsEngineAdapter()
{
    m_ttsOfflineAdapter = SpxCreateObjectWithSite<ISpxTtsEngineAdapter>("CSpxLocalTtsEngineAdapter", this);
    if (m_audioOutput)
    {
        m_ttsOfflineAdapter->SetOutput(m_audioOutput);
    }
}

std::pair<CSpxHybridTtsEngineAdapter::SwitchingPolicy, CSpxHybridTtsEngineAdapter::FallbackThreshold>
CSpxHybridTtsEngineAdapter::ClarifyPolicy(const std::string& policy)
{
    if (policy == "force_cloud")
    {
        return std::make_pair(SwitchingPolicy::ForceCloud, FallbackThreshold::None);
    }
    else if (policy == "force_offline")
    {
        return std::make_pair(SwitchingPolicy::ForceOffline, FallbackThreshold::None);
    }
    else if (policy == "cloud_first_connect")
    {
        return std::make_pair(SwitchingPolicy::CloudFirst, FallbackThreshold::Connect);
    }
    else if (policy == "cloud_first_buffer")
    {
        return std::make_pair(SwitchingPolicy::CloudFirst, FallbackThreshold::Buffer);
    }
    else if (policy == "cloud_first_finish")
    {
        return std::make_pair(SwitchingPolicy::CloudFirst, FallbackThreshold::Finish);
    }
    else if (policy == "parallel_connect")
    {
        return std::make_pair(SwitchingPolicy::Parallel, FallbackThreshold::Connect);
    }
    else if (policy == "parallel_buffer")
    {
        return std::make_pair(SwitchingPolicy::Parallel, FallbackThreshold::Buffer);
    }
    else if (policy == "parallel_finish")
    {
        return std::make_pair(SwitchingPolicy::Parallel, FallbackThreshold::Finish);
    }
    return std::make_pair(SwitchingPolicy::CloudFirst, FallbackThreshold::Finish);
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::SpeakByConnectPolicy(
    const std::string& text, bool isSsml, const std::wstring& requestId, bool retry, SwitchingPolicy switchingPolicy)
{
    SetStringValue("SpeechSynthesis_AllChunkTimeoutMs", originalCloudFinishedTimeoutMs.c_str());

    if (switchingPolicy == SwitchingPolicy::Parallel)
    {
        EnsureTtsOfflineEngineAdapter();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_synthesisState = SynthesisState::CloudSynthesizing_BufferFilling;
            m_offlineAudioStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
            UpdateStreamReader();
        }

        auto keepAlive = SpxSharedPtrFromThis<ISpxTtsEngineAdapterSite>(this);
        m_offlineResult = std::shared_future<std::shared_ptr<ISpxSynthesisResult>>(std::async(
            std::launch::async, [this, keepAlive, requestId, text, isSsml]()
            {
            return this->m_ttsOfflineAdapter->Speak(text, isSsml, requestId, false);
        }));
    }

    auto result = m_ttsCloudAdapter->Speak(text, isSsml, requestId, false);

    if (result->GetReason() == ResultReason::Canceled && result->GetAudioLength() == 0)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_synthesisState = SynthesisState::CloudCanceled;
        }

        if (m_switchingPolicy == SwitchingPolicy::CloudFirst)
        {
            EnsureTtsOfflineEngineAdapter();
            return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
        }
        else if (m_switchingPolicy == SwitchingPolicy::Parallel)
        {
            Write(m_ttsOfflineAdapter.get(), m_currentRequestId, nullptr, 0, nullptr);
            return m_offlineResult.get();
        }
    }
    return result;
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::SpeakByBufferPolicy(
    const std::string& text, bool isSsml, const std::wstring& requestId, bool retry, SwitchingPolicy switchingPolicy)
{
    SetStringValue("SpeechSynthesis_AllChunkTimeoutMs", originalCloudFinishedTimeoutMs.c_str());

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_synthesisState = SynthesisState::CloudSynthesizing_BufferFilling;
        m_cloudAudioStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
        if (switchingPolicy == SwitchingPolicy::Parallel)
        {
            m_offlineAudioStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
        }
        UpdateStreamReader();
    }

    auto keepAlive = SpxSharedPtrFromThis<ISpxTtsEngineAdapterSite>(this);
    m_cloudResult = std::shared_future<std::shared_ptr<ISpxSynthesisResult>>(std::async(std::launch::async, [this, keepAlive, requestId, text, isSsml]() {
        if (this->m_lastCloudResult.valid())
        {
            if (this->m_lastCloudResult.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
            {
                return this->DummySpeak(requestId);
            }
        }
        this->m_lastCloudResult = this->m_cloudResult;
        return this->m_ttsCloudAdapter->Speak(text, isSsml, requestId, false);
    }));

    if (switchingPolicy == SwitchingPolicy::Parallel)
    {
        m_offlineResult = std::shared_future<std::shared_ptr<ISpxSynthesisResult>>(std::async(std::launch::async, [this, keepAlive, requestId, text, isSsml]() {
            return this->m_ttsOfflineAdapter->Speak(text, isSsml, requestId, false);
        }));
    }

    const auto bufferTimeout = std::stoi(GetStringValue("SPEECH-SynthBackendFallbackBufferTimeoutMs", "800"));
    const auto bufferSize = std::stoi(GetStringValue("SPEECH-SynthBackendFallbackBufferLengthMs", "500"))
            * GetOutputFormat(nullptr)->nAvgBytesPerSec / 1000;
    for (int remaining = 0; remaining < bufferTimeout; remaining += 20) {
        if (m_cloudResult.wait_for(std::chrono::milliseconds(20)) == std::future_status::ready || m_cloudAudioStreamReader->AvailableSize() > bufferSize)
        {
            break;
        }
    }

    auto cloudResultStatus = m_cloudResult.wait_for(std::chrono::milliseconds(0));
    if ((m_cloudAudioStreamReader->AvailableSize() > bufferSize && cloudResultStatus != std::future_status::ready) ||
        (cloudResultStatus == std::future_status::ready && m_cloudResult.get()->GetReason() == ResultReason::SynthesizingAudioCompleted))
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_synthesisState = SynthesisState::CloudSynthesizing_BufferFilled;
        }

        Write(m_ttsCloudAdapter.get(), m_currentRequestId, nullptr, 0, nullptr);
        return m_cloudResult.get();
    }
    else
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_synthesisState = SynthesisState::CloudCanceled;
        }

        if (switchingPolicy == SwitchingPolicy::Parallel)
        {
            Write(m_ttsOfflineAdapter.get(), m_currentRequestId, nullptr, 0, nullptr);
            return m_offlineResult.get();
        }
        else if (switchingPolicy == SwitchingPolicy::CloudFirst)
        {
            EnsureTtsOfflineEngineAdapter();
            return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
        }
    }

    return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::SpeakByFinishPolicy(const std::string& text,
    bool isSsml, const std::wstring& requestId, bool retry, SwitchingPolicy switchingPolicy)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_synthesisState = SynthesisState::CloudSynthesizing_BufferFilling;
        m_cloudAudioStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
        if (switchingPolicy == SwitchingPolicy::Parallel)
        {
            m_offlineAudioStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
        }
        UpdateStreamReader();
    }

    if (switchingPolicy == SwitchingPolicy::Parallel)
    {
        EnsureTtsOfflineEngineAdapter();
        auto keepAlive = SpxSharedPtrFromThis<ISpxTtsEngineAdapterSite>(this);
        m_offlineResult = std::shared_future<std::shared_ptr<ISpxSynthesisResult>>(std::async(
            std::launch::async, [this, keepAlive, requestId, text, isSsml]()
            {
            return this->m_ttsOfflineAdapter->Speak(text, isSsml, requestId, false);
        }));
    }

    auto result = m_ttsCloudAdapter->Speak(text, isSsml, requestId, false);
    if (result->GetReason() == ResultReason::SynthesizingAudioCompleted)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_synthesisState = SynthesisState::CloudSynthesized;
        }

        // write here.
        Write(m_ttsCloudAdapter.get(), m_currentRequestId, nullptr, 0, nullptr);
        return result;
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_synthesisState = SynthesisState::CloudCanceled;
    }

    if (switchingPolicy == SwitchingPolicy::Parallel)
    {
        Write(m_ttsOfflineAdapter.get(), m_currentRequestId, nullptr, 0, nullptr);
        return m_offlineResult.get();
    }

    EnsureTtsOfflineEngineAdapter();
    return m_ttsOfflineAdapter->Speak(text, isSsml, requestId, retry);
}

std::shared_ptr<ISpxSynthesisResult> CSpxHybridTtsEngineAdapter::DummySpeak(const std::wstring& requestId)
{
    auto result = GetSite()->CreateEmptySynthesisResult();
    bool hasHeader = false;
    auto outputFormat = GetOutputFormat(&hasHeader);
    auto resultInit = SpxQueryInterface<ISpxSynthesisResultInit>(result);
    resultInit->InitSynthesisResult(requestId, ResultReason::Canceled, CancellationReason::Error,
            CancellationErrorCode::RuntimeError, nullptr, 0, outputFormat.get(), hasHeader);
    return result;
}

} // namespace Impl
} // namespace Speech
} // namespace CognitiveServices
} // namespace Microsoft
