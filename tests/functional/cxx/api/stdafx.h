#pragma once

#include <exception>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <iostream>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_DBG_TRACE_ALL 1
#define SPX_CONFIG_TRACE_ALL 1
#else
#define SPX_CONFIG_TRACE_ALL 1
#endif

#include <speechapi_c_diagnostics.h>

extern void test_diagnostics_log_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...);

#if defined(DEBUG) || defined(_DEBUG)
#define __SPX_DO_TRACE_IMPL diagnostics_log_trace_message
#else
#define __SPX_DO_TRACE_IMPL test_diagnostics_log_trace_message
#endif

#include <speechapi_cxx.h>
#include <spxdebug.h>
