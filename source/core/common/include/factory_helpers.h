//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// factory_helpers.h: Helper methods related to object factories
//

#pragma once
#include "spxcore_common.h"
#include "platform.h"
#include "string_utils.h"


namespace CARBON_IMPL_NAMESPACE() {


#define SPX_FACTORY_MAP_BEGIN()         \
    UNUSED(className);                  \
    UNUSED(interfaceName);

#define SPX_FACTORY_MAP_ENTRY(x, y)                                             \
    if (PAL::stricmp(className, #x) == 0)                                       \
    {                                                                           \
        if (PAL::stricmp(interfaceName, PAL::GetTypeName<y>().c_str()) == 0)    \
        {                                                                       \
            return SpxFactoryEntryCreateObject<x, y>();                         \
        }                                                                       \
    }

#define SPX_FACTORY_MAP_ENTRY_FUNC(x)                                           \
    {                                                                           \
        auto factory = x(className, interfaceName);                             \
        if (factory != nullptr)                                                 \
        {                                                                       \
            return factory;                                                     \
        }                                                                       \
    }

#define SPX_FACTORY_MAP_END()   \
    return nullptr;

template <class T, class I>
void* SpxFactoryEntryCreateObject()
{
    SPX_DBG_TRACE_VERBOSE("Creating object via %s: %s as %s", __FUNCTION__, PAL::GetTypeName<T>().c_str(), PAL::GetTypeName<I>().c_str());
    auto ptr = new T();
    auto interface = static_cast<I*>(ptr);
    return interface;
}


} // CARBON_IMPL_NAMESPACE
