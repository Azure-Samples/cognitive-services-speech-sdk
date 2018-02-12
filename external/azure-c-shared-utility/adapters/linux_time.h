// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LINUX_TIME_H
#define LINUX_TIME_H

#include <time.h>
#include <pthread.h>

#ifndef __MACH__
extern clockid_t time_basis;
#endif

extern void set_time_basis(void);
extern int get_time_ns(struct timespec* ts);
extern time_t get_time_s();

#define INVALID_TIME_VALUE      (time_t)(-1)


#define NANOSECONDS_IN_1_SECOND 1000000000L
#define MILLISECONDS_IN_1_SECOND 1000
#define NANOSECONDS_IN_1_MILLISECOND 1000000L

#endif

