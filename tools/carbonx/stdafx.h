#pragma once

#ifdef _MSC_VER
#include "windows/targetver.h"
#endif

#include <exception>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <iostream>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include <speechapi_cxx.h>

#include <spxdebug.h>

#include "carbonx_pal.h"
