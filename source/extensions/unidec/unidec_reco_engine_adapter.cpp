//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// unidec_reco_engine_adapter.cpp: Implementation definitions for CSpxUnidecRecoEngineAdapter C++ class
//

#include "stdafx.h"
#include <chrono>
#include <thread>
#include <codecvt>
#include <fstream>
#include "unidec_reco_engine_adapter.h"
#include "guid_utils.h"
#include "handle_table.h"
#include "service_helpers.h"
#include "ring_buffer.h"
#include "property_id_2_name_map.h"
#include "file_utils.h"

#include "json.h"

#define SPX_DBG_TRACE_UNIDEC_AUDIO 0


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


const unsigned long c_ringBufferSize = (16000 * 16 * 1 * 1) / 8; // 1sec of 16bit 16kHz data
const std::wstring c_modelsPath = L"%MODELSPATH%";

CSpxUnidecRecoEngineAdapter::CSpxUnidecRecoEngineAdapter() :
    m_ringBuffer(nullptr),
    m_stopImmediately(false),
    m_sentenceEndDetected(false),
    m_singleShot(false)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);
    m_ringBuffer = std::make_shared<RingBuffer>();
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
    InitUnidecConfig();
}

void CSpxUnidecRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, __FUNCTION__);
    StopEngine();
}

void CSpxUnidecRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::SetAdapterMode, singleShot: %d", singleShot);
    m_singleShot = singleShot;
}

void CSpxUnidecRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(pformat->wFormatTag).c_str(),
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);
    SPX_IFTRUE_THROW_HR(!IsInit(), SPXERR_UNINITIALIZED);

    if (pformat != nullptr)
    {
        InitFormat(pformat);
        InitEngine();
        StartEngine();
    }
    else
    {
        TermFormat();
        GetSite()->AdapterCompletedSetFormatStop(this);
    }
}

void CSpxUnidecRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_VERBOSE_IF(0, "%s(..., size=%d)", __FUNCTION__, audioChunk->size);
    SPX_IFTRUE_THROW_HR(!HasFormat(), SPXERR_UNINITIALIZED);

    if (!m_turnStarted)
    {
        GetSite()->AdapterStartingTurn(this);
        GetSite()->AdapterStartedTurn(this, "fred-guid");
        m_turnStarted = true;
    }

    AudioBufferWrite(audioChunk->data, audioChunk->size);

    if (audioChunk->size == 0)
    {
        m_endOfStream = true;
    }
}

void ReadJsonProperty(const nlohmann::json& properties, std::map<std::string, std::wstring>& config, std::string propertyName)
{
    if (properties[propertyName].is_null())
    {
        config[propertyName] = L"";
    }
    else if (properties[propertyName].is_number())
    {
        config[propertyName] = std::to_wstring(properties[propertyName].get<int>());
    }
    else if (properties[propertyName].is_string())
    {
        std::string propertyVal = properties[propertyName];
        config[propertyName].assign(propertyVal.begin(), propertyVal.end());
    }
}

void ReplaceString(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    if (from.empty())
    {
        return;
    }
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void CSpxUnidecRecoEngineAdapter::InitUnidecConfig()
{
    std::map<std::string, std::wstring> config;
    std::string modelPath = GetBaseModelPath();
    std::string propertiesFile = PAL::AppendPath(modelPath, "properties.json");
    std::ifstream ifs;
    ifs.open(propertiesFile, std::ifstream::in);
    SPX_IFTRUE_THROW_HR(ifs.fail(), SPXERR_FILE_OPEN_FAILED);

    try
    {
        nlohmann::json properties;
        ifs >> properties;
        config[g_unidecEnableSegmentation] = L"false";
        config[g_unidecBaseModelPath] = PAL::ToWString(PAL::AppendPath(modelPath, ""));
        ReadJsonProperty(properties, config, g_unidecHCLGSpec);
        ReadJsonProperty(properties, config, g_unidecHCLGSpecBase);
        ReplaceString(config[g_unidecHCLGSpec], c_modelsPath, config[g_unidecBaseModelPath]);
        m_config = std::make_unique<CSpxUnidecConfig>(config);
    }
    catch (...)
    {
        SPX_IFTRUE_THROW_HR(!m_config.get(), SPXERR_UNEXPECTED_UNIDEC_SITE_FAILURE);
    }
}

std::string CSpxUnidecRecoEngineAdapter::GetBaseModelPath()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_UNIDEC_SITE_FAILURE);
    std::string modelPathRoot = properties->GetStringValue("CARBON-INTERNAL-SPEECH-RecoLocalModelPathRoot");
    std::string modelPathLanguage = properties->GetStringValue("CARBON-INTERNAL-SPEECH-RecoLocalModelLanguage");
    std::string modelPath = PAL::AppendPath(modelPathRoot, modelPathLanguage);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::GetBaseModelPath: %s", modelPath.c_str());
    return modelPath;
}

