//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <chrono>
#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

// Data chunk interface.
struct DataChunk
{
    DataChunk(std::shared_ptr<uint8_t> data, uint32_t dataSizeInBytes)
        : data{ data }, size{ dataSizeInBytes }, receivedTime{ std::chrono::system_clock::now() }
    {}

    DataChunk(std::shared_ptr<uint8_t> data, uint32_t dataSizeInBytes, std::chrono::system_clock::time_point chunkTime)
        : data{ data }, size{ dataSizeInBytes }, receivedTime{ chunkTime }
    {}

    DataChunk(std::shared_ptr<uint8_t> data, uint32_t dataSizeInBytes, std::string&& capturedTime, std::string&& userId)
        : data{ data },
        size{ dataSizeInBytes },
        receivedTime{ std::chrono::system_clock::now() },
        capturedTime{ std::move(capturedTime) },
        userId{ std::move(userId) }
    { }

    DataChunk(std::shared_ptr<uint8_t> data, uint32_t dataSizeInBytes, std::chrono::system_clock::time_point chunkTime, std::string&& capturedTime, std::string&& userId)
        : data{ data },
        size{ dataSizeInBytes },
        receivedTime{ chunkTime },
        capturedTime{ std::move(capturedTime) },
        userId{ std::move(userId) }
    { }

    std::shared_ptr<uint8_t> data;  // audio data.
    uint32_t size;                  // Current size of valid data in bytes
    const std::chrono::system_clock::time_point receivedTime; // The receive time of audio chunk.
    std::string capturedTime;
    std::string userId;
};


using DataChunkPtr = std::shared_ptr<DataChunk>;

}}}}
