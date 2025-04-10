#pragma once

// If the project uses the Speech SDK and its API headers then the following
// flag can be used to enable traces in the standalone intent recognizer code.
//#define USE_SPX_TRACE_LOGGING

#ifdef USE_SPX_TRACE_LOGGING

#ifdef _DEBUG
#define SPX_CONFIG_DBG_TRACE_ALL 1
#define SPX_CONFIG_TRACE_ALL 1
#else
#define SPX_CONFIG_TRACE_ALL 1
#endif

#include <spxdebug.h>

#else

#ifdef SPX_DBG_TRACE_FUNCTION
#undef SPX_DBG_TRACE_FUNCTION
#endif
#define SPX_DBG_TRACE_FUNCTION(...)

#ifdef SPX_DBG_TRACE_VERBOSE
#undef SPX_DBG_TRACE_VERBOSE
#endif
#define SPX_DBG_TRACE_VERBOSE(...)

#ifdef SPX_TRACE_ERROR
#undef SPX_TRACE_ERROR
#endif
#define SPX_TRACE_ERROR(...)

#ifdef UNUSED
#undef UNUSED
#endif
#define UNUSED(x) (void)(x)

#endif
