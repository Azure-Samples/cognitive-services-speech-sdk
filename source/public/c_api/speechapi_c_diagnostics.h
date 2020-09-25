//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <speechapi_c_common.h>

SPXAPI diagnostics_log_start_logging(SPXPROPERTYBAGHANDLE hpropbag, void* reserved);
SPXAPI diagnostics_log_apply_properties(SPXPROPERTYBAGHANDLE hpropbag, void* reserved);
SPXAPI diagnostics_log_stop_logging();

SPXAPI_(void) diagnostics_log_format_message(char *buffer, size_t bufferSize, int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr);

SPXAPI_(void) diagnostics_log_trace_string(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* psz);
SPXAPI_(void) diagnostics_log_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...);
SPXAPI_(void) diagnostics_log_trace_message2(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr);

typedef void(*DIAGNOSTICS_CALLBACK_FUNC)(const char *logLine);
SPXAPI diagnostics_logmessage_set_callback(DIAGNOSTICS_CALLBACK_FUNC callback);

SPXAPI_(void) diagnostics_log_memory_start_logging();
SPXAPI_(void) diagnostics_log_memory_stop_logging();

SPXAPI_(size_t) diagnostics_log_memory_get_line_num_oldest();
SPXAPI_(size_t) diagnostics_log_memory_get_line_num_newest();
SPXAPI__(const char*) diagnostics_log_memory_get_line(size_t lineNum);

SPXAPI_(void) diagnostics_log_memory_dump_to_stderr();
SPXAPI_(void) diagnostics_log_memory_dump(const char* filename, const char* linePrefix, bool emitToStdOut, bool emitToStdErr);
