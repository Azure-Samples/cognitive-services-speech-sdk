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

#if 0
TEST_CASE("SPXTEST_ basics", "[api][cxx][testing]")
{
    SPXTEST_SECTION("Testing new SPXTEST_ MACROS w/ mock KWS")
    {
        UseMocks(true);

        auto recognizer = KeywordRecognizer::FromConfig();
        auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");

        SPXTEST_WHEN("a = true; b = false; SPXTEST_CHECK(a == b)")
        {
            bool a = true;
            bool b = false;
            SPXTEST_CHECK(a == b);
        }

        SPXTEST_WHEN("a = true; b = true; SPXTEST_CHECK_FALSE(a == b)")
        {
            bool a = true;
            bool b = true;
            SPXTEST_CHECK_FALSE(a == b);
        }

        SPXTEST_WHEN("a = true; b = false; SPXTEST_REQUIRE(a == b)")
        {
            bool a = true;
            bool b = false;
            SPXTEST_REQUIRE(a == b);
        }

        SPXTEST_WHEN("a = true; b = true; SPXTEST_REQUIRE_FALSE(a == b)")
        {
            bool a = true;
            bool b = true;
            SPXTEST_REQUIRE_FALSE(a == b);
        }

        SPXTEST_WHEN("Using SPXTEST_CHECK_THAT")
        {
            auto a = "test";
            auto b = "not test";
            SPXTEST_CHECK_THAT(a, Catch::Equals(b));
            SPXTEST_CHECK(a != b);
        }

        SPXTEST_WHEN("Using SPXTEST_REQUIRE_THAT")
        {
            auto a = "test";
            auto b = "not test";
            SPXTEST_REQUIRE_THAT(a, Catch::Equals(b));
            SPXTEST_CHECK(a != b);
        }
    }
}
#endif

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
                wait_for_future(std::move(resultFuture), WAIT_FOR_RECO_RESULT_TIME,
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
                SPXTEST_CHECK(resultCount == n);
                SPXTEST_CHECK(invalidResultCount == 0);
                SPXTEST_CHECK(timeoutCount == 0);
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
            wait_for_future(std::move(resultFuture), WAIT_FOR_RECO_RESULT_TIME,
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
                SPXTEST_CHECK(hasData);
                SPXTEST_CHECK(readData);
                SPXTEST_CHECK(detached);
                SPXTEST_CHECK(!timeout);
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
                wait_for_future(std::move(resultFuture), 1000ms, [](std::shared_ptr<KeywordRecognitionResult>) { return true; }, [&timeout]() { timeout += 1; });
            }
            THEN("Stopping should cause the future to be resolved.")
            {
                SPXTEST_CHECK(timeout == 0);
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
                wait_for_future(std::move(resultFuture), WAIT_FOR_RECO_RESULT_TIME, [](std::shared_ptr<KeywordRecognitionResult>) { return true; }, [&timeout]() { timeout += 1; });
                auto stopFuture = recognizer->StopRecognitionAsync();
                wait_for_future(std::move(stopFuture), 1000ms, []() { return true;  }, [&timeout]() { timeout += 1; });
            }
            THEN("The stop recognition task should resolve quickly.")
            {
                SPXTEST_CHECK(timeout == 0);
            }
        }
    }
}
