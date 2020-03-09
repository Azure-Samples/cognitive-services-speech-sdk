//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <memory>
#include <map>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_DBG_TRACE_ALL 1
#define SPX_CONFIG_TRACE_ALL 1
#else
#define SPX_CONFIG_TRACE_ALL 1
#endif

#include "speechapi_c_diagnostics.h"
#define __SPX_DO_TRACE_IMPL diagnostics_log_trace_message

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(param)   (void)(param)
#endif
