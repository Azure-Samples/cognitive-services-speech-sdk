//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iomanip>
#include <sstream>
#include "caption_helper.h"

int CompareTimestamps(Timestamp t1, Timestamp t2)
{
    auto msT1 = MillisecondsFromTimestamp(t1);
    auto msT2 = MillisecondsFromTimestamp(t2);
    if (msT1 > msT2)
    {
        return 1;
    }
    else if (msT1 < msT2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

uint64_t MillisecondsFromTimestamp(Timestamp ts)
{
    return ts.Hours * 3600000 + ts.Minutes * 60000 + ts.Seconds * 1000 + ts.Milliseconds;
}


std::string StringFromTimestamp(Timestamp ts, bool srt)
{
    std::ostringstream retval;
    retval << std::setfill('0') << std::setw(2) << ts.Hours << ":"
        << std::setfill('0') << std::setw(2) << ts.Minutes << ":"
        << std::setfill('0') << std::setw(2) << ts.Seconds;
    if (srt)
    {
        // SRT format requires ',' as decimal separator rather than '.'.
        retval << ",";
    }
    else
    {
        retval << ".";
    }
    retval << std::setfill('0') << std::setw(3) << ts.Milliseconds;
    return retval.str ();        
}

Timestamp TimestampFromMilliseconds(uint64_t milliseconds)
{
    const int seconds = milliseconds / 1000;
    const int minutes = seconds / 60;
    const int hours = minutes / 60;
    return Timestamp(hours, minutes % 60, seconds % 60, milliseconds % 1000);
}

Timestamp TimestampFromTicks(uint64_t ticks)
{
    const float ticksPerMillisecond = 10000.0;
    const int milliseconds = ticks / ticksPerMillisecond;
    const int seconds = milliseconds / 1000;
    const int minutes = seconds / 60;
    const int hours = minutes / 60;
    return Timestamp(hours, minutes % 60, seconds % 60, milliseconds % 1000);
}

Timestamp TimestampPlusMilliseconds(Timestamp ts, uint32_t milliseconds)
{
    return TimestampFromMilliseconds(MillisecondsFromTimestamp(ts) + milliseconds);
}