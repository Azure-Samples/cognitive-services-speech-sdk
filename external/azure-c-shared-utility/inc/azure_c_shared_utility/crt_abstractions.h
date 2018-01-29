// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CRT_ABSTRACTIONS_H
#define CRT_ABSTRACTIONS_H

#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER

#ifdef QUARKGALILEO
#define HAS_STDBOOL
#ifdef __cplusplus
typedef bool _Bool;
#else
/*galileo apparently has _Bool and bool as built in types*/
#endif
#endif

#ifndef _WIN32_WCE
#define HAS_STDBOOL
#ifdef __cplusplus
#include <cstdbool>
/*because C++ doesn't do anything about _Bool... */
#define _Bool bool
#else
#include <stdbool.h>
#endif
#else 
/* WINCE does not support bool as C datatype */
#define __bool_true_false_are_defined	1

#define HAS_STDBOOL

#define _Bool bool

#ifdef __cplusplus
#define _CSTDBOOL_
#else
typedef unsigned char bool;

#define false	0
#define true	1
#endif
#endif
#else
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
/*C99 compiler or C11*/
#define HAS_STDBOOL
#include <stdbool.h>
#endif
#endif
#endif

#ifndef HAS_STDBOOL
#ifdef __cplusplus
#define _Bool bool
#else
typedef unsigned char _Bool;
typedef unsigned char bool;
#define false 0
#define true 1
#endif
#endif


/* Codes_SRS_CRT_ABSTRACTIONS_99_001:[The module shall not redefine the secure functions implemented by Microsoft CRT.] */
/* Codes_SRS_CRT_ABSTRACTIONS_99_040 : [The module shall still compile when building on a Microsoft platform.] */
/* Codes_SRS_CRT_ABSTRACTIONS_99_002: [CRTAbstractions module shall expose the following API]*/
#ifdef _MSC_VER
#else
#include "inttypes.h"
#include <strings.h> // for strcasecmp

/* Adding definitions from errno.h & crtdefs.h */
#if !defined (_TRUNCATE)
#define _TRUNCATE ((size_t)-1)
#endif  /* !defined (_TRUNCATE) */

#if !defined STRUNCATE
#define STRUNCATE       80
#endif  /* !defined (STRUNCATE) */

typedef int errno_t;

extern int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
extern int strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
extern int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount);
extern int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ...);
#define _strcmpi strcasecmp
#define _strnicmp strncasecmp
#define _strdup strdup
#endif

MOCKABLE_FUNCTION(, int, mallocAndStrcpy_s, char**, destination, const char*, source);
MOCKABLE_FUNCTION(, int, unsignedIntToString, char*, destination, size_t, destinationSize, unsigned int, value);
MOCKABLE_FUNCTION(, int, size_tToString, char*, destination, size_t, destinationSize, size_t, value);
/*following logic shall define the ISNAN macro*/
/*if runing on Microsoft Visual C compiler, than ISNAN shall be _isnan*/
/*else if running on C99 or C11, ISNAN shall be isnan*/
/*else if running on C89 ... #error and inform user*/

#ifdef _MSC_VER
#define ISNAN _isnan
#else
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
/*C99 compiler or C11*/
#define ISNAN isnan
#else
#error update this file to contain the latest C standard.
#endif
#else
#ifdef __cplusplus
/*C++ defines isnan... in C11*/
#define ISNAN std::isnan
#else
#error unknown (or C89) compiler, provide ISNAN with the same meaning as isnan in C99 standard  
#endif

#endif
#endif

#ifdef _MSC_VER
#define INT64_PRINTF "%I64d"
#else
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
/*C99 compiler or C11*/
#define INT64_PRINTF "%" PRId64 ""
#else
#error update this file to contain the latest C standard.
#endif
#else
#ifdef __cplusplus 
#define INT64_PRINTF "%" PRId64 ""
#else
#error unknown (or C89) compiler, provide INT64_PRINTF with the same meaning as PRIdN in C99 standard
#endif
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* CRT_ABSTRACTIONS_H */