void CSpxUnidecRecoEngineAdapter::InitFormat(const SPXWAVEFORMATEX* pformat)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(HasFormat(), SPXERR_ALREADY_INITIALIZED);

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), pformat, sizeOfFormat);
}

void CSpxUnidecRecoEngineAdapter::TermFormat()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_format = nullptr;
}

void CSpxUnidecRecoEngineAdapter::InitEngine()
{
    if (m_unidecEngine.get() == nullptr)
    {
        InitSearchGraphs();
        std::string modelPath = GetBaseModelPath();
        std::wstring propertiesFile = PAL::ToWString(PAL::AppendPath(modelPath, "properties.ini"));
        IConfig* config = CreateConfig(PAL::ToWCHARString(propertiesFile).c_str());

        m_unidecEngine = std::unique_ptr<IUnidecEngine, std::function<void(IUnidecEngine*)>>(CreateUnidecStreamEngine(
            config,
            CSpxUnidecRecoEngineAdapter::NextStreamCallback,
            CSpxUnidecRecoEngineAdapter::IntermediateCallback,
            CSpxUnidecRecoEngineAdapter::SentenceCallback,
            CSpxUnidecRecoEngineAdapter::EndCallback,
            this),
            DeleteUnidecEngine);
    }
}

void CSpxUnidecRecoEngineAdapter::InitSearchGraphs()
{
    std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>> base_graphs;
    if (m_config->HCLGSpecBase.length() > 0)
    {
        wchar_string HCLGSpecBase = PAL::ToWCHARString(m_config->HCLGSpecBase);
        base_graphs =
            std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>>(
                CreateUnidecSearchGraphCombo(HCLGSpecBase.c_str()),
                DeleteUnidecSearchGraphCombo);
    }

    wchar_string HCLGSpecString = PAL::ToWCHARString(m_config->GetHCLGSpecString());
    m_graphs = std::unique_ptr<IUnidecSearchGraphCombo, std::function<void(IUnidecSearchGraphCombo*)>>(
        CreateUnidecSearchGraphComboEx(HCLGSpecString.c_str(), base_graphs.get()),
        DeleteUnidecSearchGraphCombo);
}

void CSpxUnidecRecoEngineAdapter::StartEngine()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::StartEngine enter");
    SPX_IFTRUE_THROW_HR(GetUnidecEngineStarted(m_unidecEngine.get()), SPXERR_SETFORMAT_UNEXPECTED_STATE_TRANSITION);

    m_stopImmediately = false;
    m_sentenceEndDetected = false;
    StartUnidecEngine(m_unidecEngine.get());
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::StartEngine exit");
}

void CSpxUnidecRecoEngineAdapter::StopEngine()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::StopEngine enter");

    if (GetUnidecEngineStarted(m_unidecEngine.get()))
    {
        m_mutex.lock();
        m_stopImmediately = true;
        m_mutex.unlock();
        SPX_DBG_TRACE_VERBOSE("CSpxUnidecRecoEngineAdapter::StopEngine call JoinUnidecEngine");
        JoinUnidecEngine(m_unidecEngine.get());
        FlushBuffers();
    }
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::StopEngine exit");
}

