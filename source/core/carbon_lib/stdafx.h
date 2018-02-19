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

#include <map>

#include <speechapi_c.h>
#include "spxcore_common.h"
#include "trace_message.h"
#include "handle_table.h"
