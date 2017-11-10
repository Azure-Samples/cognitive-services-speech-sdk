// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "targetver.h"

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "tchar.h"
#include "sal.h"
#include "basetsd.h"
#define SAL_Acquires_lock_(...) _Acquires_lock_(__VA_ARGS__)
#define SAL_Releases_lock_(...) _Releases_lock_(__VA_ARGS__)
typedef CRITICAL_SECTION MICROMOCK_CRITICAL_SECTION;
#define MicroMockEnterCriticalSection(...) EnterCriticalSection(__VA_ARGS__)
#define MicroMockLeaveCriticalSection(...) LeaveCriticalSection(__VA_ARGS__)

#define MicroMockInitializeCriticalSection(...) InitializeCriticalSection(__VA_ARGS__)

#define MicroMockDeleteCriticalSection(...) DeleteCriticalSection(__VA_ARGS__)
#else
typedef unsigned char UINT8;
typedef char TCHAR;
#define _T(A)   A
typedef int MICROMOCK_CRITICAL_SECTION;
#define MicroMockEnterCriticalSection(...)
#define MicroMockLeaveCriticalSection(...)
#define MicroMockInitializeCriticalSection(...)
#define MicroMockDeleteCriticalSection(...)
#define SAL_Acquires_lock_(...)
#define SAL_Releases_lock_(...)
#endif

#include "string"
#include "sstream"
#include "map"
#include "algorithm"
#include "iomanip"
#include "vector"
#include "set"

#include "sal.h"

#ifdef _MSC_VER
/*'function' : unreferenced local function has been removed*/
#pragma warning( disable: 4505 ) 

/*unreferenced inline function has been removed*/
#pragma warning( disable: 4514)
#endif

#define COUNT_OF(a)     (sizeof(a) / sizeof((a)[0]))

namespace std
{
    typedef std::basic_string<TCHAR> tstring;
    typedef std::basic_ostringstream<TCHAR> tostringstream;
}
