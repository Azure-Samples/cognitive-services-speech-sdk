//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_buffer.h: Audio buffer for preserving a stream of sent audio for replay in case of connection errors.
//

#pragma once

#include <memory>
#include <list>
#include <deque>
#include "spxcore_common.h"
#include "audio_chunk.h"
#include <mutex>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    // ProcessedAudioTimestamp presents the received time of the last audio that contributes a hypothesis or phrase result.
    // The chunkReceivedTime is the time when the last audio chunk is received, and the remainingAudioInTicks is the length
    // (in ticks) of audio in the last chunk that is not a part of hypothesis or phrase result.
    struct ProcessedAudioTimestamp
    {
        ProcessedAudioTimestamp(std::chrono::system_clock::time_point chunkTimeStamp, uint64_t remainingInTicks)
            : chunkReceivedTime{ chunkTimeStamp }, remainingAudioInTicks{ remainingInTicks }
        { }

        std::chrono::system_clock::time_point chunkReceivedTime;
        uint64_t remainingAudioInTicks;
    };

    using ProcessedAudioTimestampPtr = std::shared_ptr<ProcessedAudioTimestamp>;

    class AudioBuffer;
    using AudioBufferPtr = std::shared_ptr<AudioBuffer>;

    // Audio buffer interface.
    class AudioBuffer
    {
    public:
        // Adds a new data chunk.
        virtual void Add(const DataChunkPtr& audioChunk) = 0;

        // Gets next not-yet confirmed chunk from the buffer.
        // Returns nullptr if no more data available.
        virtual DataChunkPtr GetNext() = 0;

        // Moves internal cursor to the first unconfirmed chunk,
        // Used to resend chunks that were not confirmed after a break of connection/TurnStart/End messages.
        virtual void NewTurn() = 0;

        // Discards all chunks till the specified offset in ticks.
        // Offset is relative to the current turn. Should be called on successful final result.
        // It returns the timestamp of the last discarded audio.
        virtual ProcessedAudioTimestampPtr DiscardTill(uint64_t offsetInTicksTurnRelative /*Tick = HNS*/) = 0;

        // Discards "size" bytes from the beginning of unconfirmed chunks.
        // Currently used only for KWS, (in the future DiscardTill should be used instead).
        virtual void DiscardBytes(uint64_t size) = 0;

        // Converts a turn relative offset to a global offset (offset in HNS)
        virtual uint64_t ToAbsolute(uint64_t offsetInTicksTurnRelative /*Tick = HNS*/) const = 0;

        // Number of bytes that were stashed ( = difference in bytes between Add/Add/.../Add and Get/Get/.../Get)
        // and not yet sent to audio processor (KWS or service).
        // Note, this is different from the non acknowledged bytes.
        virtual uint64_t StashedSizeInBytes() const = 0;

        // Number of bytes that were sent to service from the start of the turn,
        // but have not yet been confirmed.
        virtual uint64_t NonAcknowledgedSizeInBytes() const = 0;

        // Drops current chunks from the buffer.
        virtual void Drop() = 0;

        // Gets current absolute offset in HNS.
        virtual uint64_t GetAbsoluteOffset() const = 0;

        // Copies unconfirmed chunks to the target buffer.
        virtual void CopyNonAcknowledgedDataTo(AudioBufferPtr buffer) const = 0;

        // Gets timestamp of the last audio that matches the specified offset in ticks.
        // Offset is relative to the current turn.
        virtual ProcessedAudioTimestampPtr GetTimestamp(uint64_t offsetInTicksTurnRelative /*Tick = HNS*/) const = 0;

        virtual ~AudioBuffer() {};
    };

    // Audio buffer based on pcm format.
    class PcmAudioBuffer : public AudioBuffer
    {
    public:
        explicit PcmAudioBuffer(const SPXWAVEFORMATEX& header);

        void Add(const DataChunkPtr& audioChunk) override;
        DataChunkPtr GetNext() override;
        void NewTurn() override;
        ProcessedAudioTimestampPtr DiscardTill(uint64_t offset) override;
        void DiscardBytes(uint64_t bytes) override;
        uint64_t ToAbsolute(uint64_t offsetInTicksTurnRelative) const override;
        uint64_t StashedSizeInBytes() const override;
        void Drop() override;
        void CopyNonAcknowledgedDataTo(AudioBufferPtr buffer) const override;
        uint64_t NonAcknowledgedSizeInBytes() const override;
        uint64_t GetAbsoluteOffset() const override;
        ProcessedAudioTimestampPtr GetTimestamp(uint64_t offsetInTicksTurnRelative /*Tick = HNS*/) const override;

    private:
        DISABLE_COPY_AND_MOVE(PcmAudioBuffer);

        ProcessedAudioTimestampPtr DiscardBytesUnlocked(uint64_t bytes);
        DataChunkPtr GetNextUnlocked();
        ProcessedAudioTimestampPtr DiscardTillUnlocked(uint64_t offsetInTicks);

        uint64_t DurationToBytes(uint64_t durationInTicks) const;
        uint64_t BytesToDurationInTicks(uint64_t bytes) const;

        uint64_t NonAcknowledgedSizeInBytesUnlocked() const;
        uint64_t StashedSizeInBytesUnlocked() const;

        const uint32_t MillisecondsInSecond = 1000;
        const uint32_t TicksInMillisecond = 10000;

        const SPXWAVEFORMATEX m_header;
        std::deque<DataChunkPtr> m_audioBuffers;
        uint64_t m_totalSizeInBytes;

        size_t m_currentChunk;
        uint64_t m_bufferStartOffsetInBytesTurnRelative;
        uint64_t m_bufferStartOffsetInBytesAbsolute;
        mutable std::mutex m_lock;
        const uint32_t m_bytesPerSample;
        const uint32_t m_samplesPerSecond;
    };
}}}}
