//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <chrono>
#include <thread>

#include "test_utils.h"
#include "recognizer_utils.h"
#include "file_utils.h"

#include <spxerror.h>

using namespace std::chrono_literals;

template<typename T, typename OnSuccessFn, typename OnTimeoutFn, typename Rep, typename Period, typename std::enable_if_t<!std::is_void<T>::value, int> = 0>
void wait_for_future(
    std::future<T> future,
    std::chrono::duration<Rep, Period> timeout, OnSuccessFn success_cb, OnTimeoutFn timeout_cb)
{
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout)
    {
        timeout_cb();
        return;
    }
    success_cb(future.get());
}

template<typename T, typename OnSuccessFn, typename OnTimeoutFn, typename Rep, typename Period, typename std::enable_if_t<std::is_void<T>::value, int> = 0>
void wait_for_future(
    std::future<T> future,
    std::chrono::duration<Rep, Period> timeout, OnSuccessFn success_cb, OnTimeoutFn timeout_cb)
{
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout)
    {
        timeout_cb();
        return;
    }
    future.get();
    success_cb();
}

TEST_CASE("Keyword Recognizer basics", "[api][cxx][keyword_recognizer]")
{
    SPXTEST_GIVEN("Mocks for KWS and Microphone; Model.")
    {
        UseMocks(true);

        auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");

        SPXTEST_WHEN("We do RecognizeOnce in a loop")
        {
            auto recognizer = KeywordRecognizer::FromConfig();
            constexpr int n{ 3 };
            int i{ 0 };
            int resultCount{ 0 };
            int invalidResultCount{ 0 };
            int timeoutCount{ 0 };
            while (i++ < n)
            {
                auto resultFuture = recognizer->RecognizeOnceAsync(model);
                wait_for_future(std::move(resultFuture), 10s,
                    [&resultCount, &invalidResultCount](std::shared_ptr<KeywordRecognitionResult> result)
                    {
                        if (result->Reason == ResultReason::RecognizedKeyword)
                        {
                            resultCount += 1;
                        }
                        else
                        {
                            invalidResultCount += 1;
                        }
                    },
                    [&timeoutCount]()
                    {
                        timeoutCount += 1;
                    });
            }
            THEN("We should have only valid results")
            {
                CHECK(resultCount == n);
                CHECK(invalidResultCount == 0);
                CHECK(timeoutCount == 0);
            }
        }
        SPXTEST_WHEN("We do a keyword recognition")
        {
            auto recognizer = KeywordRecognizer::FromConfig();
            auto resultFuture = recognizer->RecognizeOnceAsync(model);
            bool hasData{ false };
            bool readData{ false };
            bool detached{ false };
            bool timeout{ false };
            wait_for_future(std::move(resultFuture), 10s,
                [&hasData, &readData, &detached](std::shared_ptr<KeywordRecognitionResult> result)
                {
                    auto stream = AudioDataStream::FromResult(result);
                    auto buffer = std::make_unique<uint8_t[]>(100);
                    std::this_thread::sleep_for(3s);
                    if (!stream->CanReadData(100))
                    {
                        return;
                    }
                    hasData = true;
                    stream->ReadData(buffer.get(), 100);
                    readData = true;
                    stream->DetachInput();
                    detached = true;
                },
                [&timeout]()
                {
                    timeout = true;
                });
            THEN("We should have been able to read the associated stream and detach the input.")
            {
                CHECK(hasData);
                CHECK(readData);
                CHECK(detached);
                CHECK(!timeout);
            }
        }

        SPXTEST_WHEN("Start and Stop keyword recognition")
        {
            auto recognizer = KeywordRecognizer::FromConfig();
            int i{ 0 };
            int timeout{ 0 };
            while(i++ < 3)
            {
                auto resultFuture = recognizer->RecognizeOnceAsync(model);
                std::this_thread::sleep_for(2s);
                recognizer->StopRecognitionAsync().wait();
                wait_for_future(std::move(resultFuture), 500ms, [](std::shared_ptr<KeywordRecognitionResult>) { return true; }, [&timeout]() { timeout += 1; });
            }
            THEN("Stopping should cause the future to be resolved.")
            {
                CHECK(timeout == 0);
            }
        }

        SPXTEST_WHEN("Stop keyword recognition after keyword was detected")
        {
            auto recognizer = KeywordRecognizer::FromConfig();
            int i{ 0 };
            int timeout{ 0 };
            while(i++ < 3)
            {
                auto resultFuture = recognizer->RecognizeOnceAsync(model);
                wait_for_future(std::move(resultFuture), 10s, [](std::shared_ptr<KeywordRecognitionResult>) { return true; }, [&timeout]() { timeout += 1; });
                auto stopFuture = recognizer->StopRecognitionAsync();
                wait_for_future(std::move(stopFuture), 500s, []() { return true;  }, [&timeout]() { timeout += 1; });
            }
            THEN("The stop recognition task should resolve quickly.")
            {
                CHECK(timeout == 0);
            }
        }

    }
}
