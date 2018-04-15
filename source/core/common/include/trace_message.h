//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// trace_message.h: SpxTraceMessage() implementation declaration
//

#pragma once

#include "azure_c_shared_utility_xlogging_wrapper.h"


void SpxTraceMessage(int level, const char* pszTitle, const char* pszFormat, ...);
void SpxConsoleLogger_Log(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...);
