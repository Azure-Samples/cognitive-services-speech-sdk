//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// shared_ptr_helpers.h: Implementation declarations/definitions for SpxSharedPtr* helper methods
//

#pragma once
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


template <class T>
inline std::shared_ptr<T> SpxSharedPtrFromThis(T* ptr)
{
    return ptr != nullptr
        ? ptr->shared_from_this()
        : nullptr;
}


} // CARBON_IMPL_NAMESPACE
