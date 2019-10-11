//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <chrono>
#include <thread>
#include <random>
#include <string>

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "test_utils.h"
#include "ispxinterfaces.h"
#include "read_write_ring_buffer.h"
#include "blocking_read_write_ring_buffer.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;


TEST_CASE("CSpxReadWriteRingBuffer Basics", "[ringbuffer]")
{
    auto rb = std::make_shared<CSpxReadWriteRingBuffer>();
    REQUIRE(rb != nullptr);

    // Prepare a random buffer to play with
    size_t size = 1024;

    auto data = new uint8_t[size];
    for (size_t i = 0; i < size; i++) data[i] = (uint8_t)(i % 256);

    SPXTEST_WHEN("initialized at 1024")
    {
       auto name = "TestCase: ReadWriteRingBuffer Basics 1024";
       REQUIRE_NOTHROW(rb->SetName(name));
       SPXTEST_REQUIRE(rb->GetName() == name);

       REQUIRE_NOTHROW(rb->SetSize(size));
       SPXTEST_REQUIRE(size == rb->GetSize());

       REQUIRE(rb->GetWritePos() == 0);
       REQUIRE(rb->GetReadPos() == 0);

       auto write = SpxQueryInterface<ISpxReadWriteBuffer>(rb);
       SPXTEST_WHEN("no data")
       {
           REQUIRE_THROWS(rb->Read(data, 1));
           REQUIRE_THROWS(rb->Read(data, size));
       }

       SPXTEST_WHEN("half full")
       {
           REQUIRE_NOTHROW(rb->Write(data, size / 2));
           REQUIRE(rb->GetWritePos() == size / 2);
           REQUIRE(rb->GetReadPos() == 0);

           REQUIRE_THROWS(rb->Read(data, size)); // not that much available
           REQUIRE_THROWS(rb->Write(data, size)); // not that much space available

           REQUIRE(rb->GetWritePos() == size / 2); // the Read and Write earlier shouldn't have changed this position
           REQUIRE(rb->GetReadPos() == 0); // the Read and Write earlier shouldn't have changed this position

           SPXTEST_WHEN("its termed")
           {
               REQUIRE_NOTHROW(rb->Term());
               REQUIRE_THROWS(rb->GetSize());
               REQUIRE_THROWS(rb->Write(data, 1));
               REQUIRE_THROWS(rb->ReadShared(1));
           }

           SPXTEST_WHEN("read all of it at once")
           {
               auto read = new uint8_t[size];
               REQUIRE_NOTHROW(rb->Read(read, size /2 ));
               REQUIRE(rb->GetWritePos() == size / 2);
               REQUIRE(rb->GetReadPos() == size / 2);

               SPXTEST_REQUIRE(memcmp(data, read, size / 2) == 0);

               REQUIRE_THROWS(rb->Read(read, 1)); // nothing left to read, not even 1 byte
               REQUIRE_THROWS(rb->Read(read, size)); // nor huge amount
               REQUIRE_THROWS(rb->Read(read, size / 2)); // nor half as much
           }

           SPXTEST_WHEN("read all of it in parts")
           {
               auto read = new uint8_t[size];
               auto ptr = &read[0];

               size_t bytesToRead1 = 10;
               REQUIRE_NOTHROW(rb->Read(ptr, bytesToRead1));
               REQUIRE(rb->GetReadPos() == bytesToRead1);
               ptr += bytesToRead1;

               size_t bytesRead2 = 0;
               REQUIRE_NOTHROW(rb->Read(ptr, size, &bytesRead2));
               REQUIRE(rb->GetReadPos() == bytesToRead1 + bytesRead2);
               ptr += bytesRead2;

               SPXTEST_REQUIRE((bytesToRead1 + bytesRead2) == size / 2);
               SPXTEST_REQUIRE(memcmp(data, read, size / 2) == 0);

               REQUIRE(rb->GetWritePos() == size / 2);
               REQUIRE(rb->GetReadPos() == size / 2);

               REQUIRE_THROWS(rb->Read(read, 1)); // nothing left to read, not even 1 byte
               REQUIRE_THROWS(rb->Read(read, size)); // nor huge amount
               REQUIRE_THROWS(rb->Read(read, size / 2)); // nor half as much

               REQUIRE(rb->GetWritePos() == size / 2);
               REQUIRE(rb->GetReadPos() == size / 2);
           }
       }
    }

    SPXTEST_WHEN("initialized at 1025")
    {
        auto name = "TestCase: ReadWriteRingBuffer Basics 1025";
        REQUIRE_NOTHROW(rb->SetName(name));
        SPXTEST_REQUIRE(rb->GetName() == name);

        REQUIRE_NOTHROW(rb->SetSize(size + 1));
        SPXTEST_REQUIRE(size + 1 == rb->GetSize());

        uint64_t initPos = 7;
        REQUIRE_NOTHROW(rb->SetInitPos(initPos));
        SPXTEST_REQUIRE(rb->GetInitPos() == initPos);

        REQUIRE(rb->GetWritePos() == initPos);
        REQUIRE(rb->GetReadPos() == initPos);

        auto write = SpxQueryInterface<ISpxReadWriteBuffer>(rb);

        SPXTEST_WHEN("writing lots and lots of data, one byte at a time")
        {
           auto lotsOfBytes = size + 37;
           for (size_t i = 0; i < lotsOfBytes; i++)
           {
               uint8_t theByteToWrite = data[(i % size)];
               REQUIRE_NOTHROW(rb->Write(&theByteToWrite, 1));
               REQUIRE(rb->GetWritePos() == rb->GetReadPos() + 1);

               uint8_t theByteRead;
               REQUIRE_NOTHROW(rb->Read(&theByteRead, 1));
               REQUIRE(rb->GetReadPos() == rb->GetWritePos());

               REQUIRE(theByteToWrite == theByteRead);
           }
        }

        SPXTEST_WHEN("writing lots and lots of data, biggish buggers")
        {
            SPXTEST_SECTION("using raw buffers")
            {
                auto read = new uint8_t[size];

                auto lotsOfTimes = 3;
                auto parts = 4;
                for (int i = 0; i < lotsOfTimes * parts; i++)
                {
                    auto ptr = &data[size * (i % parts) / parts];
                    REQUIRE_NOTHROW(rb->Write(ptr, size / parts));

                    REQUIRE_NOTHROW(rb->Read(read, size / parts));
                    REQUIRE(rb->GetReadPos() == rb->GetWritePos());

                    SPXTEST_REQUIRE(memcmp(ptr, read, size / parts) == 0);
                }

                REQUIRE_NOTHROW(rb->ReadAtBytePos(rb->GetReadPos() - size, read, size));
                SPXTEST_REQUIRE(memcmp(data, read, size) == 0);
            }

            SPXTEST_SECTION("using shared buffers")
            {
                auto lotsOfTimes = 3;
                auto parts = 4;
                for (int i = 0; i < lotsOfTimes * parts; i++)
                {
                    auto ptr = &data[size * (i % parts) / parts];
                    REQUIRE_NOTHROW(rb->Write(ptr, size / parts));

                    auto sharedData = rb->ReadShared(size / parts);
                    REQUIRE(rb->GetReadPos() == rb->GetWritePos());

                    SPXTEST_REQUIRE(memcmp(ptr, sharedData.get(), size / parts) == 0);
                }

                auto dataShared = rb->ReadSharedAtBytePos(rb->GetReadPos() - size, size);
                SPXTEST_REQUIRE(memcmp(data, dataShared.get(), size) == 0);
            }
        }
    }
}

