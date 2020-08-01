//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_client.cpp: Implementation definitions for RnntClient C++ class
//

#include <vector>
#include <queue>
#include <cstring>
#include <cstdint>

#include "stdafx.h"
#include "rnnt_client.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

RnntDecoderNBest::~RnntDecoderNBest()
{
    RnntDll::DeleteUnimicDecoderNBest(m_nbest);
}

size_t RnntDecoderNBest::GetCount() const
{
    return RnntDll::GetUnimicDecoderNBestCount(m_nbest);
}

RnntEntryPtr RnntDecoderNBest::GetEntry(size_t i) const
{
    size_t length = 0;
    size_t scoresCount = 3;
    std::vector<float> scores(scoresCount);
    auto tokens = RnntDll::GetUnimicDecoderNBestEntry(m_nbest, i, &length, scores.data(), scoresCount);
    return std::make_unique<RnntEntry>(length, tokens, std::move(scores));
}

RnntDecoderNBest::RnntDecoderNBest(IUnimicDecoderNBest* nbest) :
    m_nbest(nbest)
{}

/*static*/ std::unique_ptr<RnntDecoder> RnntDecoder::CreateDecoder(const std::wstring& spec)
{
    SPX_DBG_TRACE_VERBOSE("%s: decoder spec=\"%ls\"", __FUNCTION__, spec.c_str());
    return RnntDecoderPtr(new RnntDecoder(spec));
}

RnntDecoder::~RnntDecoder()
{
    RnntDll::DeleteUnimicDecoder(m_decoder);
}

size_t RnntDecoder::GetInputDim()
{
    return RnntDll::GetUnimicDecoderInputDim(m_decoder);
}

bool RnntDecoder::Run()
{
    return RnntDll::RunUnimicDecoder(m_decoder);
}

RnntDecoderNBestPtr RnntDecoder::GetNBest()
{
    return RnntDecoderNBestPtr(new RnntDecoderNBest(RnntDll::GetUnimicDecoderNBest(m_decoder)));
}

void RnntDecoder::Reset(IUnimicSource* source)
{
    RnntDll::ResetUnimicDecoder(m_decoder, source, nullptr);
}

RnntDecoder::RnntDecoder(const std::wstring& spec) :
    m_decoder(RnntDll::CreateUnimicDecoder(spec.c_str()))
{
    SPX_THROW_HR_IF(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE, m_decoder == nullptr);
}

RnntFeatureReader::RnntFeatureReader(IUnimicSource* source, size_t featDim, size_t stride, size_t maxEndPaddingDim) :
    m_source(source),
    m_channels(RnntDll::GetUnimicSourceChannelCount(m_source)),
    m_featDim(featDim),
    m_stride(stride),
    m_buf(std::make_unique<float[]>(m_featDim)),
    m_maxEndPaddingDim(maxEndPaddingDim)
{
    SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, m_channels == 0U || m_featDim % m_channels != 0U || m_stride % m_channels != 0U);
}

bool RnntFeatureReader::Forward()
{
    size_t offset;
    size_t cnt;

    if (m_start)
    {
        offset = 0;
        cnt = m_featDim;
        m_start = false;
    }
    else
    {
        memmove(&m_buf[0], &m_buf[m_stride], (m_featDim - m_stride) * sizeof(m_buf[0]));

        offset = m_featDim - m_stride;
        cnt = m_stride;
    }

    size_t read = 0U;
    if (!m_audioEnded)
    {
        read = ReadSource(m_source, &m_buf[offset], cnt, m_channels);
        if (read < cnt)
        {
            m_audioEnded = true;
        }
    }
    if (read != cnt)
    {
        m_endPadding += cnt - read;
    }

    return m_endPadding <= m_maxEndPaddingDim;
}

size_t RnntFeatureReader::GetChannelCount() const
{
    return m_featDim;
}

IUnimicSource* RnntFeatureReader::GetSource()
{
    if (m_output == nullptr)
    {
        m_output = SourcePtr(
            // 30ms per frame -> 33. However, this parameter has no real meaning for this stream.
            RnntDll::CreateCustomUnimicSource(m_featDim, 33, ReadCallback, this),
            [](IUnimicSource* source) { RnntDll::DeleteUnimicSource(source); });
    }
    return m_output.get();
}

/*static*/ bool RnntFeatureReader::ReadCallback(float* sample, void* context)
{
    return reinterpret_cast<RnntFeatureReader*>(context)->Read(sample);
}

size_t RnntFeatureReader::ReadSource(IUnimicSource* source, float* samples, size_t count, size_t channels)
{
    size_t read = 0U;
    for (size_t i = 0; i < count / channels; i++)
    {
        if (!RnntDll::ReadUnimicSource(source, samples))
        {
            break;
        }
        samples += channels;
        read += channels;
    }
    return read;
}

bool RnntFeatureReader::Read(float* sample)
{
    if (!Forward())
    {
        return false;
    }
    memcpy(sample, m_buf.get(), m_featDim * sizeof(sample[0]));
    return true;
}

