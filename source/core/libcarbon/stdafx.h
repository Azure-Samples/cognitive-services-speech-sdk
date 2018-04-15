// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include "windows/targetver.h"
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include "spx_namespace.h"
#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) CARBON_IMPL_NAMESPACE()::ThrowWithCallstack(hr)

#include "speechapi_c.h"
#include "handle_table.h"
