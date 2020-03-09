//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <chrono>
#include <thread>
#include <random>
#include <string>

#include "test_utils.h"
#include "thread_service.h"
#include "guid_utils.h"

using namespace std;
using namespace std::chrono;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

const vector<ISpxThreadService::Affinity> g_affinities =
{
    ISpxThreadService::Affinity::User,
    ISpxThreadService::Affinity::Background
};

TEST_CASE("ThreadService: Start/Stop", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());
    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Start twice fails", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());
    REQUIRE_THROWS_WITH(service->Init(), Catch::Contains("INVALID_STATE"));
}

TEST_CASE("ThreadService: Term not initialized", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Term twice, term is idempotent", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Term());
    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Execute a task on uninitialized service fails", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    int counter = 0;
    packaged_task<void()> task([&counter]()
    {
        counter++;
    });

    REQUIRE_THROWS_WITH(service->ExecuteAsync(move(task)), Catch::Contains("INVALID_STATE"));
}

TEST_CASE("ThreadService: Execute tasks on background/user threads", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();

    REQUIRE_NOTHROW(service->Init());

    for (auto affinity : g_affinities)
    {
        vector<thread::id> ids;
        const int NumIterations = 10;
        vector<future<void>> futures;
        for (int i = 0; i < NumIterations; ++i)
        {
            packaged_task<void()> task([&ids]() { ids.push_back(this_thread::get_id()); });
            futures.emplace_back(task.get_future());
            REQUIRE_NOTHROW(service->ExecuteAsync(move(task), affinity));
        }

        for (auto& f : futures)
            f.get();

        REQUIRE(ids.size() == NumIterations);
        auto expected = ids.front();
        REQUIRE(this_thread::get_id() != expected);
        for (auto& id : ids)
        {
            REQUIRE(id == expected);
        }
    }

    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: User and background threads are different", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    vector<thread::id> ids;

    REQUIRE_NOTHROW(service->Init());

    mutex locker;

    vector<future<void>> futures;
    packaged_task<void()> taskUser([&ids, &locker]()
    {
        unique_lock<mutex> lock(locker);
        ids.push_back(this_thread::get_id());
    });
    futures.emplace_back(taskUser.get_future());

    packaged_task<void()> taskBackground([&ids, &locker]()
    {
        unique_lock<mutex> lock(locker);
        ids.push_back(this_thread::get_id());
    });
    futures.emplace_back(taskBackground.get_future());

    REQUIRE_NOTHROW(service->ExecuteAsync(move(taskUser), ISpxThreadService::Affinity::User));
    REQUIRE_NOTHROW(service->ExecuteAsync(move(taskBackground), ISpxThreadService::Affinity::Background));

    for (auto& f : futures)
        f.get();

    REQUIRE_NOTHROW(service->Term());

    REQUIRE(ids.size() == 2);
    REQUIRE(ids[0] != ids[1]);
    REQUIRE(ids[0] != this_thread::get_id());
    REQUIRE(ids[1] != this_thread::get_id());
}

