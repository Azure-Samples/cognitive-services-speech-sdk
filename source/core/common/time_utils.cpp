//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <inttypes.h>
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
}


