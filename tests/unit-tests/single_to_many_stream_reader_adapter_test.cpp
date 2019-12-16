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

#include "thread_service.h"
#include "test_utils.h"
#include "guid_utils.h"
#include "create_object_helpers.h"
#include "interface_helpers.h"
#include "site_helpers.h"
#include "sequence_generator_audio_stream_reader.h"
#include "single_to_many_stream_reader_adapter.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

bool ValidateBufferSequence(uint8_t* buffer, int readCount, int* previousLastValue = nullptr)
{
    if (previousLastValue != nullptr && *previousLastValue >= 0)
    {
        uint8_t lastValue = (uint8_t)* previousLastValue;
        if ((buffer[0] - lastValue != 1) &&
            (buffer[0] != 0 || lastValue != 255))
        {
            return false;
        }
    }

    for (int i = 1; i < readCount; i++)
    {
        if ((buffer[i] - buffer[i - 1] != 1) &&
            (buffer[i] != 0 || buffer[i - 1] != 255))
        {
            return false;
        }
    }
    return true;
}

void ReadStreamAndValidateIsInSequence(std::shared_ptr<ISpxAudioStreamReader> reader, int bufferSize, int totalReadCount, int* previousLastValue = nullptr)
{
    std::unique_ptr<uint8_t[]> buffer{ new uint8_t[bufferSize] };
    
    while (totalReadCount > 0)
    {
        reader->Read(buffer.get(), bufferSize);
        REQUIRE(ValidateBufferSequence(buffer.get(), bufferSize, previousLastValue));

        if (previousLastValue != nullptr)
        {
            *previousLastValue = buffer.get()[bufferSize - 1];
        }

        totalReadCount --;
    }
}