void CSpxUnidecRecoEngineAdapter::FlushBuffers()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::FlushBuffers enter");
    m_audioBuffer.clear();
    m_ringBuffer->ReleaseBuffers();
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::FlushBuffers exit");
}

bool CSpxUnidecRecoEngineAdapter::NextStreamCallback(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo, bool* continuousReco, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    return unidecRecoEngineAdapter->NextStream(pAudioStream, pCombo, continuousReco);
}

void CSpxUnidecRecoEngineAdapter::IntermediateCallback(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->Intermediate(wavId, sentenceIndex, framePos, intermediateResult);
}

void CSpxUnidecRecoEngineAdapter::SentenceCallback(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->Sentence(wavId, sentenceIndex, framePos, nbest);
}

void CSpxUnidecRecoEngineAdapter::EndCallback(const WCHAR* wavId, const AudioEndMetadata& metadata, void* callbackContext)
{
    auto unidecRecoEngineAdapter = (CSpxUnidecRecoEngineAdapter*)callbackContext;
    unidecRecoEngineAdapter->End(wavId, metadata);
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

bool CSpxUnidecRecoEngineAdapter::NextStream(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo, bool* continuousReco)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::NextStream, HasFormat() =%d, m_stopImmediately =%d, m_sentenceEndDetected: %d", HasFormat(), m_stopImmediately.load(), m_sentenceEndDetected);
    SPX_DBG_ASSERT(continuousReco);

    *continuousReco = !m_singleShot;

    if (!HasFormat() || m_stopImmediately || (m_sentenceEndDetected && m_singleShot))
    {
        return false;
    }

    return InitStream(pAudioStream, pCombo);
}

bool CSpxUnidecRecoEngineAdapter::InitStream(AudioStreamDescriptor** pAudioStream, IUnidecSearchGraphCombo*& pCombo)
{
    m_streamId = PAL::ToWCHARString(PAL::CreateGuidWithoutDashes());
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
    m_audioStream.PrefixKeywords = nullptr;
    m_audioStream.PrefixKeywordsCount = 0;
    m_audioStream.PrefixKeywordSilenceThreshold = 0;

    m_audioStream.CallbackContext = static_cast<void*>(this);
    m_audioStream.ReadCallback = CSpxUnidecRecoEngineAdapter::AudioStreamReadCallback;
    m_audioStream.IsEndCallback = CSpxUnidecRecoEngineAdapter::AudioStreamIsEndCallback;
    m_audioStream.IsErrorCallback = CSpxUnidecRecoEngineAdapter::AudioStreamIsErrorCallback;

    *pAudioStream = &m_audioStream;

    pCombo = m_graphs.get();
    m_sentenceEndDetected = false;

    return true;
}

void CSpxUnidecRecoEngineAdapter::Intermediate(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecIntermediateResult* intermediateResult)
{
    UNUSED(sentenceIndex);
    UNUSED(wavId);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::Intermediate enter");

    m_sentenceEndDetected = false;

    auto wordCount = GetUnidecIntermediateResultWordCount(m_graphs.get(), intermediateResult, false);

    std::vector<const WCHAR*> words(wordCount);
    GetUnidecIntermediateResultWords(m_graphs.get(), intermediateResult, false, words.data(), wordCount);

    auto text = TrimWords(words, _W("!sent_start"), _W("!sent_end"));

    auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
    auto offset = (uint64_t)(framePos.SentenceStartMilliseconds * 10000);
    auto duration = (uint64_t)(framePos.SpeechFramesLengthInMilliseconds * 10000);

    auto result = factory->CreateIntermediateResult(nullptr, text.c_str(), offset, duration);
    SetServiceJsonResultProperties(result, offset, duration, PAL::ToString(text));

    GetSite()->FireAdapterResult_Intermediate(this, offset, result);

    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::Intermediate exit");
}