IRnntClientPtr CreateRnntClient(
    CallbacksPtr callback,
    std::shared_ptr<Impl::ISpxThreadService> threadService,
    const std::wstring& modelFile,
    const std::wstring& tokenFile,
    bool displayText,
    size_t maxEndPaddingDim)
{
    SPX_DBG_TRACE_VERBOSE("%s: modelFile=\"%ls\", tokenFile=\"%ls\", %s, maxEndPaddingDim=%zu",
        __FUNCTION__,
        modelFile.c_str(),
        tokenFile.c_str(),
        displayText ? "DisplayText" : "LexicalText",
        maxEndPaddingDim);
    return IRnntClientPtr(new RnntClient(std::move(callback), std::move(threadService), modelFile, tokenFile, displayText, maxEndPaddingDim));
}

RnntClient::RnntClient(
    CallbacksPtr callback,
    std::shared_ptr<Impl::ISpxThreadService> threadService,
    const std::wstring& modelSpec,
    const std::wstring& tokenFile,
    bool displayText,
    size_t maxEndPaddingDim) :
    m_callback(std::move(callback)),
    m_threadService(std::move(threadService)),
    m_tokens(tokenFile),
    m_decoder(RnntDecoder::CreateDecoder(ComposeDecoderSpec(modelSpec, m_tokens.GetTokenDefs()))),
    m_displayText(displayText),
    m_maxEndPaddingDim(maxEndPaddingDim),
    m_source(RnntDll::CreateCustomUnimicSource(1U, 16000, UnimicSourceCallback, this), RnntDll::DeleteUnimicSource),
    m_featureExtractor(RnntDll::CreateUnimicFilter(L"fe(16kHzStaticStreamE14LFB80)"), RnntDll::DeleteUnimicFilter),
    m_features(RnntDll::GetUnimicFilterOutputPort(m_featureExtractor.get(), 0)),
    m_running(false),
    m_consumedSourceFrames(0),
    m_committedSourceFrames(0)
{
    RnntDll::SetUnimicFilterInputPort(m_featureExtractor.get(), 0, m_source.get());
    SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, RnntDll::GetUnimicFilterOutputPortCount(m_featureExtractor.get()) != 1U);
}

RnntClient::~RnntClient()
{
    if (Running())
    {
        Stop();
    }
}

/*static*/ std::wstring RnntClient::ComposeDecoderSpec(const std::wstring& modelSpec, const TokenDefs& tokenDefs)
{
    return std::wstring(L"rnnt(") + modelSpec + L"," +
        std::to_wstring(tokenDefs.m_blank) + L"," +
        std::to_wstring(tokenDefs.m_resetWithBlankToken) + L"," +
        std::to_wstring(tokenDefs.m_replaceWithBlankToken) + L"," +
        std::to_wstring(tokenDefs.m_noopToken) + L"," +
        std::to_wstring(tokenDefs.m_nodupToken) + L"," +
        std::to_wstring(tokenDefs.m_blockToken) + L"," +
        std::to_wstring(tokenDefs.m_lastToken) + L")";
}

void RnntClient::ProcessAudio(const Impl::DataChunkPtr& audioChunk)
{
    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, audioChunk->isWavHeader);
    SPX_TRACE_ERROR_IF(audioChunk->size % sizeof(int16_t) != 0, "Audio chunk size is wrong.");

    if (!m_turnStarted.test_and_set())
    {
        std::packaged_task<void()> task([this]()
        {
            TurnStartMsg msg("rnntdecoder");
            m_callback->OnTurnStart(msg);
        });
        m_threadService->ExecuteAsync(std::move(task), Impl::ISpxThreadService::Affinity::Background);
    }

    if (audioChunk->size >= sizeof(int16_t))
    {
        m_buffer.Write([&audioChunk](std::queue<float>& rawData)
            {
                int16_t ivalue;
                float fvalue;
                uint8_t* p = audioChunk->data.get();
                uint8_t* end = p + audioChunk->size;

                for (; p < end; p += sizeof(int16_t))
                {
                    memcpy(&ivalue, p, sizeof(int16_t));
                    fvalue = (float)ivalue / (1U << (sizeof(int16_t) * 8U - 1U));

                    rawData.push(fvalue);
                }
                return true;
            });
    }
    else
    {
        m_buffer.SetEndOfStream();
    }
}

void RnntClient::FlushAudio()
{
    m_buffer.SetEndOfStream();
}

void RnntClient::SetRecognitionMode(RNNT::RecognitionMode mode)
{
    m_recognitionMode = mode;
}

void RnntClient::Start()
{
    if (!m_running)
    {
        m_consumedSourceFrames = 0U;
        m_committedSourceFrames = 0U;
        m_processor = std::thread(&RnntClient::Decode, this);
        m_running = true;
    }
}

