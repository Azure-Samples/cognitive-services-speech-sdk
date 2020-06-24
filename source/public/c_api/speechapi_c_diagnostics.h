//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <stdarg.h>
#include <speechapi_c_common.h>

SPXAPI diagnostics_log_start_logging(SPXPROPERTYBAGHANDLE hpropbag, void* reserved);
SPXAPI diagnostics_log_apply_properties(SPXPROPERTYBAGHANDLE hpropbag, void* reserved);
SPXAPI diagnostics_log_stop_logging();

SPXAPI_(void) diagnostics_log_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...);
SPXAPI_(void) diagnostics_log_trace_message2(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr);
