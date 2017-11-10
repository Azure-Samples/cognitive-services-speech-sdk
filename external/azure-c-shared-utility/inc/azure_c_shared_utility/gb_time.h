// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef GB_TIME_H
#define GB_TIME_H

/*this file, if included instead of <stdio.h> has the following functionality:
1) if GB_TIME_INTERCEPT is defined then
    a) some of the time.h symbols shall be redefined, for example: time => gb_time
    b) all "code" using the time will actually (because of the preprocessor) call to gb_time
    c) gb_time shall blindly call into time, thus realizing a passthrough
    
    reason is: unittesting. time comes with the C Run Time and cannot be mocked (that is, in the global namespace cannot exist a function called time

2) if GB_TIME_INTERCEPT is not defined then
    a) it shall include <time.h> => no passthrough, just direct linking.
*/

#ifndef GB_TIME_INTERCEPT
#include <time.h>
#else

/*source level intercepting of function calls*/
#define time                    time_never_called_never_implemented_always_forgotten
#define localtime               localtime_never_called_never_implemented_always_forgotten
#define strftime                strftime_never_called_never_implemented_always_forgotten

#ifdef __cplusplus
#include <ctime.h>
extern "C"
{
#else
#include <time.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

#undef time
#define time gb_time
MOCKABLE_FUNCTION(, time_t, time, time_t *, timer);

#undef localtime
#define localtime gb_localtime
MOCKABLE_FUNCTION(, struct tm *, localtime, const time_t *, timer);

#undef strftime
#define strftime gb_strftime
MOCKABLE_FUNCTION(, size_t, strftime, char *, s, size_t, maxsize, const char *, format, const struct tm *, timeptr);


#ifdef __cplusplus
}
#endif

#endif /*GB_TIME_INTERCEPT*/

#endif /* GB_TIME_H */
