//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <inttypes.h>
#include <sstream>
#include <iomanip>
#include "time_utils.h"

using namespace std::chrono;

namespace PAL
{
    std::string GetTimeInString(const system_clock::time_point& t)
    {
        constexpr size_t length = 128;
        char buffer[length];
        time_t rawtime;
        struct tm timeinfo;

        rawtime = system_clock::to_time_t(t);
#ifdef _MSC_VER
        gmtime_s(&timeinfo, &rawtime);
#else
        gmtime_r(&rawtime, &timeinfo);
#endif

        size_t bytesWritten = strftime(buffer, length, "%FT%T.", &timeinfo);
        if (bytesWritten == 0)
        {
            return "";
        }

        uint64_t subSecondPeriods = t.time_since_epoch().count() * system_clock::period::num % system_clock::period::den;
        uint64_t ticks = subSecondPeriods * 10000000 * system_clock::period::num / system_clock::period::den;
        int ret = snprintf(buffer + bytesWritten, length - bytesWritten, "%07" PRIu64 "Z", ticks);
        if (ret < 0)
        {
            return "";
        }

        return buffer;
    }

    uint64_t GetTicks(const system_clock::duration& t)
    {
        constexpr uint64_t nanosecondsInTick = 100;
        nanoseconds durationInNanoseconds = t;
        return durationInNanoseconds.count() / nanosecondsInTick ;
    }

    bool TryParseUtcTimeString(const std::string& timeString, const char* format, system_clock::time_point& localTime)
    {
        // Attempt to parse the date time string
        std::tm utc_tm;

        std::istringstream ss(timeString);
        ss.imbue(std::locale("")); // force to C locale for consistent parsing
        ss >> std::get_time(&utc_tm, format);

        if (ss.fail())
        {
            return false;
        }

        std::time_t t;

#ifdef _MSC_VER
        t = _mkgmtime(&utc_tm);
#else
        t = timegm(&utc_tm);
#endif

        if (t == static_cast<time_t>(-1))
        {
            // failed to convert to a time_t
            return false;
        }

        localTime = system_clock::from_time_t(t);
        return true;
    }
}


