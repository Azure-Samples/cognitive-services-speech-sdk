//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_object_helpers.h: Implementation declarations/definitions for SpxCreateObject* helper methods
//

#pragma once
#include "interface_helpers.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


template <class I>
inline std::shared_ptr<I> SpxCreateObjectWithSite(const char* className, ISpxGenericSite* site)
{
    // convert the argument to a shared pointer to the base site interafce
    auto sharedSitePtr = site->shared_from_this();

    // create the object with that site
    return SpxCreateObjectWithSite<I>(className, sharedSitePtr);
}

template <class I>
inline std::shared_ptr<I> SpxCreateObjectWithSite(const char* className, std::shared_ptr<ISpxGenericSite> site)
{
    // create the object
    auto factory = SpxQueryService<ISpxObjectFactory>(site);
    if (factory == nullptr)
    {
        SPX_DBG_TRACE_ERROR("site does not support ISpxObjectFactory");
        SPX_THROW_HR(SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
        return nullptr;
    }
    auto ptr = factory->CreateObject<I>(className);

    // set its site if appropriate
    auto objectWithSite = SpxQueryInterface<ISpxObjectWithSite>(ptr);
    if (objectWithSite != nullptr && site != nullptr)
    {
        objectWithSite->SetSite(site);
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE_IF(site != nullptr && objectWithSite == nullptr, "%s: Attempted SetSite, objectWithSite==nullptr", __FUNCTION__);
        SPX_DBG_TRACE_WARNING_IF(site == nullptr && objectWithSite != nullptr, "%s: Object Expects Site; No Site provided! UNEXPECTED!", __FUNCTION__);
        SPX_IFTRUE_THROW_HR(site == nullptr && objectWithSite != nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
    }

    return ptr;
}

template <class T>
inline void SpxTerm(const std::shared_ptr<T>& ptr)
{
    if (ptr == nullptr)
        return;

    SPX_DBG_TRACE_VERBOSE("%s: ptr=0x%8x", __FUNCTION__, ptr.get());
    auto objectWithSite = SpxQueryInterface<ISpxObjectWithSite>(ptr);
    auto objectInit = SpxQueryInterface<ISpxObjectInit>(ptr);
    if (objectWithSite != nullptr)
    {
        objectWithSite->SetSite(std::weak_ptr<ISpxGenericSite>());
    }
    else if (objectInit != nullptr)
    {
        objectInit->Term();
    }
}


template <class T>
inline void SpxTermAndClear(std::shared_ptr<T>& ptr)
{
    if (ptr != nullptr)
    {
        SpxTerm(ptr);
        ptr = nullptr;
    }
}

template <class T>
inline void SpxTermAndClearNothrow(std::shared_ptr<T>& ptr) noexcept
{
    if (ptr != nullptr)
    {
        try
        {
            SpxTerm(ptr);
        }
        catch (...)
        {
            // ignored
        }
        ptr = nullptr;
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
