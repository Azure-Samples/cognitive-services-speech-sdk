//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// trace_message.h: SpxTraceMessage() implementation declaration
//

#pragma once

#include "azure_c_shared_utility_xlogging_wrapper.h"


void SpxTraceMessage(int level, const char* pszTitle, bool enableDebugOutput, const char* fileName, const int lineNumber, const char* pszFormat, ...)
#ifdef __clang__
    __attribute__ (( format( printf, 6, 7 ) ))
#endif
    ;
void SpxConsoleLogger_Log(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...);
