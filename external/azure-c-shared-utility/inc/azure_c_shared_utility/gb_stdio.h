// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef GB_STDIO_H
#define GB_STDIO_H

/*this file, if included instead of <stdio.h> has the following functionality:
1) if GB_STDIO_INTERCEPT is defined then
    a) some of the stdio.h symbols shall be redefined, for example: fopen => gb_fopen
    b) all "code" using the fopen will actually (because of the preprocessor) call to gb_fopen
    c) gb_fopen shall blindly call into fopen, thus realizing a passthrough
    
    reason is: unittesting. fopen is comes with the C Run Time and cannot be mocked (that is, in the global namespace cannot exist a function called fopen

2) if GB_STDIO_INTERCEPT is not defined then
    a) it shall include <stdio.h> => no passthrough, just direct linking.
*/

#ifndef GB_STDIO_INTERCEPT
#include <stdio.h>
#else

/*source level intercepting of function calls*/
#define fopen           fopen_never_called_never_implemented_always_forgotten
#define fclose          fclose_never_called_never_implemented_always_forgotten
#define fseek           fseek_never_called_never_implemented_always_forgotten
#define ftell           ftell_never_called_never_implemented_always_forgotten
#define fprintf         fprintf_never_called_never_implemented_always_forgotten

#include "azure_c_shared_utility/umock_c_prod.h"


#ifdef __cplusplus
#include <cstdio.h>
extern "C"
{
#else
#include <stdio.h>
#endif

#undef fopen
#define fopen gb_fopen
MOCKABLE_FUNCTION(, FILE*, gb_fopen, const char*, filename, const char*, mode);


#undef fclose
#define fclose gb_fclose
MOCKABLE_FUNCTION(, int, fclose, FILE *, stream);

#undef fseek
#define fseek gb_fseek
MOCKABLE_FUNCTION(, int, fseek, FILE *,stream, long int, offset, int, whence);

#undef ftell
#define ftell gb_ftell
MOCKABLE_FUNCTION(, long int, ftell, FILE *, stream);

#undef fprintf
#define fprintf gb_fprintf
extern int fprintf(FILE * stream, const char * format, ...);


#ifdef __cplusplus
}
#endif

#endif /*GB_STDIO_INTERCEPT*/

#endif /* GB_STDIO_H */