void CSpxUnidecRecoEngineAdapter::Sentence(const WCHAR* wavId, size_t sentenceIndex, const UnidecFramePosition& framePos, const IUnidecNBestList* nbest)
{
    UNUSED(sentenceIndex);
    UNUSED(wavId);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::Sentence enter");

    std::shared_ptr<ISpxRecognitionResult> finalResult;

    auto offset = (uint64_t)(framePos.SentenceStartMilliseconds * 10000);
    auto duration = (uint64_t)(framePos.SpeechFramesLengthInMilliseconds * 10000);

    auto nbestCount = GetUnidecNBestListSize(nbest);
    for (size_t index = 0; index < nbestCount; index++)
    {
        auto cost = GetUnidecNBestListCost(nbest, index);
        auto amCost = GetUnidecNBestListAMCost(nbest, index);
        auto lmCost = GetUnidecNBestListLMCost(nbest, index);
        auto prunedLMCost = GetUnidecNBestListPrunedLMCost(nbest, index);
        auto confidence = GetUnidecNBestListConfidence(m_graphs.get(), nbest, index);
        auto reserved = GetUnidecNBestListReserved(nbest, index);
        auto wordCount = GetUnidecNBestListWordCount(m_graphs.get(), nbest, index, false);

        UNUSED(amCost);
        UNUSED(lmCost);
        UNUSED(prunedLMCost);
        UNUSED(reserved);
        UNUSED(confidence);
        UNUSED(cost);

        std::vector<const WCHAR*> words(wordCount);
        GetUnidecNBestListWords(m_graphs.get(), nbest, index, false, words.data(), wordCount);

        auto text = TrimWords(words, _W("!sent_start"), _W("!sent_end"));

        // unidec orders result so that first item has best result
        // It is not necessary to go through all items (unless want to see those in the traces
        if (index == 0 && !text.empty())
        {
            m_sentenceEndDetected = true;

            auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());
            finalResult = factory->CreateFinalResult(nullptr, ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, CancellationErrorCode::NoError, text.c_str(), offset, duration);
            SetServiceJsonResultProperties(finalResult, offset, duration, PAL::ToString(text));
        }
    }

    if (finalResult != nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        GetSite()->FireAdapterResult_FinalResult(this, offset, finalResult);
    }

    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::Sentence exit");
}

void CSpxUnidecRecoEngineAdapter::End(const WCHAR* wavId, const AudioEndMetadata& metadata)
{
    UNUSED(wavId);
    UNUSED(metadata);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::End enter");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_turnStarted = false;

    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdapterStoppedTurn(this);

    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::End exit");
}

size_t CSpxUnidecRecoEngineAdapter::AudioStreamRead(AudioStreamDescriptor* pAudioStream, void *pBuffer, size_t maxSize)
{
    UNUSED(pAudioStream);
    return AudioBufferRead(reinterpret_cast<uint8_t*>(pBuffer), maxSize);
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsEnd(AudioStreamDescriptor* pAudioStream)
{
    UNUSED(pAudioStream);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioStreamIsEnd, m_stopImmediately:%d, m_sentenceEndDetected:%d", m_stopImmediately.load(), m_sentenceEndDetected);
    return (m_stopImmediately || (m_sentenceEndDetected && m_singleShot));
}

bool CSpxUnidecRecoEngineAdapter::AudioStreamIsError(AudioStreamDescriptor* pAudioStream)
{
    UNUSED(pAudioStream);
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioStreamIsError");
    return false;
}

void CSpxUnidecRecoEngineAdapter::AudioBufferWrite(std::shared_ptr<uint8_t> buffer, size_t size)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferWrite enter, size=%d", size);
    std::unique_lock<std::mutex> lock(m_mutex);

    SpxSharedAudioBuffer_Type sharedmem = SpxAllocSharedAudioBuffer(size);
    memcpy(sharedmem.get(), buffer.get(), size);
    m_ringBuffer->AddBuffer(std::make_shared<DataChunk>(sharedmem, (uint32_t)size));

    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferWrite exit");
}