TEST_CASE("Demux audio stream adapter is properly functioning", "[core][demux adapter]")
{
    const uint32_t c_SequentialBufferSize = 500;

    std::shared_ptr<ISpxGenericSite> rootSite;
    // try to get the root site by reaching out to the resource manager first as that is the item to have. 
    INFO("Try to reach the root site from the site at hand by finding the resource manager on a given site")
    {
        auto objectFactory = SpxQueryService<ISpxObjectFactory>(SpxGetRootSite());
        rootSite = SpxQueryInterface<ISpxGenericSite>(objectFactory);
    }

    auto demuxAudioReaderAdapter = SpxCreateObjectWithSite<ISpxSingleToManyStreamReaderAdapter>("CSpxSingleToManyStreamReaderAdapter", rootSite);

    auto singletonMockAudioReader = std::make_shared<CSpxSequenceGeneratorAudioStreamReader>();
    singletonMockAudioReader->UseSequentialBufferedData(c_SequentialBufferSize);
    singletonMockAudioReader->SetRealTimeThrottlePercentage(100);

    auto singletonAudioReaderInit = SpxQueryInterface<ISpxObjectWithSite>(singletonMockAudioReader);
    singletonAudioReaderInit->SetSite(SpxGetRootSite());

    auto mockAudioReader = SpxQueryInterface<ISpxAudioStreamReader>(singletonMockAudioReader);
    demuxAudioReaderAdapter->SetSingletonReader(mockAudioReader);

    auto audioReaderFactory = SpxQueryInterface<ISpxAudioStreamReaderFactory>(demuxAudioReaderAdapter);
    SECTION("Validate GetFormat")
    {
        // Get the audio format. We pass this information to the individual readers.
        auto mockSingletonFormatSize = singletonMockAudioReader->GetFormat(nullptr, 0);
        auto sourceFormat = SpxAllocWAVEFORMATEX(mockSingletonFormatSize);
        singletonMockAudioReader->GetFormat(sourceFormat.get(), mockSingletonFormatSize);

        auto demuxReader = audioReaderFactory->CreateReader();
        auto demuxReaderFormatSize = demuxReader->GetFormat(nullptr, 0);
        auto demuxReaderFormat = SpxAllocWAVEFORMATEX(demuxReaderFormatSize);
        demuxReader->GetFormat(demuxReaderFormat.get(), demuxReaderFormatSize);

        REQUIRE(demuxReaderFormat.get() != nullptr);
        REQUIRE(demuxReaderFormat->cbSize == sourceFormat->cbSize);
        REQUIRE(memcmp(demuxReaderFormat.get(), sourceFormat.get(), sizeof(SPXWAVEFORMATEX) + sourceFormat->cbSize) == 0);

        demuxReader->Close();
    }

    SECTION("Create one reader")
    {
        auto demuxReader = audioReaderFactory->CreateReader();
        uint8_t buffer[10];
        auto readBytes = demuxReader->Read(buffer, 10);
        REQUIRE(readBytes == 10);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));
        demuxReader->Close();
    }

    SECTION("Create one reader after the other")
    {
        auto demuxReader = audioReaderFactory->CreateReader();
        uint8_t buffer[10];
        auto readBytes = demuxReader->Read(buffer, 10);
        REQUIRE(readBytes == 10);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));

        uint8_t buffer2[10];
        auto demuxReader2 = audioReaderFactory->CreateReader();
        auto readBytes2 = demuxReader2->Read(buffer2, 10);
        REQUIRE(readBytes2 == 10);
        REQUIRE(ValidateBufferSequence(buffer2, readBytes2));
        demuxReader->Close();
        demuxReader2->Close();
    }

    SECTION("Create one reader after the other and close them")
    {
        auto demuxReader = audioReaderFactory->CreateReader();
        uint8_t buffer[10];
        auto readBytes = demuxReader->Read(buffer, 10);
        REQUIRE(readBytes == 10);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));
        demuxReader->Close();
        demuxReader.reset();

        demuxReader = audioReaderFactory->CreateReader();
        readBytes = demuxReader->Read(buffer, 10);
        REQUIRE(readBytes == 10);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));
        demuxReader->Close();
    }

    SECTION("Exception gets propagated to 1 client")
    {
        auto demuxReader = audioReaderFactory->CreateReader();
        const int size = 3200 * 2;
        uint8_t buffer[size];
        auto readBytes = demuxReader->Read(buffer, size / 2);
        singletonMockAudioReader->ThrowNextRead();

        REQUIRE(readBytes == (size / 2));
        REQUIRE(ValidateBufferSequence(buffer, readBytes));
        REQUIRE_THROWS(demuxReader->Read(buffer, size));

        auto demuxReaderAfter = audioReaderFactory->CreateReader();
        REQUIRE_NOTHROW(readBytes = demuxReaderAfter->Read(buffer, size));
        REQUIRE(readBytes == size);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));

        demuxReader->Close();
        demuxReaderAfter->Close();
    }

    SECTION("Exception gets propagated to 2 clients")
    {
        auto demuxReader1 = audioReaderFactory->CreateReader();
        auto demuxReader2 = audioReaderFactory->CreateReader();
        const int size = 3200 * 2;
        uint8_t buffer[size];
        auto readBytes = demuxReader1->Read(buffer, size / 2);
        singletonMockAudioReader->ThrowNextRead();

        REQUIRE(readBytes == (size / 2));
        REQUIRE(ValidateBufferSequence(buffer, readBytes));
        REQUIRE_THROWS(demuxReader1->Read(buffer, size));
        REQUIRE_THROWS(demuxReader2->Read(buffer, size));

        auto demuxReaderAfter = audioReaderFactory->CreateReader();
        REQUIRE_NOTHROW(readBytes = demuxReaderAfter->Read(buffer, size));
        REQUIRE(readBytes == size);
        REQUIRE(ValidateBufferSequence(buffer, readBytes));

        demuxReader1->Close();
        demuxReader2->Close();
        demuxReaderAfter->Close();
    }


    return;
}

