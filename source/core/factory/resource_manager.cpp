//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_factory.cpp: Implementation definitions for CSpxResourceManager C++ class
//

#include "stdafx.h"
#include "resource_manager.h"
#include "module_factory.h"
#include "factory_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxResourceManager::CSpxResourceManager()
{
    // TODO: This list should not be compiled into carbon as it breaks the extensibility model
    //       by requiring to recompile carbon for every new extension dll that is to be supported.
    //
    // At least we should add some generic name that allows to add ONE extension lib without changing
    // the core implementation (e.g., take an environment variable).
    //
    // **IMPORTANT**: Do NOT change the order in which module factories are added here!!!
    //
    //   They will be searched in order for objects to create (See ::CreateObject).
    //   Changing the order will have adverse side effects on the intended behavior. 
    //
    //   FOR EXAMPLE: CSpxResourceManager intentionally searches for mock objects first. 
    //                This allows "at runtime testing". 

#ifdef __linux__
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libcarbon-mock.so"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libcarbon-pmakws.so"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon"));
#elif __MACH__
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libcarbon-mock.dylib"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libcarbon-pmakws.so"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon"));
#else
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon-mock"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon-pmakws"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon"));
    m_moduleFactories.push_back(CSpxModuleFactory::Get("carbon-unidec"));
#endif
}

void* CSpxResourceManager::CreateObject(const char* className, const char* interfaceName)
{
    // Loop thru each of our module factories, and see if they can create the object.
    //
    // If more than one module factory can create the object, we'll use the instance
    // from the first module factory that can create it. This enables "mocking" and
    // general "replacement" following the order in which the module factories are
    // added into the module factory list (see ctor...)

    for (auto factory: m_moduleFactories)
    {
        auto obj = factory->CreateObject(className, interfaceName);
        if (obj != nullptr)
        {
            return obj;
        }
    }

    return nullptr;
}


} // CARBON_IMPL_NAMESPACE
