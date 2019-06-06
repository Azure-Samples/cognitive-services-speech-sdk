//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ring_buffer.h: Implementation declarations for RingBuffer C++ class
//

#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <memory>
#include <queue>
#include <mutex>
#include "audio_chunk.h"

#undef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class RingBuffer
{
public:
    RingBuffer();
    void AddBuffer(const DataChunkPtr& audioChunk);
    void ReleaseBuffers();
    uint32_t GetData(uint8_t* buffer, uint32_t bytesToRead);
    uint32_t GetCurrentSize();

private:
    std::queue<DataChunkPtr> m_bufferQueue;
    std::mutex m_mtx;
    uint32_t m_currentSize;
    uint32_t m_firstBufferStartPosition;
};

}}}} // Microsoft::CognitiveServices::Speech::Impl

#endif

