// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/xlogging.h"
#include "linux_time.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


#ifndef __MACH__
clockid_t time_basis = -1;
#endif

void set_time_basis(void)
{
// The time basis depends on what clock is available. Prefer CLOCK_MONOTONIC,
// then CLOCK_REALTIME, otherwise query the default pthread_condattr_t value 
// and use that. Note the time basis stuff requires _POSIX_TIMERS [TMR] at a
// minimum; querying pthread_condattr_t requires _POSIX_CLOCK_SELECTION [CS].
// OSX has neither so we use a platform-specific clock.
#ifndef __MACH__
#if defined(CLOCK_MONOTONIC)
    time_basis = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
    time_basis = CLOCK_REALTIME;
#else
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_getclock(&cattr, &time_basis);
    pthread_condattr_destroy(&cattr);
#endif
#endif
}

int get_time_ns(struct timespec* ts)
{
    int err;

#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    err = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    if (!err)
    {
        err = clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);

        if (!err)
        {
            ts->tv_sec = mts.tv_sec;
            ts->tv_nsec = mts.tv_nsec;
        } 
    }
#else
    err = clock_gettime(time_basis, ts);
#endif
    return err;
}

time_t get_time_s()
{   
    struct timespec ts;
    if (get_time_ns(&ts) != 0)
    {
        LogError("Failed to get the current time");
        return INVALID_TIME_VALUE;
    }

    return (time_t)ts.tv_sec;
}

