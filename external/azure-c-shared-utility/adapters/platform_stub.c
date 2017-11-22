// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/platform.h"

int platform_init(void)
{
    // seed the randomizer
#if defined(_MSC_VER)
    srand(GetTickCount());
#else
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);
#endif

	return 0;
}


void platform_deinit(void)
{
	return;
}
