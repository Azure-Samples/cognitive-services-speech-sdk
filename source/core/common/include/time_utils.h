//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <string>
#include <chrono>

namespace PAL
{
    std::string GetTimeInString(const std::chrono::system_clock::time_point& t);
    std::pair<uint64_t, uint64_t> GetTimeInSecondsAndTicks(const std::chrono::system_clock::duration& t);
}
