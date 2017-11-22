// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file agenttime.h
*	@brief Function prototypes for time related functions.
*
*	@details These functions are implemented with C standard functions,
*	and therefore they are platform independent. But then a platform
*	can replace these functions with its own implementation as necessary.
*/

#ifndef AGENTTIME_H
#define AGENTTIME_H

#include <time.h>
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Get current calendar time.
*
*	@details This function provides the same functionality as the
*	standard C @c time() function.
*/
MOCKABLE_FUNCTION(, time_t, get_time, time_t*, currentTime);

/** @brief Get UTC in @c tm struct.
*
*	@details This function provides the same functionality as the
*	standard C @c gmtime() function.
*/
MOCKABLE_FUNCTION(, struct tm*, get_gmtime, time_t*, currentTime);

/** @brief Gets a C-string representation of the given time.
*
*	@details This function provides the same functionality as the
*	standard C @c ctime() function.
*/
MOCKABLE_FUNCTION(, char*, get_ctime, time_t*, timeToGet);

/** @brief Gets the difference in seconds between @c stopTime and
*	@c startTime.
*
*	@details This function provides the same functionality as the
*	standard C @c difftime() function.
*/
MOCKABLE_FUNCTION(, double, get_difftime, time_t, stopTime, time_t, startTime);

#ifdef __cplusplus
}
#endif

#endif  // AGENTTIME_H
