// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TIMEEX_H
#define TIMEEX_H
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TimeInstance
{
    unsigned int hour;
    unsigned int minutes;
    unsigned int weekday;
} TimeInstance;

const TimeInstance timeNow();
int secondsRemainingToMinute();

#define cortana_gettickcount    telemetry_gettime

#ifdef __cplusplus
}
#endif

#endif /* TIMEEX_H */