TEST_CASE("Demux audio stream adapter stress and multithreaed", "[core][demux adapter]")
{
    const uint32_t c_SequentialBufferSize = 500;

    std::shared_ptr<ISpxGenericSite> rootSite;
    // try to get the root site by reaching out to the resource manager first as that is the item to have. 
    INFO("Try to reach the root site from the site at hand by finding the resource manager on a given site")
    {
        auto objectFactory = SpxQueryService<ISpxObjectFactory>(SpxGetRootSite());
        rootSite = SpxQueryInterface<ISpxGenericSite>(objectFactory);
    }

    auto demuxAudioReaderAdapter = SpxCreateObjectWithSite<ISpxSingleToManyStreamReaderAdapter>("CSpxSingleToManyStreamReaderAdapter", rootSite);

    auto singletonMockAudioReader = std::make_shared<CSpxSequenceGeneratorAudioStreamReader>();
    singletonMockAudioReader->UseSequentialBufferedData(c_SequentialBufferSize);
    singletonMockAudioReader->SetRealTimeThrottlePercentage(100);

    auto singletonAudioReaderInit = SpxQueryInterface<ISpxObjectWithSite>(singletonMockAudioReader);
    singletonAudioReaderInit->SetSite(SpxGetRootSite());

    auto mockAudioReader = SpxQueryInterface<ISpxAudioStreamReader>(singletonMockAudioReader);
    demuxAudioReaderAdapter->SetSingletonReader(mockAudioReader);

    auto audioReaderFactory = SpxQueryInterface<ISpxAudioStreamReaderFactory>(demuxAudioReaderAdapter);

    SECTION("Create many readers and close them")
    {
        const int c_iterationCount = 100;
        for (int i = 0; i < c_iterationCount; i++)
        {
            auto demuxReader = audioReaderFactory->CreateReader();
            uint8_t buffer[10];
            auto readBytes = demuxReader->Read(buffer, 10);

            // Validate 1 in 10
            if ((i % 10) == 0)
            {
                REQUIRE(readBytes == 10);
                REQUIRE(ValidateBufferSequence(buffer, readBytes));
            }
            demuxReader->Close();
        }
    }


    SECTION("Parallel run with 4 readers created dynamically")
    {
        int iterationCountPerTask = 150;
        auto taskAction = [&iterationCountPerTask, &audioReaderFactory](string taskName)
        {
            // not particularely important just little larger than the pump buffer
            const int c_bufferSize = 3203;
            const int c_totalReadCount = 5;
            std::shared_ptr<ISpxAudioStreamReader> reader;
            for (int i = 0; i < iterationCountPerTask; i++)
            {
                int lastReadValue = -1;

                SPX_TRACE_INFO("Task:'%s' Iteration:'%d' ", taskName.c_str(), i);
                UNUSED(taskName); // unused in release builds
                // Reset previous byte validation here
                lastReadValue = -1;

                // One in 3 close and reuse. Otherwise destroy
                if (((i + 1) % 3 == 0) && (reader != nullptr))
                {
                    reader->Close();
                    auto readerInit = SpxQueryInterface<ISpxObjectInit>(reader);

                    readerInit->Init();
                }
                else
                {
                    if (reader != nullptr)
                    {
                        reader->Close();
                        reader.reset();
                    }
                    reader = audioReaderFactory->CreateReader();
                }

                ReadStreamAndValidateIsInSequence(reader, c_bufferSize, c_totalReadCount, &lastReadValue);
            }

            if (reader != nullptr)
            {
                reader->Close();
            }
        };

        packaged_task<void(string)> task1(taskAction);
        packaged_task<void(string)> task2(taskAction);
        packaged_task<void(string)> task3(taskAction);
        packaged_task<void(string)> task4(taskAction);

        // Fetch the associated future<> from packaged_task<>
        std::future<void> result1 = task1.get_future();
        std::future<void> result2 = task2.get_future();
        std::future<void> result3 = task3.get_future();
        std::future<void> result4 = task4.get_future();

        // Pass the packaged_task to thread to run asynchronously
        std::thread th1(std::move(task1), "task1");
        std::thread th2(std::move(task2), "task2");
        std::thread th3(std::move(task3), "task3");
        std::thread th4(std::move(task4), "task4");

        // Join the threads. Its blocking and returns when thread is finished.
        th1.join();
        th2.join();
        th3.join();
        th4.join();
    }

    return;
}
