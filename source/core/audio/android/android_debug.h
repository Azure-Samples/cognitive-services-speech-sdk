//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <speechapi_cxx.h>
#include <spxdebug.h>

#define LOGV(...) SPX_TRACE_VERBOSE(__VA_ARGS__)
#define LOGD(...) SPX_TRACE_VERBOSE(__VA_ARGS__)
#define LOGI(...) SPX_TRACE_INFO(__VA_ARGS__)
#define LOGW(...) SPX_TRACE_WARNING(__VA_ARGS__)
#define LOGE(...) SPX_TRACE_ERROR(__VA_ARGS__)
#define LOGF(...) SPX_TRACE_ERROR(__VA_ARGS__)
