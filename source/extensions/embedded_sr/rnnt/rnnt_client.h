//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_client.h: Implementation declarations for CRnntClient C++ class
//

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>

#include "ispxinterfaces.h"
#include "spxdebug.h"
#include "rnnt.h"
#include "rnnt_tokens.h"
#include "conditional_variable_buffer.h"
#include "rnnt_dll.h"

class IUnimicDecoderNBest;
class IUnimicDecoder;
class IUnimicSource;
class IUnimicSource;
class IUnimicFilter;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

class RnntDecoderNBest
{
public:

    ~RnntDecoderNBest();

    size_t GetCount() const;
    RnntEntryPtr GetEntry(size_t i) const;

private:

    friend class RnntDecoder;

    RnntDecoderNBest(IUnimicDecoderNBest* nbest);

    DISABLE_COPY_AND_MOVE(RnntDecoderNBest);

private:

    IUnimicDecoderNBest* m_nbest;
};

using RnntDecoderNBestPtr = std::unique_ptr<RnntDecoderNBest>;

class RnntDecoder
{
public:

    static std::unique_ptr<RnntDecoder> CreateDecoder(const std::wstring& spec);

    ~RnntDecoder();

    size_t GetInputDim();
    bool Run(bool continuousReco);
    RnntDecoderNBestPtr GetNBest();
    void Reset(IUnimicSource* source);

private:

    RnntDecoder(const std::wstring& spec);

    DISABLE_COPY_AND_MOVE(RnntDecoder);

private:

    IUnimicDecoder* m_decoder;
};

using RnntDecoderPtr = std::unique_ptr<RnntDecoder>;

class RnntFeatureReader
{
public:

    RnntFeatureReader(IUnimicSource* source, size_t featDim, size_t stride, size_t maxEndPaddingDim);

    bool Forward();
    size_t GetChannelCount() const;
    IUnimicSource* GetSource();

private:

    static bool ReadCallback(float* sample, void* context);

    size_t ReadSource(IUnimicSource* source, float* samples, size_t count, size_t channels);
    bool Read(float* sample);

private:

    using SourcePtr = std::unique_ptr<IUnimicSource, std::function<void(IUnimicSource*)>>;

    IUnimicSource* m_source;
    const size_t m_channels;
    const size_t m_featDim;
    const size_t m_stride;
    std::unique_ptr<float[]> m_buf;
    const size_t m_maxEndPaddingDim;
    bool m_start = true;
    bool m_audioEnded = false;
    size_t m_endPadding = 0U;
    SourcePtr m_output;
};

class RnntClient : public IRnntClient
{
public:

    RnntClient(
        CallbacksPtr callback,
        std::shared_ptr<Impl::ISpxThreadService> threadService,
        const std::wstring& modelSpec,
        const std::wstring& tokenFile,
        bool displayText,
        size_t maxEndPaddingDim);
    ~RnntClient();

    // --- IRnntClient
    void ProcessAudio(const Impl::DataChunkPtr& audioChunk) override;
    void FlushAudio() override;
    void SetRecognitionMode(RNNT::RecognitionMode mode) override;
    void Start() override;
    void Stop() override;
    bool Running() override;

private:

    static std::wstring ComposeDecoderSpec(const std::wstring& modelSpec, const TokenDefs& tokenDefs);
    static bool UnimicSourceCallback(float* sample, void* context);

    bool UnimicSourceCallback(float* sample);
    void Decode();
    void DecodeInternal();

    void FireSpeechStartEnd(bool speechStarted);
    bool FireSpeechEvent(const RnntEntryPtr& lastEntry, bool intermediate, bool force);
    void FireDecodeDone();

private:

    CallbacksPtr m_callback;
    std::shared_ptr<Impl::ISpxThreadService> m_threadService;
    RnntTokens m_tokens;
    RnntDecoderPtr m_decoder;
    bool m_displayText;
    size_t m_maxEndPaddingDim;
    std::unique_ptr<IUnimicSource, std::function<void(IUnimicSource*)>> m_source;
    std::unique_ptr<IUnimicFilter, std::function<void(IUnimicFilter*)>> m_featureExtractor;
    IUnimicSource* m_features;
    std::atomic_flag m_turnStarted = ATOMIC_FLAG_INIT;
    std::atomic_bool m_speechStarted{ 0 };
    bool m_running;
    size_t m_consumedSourceFrames;
    size_t m_committedSourceFrames;
    RNNT::RecognitionMode m_recognitionMode;
    std::thread m_processor;
    ConditionalVariableBuffer<std::queue<float>> m_buffer;
};

}}}}