void RnntClient::Stop()
{
    m_buffer.SetEndOfStream();
    if (m_processor.joinable())
    {
        m_processor.join();
    }
    m_running = false;
}

bool RnntClient::Running()
{
    return m_running;
}

/*static*/ bool RnntClient::UnimicSourceCallback(float* sample, void* context)
{
    RnntClient* caller = (RnntClient*)context;
    return caller->UnimicSourceCallback(sample);
}

bool RnntClient::UnimicSourceCallback(float* sample)
{
    return m_buffer.Read([this, sample](std::queue<float>& data)
        {
            if (data.size() > 0)
            {
                *sample = data.front();
                data.pop();
                m_consumedSourceFrames++;
                return true;
            }
            return false;
            // Condition: A single frame should be filled up, otherwise CV should wait.
        });
}

void RnntClient::Decode()
{
    try
    {
        DecodeInternal();
    }
    catch (...)
    {
        SPX_DBG_TRACE_VERBOSE("%s: Decode threw an exception.", __FUNCTION__);
        m_buffer.SetEndOfStream();
        std::packaged_task<void()> task([this]()
        {
            m_callback->OnError("Failed to decode speech");
        });
        m_threadService->ExecuteAsync(std::move(task), Impl::ISpxThreadService::Affinity::Background);
    }
}

void RnntClient::DecodeInternal()
{
    constexpr size_t BaseFeatDim = 640U;
    constexpr size_t BaseFeatStride = 240U;

    RnntFeatureReader reader(m_features, BaseFeatDim, BaseFeatStride, m_maxEndPaddingDim);
    m_decoder->Reset(reader.GetSource());

    RnntEntryPtr lastEntry;

    while (m_decoder->Run())
    {
        auto nbest = m_decoder->GetNBest();
        if (nbest->GetCount() > 0)
        {
            if (!m_speechStarted.load())
            {
                FireSpeechStartEnd(true);
            }
            auto entry = nbest->GetEntry(0);
            if (!lastEntry || *entry != *lastEntry)
            {
                FireSpeechEvent(entry, true, false);
                lastEntry = std::move(entry);
            }
        }
    }
    FireSpeechEvent(lastEntry, false, true);
    // TODO: Remove this hack after speech end detection is supported by RNN-T engine.
    if (m_speechStarted.load())
    {
        FireSpeechStartEnd(false);
    }
    FireDecodeDone();
}

void RnntClient::FireSpeechStartEnd(bool speechStarted)
{
    auto offsetFrames = m_committedSourceFrames;
    std::packaged_task<void()> task([this, offsetFrames, speechStarted]()
    {
        constexpr DurationType TicksPerFrame = 10000000 / Impl::SAMPLES_PER_SECOND;
        if (speechStarted)
        {
            m_speechStarted = true;
            SpeechStartDetectedMsg msg(offsetFrames * TicksPerFrame);
            m_callback->OnSpeechStartDetected(msg);
        }
        else
        {
            m_speechStarted = false;
            SpeechEndDetectedMsg msg(offsetFrames * TicksPerFrame);
            m_callback->OnSpeechEndDetected(msg);
        }
    });
    m_threadService->ExecuteAsync(std::move(task), Impl::ISpxThreadService::Affinity::Background);
}

bool RnntClient::FireSpeechEvent(const RnntEntryPtr& entry, bool intermediate, bool force)
{
    auto lexicalText = entry != nullptr ? m_tokens.TokenToString(entry, m_displayText) : std::wstring(L"");
    if (lexicalText.empty() && !force)
    {
        return false;
    }
    auto durationFrames = m_consumedSourceFrames - m_committedSourceFrames;
    auto offsetFrames = m_committedSourceFrames;
    if (!intermediate)
    {
        m_committedSourceFrames = m_consumedSourceFrames;
    }

    std::packaged_task<void()> task([this, offsetFrames, durationFrames, text(std::move(lexicalText)), intermediate]() mutable
    {
        constexpr DurationType TicksPerFrame = 10000000 / Impl::SAMPLES_PER_SECOND;
        if (intermediate)
        {
            SpeechHypothesisMsg msg(offsetFrames * TicksPerFrame, durationFrames * TicksPerFrame, std::move(text));
            m_callback->OnSpeechHypothesis(msg);
        }
        else
        {
            SpeechPhraseMsg msg(offsetFrames * TicksPerFrame, durationFrames * TicksPerFrame, RecognitionStatus::Success, std::move(text));
            m_callback->OnSpeechPhrase(msg);
        }
    });
    m_threadService->ExecuteAsync(std::move(task), Impl::ISpxThreadService::Affinity::Background);
    return true;
}

void RnntClient::FireDecodeDone()
{
    std::packaged_task<void()> task([this]()
    {
        m_callback->OnTurnEnd();
        m_turnStarted.clear();
    });
    m_threadService->ExecuteAsync(std::move(task), Impl::ISpxThreadService::Affinity::Background);
}

}}}}