size_t CSpxUnidecRecoEngineAdapter::AudioBufferRead(uint8_t* buffer, size_t requestedSize)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferRead enter, requestedSize=%d", requestedSize);
    size_t copied = 0;
    m_mutex.lock();

    while (m_audioBuffer.size() < requestedSize)
    {
        size_t readBufferSize = requestedSize - m_audioBuffer.size();
        SpxSharedAudioBuffer_Type sharedmem = SpxAllocSharedAudioBuffer(readBufferSize);
        uint32_t bytesReturned = m_ringBuffer->GetData(sharedmem.get(), (uint32_t)readBufferSize);
        if (bytesReturned > 0)
        {
            m_audioBuffer.insert(m_audioBuffer.end(), &sharedmem.get()[0], &sharedmem.get()[bytesReturned]);
        }
        else
        {
            // If Unidec is stopped and read callback is being served and ringbuffer gets empty, fill the buffer with zeros for the requested size
            if (m_stopImmediately || m_endOfStream)
            {
                m_stopImmediately = true;
                size_t sizeRemaining = requestedSize - m_audioBuffer.size();
                SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferRead , sizeRemaining=%d", sizeRemaining);
                if (sizeRemaining > 0)
                {
                    std::unique_ptr<unsigned char> remaining = std::unique_ptr<unsigned char>(new unsigned char[sizeRemaining]);
                    memset(remaining.get(), 0, sizeRemaining);
                    m_audioBuffer.insert(m_audioBuffer.end(), &remaining.get()[0], &remaining.get()[sizeRemaining]);
                }
                break;
            }
            m_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            m_mutex.lock();
        }

    }

    size_t buffered = m_audioBuffer.size();

    if (buffered > 0 && requestedSize > 0)
    {
        auto bufferSize = buffered > requestedSize ? requestedSize : buffered;
        SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferRead , bufferSize=%d", bufferSize);
        if (buffer)
        {
            memcpy(buffer, m_audioBuffer.data(), bufferSize);

            copied = bufferSize;
            if (bufferSize < buffered)
            {
                std::vector<unsigned char> remaining;
                std::copy(m_audioBuffer.begin() + bufferSize, m_audioBuffer.end(), std::back_inserter(remaining));
                m_audioBuffer.swap(remaining);
}
            else
            {
                m_audioBuffer.clear();
            }
        }
    }

    m_mutex.unlock();
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_UNIDEC_AUDIO, "CSpxUnidecRecoEngineAdapter::AudioBufferRead exit, copied: %d", copied);
    return copied;
}

std::wstring CSpxUnidecRecoEngineAdapter::TrimWords(std::vector<const WCHAR*>& words, const WCHAR* trim1, const WCHAR* trim2)
{
    SPX_DBG_TRACE_VERBOSE_IF(1, "CSpxUnidecRecoEngineAdapter::TrimWords starting...");
    std::wstring text;
    for (const WCHAR* word : words)
    {
        SPX_DBG_TRACE_VERBOSE_IF(1, "CSpxUnidecRecoEngineAdapter::TrimWords: word='%ls'", PAL::WCHARToWString(word).c_str());
        if ((PAL::xcsicmp(word, trim1) != 0 && PAL::xcsicmp(word, trim2) != 0))
        {
            if (text.length() > 0)
            {
                text += L" ";
            }
            text += PAL::WCHARToWString(word);
        }
    }

    SPX_DBG_TRACE_VERBOSE_IF(1, "CSpxUnidecRecoEngineAdapter::TrimWords: text='%ls'", text.c_str());
    return text;
}

void CSpxUnidecRecoEngineAdapter::SetServiceJsonResultProperties(const std::shared_ptr<ISpxRecognitionResult>& result, uint64_t offset, uint64_t duration, const std::string& text)
{
    auto json = std::string(R"({)") +
        R"("Duration":)" + std::to_string(duration) + R"(,)" +
        R"("NBest":[{)" +
                R"("Display":)"    + R"(")" + text + R"(",)" +
                R"("ITN":)"        + R"(")" + text + R"(",)" +
                R"("Lexical":)"    + R"(")" + text + R"(",)" +
                R"("MaskedITN":)"   + R"(")" + text + R"(")" +
                R"(}],)" +
        R"("Offset":)" + std::to_string(offset) + "," +
        R"("RecognitionStatus": "Success")" +
        R"(})";

    auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);
    namedProperties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult), json.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
