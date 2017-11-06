#pragma once

#include "targetver.h"

#include <exception>
#include <stdio.h>
#include <tchar.h>
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

#include <spxdebug.h>

#include <speechapi_cxx.h>

#include "carbonx_pal.h"
