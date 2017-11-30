#pragma once

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include <speechapi_c.h>
#include <spxcore_common.h>

#include <handle_table.h>
#include <recognizer.h>
#include <recognizer_factory.h>

