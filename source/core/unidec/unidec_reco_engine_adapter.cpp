//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// unidec_reco_engine_adapter.cpp: Implementation definitions for CSpxUnidecRecoEngineAdapter C++ class
//

#include "stdafx.h"
#include "unidec_reco_engine_adapter.h"
#include "guid_utils.h"
#include "handle_table.h"
#include "service_helpers.h"


#define SPX_DBG_TRACE_UNIDEC_AUDIO 0


namespace CARBON_IMPL_NAMESPACE() {


CSpxUnidecRecoEngineAdapter::CSpxUnidecRecoEngineAdapter() :
    m_sentenceComplete(false),
    m_bytesLeftInBuffer(0),
    m_ptrIntoBuffer(nullptr),
    m_stopImmediately(false)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);
}

CSpxUnidecRecoEngineAdapter::~CSpxUnidecRecoEngineAdapter()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);
}

void CSpxUnidecRecoEngineAdapter::Init()
{
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(IsInit(), SPXERR_ALREADY_INITIALIZED);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);

    InitConfig();
}

void CSpxUnidecRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);
    StopEngine();
}

void CSpxUnidecRecoEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(!IsInit(), SPXERR_UNINITIALIZED);

    if (pformat != nullptr)
    {
        InitFormat(pformat);
        EnsureEngine();
        StartEngine();
    }
    else
    {
        TermFormat();
        StopEngine();
    }
}

void CSpxUnidecRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    AudioBufferWrite(data, size);
}

void CSpxUnidecRecoEngineAdapter::InitConfig()
{
    std::map<std::string, std::wstring> config;

    config["EnableSegmentation"] = L"true";
    config["BaseModelPath"] = GetBaseModelPath().c_str();
    config["FeatureName"] = L"16kHzStaticStreamE14LFB80Runtime";
    config["HCLGSpecBase"] = L"";
    config["BeamThreshold"] = L"200";
    config["BeamSize"] = L"7000";
    config["NBestBeamSize"] = L"1";
    config["LocaleId"] = L"1033";
    config["BaseModelId"] = L"SR_MS_en-US_LS4M_LSTM_11.0";
    config["FrameCopyCount"] = L"1";
    config["InDnnSpecPrefix"] = L"ipe(";
    config["InDnnSpecPostfix"] = L",1)";

    int pick = 4;
    switch (pick)
    {
        default:
        case 1:
            config["AmFileName"] = L"SR_MS_en-US_LS4M_LSTM_11.0_AI011033.am";
            config["FeFileName"] = L"SR_MS_en-US_LS4M_LSTM_11.0_c1033.fe";
            config["LmsFileName"] = L"SR_MS_en-US_LS4M_LSTM_11.0_lms.lms";
            config["HclgFileName"] = L"SR_MS_en-US_LS4M_LSTM_11.0_hclg.hclg";
            break;

        case 2:
            config["AmFileName"] = L"AI011033.am";
            config["FeFileName"] = L"c1033.fe";
            config["LmsFileName"] = LR"(mini\prune.3e-7.minhclg.lms)";
            config["HclgFileName"] = LR"(mini\prune.3e-7.minhclg.hclg)";
            break;
    }

    m_config = std::make_unique<CSpxUnidecConfig>(config);
}

std::wstring CSpxUnidecRecoEngineAdapter::GetBaseModelPath()
{
    // Get the named properties service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_UNIDEC_SITE_FAILURE);

    // Get the property value for the base model path
    auto value = properties->GetStringValue(L"__unidecBaseModelPath", LR"(..\..\..\external\unidec\bin\)");
    return value;
}

void CSpxUnidecRecoEngineAdapter::InitFormat(WAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(WAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);
}

void CSpxUnidecRecoEngineAdapter::TermFormat()
{
    m_format = nullptr;
}

void CSpxUnidecRecoEngineAdapter::EnsureEngine()
{
    if (m_unidecEngine.get() == nullptr)
    {
        InitEngine();
    }
}

