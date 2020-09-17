//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <factory_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C void* Core2_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        // There are currently no classes directly defined in core2 dynamic library
        // All classes are defined via linked static libraries (e.g. Session2, and others, see below)
    SPX_FACTORY_MAP_END();
}

SPX_EXTERN_C void* Session2_CreateModuleObject(const char* className, const char* interfaceName);

SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY_FUNC(Core2_CreateModuleObject);
        SPX_FACTORY_MAP_ENTRY_FUNC(Session2_CreateModuleObject);
    SPX_FACTORY_MAP_END();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
