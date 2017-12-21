#pragma once

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include <speechapi_c.h>
#include <spxcore_common.h>
#include <trace_message.h>

#include <handle_table.h>
#include <recognizer.h>
#include <recognizer_factory.h>

#include "try_catch_helpers.h"

