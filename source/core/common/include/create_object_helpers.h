//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_object_helpers.h: Implementation declarations/definitions for SpxCreateObject* helper methods
//

#pragma once
#include "service_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


template <class I>
inline std::shared_ptr<I> SpxCreateObjectWithSite(const char* className, ISpxSite* site)
{
    // convert the argument to a shared pointer to the base site interafce
    auto sharedSitePtr = site->shared_from_this();

    // create the object with that site
    return SpxCreateObjectWithSite<I>(className, sharedSitePtr);
}

template <class I>
inline std::shared_ptr<I> SpxCreateObjectWithSite(const char* className, std::shared_ptr<ISpxSite> site)
{
    // create the object
    auto factory = SpxQueryService<ISpxObjectFactory>(site);
    auto ptr = factory->CreateObject<I>(className);

    // set its site if appropriate
    auto wsite = std::dynamic_pointer_cast<ISpxObjectWithSite>(ptr);
    SPX_IFTRUE_THROW_HR(wsite && !site, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);

    if (wsite != nullptr && site != nullptr)
    {
        wsite->SetSite(site);
    }

    return ptr;
}


} // CARBON_IMPL_NAMESPACE
