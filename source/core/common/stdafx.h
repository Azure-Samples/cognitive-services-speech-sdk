#pragma once

#include <future>
#include <memory>
#include <map>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include "spxcore_common.h"
