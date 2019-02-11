//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <cmath>
#include "audio_buffer.h"

using namespace std::chrono;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    PcmAudioBuffer::PcmAudioBuffer(const SPXWAVEFORMATEX& header)
        : m_header{ header },
          m_totalSizeInBytes{ 0 },
          m_currentChunk{ 0 },
          m_bufferStartOffsetInBytesTurnRelative{ 0 },
          m_bufferStartOffsetInBytesAbsolute{ 0 },
          m_bytesPerSample{ header.wBitsPerSample / 8u },
          m_samplesPerSecond{ header.nSamplesPerSec }
    {
        if (header.wBitsPerSample % 8 != 0)
        {
            SPX_TRACE_ERROR("going to throw wrong bit per sample runtime_error");
            ThrowRuntimeError("Bits per sample '" + std::to_string(header.wBitsPerSample) + "' is not supported. It should be dividable by 8.");
        }
    }

    void PcmAudioBuffer::Add(const DataChunkPtr& audioChunk)
    {
        std::unique_lock<std::mutex> guard(m_lock);
        m_audioBuffers.push_back(audioChunk);
        m_totalSizeInBytes += audioChunk->size;
    }

    DataChunkPtr PcmAudioBuffer::GetNext()
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return GetNextUnlocked();
    }

    void PcmAudioBuffer::NewTurn()
    {
        std::unique_lock<std::mutex> guard(m_lock);
        m_bufferStartOffsetInBytesTurnRelative = 0;
        m_currentChunk = 0;
    }

    void PcmAudioBuffer::DiscardBytes(uint64_t bytes)
    {
        std::unique_lock<std::mutex> guard(m_lock);
        DiscardBytesUnlocked(bytes);
    }

    // In case when we need to have a shared pointer A into a buffer that already
    // managed by some shared pointer B, we define a custom deleter that captures pointer B
    // and resets in when ref counter of pointer A gets to 0.
    struct PtrHolder
    {
        // Have to be mutable in order to reset it in the custom deleter.
        mutable std::shared_ptr<uint8_t> data;
    };

    ProcessedAudioTimestampPtr PcmAudioBuffer::DiscardBytesUnlocked(uint64_t bytes)
    {
        system_clock::time_point audioTimestamp;
        uint64_t remainingInTicks;
        bool foundTimeStamp = false;

        uint64_t chunkBytes = 0;
        while (!m_audioBuffers.empty() && bytes &&
               (chunkBytes = m_audioBuffers.front()->size) <= bytes)
        {
            bytes -= chunkBytes;
            if (bytes == 0)
            {
                audioTimestamp = m_audioBuffers.front()->receivedTime;
                remainingInTicks = 0;
                foundTimeStamp = true;
            }
            m_audioBuffers.pop_front();
            m_currentChunk--;
            SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, m_totalSizeInBytes < chunkBytes);
            m_totalSizeInBytes -= chunkBytes;
            m_bufferStartOffsetInBytesTurnRelative += chunkBytes;
            m_bufferStartOffsetInBytesAbsolute += chunkBytes;
        }

        if (m_audioBuffers.empty())
        {
            if (m_totalSizeInBytes != 0)
            {
                SPX_DBG_TRACE_ERROR("%s: Invalid state of the audio buffer, no chunks but totalSize %d", __FUNCTION__, (int)m_totalSizeInBytes);
                SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            }

            if (bytes > 0)
            {
                SPX_DBG_TRACE_WARNING("%s: Discarding more data than what is available in the buffer %d", __FUNCTION__, (int)bytes);
            }

            m_currentChunk = 0;
        }
        else if (bytes > 0)
        {
            audioTimestamp = m_audioBuffers.front()->receivedTime;
            remainingInTicks = BytesToDurationInTicks(m_audioBuffers.front()->size - bytes);
            foundTimeStamp = true;

            // At this point, bytes is less than the size of current chunk, so safe to cast to uint32_t.
            m_audioBuffers.front()->size -= (uint32_t)bytes;
            m_bufferStartOffsetInBytesTurnRelative += bytes;
            m_bufferStartOffsetInBytesAbsolute += bytes;
            auto holder = PtrHolder{ m_audioBuffers.front()->data };
            m_audioBuffers.front()->data = std::shared_ptr<uint8_t>(holder.data.get() + bytes, [holder](void *) { holder.data.reset(); });
            SPX_THROW_HR_IF(SPXERR_RUNTIME_ERROR, m_totalSizeInBytes < bytes);
            m_totalSizeInBytes -= bytes;
        }

        return foundTimeStamp ? std::make_shared<ProcessedAudioTimestamp>(audioTimestamp, remainingInTicks) : nullptr;
    }

    ProcessedAudioTimestampPtr PcmAudioBuffer::DiscardTill(uint64_t offsetInTicks)
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return DiscardTillUnlocked(offsetInTicks);
    }

    uint64_t PcmAudioBuffer::DurationToBytes(uint64_t durationInTicks) const
    {
        if (m_samplesPerSecond % MillisecondsInSecond == 0)
        {
            return m_header.nChannels * m_bytesPerSample *
                  (m_samplesPerSecond / MillisecondsInSecond) *
                  (durationInTicks / TicksInMillisecond);
        }
        else
        {
            return (uint64_t)std::ceil(((double)m_samplesPerSecond / MillisecondsInSecond) *
                                        (durationInTicks / TicksInMillisecond))
                * m_header.nChannels * m_bytesPerSample;
        }
    }

    uint64_t PcmAudioBuffer::BytesToDurationInTicks(uint64_t bytes) const
    {
        if (m_samplesPerSecond % MillisecondsInSecond == 0)
        {
            return (bytes * TicksInMillisecond * MillisecondsInSecond) /
                (m_header.nChannels * m_bytesPerSample * m_samplesPerSecond);
        }
        else
        {
            return (uint64_t)std::ceil(bytes * TicksInMillisecond * MillisecondsInSecond / (double)m_samplesPerSecond) /
                (m_header.nChannels * m_bytesPerSample);
        }
    }

    uint64_t PcmAudioBuffer::ToAbsolute(uint64_t offsetInTicksTurnRelative) const
    {
        int64_t bytes = DurationToBytes(offsetInTicksTurnRelative) - m_bufferStartOffsetInBytesTurnRelative;
        return BytesToDurationInTicks(m_bufferStartOffsetInBytesAbsolute + bytes);
    }

    uint64_t PcmAudioBuffer::StashedSizeInBytes() const
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return StashedSizeInBytesUnlocked();
    }

    uint64_t PcmAudioBuffer::StashedSizeInBytesUnlocked() const
    {
        uint64_t size = 0;
        for (size_t i = m_currentChunk; i < m_audioBuffers.size(); ++i)
        {
            size += m_audioBuffers[i]->size;
        }
        return size;
    }

    uint64_t PcmAudioBuffer::NonAcknowledgedSizeInBytesUnlocked() const
    {
        uint64_t unconfirmedBytes = 0;
        for (size_t i = 0; i < m_currentChunk; ++i)
        {
            unconfirmedBytes += m_audioBuffers[i]->size;
        }

        return unconfirmedBytes;
    }

    void PcmAudioBuffer::Drop()
    {
        std::unique_lock<std::mutex> guard(m_lock);

        // Discarding unconfirmed bytes that we have already sent to the service.
        uint64_t unconfirmedBytes = NonAcknowledgedSizeInBytesUnlocked();
        DiscardBytesUnlocked(unconfirmedBytes);

        // Discarding chunks that we have not yet sent to the service.
        DataChunkPtr chunk;
        while ((chunk = GetNextUnlocked()) != nullptr)
        {
            DiscardBytesUnlocked(chunk->size);
        }
    }

    void PcmAudioBuffer::CopyNonAcknowledgedDataTo(AudioBufferPtr buffer) const
    {
        if (buffer.get() == this)
        {
            return;
        }

        std::unique_lock<std::mutex> guard(m_lock);
        for (const auto& c : this->m_audioBuffers)
            buffer->Add(std::make_shared<DataChunk>(c->data, c->size, c->receivedTime));
    }

    DataChunkPtr PcmAudioBuffer::GetNextUnlocked()
    {
        if (m_currentChunk >= m_audioBuffers.size())
        {
            // No data available.
            return nullptr;
        }

        DataChunkPtr result = m_audioBuffers[m_currentChunk];
        m_currentChunk++;
        return result;
    }

    ProcessedAudioTimestampPtr PcmAudioBuffer::DiscardTillUnlocked(uint64_t offsetInTicks)
    {
        uint64_t offsetInBytes = DurationToBytes(offsetInTicks);
        if (offsetInBytes < m_bufferStartOffsetInBytesTurnRelative)
        {
            SPX_DBG_TRACE_WARNING("%s: Offset is not monothonically increasing. Current turn offset in bytes %d, discarding bytes %d",
                __FUNCTION__, (int)m_bufferStartOffsetInBytesTurnRelative, (int)offsetInBytes);
            return nullptr;
        }
        return DiscardBytesUnlocked(offsetInBytes - m_bufferStartOffsetInBytesTurnRelative);
    }

    uint64_t PcmAudioBuffer::NonAcknowledgedSizeInBytes() const
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return NonAcknowledgedSizeInBytesUnlocked();
    }

    uint64_t PcmAudioBuffer::GetAbsoluteOffset() const
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return BytesToDurationInTicks(m_bufferStartOffsetInBytesAbsolute);
    }

    ProcessedAudioTimestampPtr PcmAudioBuffer::GetTimestamp(uint64_t offsetInTicks) const
    {
        std::unique_lock<std::mutex> guard(m_lock);

        uint64_t offsetInBytes = DurationToBytes(offsetInTicks);
        if (offsetInBytes < m_bufferStartOffsetInBytesTurnRelative)
        {
            SPX_DBG_TRACE_WARNING("%s: Offset is not monothonically increasing. Current turn offset in bytes %d, offset to get timestamp in bytes %d",
                __FUNCTION__, (int)m_bufferStartOffsetInBytesTurnRelative, (int)offsetInBytes);
            return nullptr;
        }

        uint64_t bytes = offsetInBytes - m_bufferStartOffsetInBytesTurnRelative;
        uint64_t chunkBytes = 0;
        auto queueSize = m_audioBuffers.size();
        size_t index;
        for (index = 0; index < queueSize; index++)
        {
            chunkBytes = m_audioBuffers[index]->size;
            if (chunkBytes >= bytes)
            {
                break;
            }
            else
            {
                bytes -= chunkBytes;
            }
        }

        if (index >= queueSize)
        {
            SPX_DBG_ASSERT_WITH_MESSAGE(bytes > 0, "Reach end of queue, but no bytes left.");
            SPX_DBG_TRACE_WARNING("%s: Offset exceeds what is available in the buffer %d. No timestamp can be retrieved.", __FUNCTION__, (int)bytes);
            return nullptr;
        }

        system_clock::time_point audioTimestamp = m_audioBuffers[index]->receivedTime;;
        uint64_t remainingInTicks = BytesToDurationInTicks(chunkBytes - bytes);
        return std::make_shared<ProcessedAudioTimestamp>(audioTimestamp, remainingInTicks);
    }

}}}}