void CSpxUnidecRecoEngineAdapter::InitEngine()
{
    InitSearchGraphs();

    m_unidecEngine = std::unique_ptr<IUnidecEngine, std::function<void(IUnidecEngine*)>>(CreateUnidecStreamEngine(
        m_graphs.get(),
        m_config->GetFronEndSpecString().c_str(),
        m_config->GetDnnSpecString().c_str(),
        std::stoi(m_config->BeamSize),
        m_config->GetBeamThresholdValue(),
        std::stoi(m_config->NBestBeamSize),
        CSpxUnidecRecoEngineAdapter::NextStreamCallback,
        CSpxUnidecRecoEngineAdapter::IntermediateCallback,
        CSpxUnidecRecoEngineAdapter::SentenceCallback,
        CSpxUnidecRecoEngineAdapter::EndCallback,
        this),
        DeleteUnidecEngine);
}

void CSpxUnidecRecoEngineAdapter::InitSearchGraphs()
{
    std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>> base_graphs;
    if (m_config->HCLGSpecBase.length() > 0)
    {
        base_graphs =
            std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>>(
                CreateUnidecSearchGraphCombo(m_config->HCLGSpecBase.c_str()),
                DeleteUnidecSearchGraphCombo);
    }

    m_graphs = std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>>(
        CreateUnidecSearchGraphComboEx(m_config->GetHCLGSpecString().c_str(), base_graphs.get()),
        DeleteUnidecSearchGraphCombo);
}

void CSpxUnidecRecoEngineAdapter::StartEngine()
{
    SPX_IFTRUE_THROW_HR(GetUnidecEngineStarted(m_unidecEngine.get()), SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION);

    m_stopImmediately = false;
    StartUnidecEngine(m_unidecEngine.get());
}

void CSpxUnidecRecoEngineAdapter::StopEngine()
{
    if (GetUnidecEngineStarted(m_unidecEngine.get()))
    {
        m_stopImmediately = true;
        m_cv.notify_all();

        JoinUnidecEngine(m_unidecEngine.get());
        m_stopImmediately = false;
    }
}

bool CSpxUnidecRecoEngineAdapter::NextStreamCallback(AudioStreamDescriptor** pAudioStream,  bool* enableSegmentation, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    return unidecRecoEngineAdapter->NextStream(pAudioStream, enableSegmentation);
}

void CSpxUnidecRecoEngineAdapter::IntermediateCallback(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->Intermediate(wavId, sentenceIndex, framePos, intermediateResult);
}

void CSpxUnidecRecoEngineAdapter::SentenceCallback(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->Sentence(wavId, sentenceIndex, framePos, nbest);
}

void CSpxUnidecRecoEngineAdapter::EndCallback(const wchar_t* wavId, UnidecEndReason reason, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->End(wavId, reason);
}

size_t CSpxUnidecRecoEngineAdapter::AudioStreamReadCallback(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)pAudioStream->CallbackContext;
    return unidecRecoEngineAdapter->AudioStreamRead(pAudioStream, pBuffer, maxSize);
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsEndCallback(AudioStreamDescriptor* pAudioStream)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)pAudioStream->CallbackContext;
    return unidecRecoEngineAdapter->AudioStreamIsEnd(pAudioStream);
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsErrorCallback(AudioStreamDescriptor* pAudioStream)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)pAudioStream->CallbackContext;
    return unidecRecoEngineAdapter->AudioStreamIsError(pAudioStream);
}

bool CSpxUnidecRecoEngineAdapter::NextStream(AudioStreamDescriptor** pAudioStream,  bool* enableSegmentation)
{
    if (!HasFormat())
    {
        return false;
    }

    return InitStream(pAudioStream, enableSegmentation);
}

