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
#include <mutex>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    // Data chunk.
    struct data_chunk
    {
        data_chunk(const uint8_t* ptr, size_t size_in_bytes) : data(ptr), size(size_in_bytes)
        {}

        const uint8_t* data; // Raw pointer to data, guaranteed to exist while data_chunk exists.
        size_t size;         // Size of the data.
    };

    using data_chunk_ptr = std::shared_ptr<data_chunk>;

    // Audio buffer interface.
    class audio_buffer
    {
    public:
        // Adds a new piece of data of the given size to the buffer.
        virtual void add(const std::shared_ptr<uint8_t>& data, size_t data_size) = 0;

        // Returns true if there is data available.
        virtual bool has_next() = 0;

        // Gets next not-yet confirmed chunk from the buffer.
        // Returns nullptr if no more data available.
        virtual data_chunk_ptr get_next() = 0;

        // Resets the offset to 0 and moves internal cursor to the first unconfirmed chunk,
        // Used to resend chunks that were not confirmed after a break of connection/TurnStart/End messages.
        virtual void new_turn() = 0;

        // Discards all chunks till the specified offset in ticks. Should be called on successful final result.
        virtual void discard_till(uint64_t offset_in_ticks) = 0;

        virtual ~audio_buffer() {}
    };

    using audio_buffer_ptr = std::shared_ptr<audio_buffer>;

    // Audio buffer based on pcm format.
    class pcm_audio_buffer : public audio_buffer
    {
    public:
        explicit pcm_audio_buffer(const WAVEFORMATEX& header);

        void add(const std::shared_ptr<uint8_t>& data, size_t data_size) override;
        bool has_next() override;
        data_chunk_ptr get_next() override;
        void new_turn() override;
        void discard_till(uint64_t offset) override;

    private:
        DISABLE_COPY_AND_MOVE(pcm_audio_buffer);

        struct pcm_data_chunk : public data_chunk
        {
            pcm_data_chunk(const std::shared_ptr<uint8_t>& owner, size_t data_size)
                : data_chunk { owner.get(), data_size }, m_owner(owner)
            {}

            std::shared_ptr<uint8_t> m_owner;
        };

        using pcm_data_chunk_ptr = std::shared_ptr<pcm_data_chunk>;

        uint64_t duration_to_bytes(uint64_t duration_in_milliseconds) const;

        const int MillisecondsInSecond = 1000;
        const int TicksInMillisecond = 10000;

        const WAVEFORMATEX m_header;
        std::deque<pcm_data_chunk_ptr> m_audio_buffers;

        size_t m_current_chunk;
        uint64_t m_buffer_start_offset_in_ticks;
        std::mutex m_lock;
    };
}}}}
