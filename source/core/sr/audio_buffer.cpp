//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "audio_buffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    pcm_audio_buffer::pcm_audio_buffer(const WAVEFORMATEX& header)
        : m_header{ header },
          m_current_chunk{ 0 },
          m_buffer_start_offset_in_ticks{ 0 }
    {
        // Make sure milliseconds are precisely represent samples.
        if (m_header.nSamplesPerSec % MillisecondsInSecond != 0)
        {
            throw std::runtime_error("Sample rate " + std::to_string(m_header.nSamplesPerSec) + " is not supported.");
        }
    }

    void pcm_audio_buffer::add(const std::shared_ptr<uint8_t>& data, size_t data_size)
    {
        std::unique_lock<std::mutex> guard(m_lock);
        m_audio_buffers.push_back(std::make_shared<pcm_data_chunk>(data, data_size));
    }

    bool pcm_audio_buffer::has_next()
    {
        std::unique_lock<std::mutex> guard(m_lock);
        return m_current_chunk < m_audio_buffers.size();
    }

    data_chunk_ptr pcm_audio_buffer::get_next()
    {
        std::unique_lock<std::mutex> guard(m_lock);
        if (m_current_chunk >= m_audio_buffers.size())
        {
            // No data available.
            return nullptr;
        }

        pcm_data_chunk_ptr result = m_audio_buffers[m_current_chunk];
        m_current_chunk++;
        return result;
    }

    void pcm_audio_buffer::new_turn()
    {
        std::unique_lock<std::mutex> guard(m_lock);
        m_buffer_start_offset_in_ticks = 0;
        m_current_chunk = 0;
    }

    void pcm_audio_buffer::discard_till(uint64_t offset_in_ticks)
    {
        std::unique_lock<std::mutex> guard(m_lock);
        int64_t duration = offset_in_ticks - m_buffer_start_offset_in_ticks;
        if (duration <= 0)
        {
            return;
        }

        int64_t duration_in_milliseconds = duration / TicksInMillisecond;
        uint64_t bytes = duration_to_bytes(duration_in_milliseconds);
        size_t chunk_bytes = 0;
        while (!m_audio_buffers.empty() && (chunk_bytes = m_audio_buffers.front()->size) <= bytes)
        {
            bytes -= chunk_bytes;
            m_audio_buffers.pop_front();
            m_current_chunk--;
        }

        m_buffer_start_offset_in_ticks = offset_in_ticks;
        if (m_audio_buffers.empty())
        {
            m_current_chunk = 0;
        }
        else if (bytes > 0)
        {
            m_audio_buffers.front()->size -= (size_t)bytes;
            m_audio_buffers.front()->data += (size_t)bytes;
        }
    }

    uint64_t pcm_audio_buffer::duration_to_bytes(uint64_t duration_in_milliseconds) const
    {
        return m_header.nChannels * (m_header.wBitsPerSample / 8) * (m_header.nSamplesPerSec / MillisecondsInSecond) * duration_in_milliseconds;
    }

}}}}
