// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

// Must be defined in the precompiled header so that all translation units
// that consume the PCH get the same macro definition (otherwise MSVC emits
// C4603 and treats getenv()/etc. as C4996 errors under /sdl).
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
#include "targetver.h"
#include <tchar.h>
#endif

#include <stdio.h>
#include <string.h>
#include <iostream>
