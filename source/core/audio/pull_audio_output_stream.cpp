//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// pull_audio_output_stream.cpp: Implementation definitions for CSpxPullAudioOutputStream C++ class
//

#include "stdafx.h"
#include "pull_audio_output_stream.h"
#include <algorithm>
#include <cstring>
#include <chrono>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxPullAudioOutputStream::CSpxPullAudioOutputStream() : m_writingEnded(false)
{
}

uint32_t CSpxPullAudioOutputStream::Write(uint8_t* buffer, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE("CSpxPullAudioOutputStream::Write buffer %p size=%d", (void*)buffer, size);

    if (size == 0)
    {
        return 0;
    }

    SPX_IFTRUE_THROW_HR(buffer == nullptr, SPXERR_INVALID_ARG);

    // Allocate the buffer for the audio, and make a copy of the data
    auto newBuffer = SpxAllocSharedAudioBuffer(size);
    memcpy(newBuffer.get(), buffer, size);

    // Store the buffer in our queue
    std::unique_lock<std::mutex> lock(m_mutex);
    m_writingEnded = false;
    m_audioQueue.emplace(newBuffer, size);
    m_inventorySize += size;
    m_cv.notify_all();

    return size;
}

void CSpxPullAudioOutputStream::ClearUnread()
{
    SPX_DBG_TRACE_VERBOSE(__FUNCTION__);

    std::unique_lock<std::mutex> lock(m_mutex);
    // clear audio queue
    std::queue<std::pair<SpxSharedAudioBuffer_Type, uint32_t>> empty;
    std::swap(m_audioQueue, empty);
    m_inventorySize = 0;
    m_frontItemPartiallyRead = false;
    m_cv.notify_all();
}

void CSpxPullAudioOutputStream::Close()
{
    SignalEndOfWriting();
}

uint32_t CSpxPullAudioOutputStream::Read(uint8_t* buffer, uint32_t bufferSize)
{
    SPX_DBG_TRACE_VERBOSE("CSpxPullAudioOutputStream::Read: is called");
    SPX_IFTRUE_THROW_HR(buffer == nullptr, SPXERR_INVALID_ARG);

    // Wait until either enough data is collected, or writing is finished, otherwise it's unexpected
    if (!WaitForMoreData(bufferSize) && !m_writingEnded)
    {
        SPX_THROW_HR(SPXERR_UNEXPECTED_AUDIO_OUTPUT_FAILURE);
    }

    return FillBuffer(buffer, bufferSize);
}

uint32_t CSpxPullAudioOutputStream::FillBuffer(uint8_t* buffer, uint32_t bufferSize)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    uint32_t offset = 0;
    uint32_t filledSize = 0;
    while (!m_audioQueue.empty() && filledSize < bufferSize)
    {
        auto item = m_audioQueue.front();
        auto itemData = item.first;
        auto itemSize = m_frontItemPartiallyRead ? m_frontItemRemainingSize : item.second;
        if (offset + itemSize <= bufferSize)
        {
            // Copy the whole item data to buffer
            memcpy(buffer + offset, itemData.get() + item.second - itemSize, itemSize);
            filledSize += itemSize;
            offset += itemSize;
            m_audioQueue.pop();

            // Reset m_frontItemPartiallyRead flag
            m_frontItemPartiallyRead = false;

            SPX_DBG_ASSERT_WITH_MESSAGE(m_inventorySize >= itemSize, "Inventory size less than item size, which is unexpected.");
            m_inventorySize -= itemSize;
        }
        else
        {
            // Copy partial item data
            memcpy(buffer + offset, itemData.get() + item.second - itemSize, bufferSize - offset);

            // Mark that the front item is partially read
            m_frontItemPartiallyRead = true;

            // Record the remaining size of front item
            m_frontItemRemainingSize = itemSize - (bufferSize - offset);

            filledSize += bufferSize - offset;

            SPX_DBG_ASSERT_WITH_MESSAGE(m_inventorySize >= itemSize, "Inventory size less than item size, which is unexpected.");
            m_inventorySize -= bufferSize - offset;
        }
    }

    SPX_DBG_TRACE_VERBOSE("CSpxPullAudioOutputStream::Read: bytesRead=%d", filledSize);
    return filledSize;
}

bool CSpxPullAudioOutputStream::WaitForMoreData(size_t minSize)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_inventorySize < minSize)
    {
        #ifdef _DEBUG
            while (!m_cv.wait_for(lock, std::chrono::milliseconds(100), [&] { return m_inventorySize >= minSize || m_writingEnded; }))
            {
                SPX_DBG_TRACE_VERBOSE("%s: waiting ...", __FUNCTION__);
            }
        #else
            m_cv.wait(lock, [&] { return m_inventorySize >= minSize || m_writingEnded; });
        #endif
    }

    return m_inventorySize >= minSize;
}

void CSpxPullAudioOutputStream::SignalEndOfWriting()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_DBG_TRACE_VERBOSE("Signal End of Writing is called");
    m_writingEnded = true;
    m_cv.notify_all();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
