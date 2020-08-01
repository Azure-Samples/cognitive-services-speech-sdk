//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.h: Implementation declarations for *CreateModuleObject* methods
//

#pragma once

#include <future>
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

#include "spxcore_common.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


#ifdef BUILD_RNNT
SPX_EXTERN_C void* RNNT_CreateModuleObject(const char* className, const char* interfaceName);
#endif
#ifdef BUILD_UNIDEC
SPX_EXTERN_C void* Unidec_CreateModuleObject(const char* className, const char* interfaceName);
#endif


} } } } // Microsoft::CognitiveServices::Speech::Impl
