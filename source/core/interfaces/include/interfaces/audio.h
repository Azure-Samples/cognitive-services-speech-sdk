//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <interfaces/base.h>
#include <interfaces/aggregates.h>
#include <interfaces/containers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/* TODO: Streams probably belong in a streams.h */
class ISpxAudioStream : public ISpxInterfaceBaseFor<ISpxAudioStream>
{
public:
    virtual uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) = 0;
};

class ISpxAudioStreamInitFormat : public ISpxInterfaceBaseFor<ISpxAudioStreamInitFormat>
{
public:
    virtual void SetFormat(SPXWAVEFORMATEX* format) = 0;
};

class ISpxAudioProcessor : public ISpxInterfaceBaseFor<ISpxAudioProcessor>
{
public:

    virtual void SetFormat(const SPXWAVEFORMATEX* pformat) = 0;
    virtual void ProcessAudio(const DataChunkPtr& audioChunk) = 0;
};

class ISpxAudioProcessorMinInput :
    public ISpxInterfaceBaseFor<ISpxAudioProcessorMinInput>
{
public:
    virtual void SetMinInputSize(const uint64_t sizeInTicks) = 0;
};

class ISpxAudioSource : public ISpxInterfaceBaseFor<ISpxAudioSource>
{
public:

    enum class State { Idle = 0, Started = 1, DataAvailable = 2, EndOfStream = 3 };
    virtual State GetState() const = 0;
    virtual SpxWaveFormatEx GetFormat() const = 0;
};

class ISpxAudioSourceInit : public ISpxInterfaceBaseFor<ISpxAudioSourceInit>
{
public:

    virtual void InitFromMicrophone() = 0;
    virtual void InitFromFile(const wchar_t* pszFileName) = 0;
    virtual void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) = 0;
};

class ISpxAudioSessionShim: public ISpxInterfaceBaseFor<ISpxAudioSessionShim>
{
public:
    virtual void StartAudio() = 0;
    virtual void StopAudio() = 0;
    virtual SpxWaveFormatEx GetFormat() = 0;
};

class ISpxSynthesisResult;

class ISpxAudioDataStream : public ISpxInterfaceBaseFor<ISpxAudioDataStream>
{
public:
    virtual void InitFromFile(const char* fileName) = 0;
    virtual void InitFromSynthesisResult(std::shared_ptr<ISpxSynthesisResult> result) = 0;
    virtual void InitFromFormat(const SPXWAVEFORMATEX& format, bool hasHeader) = 0;
    virtual StreamStatus GetStatus() noexcept = 0;
    virtual CancellationReason GetCancellationReason() = 0;
    virtual CancellationErrorCode GetCancellationErrorCode() = 0;
    virtual bool CanReadData(uint32_t requestedSize) = 0;
    virtual bool CanReadData(uint32_t requestedSize, uint32_t pos) = 0;
    virtual uint32_t Read(uint8_t* buffer, uint32_t bufferSize) = 0;
    virtual uint32_t Read(uint8_t* buffer, uint32_t bufferSize, uint32_t pos) = 0;
    virtual void SaveToWaveFile(const wchar_t* fileName) = 0;
    virtual uint32_t GetPosition() = 0;
    virtual void SetPosition(uint32_t pos) = 0;
    virtual void DetachInput() = 0;
};

} } } }