TEST_CASE("BlockingReadWriteRingBuffer Basics", "[ringbuffer]")
{
    auto rb = std::make_shared<CSpxBlockingReadWriteRingBuffer>();
    REQUIRE(rb != nullptr);

    // Prepare a random buffer to play with
    size_t size = 1024;
    std::unique_ptr<uint8_t[]> data{ new uint8_t[size] };

    for (size_t i = 0; i < size; i++) data[i] = (uint8_t)(i % 256);

    auto name = "TestCase: BlockingReadWriteRingBuffer Basics 1024";
    rb->SetName(name);
    rb->SetSize(size);

    auto writeDataFunc = [&]() {
        std::this_thread::sleep_for(50ms);
        rb->Write(data.get(), size / 2);
        std::this_thread::sleep_for(50ms);
        rb->Write(data.get() + size / 2, size / 2);
    };

    SPXTEST_WHEN("using raw buffers")
    {
        auto future = std::async([&](){ writeDataFunc(); });
        std::unique_ptr<uint8_t[]> read{ new uint8_t[size] };
        REQUIRE_NOTHROW(rb->Read(read.get(), size));
        SPXTEST_REQUIRE(memcmp(data.get(), read.get(), size) == 0);
        REQUIRE_NOTHROW(future.get());
    }

    SPXTEST_WHEN("using raw buffers with reads at certain position")
    {
        auto future = std::async([&]() { writeDataFunc(); });
        std::unique_ptr<uint8_t[]> read{ new uint8_t[size] };
        size_t readSize = 0;
        uint64_t pos = 0;

        SPX_TRACE_INFO("Reading at pos: %" PRIu64 "\n", pos);

        REQUIRE_NOTHROW(rb->ReadAtBytePos(pos, read.get(), size, &readSize));
        REQUIRE(readSize == size);
        SPXTEST_REQUIRE(memcmp(data.get(), read.get(), readSize) == 0);
        REQUIRE_NOTHROW(future.get());
    }


    SPXTEST_WHEN("using shared buffers")
    {
        auto future = std::async([&]() { writeDataFunc(); });
        auto read = rb->ReadShared(size);
        SPXTEST_REQUIRE(memcmp(data.get(), read.get(), size) == 0);
        REQUIRE_NOTHROW(future.get());
    }

    SPXTEST_WHEN("using shared buffers but not waiting for all of it")
    {
        auto future = std::async([&]() {
            writeDataFunc();

            std::this_thread::sleep_for(2000ms);
            rb->Write(nullptr, 0);
        });

        size_t bytesRead = 0;
        auto read = rb->ReadShared(size * 10, &bytesRead);
        SPXTEST_REQUIRE(memcmp(data.get(), read.get(), bytesRead) == 0);
        SPXTEST_REQUIRE(bytesRead == size);
    }
}
