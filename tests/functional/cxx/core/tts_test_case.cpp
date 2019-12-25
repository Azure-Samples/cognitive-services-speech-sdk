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
#include "authenticator.h"
#include "guid_utils.h"

using namespace std;
using namespace std::chrono;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;


TEST_CASE("Authenticator Timer: Start/Stop", "[tts]")
{
    auto timer = make_shared<Timer>();
    REQUIRE_NOTHROW(timer->Start(1000, 1000, [](){return true;}));
    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Start twice fails", "[tts]")
{
    auto timer = make_shared<Timer>();
    REQUIRE_NOTHROW(timer->Start(1000, 1000, [](){return true;}));
    REQUIRE_THROWS_WITH(timer->Start(1000, 1000, [](){return true;}), Catch::Contains("INVALID_STATE"));
}

TEST_CASE("Authenticator Timer: Expire not initialized", "[tts]")
{
    auto timer = make_shared<Timer>();
    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Wait for valid for not started timer", "[tts]")
{
    auto timer = make_shared<Timer>();
    REQUIRE(!timer->WaitUntilValid(0, 1000));
}

TEST_CASE("Authenticator Timer: Task execution succeeds", "[tts]")
{
    auto timer = make_shared<Timer>();

    auto counter = 0;
    auto successTask = [&counter]() { counter++; return true; };

    REQUIRE_NOTHROW(timer->Start(200, 20 * 1000, successTask));

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    REQUIRE(counter == 2);

    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Task execution fails", "[tts]")
{
    auto timer = make_shared<Timer>();

    auto counter = 0;
    auto failTask = [&counter]() { counter++; return false; };

    REQUIRE_NOTHROW(timer->Start(20 * 1000, 200, failTask));

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    REQUIRE(counter == 2);

    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Force renew", "[tts]")
{
    auto timer = make_shared<Timer>();

    auto counter = 0;
    auto successTask = [&counter]() { counter++; return true; };

    REQUIRE_NOTHROW(timer->Start(20 * 1000, 20 * 1000, successTask));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer->ForceRenew();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(counter == 2);

    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Wait for valid", "[tts]")
{
    auto timer = make_shared<Timer>();

    auto counter = 0;
    auto successTask = [&counter]() { counter++; return true; };

    REQUIRE_NOTHROW(timer->Start(20 * 1000, 20 * 1000, successTask));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(timer->WaitUntilValid(0, 200));

    REQUIRE_NOTHROW(timer->Expire());
}

TEST_CASE("Authenticator Timer: Wait for valid timeout", "[tts]")
{
    auto timer = make_shared<Timer>();

    auto counter = 0;
    auto successTask500Ms = [&counter]() {
        if (counter)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        counter++; 
        return true; 
        };

    REQUIRE_NOTHROW(timer->Start(20 * 1000, 20 * 1000, successTask500Ms));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer->ForceRenew();
    REQUIRE(!timer->WaitUntilValid(0, 200));
    REQUIRE(counter == 1);

    REQUIRE_NOTHROW(timer->Expire());
}