TEST_CASE("ThreadService: Throw in a task and next task succeeds", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();

    REQUIRE_NOTHROW(service->Init());

    for (auto affinity : g_affinities)
    {
        packaged_task<void()> taskBad([]()
        {
            throw runtime_error("Bad happened");
        });
        future<void> futureBad = taskBad.get_future();

        REQUIRE_NOTHROW(service->ExecuteAsync(move(taskBad), affinity));
        REQUIRE_THROWS_WITH(futureBad.get(), Catch::Contains("Bad happened"));

        int counter = 0;
        packaged_task<void()> taskGood([&counter]() { ++counter; });
        future<void> futureGood = taskGood.get_future();

        REQUIRE_NOTHROW(service->ExecuteAsync(move(taskGood), affinity));
        REQUIRE_NOTHROW(futureGood.get());
        REQUIRE(counter == 1);
    }

    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Schedule several timers.")
{
    auto service = make_shared<CSpxThreadService>();

    REQUIRE_NOTHROW(service->Init());

    int counter = 0;
    milliseconds duration500, duration1500, duration2500;
    auto last = system_clock::now();

    packaged_task<void()> taskAfter500ms([&]()
    {
        duration500 = duration_cast<milliseconds>(system_clock::now() - last);
        counter++;
    });

    packaged_task<void()> taskAfter1500ms([&]()
    {
        duration1500 = duration_cast<milliseconds>(system_clock::now() - last);
        counter++;
    });

    packaged_task<void()> taskAfter2500ms([&]()
    {
        duration2500 = duration_cast<milliseconds>(system_clock::now() - last);
        counter++;
    });

    last = system_clock::now();
    REQUIRE_NOTHROW(service->ExecuteAsync(move(taskAfter2500ms), milliseconds(2500)));
    REQUIRE_NOTHROW(service->ExecuteAsync(move(taskAfter1500ms), milliseconds(1500)));
    REQUIRE_NOTHROW(service->ExecuteAsync(move(taskAfter500ms), milliseconds(500)));

    this_thread::sleep_for(seconds(5));
    REQUIRE(counter == 3);

    SPX_TRACE_INFO("Expected 500 ms task took : %s", std::to_string(duration500.count()).c_str());
    SPX_TRACE_INFO("Expected 1500 ms task took : %s", to_string(duration1500.count()).c_str());
    SPX_TRACE_INFO("Expected 2500 ms task took : %s", to_string(duration2500.count()).c_str());

    REQUIRE(duration500 < duration1500);
    REQUIRE(duration1500 < duration2500);

    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Shutdown with immediate tasks and timers", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();

    REQUIRE_NOTHROW(service->Init());

    int counter = 0;
    const int NumIterations = 1000;

    // Schedule a timer task, shutdown can still happen if there are some timer tasks.
    for (int i = 0; i < NumIterations / 2; ++i)
    {
        packaged_task<void()> timer([&]()
        {
            this_thread::sleep_for(milliseconds(5));
            counter++;
        });

        REQUIRE_NOTHROW(service->ExecuteAsync(move(timer), milliseconds(100)));
    }

    // Schedule immediate tasks.
    for (int i = 0; i < NumIterations/2; ++i)
    {
        packaged_task<void()> task([&]() { this_thread::sleep_for(milliseconds(5)); counter++; });
        REQUIRE_NOTHROW(service->ExecuteAsync(move(task)));
    }

    REQUIRE_NOTHROW(service->Term());
    REQUIRE(counter < NumIterations);

    int counterOld = counter;
    this_thread::sleep_for(seconds(3));
    REQUIRE(counterOld == counter);
}

TEST_CASE("ThreadService: Shutdown on a background thread fails", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());
    packaged_task<void()> task([&]()
    {
        REQUIRE_THROWS_WITH(service->Term(), Catch::Contains("ABORT"));
    });

    auto future = task.get_future();
    REQUIRE_NOTHROW(service->ExecuteAsync(move(task)));
    REQUIRE_NOTHROW(future.get());
    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Shutdown on a user thread succeeds", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());
    packaged_task<void()> task([&]()
    {
        REQUIRE_NOTHROW(service->Term());
    });

    auto future = task.get_future();
    REQUIRE_NOTHROW(service->ExecuteAsync(move(task), ISpxThreadService::Affinity::User));
    REQUIRE_NOTHROW(future.get());
}

TEST_CASE("ThreadService: Synchronous execution", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());
    int counter = 0;
    packaged_task<void()> task([&]()
    {
        counter++;
    });

    REQUIRE_NOTHROW(service->ExecuteSync(std::move(task)));
    REQUIRE(counter == 1);
    REQUIRE_NOTHROW(service->Term());
}

TEST_CASE("ThreadService: Async with promise", "[sr]")
{
    auto service = make_shared<CSpxThreadService>();
    REQUIRE_NOTHROW(service->Init());

    int counter = 0;
    packaged_task<void()> task1([&]()
    {
        this_thread::sleep_for(5s);
        counter++;
    });
    auto task1Future = task1.get_future();

    packaged_task<void()> task2([&]()
    {
        counter++;
    });

    std::promise<bool> task1Finished;
    auto task1FinishedFuture = task1Finished.get_future();
    REQUIRE_NOTHROW(service->ExecuteAsync(std::move(task1), ISpxThreadService::Affinity::Background,
        std::move(task1Finished)));

    std::promise<bool> task2Finished;
    auto task2FinishedFuture = task2Finished.get_future();
    REQUIRE_NOTHROW(service->ExecuteAsync(std::move(task2), ISpxThreadService::Affinity::Background,
        std::move(task2Finished)));
    this_thread::sleep_for(2s);
    REQUIRE_NOTHROW(service->Term());

    REQUIRE_NOTHROW(task1Future.get());
    REQUIRE(task1FinishedFuture.get() == true);
    REQUIRE(task2FinishedFuture.get() == false);
    REQUIRE(counter == 1);
}