bool CSpxUnidecRecoEngineAdapter::InitStream(AudioStreamDescriptor** pAudioStream, bool* enableSegmentation)
{
    m_streamId = PAL::CreateGuid();
    m_audioStream.StreamId = m_streamId.c_str();
    
    m_audioStream.wFormatTag = m_format->wFormatTag;
    m_audioStream.nChannels = m_format->nChannels;
    m_audioStream.nSamplesPerSec = m_format->nSamplesPerSec;
    m_audioStream.nAvgBytesPerSec = m_format->nAvgBytesPerSec;
    m_audioStream.nBlockAlign = m_format->nBlockAlign;
    m_audioStream.wBitsPerSample = m_format->wBitsPerSample;

    m_audioStream.StartTimeout = 0; // ms
    m_audioStream.EndTimeout = 0;   // ms
    m_audioStream.SilenceThreshold = 300; // ms
    m_audioStream.WordCountThreshold = 5; // words
    m_audioStream.AudioLengthThreshold = 5000; // ms

    m_audioStream.CallbackContext = static_cast<void*>(this);
    m_audioStream.ReadCallback = CSpxUnidecRecoEngineAdapter::AudioStreamReadCallback;
    m_audioStream.IsEndCallback = CSpxUnidecRecoEngineAdapter::AudioStreamIsEndCallback;
    m_audioStream.IsErrorCallback = CSpxUnidecRecoEngineAdapter::AudioStreamIsErrorCallback;

    *pAudioStream = &m_audioStream;

    *enableSegmentation = m_config->EnableSegmentation;
    SPX_DBG_TRACE_VERBOSE("Enable Segmentation=%d", *enableSegmentation);

    m_sentenceComplete = false;

    return true;
}

void CSpxUnidecRecoEngineAdapter::Intermediate(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult)
{
    UNUSED(sentenceIndex);
    UNUSED(wavId);

    auto wordCount = GetUnidecIntermediateResultWordCount(m_graphs.get(), intermediateResult);

    std::vector<const wchar_t*> words(wordCount);
    GetUnidecIntermediateResultWords(m_graphs.get(), intermediateResult, words.data(), wordCount);

    auto text = TrimWords(words, L"!sent_start", L"!sent_end");

    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto result = factory->CreateIntermediateResult(nullptr, text.c_str());

    auto offset = (uint32_t)framePos.SentenceStartIndex;
    GetSite()->IntermediateRecoResult(this, offset, result);
}

void CSpxUnidecRecoEngineAdapter::Sentence(const wchar_t* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest)
{
    UNUSED(sentenceIndex);
    UNUSED(wavId);

    std::shared_ptr<ISpxRecognitionResult> finalResult;

    m_sentenceComplete = true;
    auto offset = (uint32_t)framePos.SentenceStartIndex;
    
    auto nbestCount = GetUnidecNBestListSize(nbest);
    for (size_t index = 0; index < nbestCount; index++)
    {
        auto cost = GetUnidecNBestListCost(nbest, index);
        auto amCost = GetUnidecNBestListAMCost(nbest, index);
        auto lmCost = GetUnidecNBestListLMCost(nbest, index);
        auto prunedLMCost = GetUnidecNBestListPrunedLMCost(nbest, index);
        auto confidence = GetUnidecNBestListConfidence(m_graphs.get(), nbest, index);
        auto reserved = GetUnidecNBestListReserved(nbest, index);
        auto wordCount = GetUnidecNBestListWordCount(m_graphs.get(), nbest, index);

        UNUSED(amCost);
        UNUSED(lmCost);
        UNUSED(prunedLMCost);
        UNUSED(reserved);
        UNUSED(confidence);
        UNUSED(cost);

        std::vector<const wchar_t*> words(wordCount);
        GetUnidecNBestListWords(m_graphs.get(), nbest, index, words.data(), wordCount);

        auto text = TrimWords(words, L"!sent_start", L"!sent_end");
        if (index == 0 && text.length() > 0)
        {
            auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
            finalResult = factory->CreateFinalResult(nullptr, text.c_str());
        }
    }

    if (finalResult != nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        GetSite()->FinalRecoResult(this, offset, finalResult);
    }
}

void CSpxUnidecRecoEngineAdapter::End(const wchar_t* wavId, UnidecEndReason reason)
{
    UNUSED(wavId);
    UNUSED(reason);

    SPX_DBG_ASSERT(GetSite());
    GetSite()->DoneProcessingAudio(this);
}

