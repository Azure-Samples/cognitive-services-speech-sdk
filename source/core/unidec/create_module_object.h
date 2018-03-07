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
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


SPX_EXTERN_C void* Unidec_CreateModuleObject(const char* className, const char* interfaceName);


} // CARBON_IMPL_NAMESPACE
