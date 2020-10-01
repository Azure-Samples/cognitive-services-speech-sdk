//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "test_utils.h"
#include <thread>
#include <random>
#include <string>

#include <audio_buffer.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

#define CHUNK_SIZE 3200
#define CHUNK_SIZEMS 100 * 10000

std::shared_ptr<uint8_t> make_test_buffer(size_t bufferId, size_t size)
{
    auto buffer = SpxAllocSharedAudioBuffer(size);

    for (size_t i = 0; i < size; i++)
    {
        (&(*buffer))[i++] = (uint8_t)bufferId;
        (&(*buffer))[i] = (uint8_t)(((i - 1) / 2) % 256);
    }

    return buffer;
}

std::shared_ptr<PcmAudioBuffer> make_test_audioBuffer(size_t elementCount, size_t size)
{
    SPXWAVEFORMATEX format;
    format.nChannels = 1;
    format.nSamplesPerSec = 16000;
    format.wBitsPerSample = 16;

    auto ptr = std::make_shared<PcmAudioBuffer>(format);

    for (size_t i = 0; i < elementCount; i++)
    {
        auto buffer = make_test_buffer(i, size);
        auto chunk = std::make_shared<DataChunk>(buffer, (uint32_t)size);
        ptr->Add(chunk);
    }

    return ptr;
}

TEST_CASE("PcmAudioBuffer", "PcmAudioBuffer")
{
    auto testBuffer = make_test_audioBuffer(5, CHUNK_SIZE);

    SPXTEST_SECTION("Size")
    {
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 5);
    }

    SPXTEST_SECTION("Pull Chunk")
    {
        auto pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }
    }

    SPXTEST_SECTION("Discard One Chunk by size")
    {
        // Pull the chunk
        auto pulledBuffer = testBuffer->GetNext();

        // Discard it.
        testBuffer->DiscardBytes(CHUNK_SIZE);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);
    }

    SPXTEST_SECTION("Discard Half Chunk by size")
    {
        // Pull the chunk
        auto pulledBuffer = testBuffer->GetNext();

        // Discard half of it.
        testBuffer->DiscardBytes(CHUNK_SIZE / 2);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == (CHUNK_SIZE / 2));
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        // Start a new turn, should reset the buffer
        testBuffer->NewTurn();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4.5);

        // Should pull the remaining half.
        pulledBuffer = testBuffer->GetNext();

        SPXTEST_REQUIRE(pulledBuffer->size == (CHUNK_SIZE / 2));
        for (size_t i = 0; i < 512; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i + ((CHUNK_SIZE / 2) - 1)) / 2) % 256));
        }
    }

    SPXTEST_SECTION("Discard 2 chunks by size")
    {
        auto pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 2);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 1);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        testBuffer->DiscardBytes(CHUNK_SIZE * 2);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 2);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 2);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

    }

    SPXTEST_SECTION("Discard One Chunk by offset")
    {
        // Pull the chunk
        auto pulledBuffer = testBuffer->GetNext();

        // Discard it.
        testBuffer->DiscardTill(CHUNK_SIZEMS);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);
    }

    SPXTEST_SECTION("Discard Half Chunk by Offset")
    {
        // Pull the chunk
        auto pulledBuffer = testBuffer->GetNext();

        // Discard half of it.
        testBuffer->DiscardTill(CHUNK_SIZEMS / 2);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == (CHUNK_SIZE / 2));
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        // Start a new turn, should reset the buffer
        testBuffer->NewTurn();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4.5);

        // Should pull the remaining half.
        pulledBuffer = testBuffer->GetNext();

        SPXTEST_REQUIRE(pulledBuffer->size == (CHUNK_SIZE / 2));
        for (size_t i = 0; i < 512; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i + ((CHUNK_SIZE / 2) - 1)) / 2) % 256));
        }
    }

    SPXTEST_SECTION("Discard 2 chunks by offset")
    {
        auto pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 2);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 1);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        testBuffer->DiscardTill(CHUNK_SIZEMS * 2);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 2);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 2);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

    }

    SPXTEST_SECTION("Discard 1 chunk per turn by offset")
    {
        auto pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 0);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        testBuffer->DiscardTill(CHUNK_SIZEMS);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 4);

        testBuffer->NewTurn();

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 1);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }

        // Discards until the end of the 2nd chunk.
        testBuffer->DiscardTill(CHUNK_SIZEMS * 2);
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == 0);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 3);

        pulledBuffer = testBuffer->GetNext();
        SPXTEST_REQUIRE(testBuffer->NonAcknowledgedSizeInBytes() == CHUNK_SIZE * 1);
        SPXTEST_REQUIRE(testBuffer->StashedSizeInBytes() == CHUNK_SIZE * 2);

        for (size_t i = 0; i < CHUNK_SIZE; i++)
        {
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == 2);
            i++;
            SPXTEST_REQUIRE((&(*pulledBuffer->data))[i] == (uint8_t)(((i - 1) / 2) % 256));
        }
    }
}
