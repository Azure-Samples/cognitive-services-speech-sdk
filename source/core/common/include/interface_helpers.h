//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interface_helpers.h: Implementation declarations/definitions for Interface helper methods and macros
//

#pragma once
#include "spxcore_common.h"
#include "shared_ptr_helpers.h"
#include "string_utils.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


#ifdef SPX_CONFIG_TRACE_INTERFACE_MAP
#define SPX_DBG_TRACE_INTERFACE_MAP_BEGIN() SPX_DBG_TRACE_VERBOSE("QUERYINTERFACE: interface=%s ; looking in %s ...", interfaceName, __FUNCTION__)
#define SPX_DBG_TRACE_INTERFACE_MAP_FOUND_IT() SPX_DBG_TRACE_VERBOSE("QUERYINTERFACE: interface=%s ; looking in %s ... FOUND IT!!", interfaceName, __FUNCTION__)
#else
#define SPX_DBG_TRACE_INTERFACE_MAP_BEGIN()
#define SPX_DBG_TRACE_INTERFACE_MAP_FOUND_IT()
#endif

#define SPX_INTERFACE_MAP_BEGIN()                                                       \
    void* QueryInterface(const char* interfaceName) override                            \
    {                                                                                   \
        SPX_DBG_TRACE_INTERFACE_MAP_BEGIN();

#define SPX_INTERFACE_MAP_ENTRY(x)                                                      \
        if (PAL::stricmp(PAL::GetTypeName<x>().c_str(), interfaceName) == 0)            \
        {                                                                               \
            SPX_DBG_TRACE_INTERFACE_MAP_FOUND_IT();                                     \
            return static_cast<x*>(this);                                               \
        }

#define SPX_INTERFACE_MAP_ENTRY2(x, y)                                                  \
        if (PAL::stricmp(PAL::GetTypeName<x>().c_str(), interfaceName) == 0)            \
        {                                                                               \
            SPX_DBG_TRACE_INTERFACE_MAP_FOUND_IT();                                     \
            return static_cast<x*>(static_cast<y*>(this));                              \
        }

#define SPX_INTERFACE_MAP_END()                                                         \
        return nullptr;                                                                 \
    };


} } } } // Microsoft::CognitiveServices::Speech::Impl
