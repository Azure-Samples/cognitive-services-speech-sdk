//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// trace_message.h: SpxTraceMessage() implementation declaration
//

#pragma once

#include <stdarg.h>
#include "azure_c_shared_utility_xlogging_wrapper.h"

#ifdef __clang__
#define CLANG_ATTRIB_FORMAT_PRINTF(x1, x2) __attribute__ (( format( printf, x1, x2 ) ))
#else
#define CLANG_ATTRIB_FORMAT_PRINTF(x1, x2)
#endif

SPX_EXTERN_C {

extern void SpxTraceMessage1(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...) CLANG_ATTRIB_FORMAT_PRINTF(5, 6);
extern void SpxTraceMessage2(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr);

extern void _xlogging_log_function_spx_trace_message_wrapper(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...) CLANG_ATTRIB_FORMAT_PRINTF(6, 7);

}
