//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// site_helpers.h: Implementation declarations/definitions for SpxSite* helper methods
//

#pragma once
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


template <class T>
inline std::shared_ptr<ISpxSite> SpxSiteFromThis(T* ptr)
{
    return SpxSharedPtrFromThis<ISpxSite>(ptr);
}


std::shared_ptr<ISpxSite> SpxGetRootSite();


} // CARBON_IMPL_NAMESPACE
