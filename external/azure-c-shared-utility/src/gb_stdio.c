// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*depending if the symbol GB_STDIO_INTERCEPT is defined, this file does the following

    a) if GB_STDIO_INTERCEPT is NOT defined, then the file shall be empty (almost:)
    b) if GB_STDIO_INTERCEPT is defined, then the file shall call to the 'real' stdio.h functions from their gb_* synonyms*/

#ifdef _MSC_VER
static const int avoid_a_warning_C4206 = 0; /* warning C4206: nonstandard extension used: translation unit is empty*/
#endif

#ifdef GB_STDIO_INTERCEPT

#ifdef __cplusplus
#include <cstdio>
#include <cstdarg>
#else
#include <stdio.h>
#include <stdarg.h>
#endif

#include "azure_c_shared_utility/gb_stdio.h"

/*this is fopen*/
FILE *gb_fopen(const char * filename, const char * mode)
{
    return fopen(filename, mode);
}

int gb_fclose(FILE *stream)
{
    return fclose(stream);
}

int gb_fseek(FILE *stream, long int offset, int whence)
{
    return fseek(stream, offset, whence);
}

long int gb_ftell(FILE *stream)
{
    return ftell(stream);
}

int fprintf(FILE * stream, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}

#endif
