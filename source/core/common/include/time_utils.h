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
    uint64_t GetTicks(const std::chrono::system_clock::duration& t);

    /// <summary>
    /// Parses a UTC time from a string using the specified format. The time set will be local.
    /// </summary>
    /// <param name="timeString">The time string to parse</param>
    /// <param name="format">The format string. Please refer to the C++ 'get_time' documentation for 
    /// valid values</param>
    /// <param name="time">The parsed local time to set</param>
    /// <returns>True if the string was parsed successfully, false otherwise</returns>
    bool TryParseUtcTimeString(const std::string& timeString, const char* format, std::chrono::system_clock::time_point& localTime);
}
