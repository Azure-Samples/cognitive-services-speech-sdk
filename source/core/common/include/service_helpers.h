//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// service_helpers.h: Implementation declarations/definitions for Service helper methods and macros
//

#pragma once
#include <type_traits>

#include "spxcore_common.h"
#include "shared_ptr_helpers.h"
#include "string_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


// SERVICE_MAP_* macros...
//
//      Carbon implementation uses the QueryService pattern in many locations. As such, many objects
//      need to implement ISpxServiceProvider's QueryService method. In almost all cases, this is
//      the same type of function, where it compares the input parameters with the interfaces/services
//      that this service provider can support. Once it finds one that it supports, it returns the
//      appropriate interface pointer wrapped in a std::shared_ptr.
//
//      SPX_SERVICE_MAP_BEGIN()      - Logically starts the declaration/definition the map
//      SPX_SERVICE_MAP_ENTRY()      - Return an interface pointer that this object, itself, implements
//      SPX_SERVICE_MAP_ENTRY_FUNC() - Delegate to a supporting function that will look for supported services
//      SPX_SERVICE_MAP_ENTRY_SITE() - Delegate to an object's own site ptr
//      SPX_SERVICE_MAP_END()        - Logically ends the map declaration/definition of the map
//

#ifdef SPX_CONFIG_TRACE_SERVICE_MAP
#define SPX_DBG_TRACE_SERVICE_MAP_BEGIN() SPX_DBG_TRACE_VERBOSE("QUERYSERVICE: service=%s ; looking in %s ...", serviceName, __FUNCTION__)
#define SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT() SPX_DBG_TRACE_VERBOSE("QUERYSERVICE: service=%s ; looking in %s ... FOUND IT!!", serviceName, __FUNCTION__)
#else
#define SPX_DBG_TRACE_SERVICE_MAP_BEGIN()
#define SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT()
#endif

#define SPX_SERVICE_MAP_BEGIN()                                                         \
    std::shared_ptr<ISpxInterfaceBase> QueryService(const char* serviceName) override   \
    {                                                                                   \
        SPX_DBG_TRACE_SERVICE_MAP_BEGIN();

#define SPX_SERVICE_MAP_BEGIN_NAMED(x)                                                  \
    std::shared_ptr<ISpxInterfaceBase> x(const char* serviceName)                       \
    {                                                                                   \
        SPX_DBG_TRACE_SERVICE_MAP_BEGIN();

#define SPX_SERVICE_MAP_ENTRY(x)                                                        \
        if (PAL::stricmp(SpxTypeName(x), serviceName) == 0)                             \
        {                                                                               \
            SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT();                                       \
            return SpxSharedPtrFromThis<x>((x*)this);                                   \
        }

#define SPX_SERVICE_MAP_ENTRY_OBJECT(x, y)                                              \
        if (PAL::stricmp(SpxTypeName(x), serviceName) == 0)                             \
        {                                                                               \
            SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT();                                       \
            return SpxQueryInterface<x>(y);                                     \
        }

#define SPX_SERVICE_MAP_ENTRY_FUNC(x)                                                   \
        {                                                                               \
            auto service = x(serviceName);                                              \
            if (service != nullptr)                                                     \
            {                                                                           \
                SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT();                                   \
                return service;                                                         \
            }                                                                           \
        }

#define SPX_SERVICE_MAP_ENTRY_SITE(x)                                                   \
        {                                                                               \
            auto service = SpxQueryService<ISpxInterfaceBase>(x, serviceName);          \
            if (service != nullptr)                                                     \
            {                                                                           \
                SPX_DBG_TRACE_SERVICE_MAP_FOUND_IT();                                   \
                return service;                                                         \
            }                                                                           \
        }

#define SPX_SERVICE_MAP_END()                                                           \
        return nullptr;                                                                 \
    };


template <class I>
std::shared_ptr<I> SpxQueryService(std::shared_ptr<ISpxInterfaceBase> serviceProvider, const char* serviceName)
{
    auto provider = SpxQueryInterface<ISpxServiceProvider>(serviceProvider);
    if (provider != nullptr)
    {
        auto service = provider->QueryService(serviceName);
        auto it = SpxQueryInterface<I>(service);
        return it;
    }

    return nullptr;
}

template <class I, class T>
std::shared_ptr<I> SpxQueryService(std::shared_ptr<T> serviceProvider, const char* serviceName)
{
    auto provider = SpxQueryInterface<ISpxServiceProvider>(serviceProvider);
    if (provider != nullptr)
    {
        auto service = provider->QueryService(serviceName);
        auto it = SpxQueryInterface<I>(service);
        return it;
    }

    return nullptr;
}

template <class I, class T>
std::shared_ptr<I> SpxQueryService(std::shared_ptr<T> serviceProvider)
{
    return SpxQueryService<I>(serviceProvider, SpxTypeName(I));
}

template <typename I, typename T, typename F>
void InvokeOnServiceIfAvailable(std::shared_ptr<T> serviceProvider, F&& fn)
{
    auto ptr = SpxQueryService<I>(serviceProvider);
    if (ptr)
    {
        fn(*ptr);
    }
}

template<typename I, typename T, typename F>
std::enable_if_t<
    std::is_same<
        std::shared_ptr<ISpxServiceProvider>,
        decltype(std::declval<T>()->template QueryInterface<ISpxServiceProvider>())
    >::value,
void> InvokeOnService(T serviceProvider, F&& fn)
{
    auto ptr = serviceProvider->template QueryInterface<ISpxServiceProvider>();
    SPX_DBG_ASSERT(ptr != nullptr);
    InvokeOnServiceIfAvailable<I>(ptr, std::forward<F>(fn));
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