size_t CSpxUnidecRecoEngineAdapter::AudioStreamRead(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize)
{
    UNUSED(pAudioStream);
    return AudioBufferRead(reinterpret_cast<uint8_t*>(pBuffer), maxSize);
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsEnd(AudioStreamDescriptor* pAudioStream)
{
    UNUSED(pAudioStream);
    return m_stopImmediately;
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsError(AudioStreamDescriptor* pAudioStream)
{
    UNUSED(pAudioStream);
    return false;
}

void CSpxUnidecRecoEngineAdapter::AudioBufferWrite(std::shared_ptr<uint8_t> buffer, size_t size)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "AudioBufferWrite size=%d, additionalBuffers.size()=%d", size, m_additionalBuffers.size());
    std::unique_lock<std::mutex> lock(m_mutex);

    // Only hold onto (maxHeldBuffers) buffers; if we are asked to hold more, block the call
    const size_t maxHeldBuffers = 100;
    while (m_additionalBuffers.size() >= maxHeldBuffers && !m_stopImmediately)
    {
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "AudioBufferWrite too many buffers... waiting...");
        m_cv.wait(lock, [&] { return m_additionalBuffers.size() < maxHeldBuffers || m_stopImmediately; });
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "AudioBufferWrite too many buffers... waiting... Done.");
    }

    // If we have a non-empty buffer to write, put it in our additional buffer list
    if (size > 0 && !m_stopImmediately)
    {
        auto pair = std::pair<std::shared_ptr<uint8_t>, size_t>(buffer, size);
        m_additionalBuffers.emplace(std::move(pair));
        m_cv.notify_all();
    }

    // (size == 0) indicates the end of audio; stay here in this clal to ensure all buffers are provided to the engine
    while (m_additionalBuffers.size() > 0 && size == 0 && !m_stopImmediately)
    {
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "%s(..., 0); Draining buffers (%d) ...", __FUNCTION__, m_additionalBuffers.size());
        m_cv.wait(lock, [&] { return m_additionalBuffers.size() == 0 || m_stopImmediately; });
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "%s(..., 0); Draining buffers (%d) ... Done.", __FUNCTION__, m_additionalBuffers.size());
    }
}

size_t CSpxUnidecRecoEngineAdapter::AudioBufferRead(uint8_t* buffer, size_t maxSize)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "AudioBufferRead size=%d, additionalBuffers.size()=%d", maxSize, m_additionalBuffers.size());
    size_t bytesLeftToRead = maxSize;

    while (bytesLeftToRead > 0 && !m_stopImmediately)
    {
        EnsureAudioBuffer();

        auto bytesToCopy = std::min(m_bytesLeftInBuffer, bytesLeftToRead);
        if (bytesToCopy > 0)
        {
            memcpy(buffer, m_ptrIntoBuffer, bytesToCopy);
            bytesLeftToRead -= bytesToCopy;

            m_ptrIntoBuffer += bytesToCopy;
            m_bytesLeftInBuffer -= bytesToCopy;

            if (m_bytesLeftInBuffer == 0)
            {
                m_buffer = nullptr;
                m_ptrIntoBuffer = nullptr;
            }
        }
    }

    return maxSize - bytesLeftToRead;
}

void CSpxUnidecRecoEngineAdapter::EnsureAudioBuffer()
{
    while (m_ptrIntoBuffer == nullptr && !m_stopImmediately)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_additionalBuffers.empty())
        {
            SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "EnsureAudioBuffer waiting...");
            m_cv.wait(lock, [&] { return !m_additionalBuffers.empty() || m_stopImmediately; });
            SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "EnsureAudioBuffer waiting... Done");
        }

        if (!m_additionalBuffers.empty())
        {
            auto sizeAndBuffer = m_additionalBuffers.front();
            m_additionalBuffers.pop();

            m_buffer = sizeAndBuffer.first;
            m_bytesLeftInBuffer = sizeAndBuffer.second;

            SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "EnsureAudioBuffer filling buffer; size=%d, additionalBuffers.size()=%d", m_bytesLeftInBuffer, m_additionalBuffers.size());

            m_ptrIntoBuffer = m_buffer.get();
            m_cv.notify_all();
        }
    }
}

std::wstring CSpxUnidecRecoEngineAdapter::TrimWords(std::vector<const wchar_t*>& words, const wchar_t* trim1, const wchar_t* trim2)
{
    std::wstring text;
    for (auto word : words)
    {
        if (_wcsicmp(word, trim1) != 0 && _wcsicmp(word, trim2) != 0)
        {
            if (text.length() > 0)
            {
                text += L" ";
            }
            text += word;
        }
    }

    return text;
}


} // CARBON_IMPL_NAMESPACE
