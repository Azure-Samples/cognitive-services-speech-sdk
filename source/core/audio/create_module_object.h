//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.h: Implementation declarations for *CreateModuleObject* methods
//

#pragma once
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


SPX_EXTERN_C void* AudioLib_CreateModuleObject(const char* className, const char* interfaceName);


} // CARBON_IMPL_NAMESPACE
