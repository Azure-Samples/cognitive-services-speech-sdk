//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ring_buffer.cpp: Implementation definitions for RingBuffer methods
//

#include "ring_buffer.h"
#include <cstring>
#include "spxdebug.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

RingBuffer::RingBuffer()
{
    m_currentSize = 0;
    m_firstBufferStartPosition = 0;
}

uint32_t RingBuffer::GetCurrentSize()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    return m_currentSize;
}

void RingBuffer::AddBuffer(const DataChunkPtr& audioChunk)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if (audioChunk->size > 0)
    {
        m_bufferQueue.push(audioChunk);
        m_currentSize += audioChunk->size;
    }
}

uint32_t RingBuffer::GetData(uint8_t* buffer, uint32_t bytesToRead)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    uint32_t amountFilled = 0;
    DataChunkPtr front = nullptr;
    uint32_t lastAmountCopied = 0;

    if (buffer != nullptr && bytesToRead > 0)
    {
        while (amountFilled < bytesToRead && !m_bufferQueue.empty())
        {
            front = m_bufferQueue.front();
            if (front != nullptr && front->data != nullptr)
            {
                lastAmountCopied = MIN(front->size, bytesToRead - amountFilled);

                std::memcpy(&buffer[amountFilled], &(front->data.get()[m_firstBufferStartPosition]), lastAmountCopied);
                amountFilled += lastAmountCopied;

                if (lastAmountCopied == front->size)
                {
                    m_bufferQueue.pop();
                    m_firstBufferStartPosition = 0;
                }
            }
            else
            {
                SPX_TRACE_ERROR("nullptr cannot be in the buffer queue");
                m_bufferQueue.pop();
            }
        }

        if (!m_bufferQueue.empty() &&
            (front != nullptr) &&
            (front->data != nullptr) &&
            ((front->size - lastAmountCopied) > 0))
        {
            m_firstBufferStartPosition += lastAmountCopied;
            front->size = front->size - lastAmountCopied;
        }
    }
    m_currentSize -= amountFilled;
    return amountFilled;
}

}}}} // Microsoft::CognitiveServices::Speech::Impl
