//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <atomic>
#include <map>
#include <string>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"
#include "data_buffer.h"
#include "speechapi_cxx.h"
#include "ring_buffer.h"
#include "ispxinterfaces.h"
#include <cstdlib>
#include <ctime>

using namespace Microsoft::CognitiveServices::Speech::Impl; // for mocks

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace std;

static const size_t MAX_BUFF_SIZE = 5000;
static const size_t TEST_EXIT_SIZE = 1000000;
static std::mutex mtx;
static bool done = false;

static size_t GetRandSize()
{
    size_t curNumber = rand();
    size_t val = curNumber % MAX_BUFF_SIZE;
    return val;
}

static void ProducerThread(RingBuffer *ringBuffer, size_t *totalBytesAdded)
{
    uint8_t curVal = 0;
    srand(100);

    while (1)
    {
        size_t rndSize = GetRandSize();
        SpxSharedAudioBuffer_Type sharedmem = SpxAllocSharedAudioBuffer((size_t)rndSize);
        for (size_t i = 0; i < rndSize; i++)
        {
            sharedmem.get()[i] = (curVal++) % 256;
        }
        ringBuffer->AddBuffer(std::make_shared<DataChunk>(sharedmem, (uint32_t)rndSize));
        (*totalBytesAdded) += rndSize;

        if ((*totalBytesAdded) > TEST_EXIT_SIZE)
        {
            std::unique_lock<std::mutex> lock(mtx);
            done = true;
            break;
        }
    }
}

static void ConsumerThread(RingBuffer *ringBuffer, size_t *totalBytesExtracted)
{
    uint8_t expectedVal = 0;
    srand((unsigned int)time(NULL));

    while (1)
    {
        size_t rndSize = GetRandSize();

        SpxSharedAudioBuffer_Type sharedmem = SpxAllocSharedAudioBuffer((size_t)rndSize);
        uint32_t bytesReturned = ringBuffer->GetData(sharedmem.get(), (uint32_t)rndSize);
        (*totalBytesExtracted) += bytesReturned;

        for (size_t i = 0; i < bytesReturned; i++)
        {
            int received = (int)sharedmem.get()[i];
            int expected = (int)(int)((expectedVal++) % 256);
            if (received != expected)
            {
                SPXTEST_REQUIRE(received == expected);
            }
        }

        if(bytesReturned == 0)
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (done && ringBuffer->GetCurrentSize() == 0)
            {
                break;
            }
        }
    }
}

TEST_CASE("Ring buffer basics", "[api][cxx][buffer]")
{
    SECTION("ring buffer basics works")
    {
        WHEN("Check RingBuffer Gives Valid Data In GetData")
        {
            RingBuffer ringBuffer;

            size_t totalBytesAdded = 0;
            size_t totalBytesExtracted = 0;

            std::thread producer(ProducerThread, &ringBuffer, &totalBytesAdded);
            std::thread consumer(ConsumerThread, &ringBuffer, &totalBytesExtracted);

            producer.join();
            consumer.join();
            SPXTEST_REQUIRE(totalBytesAdded == totalBytesExtracted);
        }
    }
}
